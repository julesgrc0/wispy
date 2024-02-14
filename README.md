# 🎮 Wispy

![C](https://img.shields.io/github/languages/top/julesgrc0/wispy?label=C%20🔥) ![Platform](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20🌐-purple) ![Game](https://img.shields.io/badge/Game-Wispy%20🎮-green) ![Version](https://shields.io/github/v/release/julesgrc0/wispy?label=Version&color=lightblue) [![Release Build](https://github.com/julesgrc0/wispy/actions/workflows/release.yml/badge.svg)](https://github.com/julesgrc0/wispy/actions/workflows/release.yml)


**Concept**: Wipsy is a kind of 2D Minecraft.

## 🚀 Project Insight

### 📚 Libraries

- zlib   
- raylib 
- pthread (linux)

### 🛠 Building 

**⚙ Requirements**

- [python](https://www.python.org/downloads/)
- [vcpkg](https://github.com/microsoft/vcpkg.git)
- [msbuild](https://visualstudio.microsoft.com/fr/downloads/) (Windows)
- [upx](https://github.com/upx/upx) (Optional)

**📦 Packing Assets**

```bash
python ./tools/pack_assets.py
```

**💻 Building for Windows**

> With VSCode

On your keyboard type this shortcut to run the build command:

<kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd>

Then enter the path to msbuild and select the desired configuration and platform from this one:

- Debug/Release/Release Light Size
- x64/x86

> With Visual Studio

Open the solution and select your target build configuration.

```
wispy.sln
```

**🚧 Building for Linux**

> TODO

## 🤝 Get Involved

Wellcome to the community ! We are happy to see you here. If you want to [contribute](CONTRIBUTING.md) to the project, you can start by forking the repository and then submit a pull request 🔥 .

## 📃 License

This project is licensed under the terms of the [MIT license](LICENSE).
