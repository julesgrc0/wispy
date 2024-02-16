# 🎮 Wispy

![C](https://img.shields.io/github/languages/top/julesgrc0/wispy?label=C%20🔥) ![Platform](https://img.shields.io/badge/Platform%20🌍-Windows%20|%20Linux%20|%20Android-purple) ![Game](https://img.shields.io/badge/Game%20🎮-Wispy-green) ![Version](https://shields.io/github/v/release/julesgrc0/wispy?label=Version%20🧮&color=lightblue) [![Release Build](https://github.com/julesgrc0/wispy/actions/workflows/windows.yml/badge.svg)](https://github.com/julesgrc0/wispy/actions/workflows/windows.yml)  [![Release Build](https://github.com/julesgrc0/wispy/actions/workflows/linux.yml/badge.svg)](https://github.com/julesgrc0/wispy/actions/workflows/linux.yml) [![Release Build](https://github.com/julesgrc0/wispy/actions/workflows/android.yml/badge.svg)](https://github.com/julesgrc0/wispy/actions/workflows/android.yml)


**Concept**: Wipsy is a kind of 2D Minecraft.

## 🚀 Project Insight

### 📚 Libraries

- zlib   
- raylib 
- pthread (linux)

### 🛠 Building 

**⚙ Requirements**

- [python](https://www.python.org/downloads/)
- [cmake](https://cmake.org/)
- [upx](https://github.com/upx/upx) (Optional)

**📦 Packing Assets**

```bash
python ./tools/pack_assets.py
```

**🌐 Clone the repository**

```bash
git clone --recurse-submodules https://github.com/julesgrc0/wispy.git
```

**💻 Building for Windows**

> Command Line

```bash
cmake -S . -B out --preset=<x64|x86>-<debug|release|relminsize>-<windows|linux|android>
cmake --build out --config <debug|release>
```

> VSCode

Run the build task with the following shortcut:

<kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd>

Then select your target build configuration :

- `Debug` 
- `Release`
- `RelMinSize` (Compressed with UPX)

- `x64` (64 bits)
- `x86` (32 bits)

- `windows`
- `linux` 
- `android` (Not implemented yet)

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

**🚧 Building for Android**

> TODO

## 🤝 Get Involved

Wellcome to the community ! We are happy to see you here. If you want to [contribute](CONTRIBUTING.md) to the project, you can start by forking the repository and then submit a pull request 🔥 .

## 📃 License

This project is licensed under the terms of the [MIT license](LICENSE).
