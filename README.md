# 🎮 Wispy-C

![C](https://img.shields.io/badge/dev-pure%20c-ccc)
![Language](https://img.shields.io/badge/lang-french%20🌍-blue)

<!-- <a href="https://discord.gg/QjWsuaM3aB">
<img src="https://img.shields.io/discord/1161296442577653802?logo=discord" alt="discord">
</a> -->

[![Release Build](https://github.com/julesgrc0/wispy-c/actions/workflows/msbuild.yml/badge.svg)](https://github.com/julesgrc0/wispy-c/actions/workflows/msbuild.yml)
[![raylib](https://img.shields.io/badge/lib-raylib%20🎮-purple)]()
[![zlib](https://img.shields.io/badge/lib-zlib%20📦-fff)]()

**Concept**: Wispy-C this is a version of wispy initially developed in java. Wipsy is a kind of 2D Minecraft.

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
wispy-c.sln
```

**🚧 Building for Linux**

> TODO

## 🤝 Get Involved

**Fork First**:
Want to be part of this fun journey? Join Aywen's [Discord server](https://discord.gg/QjWsuaM3aB)! Look for the "AYWEN-LOGICIELS" category and the "#vos-projets" and then search for **Wispy-C**

## 📃 License

This project is licensed under the terms of the [MIT license](LICENSE).
