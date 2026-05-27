// dllmain.cpp
// OpenGL proxy: conservative shader patching + fallback + fragdata binding + async logging
// Loads the system opengl32.dll directly from System32 - no renamed DLL required.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <cctype>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <unordered_map>

#pragma warning(disable:4273)

#pragma comment(linker, "/INCLUDE:wglGetProcAddress")
#pragma comment(linker, "/INCLUDE:glShaderSource")
#pragma comment(linker, "/INCLUDE:glShaderSourceARB")
#pragma comment(linker, "/INCLUDE:glCompileShader")
#pragma comment(linker, "/INCLUDE:glLinkProgram")
#pragma comment(linker, "/INCLUDE:glViewport")
#pragma comment(linker, "/INCLUDE:glGetString")

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef int GLint;
typedef char GLchar;
typedef void* GLhandleARB;
typedef char GLcharARB;
typedef int GLboolean;

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 0x91B9
#endif
#ifndef GL_SHADER_TYPE
#define GL_SHADER_TYPE 0x8B4F
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif
#ifndef GL_SHADING_LANGUAGE_VERSION
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#endif
#ifndef GL_VERSION
#define GL_VERSION 0x1F02
#endif

// No longer static - stubs.cpp accesses this via extern HMODULE hRealOpenGL
HMODULE hRealOpenGL = NULL;
static HMODULE g_hThisModule = NULL;

// -------------------- Conditional logging ----------------------------------
#if defined(_DEBUG) || defined(ENABLE_PROXY_LOGGING)
#define PROXY_LOG_ENABLED 1
#else
#define PROXY_LOG_ENABLED 0
#endif

// -------------------- Async logger -----------------------------------------
#if PROXY_LOG_ENABLED

namespace ProxyLog {
    static std::mutex queueMutex;
    static std::condition_variable queueCv;
    static std::vector<std::string> queue;
    static std::atomic<bool> running(false);
    static std::thread worker;
    static std::string logPath;

    inline void Enqueue(const std::string& msg) {
        { std::lock_guard<std::mutex> lk(queueMutex); queue.push_back(msg); }
        queueCv.notify_one();
    }

    static std::string BuildLogPath() {
        char path[MAX_PATH] = { 0 };
        if (!g_hThisModule)
            GetModuleHandleExA(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCSTR)&BuildLogPath, &g_hThisModule);
        if (!g_hThisModule) return "opengl_proxy_shader_log.txt";
        if (!GetModuleFileNameA(g_hThisModule, path, MAX_PATH))
            return "opengl_proxy_shader_log.txt";
        std::string p(path);
        size_t pos = p.find_last_of("\\/");
        std::string dir = (pos == std::string::npos) ? "" : p.substr(0, pos + 1);
        return dir + "opengl_proxy_shader_log.txt";
    }

    static void WorkerThread() {
        const std::chrono::milliseconds flushInterval(500);
        while (running.load()) {
            std::unique_lock<std::mutex> lk(queueMutex);
            if (queue.empty()) queueCv.wait_for(lk, flushInterval);
            if (!queue.empty()) {
                std::vector<std::string> local;
                local.swap(queue);
                lk.unlock();
                std::ofstream f(logPath, std::ios::app);
                if (f.is_open()) { for (const auto& m : local) f << m << "\n"; }
                for (const auto& m : local) { OutputDebugStringA(m.c_str()); OutputDebugStringA("\n"); }
            } else { lk.unlock(); }
        }
        std::vector<std::string> remaining;
        { std::lock_guard<std::mutex> lk(queueMutex); remaining.swap(queue); }
        if (!remaining.empty()) {
            std::ofstream f(logPath, std::ios::app);
            if (f.is_open()) { for (const auto& m : remaining) f << m << "\n"; }
            for (const auto& m : remaining) { OutputDebugStringA(m.c_str()); OutputDebugStringA("\n"); }
        }
    }

    inline void Start() {
        if (running.exchange(true)) return;
        logPath = BuildLogPath();
        worker = std::thread(WorkerThread);
    }
    inline void Stop() {
        if (!running.exchange(false)) return;
        queueCv.notify_one();
        if (worker.joinable()) worker.join();
    }
    inline void Log(const std::string& s) { Enqueue(s); }
}

#else

namespace ProxyLog {
    inline void Start() {}
    inline void Stop() {}
    inline void Log(const std::string&) {}
}

