# GLSLCompat

A proxy DLL that intercepts OpenGL calls to patch deprecated GLSL features for compatibility with modern drivers.  
It allows legacy demos and applications to run cleanly without shader compilation warnings or errors.

This project was written entirely with AI in an attempt to make "For Your Love" demo by ASD work on newer Nvidia GPU drivers.

## Features
- Replaces deprecated `varying` with modern `in/out` qualifiers
- Injects attributes for `gl_Vertex` to future-proof shaders
- Binds fragment outputs (`gl_FragColor`, `gl_FragData`) to user-defined outputs
- Async logging for debugging (enabled in Debug builds, disabled in Release builds)
- Caches transformed shader sources to avoid redundant work


## Usage

Place the `opengl32.dll` proxy file from the archive next to the app's executable.

(Use `Bin` folder for the "For Your Love" demo).

The proxy will automatically load the system OpenGL library and apply shader fixes.


## Build

Build the project in Visual Studio (DLL project).

## Build Configurations
- **Debug**: Logging enabled (`ENABLE_PROXY_LOGGING`), useful for development.
- **Release**: Logging disabled, maximum performance.
