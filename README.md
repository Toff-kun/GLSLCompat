# GLSLCompat

## Project Summary
**GLSLCompat** is an OpenGL proxy DLL that intercepts shader submission calls and applies **version‑aware** compatibility patches so legacy GLSL shaders run on modern drivers without causing compile errors. It preserves valid legacy syntax for GLSL \< 130 while modernizing GLSL ≥ 130, and it records fragment outputs for binding when appropriate.

This project was written entirely with AI to make the **"For Your Love"** demo by ASD work on newer NVIDIA GPU drivers.  
Optimizations were made to ensure the wrapper does not introduce additional rendering performance overhead.

## Key Features
- **GLSL version aware transforms** — injects `in/out` only for GLSL ≥ 130; uses `attribute` for legacy vertex replacements.
- **Safe fragment handling** — avoids injecting modern fragment outputs into GLSL \< 130 to prevent compile failures.
- **Fragment output binding** — records and binds replacements for `gl_FragColor` / `gl_FragData` on supported GLSL versions.
- **Transform cache** — caches transformed shader sources to avoid redundant processing.
- **Async debug logging** — logs shader transforms and info logs when enabled (Debug builds); disabled in Release builds.
- **Lazy real proc resolution** — resolves and caches real GL entry points on first use to avoid per‑call overhead.

## Usage
1. Download the GLSLCompat archive from the [Releases](https://github.com/Toff-kun/GLSLCompat/releases) page.
2. Place the `opengl32.dll` proxy file from the archive next to your application's executable.  
(For the **"For Your Love"** demo, use the `Bin` folder).

The proxy will automatically load the system OpenGL library and apply the required shader fixes.

## Build

- Open the solution in Visual Studio and build the project as a **DLL**.
- **Debug**: `ENABLE_PROXY_LOGGING` enabled — useful for development and troubleshooting.
- **Release**: logging disabled — optimized for performance.
