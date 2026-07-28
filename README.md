# Minecart Engine

A CMake + C++23 workspace split into two projects:

- **`engine/`** — a static library that owns SDL3 (fetched via CMake's
  `FetchContent`, pinned to the latest full SDL3 release), the `Window` and
  `Renderer` abstractions, and the abstract `Application` base class. Nothing
  outside `engine/` touches SDL3 directly.
- **`examples/color_demo/`** — a concrete application built on top of the
  engine: it implements `Application`'s `onInit()` hook and provides two
  concrete `Renderer`s.

## Project layout

```
.
├── CMakeLists.txt                     # top-level: wires engine + examples together
├── engine/
│   ├── CMakeLists.txt                 # FetchContent-declares/builds SDL3, builds libengine
│   ├── include/engine/
│   │   ├── Application.hpp            # abstract Application: onInit/onEvent/onTick
│   │   ├── Window.hpp                 # one SDL_Window + command-buffer/swapchain mechanics
│   │   └── Renderer.hpp               # abstract interface: "how a frame gets rendered"
│   └── src/
│       ├── Application.cpp
│       └── Window.cpp
└── examples/
    └── color_demo/
        ├── CMakeLists.txt             # builds the color_demo executable, links engine
        └── src/
            ├── main.cpp                          # constructs ColorDemoApplication, calls run()
            ├── ColorDemoApplication.hpp/.cpp      # concrete Application: creates windows in onInit()
            ├── ClearColorRenderer.hpp/.cpp        # Renderer: clears to a fixed color
            └── PulsingClearColorRenderer.hpp/.cpp # Renderer: animates between two colors
```

## 1. Configure and build

No submodule init step needed — CMake's `FetchContent` clones SDL3 for you
during configure (from `engine/CMakeLists.txt`), pinned to a specific tag:

```cmake
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.12   # latest full SDL3 release as of writing
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3)
```

Just configure and build normally from the repo root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The first configure will `git clone` SDL3 into `build/_deps/sdl3-src` (shallow,
just that one tag) and the first build compiles it as a static library linked
into `engine`, so expect the initial run to take a few minutes. Subsequent
configures/builds reuse what's already in `build/_deps` and are fast.

To move to a newer SDL3 release later, bump `GIT_TAG` in `engine/CMakeLists.txt`
to the new tag (e.g. `release-3.5.0`) and re-run CMake — no submodule commands
required.

### Platform notes
- **Linux**: make sure X11/Wayland dev packages are installed (SDL's own
  README lists exact package names per distro) and that you have a Vulkan
  driver, since SDL_GPU picks Vulkan on Linux by default.
- **macOS**: SDL_GPU uses Metal; no extra setup needed beyond Xcode command
  line tools.
- **Windows**: SDL_GPU uses D3D12 by default; build with Visual Studio's
  generator (`cmake -G "Visual Studio 17 2022"`) or Ninja + MSVC.

## 2. Run it

```bash
./build/examples/color_demo/color_demo
```

You should see three windows open ("Window A - Red", "Window B - Green",
"Window C - Pulsing"), each cleared to its own color via a distinct SDL_GPU
render pass every frame. Close them individually (or all at once) — the app
keeps running windows alive independently and exits once the last one is
closed.

## How the pieces fit together

- **`engine::Application`** owns SDL's lifetime and the single shared
  `SDL_GPUDevice` (SDL_GPU's intended pattern: one device shared across the
  app), owns the `std::vector<std::unique_ptr<Window>>`, and runs the generic
  event/render loop in `run()`. It knows nothing about *which* windows exist
  or *what* they render — that's left entirely to a subclass via three hooks:
  - `onInit()` — called once, before the loop starts, so a concrete
    application can build its initial windows.
  - `onEvent(event)` — called for every SDL event, after the base class's own
    handling of quit/close-requested.
  - `onTick(deltaTime)` — called once per frame with the elapsed time in
    seconds since the previous tick.

  `main()` never touches window setup itself — it just constructs the
  concrete `Application` subclass and calls `run()`.

- **`engine::Window`** wraps a single `SDL_Window*`, claims it against the
  shared GPU device with `SDL_ClaimWindowForGPUDevice`, and owns exactly one
  `std::unique_ptr<Renderer>`. `Window::render()` handles only the GPU
  *mechanics*: acquire a command buffer, wait on/acquire that window's
  swapchain texture, hand both to the `Renderer`, then submit. It has no idea
  what actually gets drawn.

- **`engine::Renderer`** is the abstract interface that owns the render pass
  itself (`render(commandBuffer, swapchainTexture, width, height)`). Because
  it's abstract, a concrete application's `onInit()` can freely mix multiple
  `Renderer` subclasses across windows with no changes to `Window` or
  `Application`.

- **`ColorDemoApplication`** (in `examples/color_demo/`) is the concrete
  `Application`. Its `onInit()` creates three windows:
  - **`ClearColorRenderer`** — used by both Window A and Window B, i.e. two
    windows with the *same* Renderer subclass, just constructed with
    different colors.
  - **`PulsingClearColorRenderer`** — used by Window C, an entirely
    *different* Renderer subclass that animates between two colors over
    time using `SDL_GetTicks()`.

## Extending this

- To add a new example application, add a new directory under `examples/`
  with its own `CMakeLists.txt` that links against `engine`, its own
  `Application` subclass, and any `Renderer`s it needs — nothing in `engine/`
  needs to change.
- To add a new way of rendering, write a new `Renderer` subclass and pass an
  instance of it to `createWindow()` from your `Application`'s `onInit()`.
- To actually draw geometry instead of just clearing, build a graphics
  pipeline (`SDL_CreateGPUGraphicsPipeline`) with shaders compiled to
  SPIR-V/DXIL/MSL inside a new `Renderer::render()` override, and issue draw
  calls between `SDL_BeginGPURenderPass`/`SDL_EndGPURenderPass`.
- To let two windows *share* the same `Renderer` instance (rather than two
  separate instances of the same class), change `Window` to hold a
  `std::shared_ptr<Renderer>` instead of `std::unique_ptr<Renderer>`.

## License

The engine is licensed under the [Polyform Perimeter License 1.0.1](./LICENSE-ENGINE). Examples are licensed under the [MIT License](./LICENSE-EXAMPLES). See the respective LICENSE files for details.

> Required Notice: Copyright Samuel Hulme. (https://github.com/samuelh2005)