#endif // PROXY_LOG_ENABLED

static inline std::string ToStringInt(int v) {
    char buf[32]; _itoa_s(v, buf, 10); return std::string(buf);
}

// -------------------- Version patching -------------------------------------
static inline void trim_inplace(std::string& s) {
    while (!s.empty() && std::isspace((unsigned char)s.front())) s.erase(s.begin());
    while (!s.empty() && std::isspace((unsigned char)s.back()))  s.pop_back();
}

static std::string EnsureCompatibilityVersion(const char* src, size_t len) {
    if (!src) return std::string();
    std::string s(src, len ? len : strlen(src));

    size_t found = std::string::npos, searchPos = 0;
    while (true) {
        found = s.find("#version", searchPos);
        if (found == std::string::npos) break;
        size_t lineStart = s.rfind('\n', found);
        if (lineStart == std::string::npos) lineStart = 0; else lineStart++;
        std::string line = s.substr(lineStart, s.find('\n', lineStart) - lineStart);
        size_t nonws = line.find_first_not_of(" \t\r");
        if (nonws != std::string::npos && line.compare(nonws, 2, "//") == 0) {
            searchPos = found + 8; continue;
        }
        break;
    }

    std::string versionLine;
    if (found != std::string::npos) {
        size_t lineEnd = s.find('\n', found);
        if (lineEnd == std::string::npos) lineEnd = s.size();
        versionLine = s.substr(found, lineEnd - found);
        trim_inplace(versionLine);
        s.erase(found, (lineEnd < s.size() ? (lineEnd - found + 1) : (lineEnd - found)));
    }

    if (!versionLine.empty()) {
        std::istringstream iss(versionLine);
        std::string tok; iss >> tok;
        int ver = 0; iss >> ver;
        std::string rest; std::getline(iss, rest);
        trim_inplace(rest);
        if (rest.find("core") != std::string::npos) {
            size_t posCore = rest.find("core");
            rest.replace(posCore, 4, "compatibility");
        }
        if (rest.empty() && ver >= 150) rest = "compatibility";
        std::ostringstream oss;
        oss << "#version " << ver;
        if (!rest.empty()) oss << " " << rest;
        versionLine = oss.str();
    } else {
        versionLine = "#version 330 compatibility";
    }

    size_t firstNonWs = s.find_first_not_of(" \t\r\n");
    std::string remainder = (firstNonWs == std::string::npos) ? "" : s.substr(firstNonWs);
    return versionLine + "\n" + remainder;
}

// Returns the GLSL version number declared in the (already-patched) source.
// Used to pick the correct attribute qualifier when injecting declarations.
static int GetGLSLVersion(const std::string& src) {
    size_t pos = src.find("#version");
    if (pos == std::string::npos) return 110;
    int ver = 0;
    sscanf_s(src.c_str() + pos + 8, " %d", &ver);
    return (ver > 0) ? ver : 110;
}

// -------------------- Transform cache --------------------------------------
struct TransformCacheEntry {
    std::string transformed;
    std::vector<std::pair<int, std::string>> outputs;
};

static std::unordered_map<size_t, TransformCacheEntry> g_shaderTransformCache;
static std::mutex g_shaderTransformMutex;

static std::unordered_map<GLuint, std::string> g_originalSource;
static std::mutex g_originalMutex;

// Fragment data outputs only. Vertex attribute bindings are handled separately
// via the unconditional glBindAttribLocation call in Internal_glLinkProgram_impl.
static std::unordered_map<GLuint, std::vector<std::pair<int, std::string>>> g_injectedOutputs;
static std::mutex g_injectedMutex;

static size_t HashStringWithStage(const std::string& s, GLenum stage) {
    std::hash<std::string> h;
    size_t hv = h(s);
    hv ^= (static_cast<size_t>(stage) + 0x9e3779b97f4a7c15ULL + (hv << 6) + (hv >> 2));
    return hv;
}

static void ReplaceAll(std::string& s, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.length(), to);
        pos += to.length();
    }
}

static void WordReplace(std::string& s, const std::string& from, const std::string& to) {
    auto isword = [](char c) { return isalnum((unsigned char)c) || c == '_'; };
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        bool pre  = (pos == 0) || !isword(s[pos - 1]);
        bool post = (pos + from.size() >= s.size()) || !isword(s[pos + from.size()]);
        if (pre && post) { s.replace(pos, from.size(), to); pos += to.size(); }
        else pos++;
    }
}

