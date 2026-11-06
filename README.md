# Chess

## Build on Windows

Install Visual Studio 2022 with the **Desktop development with C++** workload and install [vcpkg](https://learn.microsoft.com/vcpkg/). Run `vcpkg integrate install` once so MSBuild can restore the dependencies declared in `vcpkg.json`.

Open `Chess.sln`, select **x64** and **Release**, then build. In VS Code, use **Terminal: Run Build Task** (Ctrl+Shift+B) to run the configured Visual Studio MSBuild task. The project also supports Win32. MSBuild uses the matching vcpkg triplet to install GLFW, GLEW, and GLM; the shader files and textures are copied beside the executable.

Do not use `dotnet build` for this Visual C++ solution. It does not provide the Visual C++ MSBuild targets required by `.vcxproj` projects and produces `MSB4278` when those targets cannot be found.

To build from a Visual Studio Developer PowerShell after integrating vcpkg:

```powershell
msbuild Chess.sln -m -p:Configuration=Release -p:Platform=x64 -verbosity:minimal
```

<img width="796" height="597" alt="image" src="https://github.com/user-attachments/assets/403675ee-8747-4700-a564-32233a091d56" />
