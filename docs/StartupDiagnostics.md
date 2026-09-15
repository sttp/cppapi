# Diagnosing connection-to-first-data delays

## Branch scope

This diagnostics branch is based on production commit 5a5edac. Performance and int32 fixes, together with samples/ConfigurationFramesTest, are already in main. This branch adds opt-in tracing, StartupTiming, MetadataBenchmark, and investigation notes. Measurements below were collected during the original investigation unless explicitly marked as branch validation.


## Findings from source inspection

The default SubscriberInstance path is sequential:

connection ready -> request metadata -> receive entire response -> callback queue -> decompress -> parse XML -> construct device/measurement/phasor objects -> construct configuration frames -> install maps -> ParsedMetadata callback -> destroy temporary metadata structures -> send Subscribe -> publisher processes subscription -> receive/decode signal index cache -> receive/decode data -> measurement callback.

`SubscriberInstance::HandleMetadata` waits for `ReceivedMetadata` to return before calling `Subscribe`. The subscription filter does not limit the metadata request: that uses the separate `MetadataFilters` setting. A filter excluding STAT therefore still downloads/processes the default metadata set.

### Findings in the original implementation

1. **Configuration-frame construction.** `ConstructConfigurationFrames` calls `TryFindMeasurement` (a linear scan of the device's measurement vector) for every analog/digital index. `GetSignalKindCount` actually returns the maximum signal-reference index, not the number of records. Cost grows roughly with `measurements-per-device * (maximum-analog-index + maximum-digital-index)`. Dense 50,000 analogs on one device can mean roughly 1.25 billion comparisons. Sparse high indexes also generate placeholder records. The phasor configuration loop repeatedly scans the phasor list. This is per-device scaling, not automatically quadratic in the total metadata count across small devices.
2. **Phasor association.** For each phasor, ReceivedMetadata scans the associated device's measurements to locate angle/magnitude. Approximately `sum(phasors-per-device * measurements-per-device)` in the worst case.
3. **Metadata materialization and memory pressure.** Every record creates objects/strings and parses UpdatedOn using string processing and locale-based streams. The metadata maps are copied into member maps; temporary maps, XML storage, and the expanded payload are then destroyed before Subscribe. Raw XML parsing is only one part of this work.
4. **Transfer/decompression and signal cache.** Metadata and cache decompression use CopyStream, which appends bytes one at a time to a vector. Cache Decode performs per-record allocations/hash insertions, with no pre-reservation. These deserve measurement, but do not show the repeated whole-device scans above.

The client Subscribe method builds and sends a small connection string containing the filter; it does not expand the filter to every measurement locally. The publisher performs selection and subscription setup. A long gap after sending Subscribe and before receiving its response/cache points toward publisher work or transfer, rather than this client's filter evaluation. The C++ publisher code does not establish what openHistorian's publisher spends time doing.

**Fixed:** configuration-frame construction now builds per-device index lookups instead of repeatedly scanning each device. Metadata indexes and publisher mappings use int32_t; frame loops use int64_t counters to avoid wraparound. See [MetadataScaling.md](MetadataScaling.md) for the local reproduction, measurements, and boundary tests. Phasor association remains a separate potential hotspot.

## Opt-in instrumentation

Rebuild the native C++ library/client with the modified sources, then launch from PowerShell:

```powershell
$env:STTP_STARTUP_TRACE = '1'
# Run your rebuilt native client with its normal IP/port arguments, redirecting stderr:
# .\YourClient.exe 127.0.0.1 7175 2> startup-trace.log
```

The metadata field changes alter the native ABI. Update the three corresponding SWIG field declarations to int32_t and regenerate the bindings before rebuilding the wrapper. For the .NET wrapper, rebuild/relink `sttp.net.lib.dll` against these instrumented sources and redeploy it before testing ConnectionTest. Merely republishing the .NET executable does not incorporate these C++ changes. The old prebuilt native DLL will not emit these diagnostics.

Unset STTP_STARTUP_TRACE or set it to 0 to disable tracing. The setting is read once per process. Events go directly to stderr and are flushed; they are independent of StatusMessage overrides and the callback queue. No measurement values or metadata content are logged. Before constructing configuration frames, a diagnostic-only scan reports maximum per-device measurement/phasor counts and maximum analog/digital/phasor reference indexes. Its duration is reported separately.

Log format contains monotonic milliseconds since the first trace event, native DataSubscriber pointer (to separate connections), stage duration, and item/byte count where applicable. Scope labels appear on entry and exit; marks report elapsed time since the previous mark. Logs can interleave across threads, so compare timestamps and subscriber pointers. Diagnostic I/O adds some overhead; redirect to a local file.

| Compare stages | What the interval covers |
|---|---|
| metadata request sending -> metadata response received | Request sending, publisher preparation, response transfer |
| metadata response received -> response queued -> metadata callback scope entry | Payload copying and callback scheduling; queue and callback may overlap |
| metadata decompress/copy complete | Gzip expansion or uncompressed payload copy |
| XML parse complete | pugixml load_buffer_inplace |
| device / measurement objects complete | Record conversion, timestamps, strings and map insertion |
| phasor objects and measurement matching complete | Phasor record conversion and association scans |
| configuration frames complete | Indexed lookups and placeholder construction |
| metadata maps installed | Configuration lock, map copying/replacement |
| ParsedMetadata callback complete -> metadata processing scope exit | Temporary metadata/XML/buffer destruction |
| subscribe command built -> subscribe send returned | Local command construction/send; not publisher completion |
| subscribe send -> acknowledged / signal cache scope entry | Publisher processing and transfer; response ordering may vary |
| cache decompress/copy -> cache decode complete | Cache expansion and decoding |
| first nonempty packet received -> first packet decoded | First packet's decode before measurement callback |

The first-packet marker uses the existing per-subscription receive counter. It is a packet diagnostic, not a guarantee of successful measurements if the packet cannot be decoded or its cache is absent.

## Quick comparison test

Use the same endpoint/filter with one counting subscriber, once with default metadata parsing and once with:

```cpp
subscriber.SetAutoParseMetadata(false); // before Connect/ConnectAsync
```

This skips both metadata retrieval and metadata construction and immediately subscribes with the configured filter. Signal-index-cache decoding is still required. It is suitable for the counting test; code that depends on parsed device/configuration metadata will lose that metadata. A large improvement isolates the metadata path, but does not distinguish server metadata preparation/transfer from client parsing; use the stage timings for that distinction.

Then compare the same subscriber with a small filter versus the full filter while metadata is disabled to assess publisher subscription/cache scaling. Keep compression, endpoint and build configuration fixed. Use Release builds for representative performance and record metadata size, per-device point counts and maximum analog/digital/phasor indexes.

## Native build and measured results

The Windows x64 Release diagnostic client is now available at `build/startup-diagnostics/StartupTiming.exe`. Build and run instructions are in `src/samples/StartupTiming/README.md`. Its `--no-metadata` option runs the comparison above without changing the filter. The native library and client were successfully compiled using MSVC v145 and Boost 1.92.

Required compatibility changes: use Boost's built-in UUID std::hash from 1.86 onward, use modern Asio resolver/address APIs with explicit Boost version checks, and explicitly include chrono in ANTLR's profiling source. The configuration-frame optimization is described in MetadataScaling.md. The pre-1.66 Boost branches have not been compiled in this environment.

Initial five-second tests against openHistorian at 127.0.0.1:7175:

| Mode | Connection to first measurement | Received | Exit code |
|---|---:|---:|---:|
| Metadata enabled | 241.080 ms | 19,584 | 0 |
| Metadata disabled | 35.175 ms | 20,402 | 0 |

Both streamed approximately 4,100 measurements/sec. These are individual diagnostic runs, not benchmark averages. Raw traces and console output are in `build/startup-diagnostics/metadata.log`, `metadata-output.log`, `no-metadata.log`, and `no-metadata-output.log`.

For the first metadata response (50,802 compressed bytes, 448,356 expanded bytes, 747 measurement records across 5 devices), the timings were approximately:

- Metadata request to complete response: 116 ms.
- Decompression/copy: 14.2 ms.
- XML parsing: 0.8 ms.
- Measurement-object construction: 15.6 ms.
- Phasor conversion/matching: 3.6 ms.
- Configuration frames: 0.23 ms.
- Signal cache decompression/decoding: approximately 1 ms combined.

This small metadata set does not reproduce the large-device configuration scaling hypothesis. Its maximum per-device measurement count was 169, with maximum analog/digital/phasor indexes of 14/2/59. A trace from the slow client is still needed to locate the 2.5-minute delay.

### Additional observed defect: duplicate startup requests

The trace shows two `connection ready` events, two metadata requests, and two metadata responses/subscriptions for one connection. With metadata disabled it shows two immediate Subscribe commands. Source inspection confirms that `SubscriberInstance::Connect()` calls `ConnectionEstablished()` and `HandleConnect()`, and the registered `HandleConnectionEstablished()` callback also calls them. This duplicates startup work and can trigger a resubscription while the first stream is starting.

The diagnostic patch deliberately retains this behavior so it can be measured. This finding is specific to the current C++ checkout; it does not prove that the older prebuilt SWIG DLL has the same behavior. The diagnostic client retains the earliest connection callback timestamp so duplicate notifications do not reset its timer.




## Diagnostics branch validation

Rebuilt on codex/startup-diagnostics, based on production commit 5a5edac. With tracing enabled, the local openHistorian check at 127.0.0.1:7175 received first data in 171.366 ms and 19,856 measurements over the five-second run (roughly 4,100/sec). Output and trace: build/review-stages/04-live.out and 04-live.log. Timings are individual runs, not benchmark averages.
