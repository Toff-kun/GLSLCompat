# GLSLCompat

A proxy DLL that intercepts OpenGL calls to patch deprecated GLSL features for compatibility with modern drivers.  
It allows legacy demos and applications to run properly, without shader compilation warnings and errors.

This project was written entirely with AI to make the **"For Your Love"** demo by ASD work on newer NVIDIA GPU drivers.  
Optimizations were made to ensure the wrapper does not introduce additional rendering performance overhead.

## Features
- Replaces deprecated `varying` with modern `in/out` qualifiers
- Injects attributes for `gl_Vertex` to future-proof shaders
- Binds fragment outputs (`gl_FragColor`, `gl_FragData`) to user-defined outputs
- Async logging for debugging (enabled in Debug builds, disabled in Release builds)
- Caches transformed shader sources to avoid redundant processing

## Usage
1. Download the GLSLCompat archive from the [Releases](https://github.com/Toff-kun/GLSLCompat/releases) page.
2. Place the `opengl32.dll` proxy file from the archive next to your application's executable.  
(For the **"For Your Love"** demo, use the `Bin` folder).

The proxy will automatically load the system OpenGL library and apply the required shader fixes.

## Build

Open the solution in Visual Studio and build it as a DLL project.

### Build Configurations
- **Debug**: Logging enabled (`ENABLE_PROXY_LOGGING`) – useful for development and troubleshooting.
- **Release**: Logging disabled – maximum performance.
