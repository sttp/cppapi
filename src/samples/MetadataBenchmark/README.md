# Native metadata benchmark

Build from the C++ repository root:

```powershell
.\src\samples\MetadataBenchmark\build.cmd
.\build\startup-diagnostics\MetadataBenchmark.exe --regression
```

Uses the same Visual Studio / Boost setup as ../StartupTiming/build.cmd. Output is build/startup-diagnostics/MetadataBenchmark.exe. Tests return nonzero on failure.

Generate and validate metadata without a publisher:

```powershell
$env:STTP_STARTUP_TRACE = '1'
.\build\startup-diagnostics\MetadataBenchmark.exe analog 8 60000 1
.\build\startup-diagnostics\MetadataBenchmark.exe phasor 1 5000 1 gzip
.\src\samples\MetadataBenchmark\run-suite.ps1 -Label current
```

Arguments: analog|digital|phasor DEVICES POINTS_PER_DEVICE [STRIDE=1] [gzip]. A phasor point creates two measurement records. Stride produces sparse indexes and tests placeholders. The harness caps input at 500,000 measurements and 1,000,000 configuration slots to bound memory usage; these are benchmark safeguards, not protocol limits.

RESULT rows contain kind, devices, points per device, stride, gzip flag, XML bytes, payload bytes, processing milliseconds, and PASS. Timing includes the real metadata-processing call and its local cleanup, but excludes generation, compression, validation, and network work. Large cases can use over 1 GB of memory. Run comparisons sequentially in Release builds on an otherwise quiet machine.

The suite writes separate output/trace files per case under build/startup-diagnostics/LABEL, imposes a per-case timeout (default 180 seconds), and fails if any case fails or times out. Use -Executable to compare a separately preserved baseline binary. Do not run old binaries on index 65,535: the original frame loop can wrap indefinitely.

Regression mode verifies dense/sparse/duplicate metadata, 65K boundaries, 70,000 measurements, publisher index collisions, signal-cache and compact wire decoding, and TSSC wide-index round trips. See ../../../docs/MetadataScaling.md for measured results and integration limits.
