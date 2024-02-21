# 🎮 Wispy

![C](https://img.shields.io/github/languages/top/julesgrc0/wispy?label=C%20🔥) ![Platform](https://img.shields.io/badge/Platform%20🌍-Windows%20|%20Linux%20|%20Android-purple) ![Game](https://img.shields.io/badge/Game%20🎮-Wispy-green) ![Version](https://shields.io/github/v/release/julesgrc0/wispy?label=Version%20🧮&color=lightblue) [![Release Build](https://github.com/julesgrc0/wispy/actions/workflows/windows.yml/badge.svg)](https://github.com/julesgrc0/wispy/actions/workflows/windows.yml) [![Release Build](https://github.com/julesgrc0/wispy/actions/workflows/linux.yml/badge.svg)](https://github.com/julesgrc0/wispy/actions/workflows/linux.yml) [![Release Build](https://github.com/julesgrc0/wispy/actions/workflows/android.yml/badge.svg)](https://github.com/julesgrc0/wispy/actions/workflows/android.yml)

**Concept**: Wipsy is a kind of 2D Minecraft.

<div align="center">

![Video 0](./preview/demo0.gif)

</div>

| ![Screenshot 1](./preview/menu.png)  | ![Screenshot 2](./preview/game0.png) |
| ------------------------------------ | ------------------------------------ |
| ![Screenshot 3](./preview/game1.png) | ![Screenshot 4](./preview/game2.png) |

## 🚀 Project Insight

### 📚 Libraries

- zlib
- raylib
- pthread (linux/android)

### 🛠 Building

**⚙ Requirements**

- [python](https://www.python.org/downloads/)
- [cmake](https://cmake.org/)
- [upx](https://github.com/upx/upx) (windows)

**📦 Packing Assets**

```bash
python ./tools/pack_assets.py
```

**🌐 Clone the repository**

```bash
git clone --recurse-submodules https://github.com/julesgrc0/wispy.git
```

**💻 Building for Windows**

> VSCode

Run the build task with the following shortcut:

<kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd>

Then select your target build configuration :

- `Debug`/`Release`/`RelMinSize` (Compressed with UPX)
- `x64`/`x86`
- `windows`/`linux`

> Visual Studio

Open the project folder and select the build configuration.

**🐧 Building for Linux**

> Install the required packages

```bash
sudo apt-get update -y
sudo apt-get install -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev
```

> Build with CMake

```bash
cmake -S . -B out --preset=<x64|x86>-<debug|release|relminsize>-<windows|linux|android>
cmake --build out --config <debug|release>
```

**📱 Building for Android**

> 🚧 Experimental 🚧

The android version is still in development and may not work as expected. For the moment, the camera is laggy and the game is not very optimized for mobile devices.

> Build with Android Studio

Open the project folder with Android Studio and select the build configuration.

> Command line

See the workflow file [android.yml](.github/workflows/android.yml) for more details.

## 🤝 Get Involved

Wellcome to the community ! We are happy to see you here. If you want to [contribute](CONTRIBUTING.md) to the project, you can start by forking the repository and then submit a pull request 🔥 .

## 📃 License

This project is licensed under the terms of the [MIT license](LICENSE).
