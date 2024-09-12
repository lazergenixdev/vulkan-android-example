# Application Template for Windows and Android

# 🔗 Dependencies
- [Premake](https://premake.github.io/)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) (already included with Android NDK)

# 🛠️ Building

## Windows 🪟

Generate project files
```sh
premake5 vs2022
```

Solution is located in `build/vs2022`, so now open in Visual Studio or build in the commandline with `msbuild`.

## Android 🍭

Generate project files
```sh
premake5 android-studio
```

Project is located in `build/android-studio`, so now open in Android Studio or build in the commandline with `gradle` (not tested).