static void InsertAfterVersion(std::string& src, const std::string& decl) {
    size_t pos = src.find('\n');
    if (pos == std::string::npos) src = decl + "\n" + src;
    else src.insert(pos + 1, decl + "\n");
}

static GLenum DetectShaderStageByContent(const std::string& src) {
    if (src.find("gl_FragColor")  != std::string::npos) return GL_FRAGMENT_SHADER;
    if (src.find("gl_FragData")   != std::string::npos) return GL_FRAGMENT_SHADER;
    if (src.find("gl_FragCoord")  != std::string::npos) return GL_FRAGMENT_SHADER;
    if (src.find("gl_Frag")       != std::string::npos) return GL_FRAGMENT_SHADER;
    if (src.find("gl_Position")   != std::string::npos) return GL_VERTEX_SHADER;
    if (src.find("attribute ")    != std::string::npos) return GL_VERTEX_SHADER;
    if (src.find("gl_Vertex")     != std::string::npos) return GL_VERTEX_SHADER;
    return 0;
}

static std::string TransformShaderSourceConservative(GLenum shaderType, const std::string& original, GLuint shaderId) {
    size_t key = HashStringWithStage(original, shaderType);
    {
        std::lock_guard<std::mutex> lk(g_shaderTransformMutex);
        auto it = g_shaderTransformCache.find(key);
        if (it != g_shaderTransformCache.end()) {
            if (!it->second.outputs.empty() && shaderId != 0) {
                std::lock_guard<std::mutex> lk2(g_injectedMutex);
                g_injectedOutputs[shaderId] = it->second.outputs;
            }
            return it->second.transformed;
        }
    }

    std::string s = EnsureCompatibilityVersion(original.c_str(), original.size());
    GLenum effectiveStage = shaderType;
    if (effectiveStage == 0) effectiveStage = DetectShaderStageByContent(s);

    std::vector<std::pair<int, std::string>> outputsLocal;

    if (effectiveStage == GL_FRAGMENT_SHADER) {
        if (s.find("gl_FragColor") != std::string::npos) {
            std::string name = "_fragColor0";
            if (s.find("out vec4 " + name) == std::string::npos)
                InsertAfterVersion(s, "// injected compatibility output\nout vec4 " + name + ";");
            ReplaceAll(s, "gl_FragColor", name);
            outputsLocal.emplace_back(0, name);
        }
        for (int i = 0; i < 8; ++i) {
            std::ostringstream tok; tok << "gl_FragData[" << i << "]";
            std::string token = tok.str();
            if (s.find(token) != std::string::npos) {
                std::string nm = "_fragColor" + std::to_string(i);
                if (s.find("out vec4 " + nm) == std::string::npos)
                    InsertAfterVersion(s, "// injected fragdata output\nout vec4 " + nm + ";");
                ReplaceAll(s, token, nm);
                outputsLocal.emplace_back(i, nm);
            }
        }
        WordReplace(s, "varying", "in");
    }
    else if (effectiveStage == GL_VERTEX_SHADER) {
        WordReplace(s, "varying", "out");

        // FIX: use 'attribute' for GLSL < 130, 'in' for GLSL >= 130.
        // 'in' is not valid syntax for vertex attributes in #version 120 shaders.
        if (s.find("gl_Vertex") != std::string::npos) {
            if (s.find("_aVertex") == std::string::npos) {
                int glslVer = GetGLSLVersion(s);
                std::string qualifier = (glslVer >= 130) ? "in" : "attribute";
                InsertAfterVersion(s, "// injected replacement for gl_Vertex\n" + qualifier + " vec4 _aVertex;");
            }
            WordReplace(s, "gl_Vertex", "_aVertex");
            // NOTE: no g_injectedOutputs entry here - _aVertex is a vertex attribute,
            // not a fragment output. glBindAttribLocation is called unconditionally
            // for all programs in Internal_glLinkProgram_impl instead.
        }
    }

    TransformCacheEntry entry;
    entry.transformed = s;
    entry.outputs     = outputsLocal;

    if (!outputsLocal.empty() && shaderId != 0) {
        std::lock_guard<std::mutex> lk(g_injectedMutex);
        g_injectedOutputs[shaderId] = outputsLocal;
    }
    {
        std::lock_guard<std::mutex> lk(g_shaderTransformMutex);
        g_shaderTransformCache.emplace(key, std::move(entry));
    }
    return s;
}

