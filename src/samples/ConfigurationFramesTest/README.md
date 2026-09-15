# Configuration-frame regression tests

From the repository root:

```powershell
.\src\samples\ConfigurationFramesTest\build.cmd
.\build\tests\ConfigurationFramesTest\ConfigurationFramesTest.exe
```

Requires the installed Visual Studio C++ tools and compiled Boost in the sibling boost directory. The build defaults to toolset v145 (override STTP_PLATFORM_TOOLSET if needed). The test builds against the actual native library and parses synthetic metadata without a network connection.

Checks dense and sparse analog/digital/phasor configurations, gzip input, ordering, signal identities, missing-index placeholders, first-match behavior for duplicate indexes, and termination at index 65,535. Returns nonzero on failure. No startup tracing or performance timing is included.

This change preserves the existing uint16_t metadata ABI. Its uint32_t frame loop counters prevent wraparound at 65,535; widening the public metadata indexes is a separate change.

## Visual Studio

Add ConfigurationFramesTest.vcxproj to src/sttp.cpp.sln, then set ConfigurationFramesTest as the startup project. Select Debug or Release and the desired platform. The project references sttp.cpp so Visual Studio builds the matching native library first. It uses v145 and the same Boost and output paths as InteropTest.

Run with Ctrl+F5 to keep the console open after the tests finish. Visual Studio outputs the executable to build/output/PLATFORM/CONFIGURATION/samples/ConfigurationFramesTest.exe. The standalone build.cmd uses build/tests/ConfigurationFramesTest instead.
