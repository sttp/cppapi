# Subscriber shutdown regression

Requires a running STTP publisher that provides metadata and measurements. The default port is 7165, with ten iterations per run.

```bash
cmake -S src -B ~/sttp
cmake --build ~/sttp --target SubscriberShutdownTest -j6
~/sttp/Samples/SubscriberShutdownTest RITCHIE-LAPTOP.local 7175 10 data
~/sttp/Samples/SubscriberShutdownTest RITCHIE-LAPTOP.local 7175 10 metadata
```

On Windows, build `SubscriberShutdownTest.vcxproj` from Visual Studio or use `build.cmd` from a developer terminal, then run the executable with `127.0.0.1 7175 10 data` or `127.0.0.1 7175 10 metadata`.

- `data`: disconnect and destroy the subscriber immediately after its first measurement batch.
- `metadata`: hold a metadata callback until 100 ms after the owner starts disconnecting and destroying the subscriber. The callback then finishes parsing metadata while shutdown waits. This makes the original lifetime failure reproducible without requiring a large metadata set.

The test's synchronization objects live outside the subscriber and remain alive through destruction. Each successful iteration prints `PASS,<mode>,<iteration>`. An external timeout can also be used to detect a deadlock.

## Defect covered

`SubscriberInstance` declared its `DataSubscriber` pointer first, so C++ destroyed it last. Its destructor started asynchronous disconnect but returned without waiting for callbacks. Metadata maps and their mutex were consequently destroyed before `DataSubscriber` destruction finally joined the callback thread. GDB captured `ReceivedMetadata` locking the already-destroyed `m_configurationUpdateLock`, causing a Boost mutex error and process abort.

The instance destructor now finishes connection/shutdown threads while its members are alive. `DataSubscriber` destruction also joins an existing shutdown thread even if the disconnected flag is set: that flag precedes the worker's final mutex unlock. Public `Disconnect()` remains asynchronous for callback callers. Object destruction must be performed by the owner outside the subscriber's callback threads.

## Validation

The original library aborted in the held-metadata native test with the same destroyed-mutex stack seen in the .NET reproducer. With the fix:

- Windows x64 Release: 10 held-metadata and 10 first-data native cycles passed.
- Ubuntu 22.04 WSL x64 Debug: 10 held-metadata and 10 first-data native cycles passed.
- Immediate .NET/SWIG disposal: 10 Windows runs and 8 Linux runs under GDB exited normally, without a post-data delay.

Wrapper validation used isolated builds linked to the modified canonical C++ source. The net-cppapi submodule and packaged wrapper binaries were left at their existing revision for the normal review/commit/submodule-update workflow. This is targeted coverage of the reproduced lifetime defect, not a proof that all reconnect and shutdown interleavings are race-free.