// -------------------- Real function pointers -------------------------------
typedef void (APIENTRY* PFN_glShaderSource)(GLuint, GLsizei, const GLchar* const*, const GLint*);
typedef void (APIENTRY* PFN_glShaderSourceARB)(GLhandleARB, GLsizei, const GLcharARB**, const GLint*);
typedef void (APIENTRY* PFN_glCompileShader)(GLuint);
typedef void (APIENTRY* PFN_glLinkProgram)(GLuint);
typedef void (APIENTRY* PFN_glGetShaderiv)(GLuint, GLenum, GLint*);
typedef void (APIENTRY* PFN_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRY* PFN_glGetProgramiv)(GLuint, GLenum, GLint*);
typedef void (APIENTRY* PFN_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRY* PFN_glBindFragDataLocation)(GLuint, GLuint, const char*);
typedef void (APIENTRY* PFN_glBindAttribLocation)(GLuint, GLuint, const char*);
typedef void (APIENTRY* PFN_glViewport)(GLint, GLint, GLsizei, GLsizei);
typedef const unsigned char* (APIENTRY* PFN_glGetString)(GLenum);

static PFN_glShaderSource         real_glShaderSource         = nullptr;
static PFN_glShaderSourceARB      real_glShaderSourceARB      = nullptr;
static PFN_glCompileShader        real_glCompileShader        = nullptr;
static PFN_glLinkProgram          real_glLinkProgram          = nullptr;
static PFN_glGetShaderiv          real_glGetShaderiv          = nullptr;
static PFN_glGetShaderInfoLog     real_glGetShaderInfoLog     = nullptr;
static PFN_glGetProgramiv         real_glGetProgramiv         = nullptr;
static PFN_glGetProgramInfoLog    real_glGetProgramInfoLog    = nullptr;
static PFN_glBindFragDataLocation real_glBindFragDataLocation = nullptr;
static PFN_glBindAttribLocation   real_glBindAttribLocation   = nullptr;
static PFN_glViewport             real_glViewport             = nullptr;
static PFN_glGetString            real_glGetString            = nullptr;

// -------------------- Proc helpers -----------------------------------------
static PROC RealWglGetProcAddress(LPCSTR name) {
    if (!hRealOpenGL) return NULL;
    typedef PROC(WINAPI* pfnWGL)(LPCSTR);
    static pfnWGL realWgl = nullptr;
    if (!realWgl) realWgl = (pfnWGL)GetProcAddress(hRealOpenGL, "wglGetProcAddress");
    return realWgl ? realWgl(name) : NULL;
}

static void* GetRealProc(const char* name) {
    if (!hRealOpenGL || !name) return nullptr;
    void* p = (void*)GetProcAddress(hRealOpenGL, name);
    if (p) return p;
    return (void*)RealWglGetProcAddress(name);
}

static void EnsureRealFuncs() {
    if (!hRealOpenGL) return;
    if (!real_glShaderSource)         real_glShaderSource         = (PFN_glShaderSource)         GetRealProc("glShaderSource");
    if (!real_glShaderSourceARB)      real_glShaderSourceARB      = (PFN_glShaderSourceARB)      GetRealProc("glShaderSourceARB");
    if (!real_glCompileShader)        real_glCompileShader        = (PFN_glCompileShader)         GetRealProc("glCompileShader");
    if (!real_glLinkProgram)          real_glLinkProgram          = (PFN_glLinkProgram)           GetRealProc("glLinkProgram");
    if (!real_glGetShaderiv)          real_glGetShaderiv          = (PFN_glGetShaderiv)           GetRealProc("glGetShaderiv");
    if (!real_glGetShaderInfoLog)     real_glGetShaderInfoLog     = (PFN_glGetShaderInfoLog)      GetRealProc("glGetShaderInfoLog");
    if (!real_glGetProgramiv)         real_glGetProgramiv         = (PFN_glGetProgramiv)          GetRealProc("glGetProgramiv");
    if (!real_glGetProgramInfoLog)    real_glGetProgramInfoLog    = (PFN_glGetProgramInfoLog)     GetRealProc("glGetProgramInfoLog");
    if (!real_glBindFragDataLocation) real_glBindFragDataLocation = (PFN_glBindFragDataLocation)  GetRealProc("glBindFragDataLocation");
    if (!real_glBindAttribLocation)   real_glBindAttribLocation   = (PFN_glBindAttribLocation)    GetRealProc("glBindAttribLocation");
    if (!real_glViewport)             real_glViewport             = (PFN_glViewport)              GetRealProc("glViewport");
    if (!real_glGetString)            real_glGetString            = (PFN_glGetString)             GetRealProc("glGetString");
}

