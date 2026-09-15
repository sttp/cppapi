# Configuration-frame regression tests

From the repository root:

```powershell
.\src\samples\ConfigurationFramesTest\build.cmd
.\build\tests\ConfigurationFramesTest\ConfigurationFramesTest.exe
```

Requires the installed Visual Studio C++ tools and compiled Boost in the sibling boost directory. The build defaults to toolset v145 (override STTP_PLATFORM_TOOLSET if needed). The test builds against the actual native library and parses synthetic metadata without a network connection.

Checks dense and sparse analog/digital/phasor configurations, gzip input, ordering, signal identities, missing-index placeholders, first-match behavior for duplicate indexes, and termination at index 65,535. Returns nonzero on failure. No startup tracing or performance timing is included.

Metadata indexes now use int32_t, with int64_t frame loop counters to avoid wraparound. This changes the public native metadata ABI: rebuild native consumers and regenerate the corresponding SWIG declarations/bindings before updating the .NET wrapper.

## Wide-index regressions

The suite additionally checks indexes 65,536 and 100,000 for analog/digital/phasor configurations, 70,000 actual measurement records, a 70,001-entry signal-cache wire payload, publisher metadata indexes that previously collided after 16-bit narrowing, and TSSC compressed round trips above 65,535. Signal-reference parsing and cache/compact-measurement round trips include INT32_MAX.

Runtime cache indexes were already int32_t; this change does not alter their wire format. Configuration frames still create placeholders through the maximum index, so very sparse large indexes require proportional memory. INT32_MAX tests do not allocate that many configuration slots or TSSC states.
