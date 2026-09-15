# Native startup timing client (Windows x64)

Uses one subscriber with `FILTER ActiveMeasurements WHERE SignalType <> 'STAT'`.
Reports first-data timing and measurements/sec; any key or Ctrl+C exits. `--seconds N` limits a diagnostic run; exit code 2 means no measurements arrived before exit, 1 means an argument/startup exception, and 0 means measurements were received.

## Build

From the repository root:

```powershell
.\src\samples\StartupTiming\build.cmd
```

Builds the instrumented C++ library and the client in Release with the installed Visual Studio C++ tools. The default toolset is v145; set `STTP_PLATFORM_TOOLSET` to select another installed toolset. Boost headers and compiled libraries must be in the sibling `..\boost` folder as expected by the existing library project. The standalone client link step also accepts `STTP_BOOST_ROOT` to override its Boost lookup.

Output: `build\startup-diagnostics\StartupTiming.exe`.

## Compare startup with and without metadata

```powershell
$env:STTP_STARTUP_TRACE = '1'
.\build\startup-diagnostics\StartupTiming.exe 127.0.0.1 7175 --seconds 10 2> build\startup-diagnostics\metadata.log
.\build\startup-diagnostics\StartupTiming.exe 127.0.0.1 7175 --no-metadata --seconds 10 2> build\startup-diagnostics\no-metadata.log
```

For a slow remote system, omit `--seconds` or use a duration long enough to include the reported 2.5-minute startup (for example 300). IP/hostname is required; port defaults to 7165. `--no-metadata` skips metadata retrieval and configuration construction; the subscription filter remains identical and runtime signal-cache decoding still occurs.

Set `STTP_STARTUP_TRACE=0` or remove the environment variable to turn off native diagnostics. Trace output goes to stderr and rate output to stdout. Detailed stage interpretation is in `docs/StartupDiagnostics.md`.

This is a native executable linked to the C++ static library. It does not update the .NET test executable or its SWIG DLL. The Visual C++ runtime must be available on the target machine.