// -------------------- Shader log helpers -----------------------------------
static void DumpShaderLog(GLuint shader) {
    EnsureRealFuncs();
    if (!real_glGetShaderiv || !real_glGetShaderInfoLog) return;
    GLint status = 0;
    real_glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        GLint len = 0;
        real_glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 0) {
            std::vector<char> buf(len + 1);
            real_glGetShaderInfoLog(shader, len, nullptr, buf.data());
            ProxyLog::Log(std::string("Shader compile failed: ") + buf.data());
        } else { ProxyLog::Log("Shader compile failed: (no log)"); }
    }
}

static void DumpProgramLog(GLuint program) {
    EnsureRealFuncs();
    if (!real_glGetProgramiv || !real_glGetProgramInfoLog) return;
    GLint status = 0;
    real_glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == 0) {
        GLint len = 0;
        real_glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        if (len > 0) {
            std::vector<char> buf(len + 1);
            real_glGetProgramInfoLog(program, len, nullptr, buf.data());
            ProxyLog::Log(std::string("Program link failed: ") + buf.data());
        } else { ProxyLog::Log("Program link failed: (no log)"); }
    }
}

// -------------------- Internal implementations -----------------------------
static void Internal_glShaderSource_impl(GLuint shader, GLsizei count,
    const GLchar* const* string, const GLint* length)
{
    EnsureRealFuncs();
    if (count <= 0 || !string) {
        if (real_glShaderSource) real_glShaderSource(shader, count, string, length);
        return;
    }
    std::string combined;
    combined.reserve(4096);
    for (int i = 0; i < count; ++i) {
        const char* s = string[i];
        if (!s) continue;
        size_t len = (length && length[i] >= 0) ? (size_t)length[i] : strlen(s);
        combined.append(s, len);
    }
    { std::lock_guard<std::mutex> lk(g_originalMutex); g_originalSource[shader] = combined; }

    GLenum shaderType = 0;
    if (real_glGetShaderiv) {
        GLint st = 0;
        real_glGetShaderiv(shader, GL_SHADER_TYPE, &st);
        if (st == GL_VERTEX_SHADER || st == GL_FRAGMENT_SHADER || st == GL_COMPUTE_SHADER)
            shaderType = (GLenum)st;
    }
    if (shaderType == 0) shaderType = DetectShaderStageByContent(combined);

    std::string transformed = TransformShaderSourceConservative(shaderType, combined, shader);
    {
        std::ostringstream oss;
        oss << "---- Shader ID: " << shader << " (stage=" << shaderType << ") ----";
        ProxyLog::Log(oss.str());
        ProxyLog::Log("Original:"); ProxyLog::Log(combined.empty() ? "(empty)" : combined);
        ProxyLog::Log("Transformed:"); ProxyLog::Log(transformed.empty() ? "(empty)" : transformed);
        ProxyLog::Log("-------------------------------");
    }
    const char* p = transformed.c_str();
    if (real_glShaderSource) real_glShaderSource(shader, 1, &p, nullptr);
    else { auto f = (PFN_glShaderSource)GetRealProc("glShaderSource"); if (f) f(shader, 1, &p, nullptr); }
}

static void Internal_glShaderSourceARB_impl(GLhandleARB shader, GLsizei count,
    const GLcharARB** string, const GLint* length)
{
    EnsureRealFuncs();
    if (count <= 0 || !string) {
        if (real_glShaderSourceARB) real_glShaderSourceARB(shader, count, string, length);
        return;
    }
    std::string combined;
    combined.reserve(4096);
    for (int i = 0; i < count; ++i) {
        const char* s = string[i];
        if (!s) continue;
        size_t len = (length && length[i] >= 0) ? (size_t)length[i] : strlen(s);
        combined.append(s, len);
    }
    GLenum shaderType = DetectShaderStageByContent(combined);
    std::string transformed = TransformShaderSourceConservative(shaderType, combined, 0);
    {
        std::ostringstream oss;
        oss << "---- ARB Shader ID: " << (uintptr_t)shader << " (stage=" << shaderType << ") ----";
        ProxyLog::Log(oss.str());
        ProxyLog::Log("Original:"); ProxyLog::Log(combined.empty() ? "(empty)" : combined);
        ProxyLog::Log("Transformed:"); ProxyLog::Log(transformed.empty() ? "(empty)" : transformed);
        ProxyLog::Log("-------------------------------");
    }
    const char* p = transformed.c_str();
    if (real_glShaderSourceARB) real_glShaderSourceARB(shader, 1, &p, nullptr);
    else { auto f = (PFN_glShaderSourceARB)GetRealProc("glShaderSourceARB"); if (f) f(shader, 1, &p, nullptr); }
}

