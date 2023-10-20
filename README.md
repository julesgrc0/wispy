# 🎮 Wispy-C

**Concept**: Wispy-C  this is a version of wispy initially developed in java. Wipsy is a kind of 2D minecrat.

## 🚀 Project Insight

### 📚 Libraries

> To simplify the use of third-party libraries we use **vcpkg** in this project.

- zlib
- raylib

### 🌐 Portability

> Wispy was developed for Windows but in the future it will be compatible with Linux too!

**🛠 Requirements**

- [vcpkg](https://github.com/microsoft/vcpkg.git)
- [python](https://www.python.org/downloads/)
- [MSBuild](https://visualstudio.microsoft.com/fr/downloads/)

**📦 Packing Assets**

```bash
python ./tools/pack_assets.py
```

**💻 Building for Windows**

> I use VSCode

<kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd>
```
>task: Build
```

Then enter the path to msbuild and select the desired configuration and platform from this one:

- Debug x64
- Debug x86
- Release x64
- Release x86


> I use Visual Studio (2017/2019/2022)

Open the solution with Visual Studio
```
wispy-c.sln
```

**🚧 Building for Linux**

> TODO

## 🤝 Get Involved
**Fork First**: 
Want to be part of this fun journey? Join Aywen's [Discord server](https://discord.gg/QjWsuaM3aB)! Look for the "AYWEN-LOGICIELS" category and the "#vos-projets" and then search for **Wispy-C**


## 📃 License

This project is under the [GPL-3.0 license](https://choosealicense.com/licenses/gpl-3.0/).