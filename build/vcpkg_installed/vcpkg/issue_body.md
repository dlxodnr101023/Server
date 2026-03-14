Package: libpq[bonjour,core,lz4,openssl,zlib]:arm64-osx@16.9#3

**Host Environment**

- Host: arm64-osx
- Compiler: AppleClang 17.0.0.17000604
- CMake Version: 4.2.3
-    vcpkg-tool version: 2025-12-16-44bb3ce006467fc13ba37ca099f64077b8bbf84d
    vcpkg-scripts version: 05442024c3 2026-02-20 (7 days ago)

**To Reproduce**

`vcpkg install `

**Failure logs**

```
Downloading postgresql-16.9.tar.bz2, trying https://ftp.postgresql.org/pub/source/v16.9/postgresql-16.9.tar.bz2
Successfully downloaded postgresql-16.9.tar.bz2
-- Extracting source /Users/taeuk/vcpkg/downloads/postgresql-16.9.tar.bz2
-- Applying patch unix/installdirs.patch
-- Applying patch unix/fix-configure.patch
-- Applying patch unix/single-linkage.patch
-- Applying patch unix/no-server-tools.patch
-- Applying patch unix/mingw-install.patch
-- Applying patch unix/mingw-static-importlib-fix.patch
-- Applying patch unix/python.patch
-- Applying patch windows/macro-def.patch
-- Applying patch windows/win_bison_flex.patch
-- Applying patch windows/msbuild.patch
-- Applying patch windows/spin_delay.patch
-- Applying patch windows/tcl-9.0-alpha.patch
-- Applying patch android/unversioned_so.patch
-- Using source at /Users/taeuk/vcpkg/buildtrees/libpq/src/tgresql-16-09acf38b01.clean
-- Getting CMake variables for arm64-osx
-- Loading CMake variables from /Users/taeuk/vcpkg/buildtrees/libpq/cmake-get-vars_C_CXX-arm64-osx.cmake.log
-- Getting CMake variables for arm64-osx-dbg
-- Getting CMake variables for arm64-osx-rel
CMake Error at scripts/cmake/vcpkg_configure_make.cmake:721 (message):
  libpq requires autoconf from the system package manager (example: "sudo
  apt-get install autoconf")
Call Stack (most recent call first):
  ports/libpq/portfile.cmake:110 (vcpkg_configure_make)
  scripts/ports.cmake:206 (include)



```

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "name": "servers",
  "version-string": "1.0.0",
  "dependencies": [
    "fmt",
    "boost-asio",
    "openssl",
    "spdlog",
    "protobuf",
    "libpqxx",
    "argon2"
  ]
}

```
</details>