static void Internal_glCompileShader_impl(GLuint shader) {
    EnsureRealFuncs();
    if (real_glCompileShader) real_glCompileShader(shader);
    else { auto f = (PFN_glCompileShader)GetRealProc("glCompileShader"); if (f) f(shader); }

    bool ok = true;
    if (real_glGetShaderiv) {
        GLint status = 0; real_glGetShaderiv(shader, GL_COMPILE_STATUS, &status); ok = (status != 0);
    }
    if (!ok) {
        DumpShaderLog(shader);
        std::string orig;
        { std::lock_guard<std::mutex> lk(g_originalMutex); auto it = g_originalSource.find(shader); if (it != g_originalSource.end()) orig = it->second; }
        if (!orig.empty()) {
            ProxyLog::Log("Transformed shader failed; retrying original source as fallback.");
            const char* p = orig.c_str();
            if (real_glShaderSource) real_glShaderSource(shader, 1, &p, nullptr);
            else { auto f = (PFN_glShaderSource)GetRealProc("glShaderSource"); if (f) f(shader, 1, &p, nullptr); }
            if (real_glCompileShader) real_glCompileShader(shader);
            else { auto f = (PFN_glCompileShader)GetRealProc("glCompileShader"); if (f) f(shader); }
            if (real_glGetShaderiv) {
                GLint s2 = 0; real_glGetShaderiv(shader, GL_COMPILE_STATUS, &s2);
                if (s2 == 0) { ProxyLog::Log("Fallback original shader also failed."); DumpShaderLog(shader); }
                else {
                    ProxyLog::Log("Fallback original shader compiled successfully.");
                    std::lock_guard<std::mutex> lk2(g_injectedMutex); g_injectedOutputs.erase(shader);
                }
            }
        }
    } else { DumpShaderLog(shader); }
}

static void Internal_glLinkProgram_impl(GLuint program) {
    EnsureRealFuncs();

    // Bind fragment data outputs (gl_FragColor / gl_FragData replacements).
    // _fragColorN always maps to location N; binding a non-existent name is a no-op.
    {
        std::lock_guard<std::mutex> lk(g_injectedMutex);
        if (real_glBindFragDataLocation) {
            for (const auto& kv : g_injectedOutputs) {
                for (const auto& pr : kv.second) {
                    real_glBindFragDataLocation(program, (GLuint)pr.first, pr.second.c_str());
                    std::ostringstream oss;
                    oss << "Bound frag data: program=" << program
                        << " name=" << pr.second << " loc=" << pr.first;
                    ProxyLog::Log(oss.str());
                }
            }
        }
    }

    // Bind vertex attribute replacement for gl_Vertex to slot 0.
    // gl_Vertex is attribute slot 0 in the fixed-function pipeline.
    // Binding a non-existent name is a no-op, so this is safe for all programs.
    if (real_glBindAttribLocation) {
        real_glBindAttribLocation(program, 0, "_aVertex");
        ProxyLog::Log("Bound attrib: program=" + ToStringInt(program) + " _aVertex -> slot 0");
    }

    if (real_glLinkProgram) real_glLinkProgram(program);
    else { auto f = (PFN_glLinkProgram)GetRealProc("glLinkProgram"); if (f) f(program); }
    DumpProgramLog(program);
}

// -------------------- glViewport -------------------------------------------
extern "C" void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    EnsureRealFuncs();
#if PROXY_LOG_ENABLED
    static std::mutex vpMutex;
    static int lastW = -1, lastH = -1;
    static uint64_t lastLogMs = 0;
    uint64_t nowMs = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    bool shouldLog = false;
    {
        std::lock_guard<std::mutex> lk(vpMutex);
        if (width != lastW || height != lastH) { shouldLog = true; lastW = width; lastH = height; lastLogMs = nowMs; }
        else if (nowMs - lastLogMs >= 1000) { shouldLog = true; lastLogMs = nowMs; }
    }
    if (shouldLog)
        ProxyLog::Log(std::string("glViewport: x=") + ToStringInt(x) + " y=" + ToStringInt(y) +
            " w=" + ToStringInt(width) + " h=" + ToStringInt(height));
