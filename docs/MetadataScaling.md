# Metadata scaling and 32-bit indexes

## Local reproduction

Windows x64 Release, MSVC v145, Boost 1.92. The benchmark generates deterministic metadata and passes it through the actual SubscriberInstance metadata parser and configuration builder. Its timer excludes XML generation, compression, network transfer, and post-parse validation.

480,000 analog measurements concentrated on eight devices (60,000 each), 185,450,687 XML bytes:

| Stage | Original implementation | Indexed frame construction |
|---|---:|---:|
| Complete metadata processing | 107.081 s | 10.947 s |
| Configuration frames | 92.934 s | 0.348 s |
| XML parsing | 0.504 s | 0.369 s |

A final run after widening metadata indexes completed in **9.783 s**, with all output validation passing. These are individual runs and vary with machine load. Three alternating 50,000-point comparisons gave median totals of 5.479 s original and 1.211 s optimized.

This reproduces a substantial client-side delay locally and identifies configuration construction as the dominant cost for this shape. It does not establish the cause of the client's reported 2.5-minute connection delay: their metadata distribution, server work, transfer, and hardware remain unmeasured.

## Fix

ConstructConfigurationFrames previously scanned a device's measurements for every analog/digital index and scanned its phasors for every phasor index. It now builds lookup vectors once per device. Ordering, missing-index placeholders, and first-match handling of duplicate indexes are preserved.

SignalReference.Index, MeasurementMetadata.PhasorSourceIndex, and PhasorMetadata.SourceIndex now use signed int32_t. Parsing, helper signatures, publisher phasor maps, and metadata filtering preserve these values. Frame loops use int64_t counters so neither 65,535 nor INT32_MAX causes counter wraparound. The aggregate phasor count uses size_t.

Runtime signal-cache and compact-measurement indexes were already int32_t; their wire layout is unchanged. Cache comments have been corrected accordingly.

## Validation

The native benchmark regression mode checks:

- Dense and sparse analog, digital, and phasor metadata, including gzip input.
- Indexes 65,535, 65,536, and 100,000 for each kind; placeholder positions and signal identities.
- Duplicate indexes retain the original first-match behavior.
- A complete 70,000-measurement metadata set.
- A 70,001-entry wire cache and compact-measurement round trips, including INT32_MAX.
- Publisher metadata round trips for indexes 34,464 and 100,000, which collided when narrowed to 16 bits.
- TSSC compressed round trips through indexes 65,535, 65,536, and 100,000.

Build and execution instructions: [MetadataBenchmark](../src/samples/MetadataBenchmark/README.md).

Local evidence is under build/startup-diagnostics (ignored build artifacts): baseline-summary.txt, optimized-summary.txt, massive-baseline.out/.log, massive-optimized.out/.log, massive-int32.out/.log, repeat-baseline-*.out/.log, repeat-optimized-*.out/.log, and int32-regressions.out/.log. MetadataBenchmark-baseline.exe preserves the original frame algorithm for local comparisons.

## Limits and integration

- Configuration frames still allocate placeholders through the maximum index. A sparse enormous index can exhaust memory; signed 32-bit fields do not make billions of frame slots practical. INT32_MAX was tested in parsing, cache, and compact records, not by allocating that many configuration slots or TSSC states.
- Per-phasor angle/magnitude association still scans device measurements and can become costly on a device with many phasors. This optimization addresses configuration-frame construction.
- The separately observed duplicate connection initialization remains unchanged; see StartupDiagnostics.md.
- These changes alter the public native metadata ABI. Rebuild all native consumers. Before using this library with net-cppapi, update its three SWIG field declarations, regenerate bindings, and rebuild both managed and native wrapper components together. Existing .NET binaries have not been replaced.
- Boost compatibility uses explicit version checks; pre-1.66 branches were not compiled in this environment.

## Final native live check

After the performance and int32 changes, the rebuilt StartupTiming.exe connected to openHistorian at 127.0.0.1:7175 with metadata enabled. First measurement arrived 143.550 ms after connection; the five-second run received 19,860 measurements at approximately 4,100/sec and exited successfully. Evidence: build/startup-diagnostics/final-live.out and final-live.log. This checks compatibility with the local publisher, whose small metadata set does not exercise the large-index cases covered by the synthetic regressions.

Production correctness tests are also available in [ConfigurationFramesTest](../src/samples/ConfigurationFramesTest/README.md), independently of this diagnostics branch.

