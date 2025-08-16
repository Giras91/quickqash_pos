# QuickQash POS (Scaffold)

This is a scaffold for QuickQash POS — a Windows-only retail point-of-sale using C++20, Qt6 Widgets, and SQLite.

Quick start (Windows, PowerShell):

1. Install Qt6 (recommended: via the Qt installer or vcpkg)
2. Configure build with CMake and build:

```powershell
Using Visual Studio (recommended):

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
.\build\Debug\QuickQash.exe
```

Using vcpkg toolchain for dependency management:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
```
```

Notes:
- UI files are loaded at runtime from the `ui/` directory using QUiLoader.
- Database default path is `pos.db` in the working directory.
- See `data/schema.sql` for the initial schema.

At startup the application will attempt to open `pos.db` and apply migrations from `data/schema.sql` if the database is empty. Ensure the application has write permission to the working directory.

Next steps (Phase 1 roadmap): implement ProductRepo, ProductDialog, PosView, printer integration, and migrations.

Packaging and CI
----------------

The repository includes a simple CPack configuration that will produce a ZIP package. On CI the package is produced and uploaded as a workflow artifact named `quickqash-package`.

To build, install and create a ZIP locally (PowerShell):

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --install build --config Release --prefix install
pushd build; cpack --config CPackConfig.cmake; popd
```

CI (GitHub Actions) will install Qt via vcpkg, run the unit tests with CTest, then produce a ZIP package and upload it as an artifact.

The CI also builds a Windows installer using Inno Setup and uploads it as an artifact named `quickqash-installer` (look in the Actions run artifacts).
