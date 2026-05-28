# GLSLCompat

**An OpenGL proxy DLL that makes legacy GLSL shaders work on modern drivers.**

GLSLCompat intercepts shader compilation calls and automatically applies compatibility patches so old GLSL code (especially pre-GLSL 130) can run without modification on modern OpenGL implementations.

This project was created to fix shader compilation issues in the **"For Your Love"** demo by ASD on newer NVIDIA drivers.

---

## Key Features

- **Smart Version-Aware Patching** — Correctly uses `attribute`/`varying` for legacy shaders (< 130) and `in`/`out` for modern ones.
- **Safe Fragment Output Handling** — Replaces `gl_FragColor` and `gl_FragData[]` with modern `out` variables and binds them.
- **gl_Vertex Support** — Injects compatible attribute replacement for legacy vertex shaders.
- **Transform Caching** — Avoids reprocessing identical shaders for maximum performance.
- **Lazy Function Resolution** — Minimal runtime overhead by resolving real OpenGL functions only when first used.
- **Async Logging** — Detailed transformation and error logs in Debug builds (completely disabled in Release).

---

## Usage

1. Download the latest release from the [Releases page](https://github.com/Toff-kun/GLSLCompat/releases).
2. Extract `opengl32.dll` from the downloaded archive.
3. Place it **next to your application's executable** (.exe file).

> **For the "For Your Love" demo**: Put the DLL inside the `Bin` folder.

The proxy will automatically load the real system `opengl32.dll` and apply the necessary fixes transparently.

---

## How It Works

GLSLCompat acts as a drop-in replacement for `opengl32.dll`. When an application submits shaders, it:

- Captures the original shader source
- Rewrites legacy syntax while preserving original behavior
- Caches the transformed version
- Forwards it to the real OpenGL driver
- Binds fragment outputs (`gl_FragColor` / `gl_FragData`) and vertex attributes as needed

This allows many old fixed-function style shaders to compile and run correctly on strict modern drivers.

---

## Building from Source

1. Open `GLSLCompat.sln` in **Visual Studio**.
2. Build the project as a **DLL**.
   - **Debug** — Logging enabled (great for troubleshooting)
   - **Release** — Logging disabled (best performance)

---

## Notes

- This is a **compatibility layer**, not a complete OpenGL implementation.
- Works best with legacy GLSL shaders (versions 110–330).
- Performance impact is negligible due to caching and lazy loading.

---