#endif
    if (real_glViewport) real_glViewport(x, y, width, height);
    else { auto f = (PFN_glViewport)GetRealProc("glViewport"); if (f) f(x, y, width, height); }
}

// -------------------- glGetString ------------------------------------------
extern "C" const unsigned char* APIENTRY glGetString(GLenum name) {
    EnsureRealFuncs();
    const unsigned char* ret = nullptr;
    if (real_glGetString) ret = real_glGetString(name);
    else { auto f = (PFN_glGetString)GetRealProc("glGetString"); if (f) ret = f(name); }
    if (name == GL_SHADING_LANGUAGE_VERSION)
        ProxyLog::Log(std::string("glGetString(GL_SHADING_LANGUAGE_VERSION) => ") + (ret ? (const char*)ret : "NULL"));
    else if (name == GL_VERSION)
        ProxyLog::Log(std::string("glGetString(GL_VERSION) => ") + (ret ? (const char*)ret : "NULL"));
    return ret;
}

// -------------------- Exported interceptors --------------------------------
extern "C" void APIENTRY glShaderSource(GLuint shader, GLsizei count,
    const GLchar* const* string, const GLint* length) {
    Internal_glShaderSource_impl(shader, count, string, length);
}
extern "C" void APIENTRY glShaderSourceARB(GLhandleARB shader, GLsizei count,
    const GLcharARB** string, const GLint* length) {
    Internal_glShaderSourceARB_impl(shader, count, string, length);
}
extern "C" void APIENTRY glCompileShader(GLuint shader) {
    Internal_glCompileShader_impl(shader);
}
extern "C" void APIENTRY glLinkProgram(GLuint program) {
    Internal_glLinkProgram_impl(program);
}

// -------------------- DllMain ----------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_hThisModule = hModule;
        ProxyLog::Start();
        char path[MAX_PATH];
        if (GetSystemDirectoryA(path, MAX_PATH)) {
            strcat_s(path, "\\opengl32.dll");
            hRealOpenGL = LoadLibraryA(path);
            if (hRealOpenGL) {
                EnsureRealFuncs();
                ProxyLog::Log("opengl proxy loaded (system opengl32.dll)");
            } else {
                ProxyLog::Log("opengl proxy: failed to load system opengl32.dll");
            }
        } else {
            ProxyLog::Log("opengl proxy: GetSystemDirectoryA failed");
        }
    } else if (reason == DLL_PROCESS_DETACH) {
        ProxyLog::Log("opengl proxy unloading");
        ProxyLog::Stop();
        if (hRealOpenGL) { FreeLibrary(hRealOpenGL); hRealOpenGL = NULL; }
    }
    return TRUE;
}

// -------------------- wglGetProcAddress ------------------------------------
extern "C" PROC WINAPI wglGetProcAddress(LPCSTR procName) {
    if (!procName) return NULL;
#if PROXY_LOG_ENABLED
    static std::atomic<uint64_t> lastLogTime(0);
    uint64_t nowMs = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    if (nowMs - lastLogTime.load() > 2000) {
        ProxyLog::Log(std::string("wglGetProcAddress: ") + procName);
        lastLogTime.store(nowMs);
    }
#endif
    if (strcmp(procName, "glShaderSource")    == 0) return (PROC)glShaderSource;
    if (strcmp(procName, "glShaderSourceARB") == 0) return (PROC)glShaderSourceARB;
    if (strcmp(procName, "glCompileShader")   == 0) return (PROC)glCompileShader;
    if (strcmp(procName, "glLinkProgram")     == 0) return (PROC)glLinkProgram;
    if (strcmp(procName, "glViewport")        == 0) return (PROC)glViewport;
    if (strcmp(procName, "glGetString")       == 0) return (PROC)glGetString;

    typedef PROC(WINAPI* pfnWGL)(LPCSTR);
    pfnWGL real = nullptr;
    if (hRealOpenGL) real = (pfnWGL)GetProcAddress(hRealOpenGL, "wglGetProcAddress");
    return real ? real(procName) : NULL;
}
