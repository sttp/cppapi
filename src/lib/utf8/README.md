# Vendored UTF-CPP

Upstream: https://github.com/nemtrif/utfcpp
Release: v4.2.0 (https://github.com/nemtrif/utfcpp/releases/tag/v4.2.0)

The adjacent ../utf8.h and all *.h files in this directory are unmodified copies of the release's source/ tree. LICENSE is copied from the same release. No compiled library is required.

To update, replace those headers from a tagged release, retain the license, and update the header lists in ../sttp.cpp.vcxproj, ../sttp.cpp.vcxproj.filters, and ../../CMakeLists.txt if upstream adds files.

## Integration validation

Validated on Windows x64 with MSVC v145: STTP Release build and ConfigurationFramesTest passed. Upstream negative-input, general API, C++11, C++17, and C++20 tests passed with /Zc:__cplusplus; the upstream no-exceptions target was not run. CMake generation was checked to include/copy both new headers. Full Linux compilation was not performed.

The update also corrects STTP's Convert.cpp adapter: 16-bit wchar_t uses UTF-16 and 32-bit wchar_t uses UTF-32. A supplementary-character round trip failed with the old Windows adapter and passed after the correction. Additional checks covered empty/ASCII/BMP strings, embedded NULs, exact surrogate-pair output, malformed UTF-8, and an unpaired surrogate. Validation sources and logs are in the local ignored build/utfcpp-update directory.
