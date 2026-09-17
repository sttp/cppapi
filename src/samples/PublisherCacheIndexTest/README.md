# PublisherCacheIndexTest

Regression test for the `DataPacketFlags::CacheIndex` flag on data packets emitted by the publisher.

For STTP v2 and later the publisher alternates signal index cache indexes: the first cache of a connection is index 0, the cache sent for the next subscription is index 1, then 0 again. Subscribers select the cache used to decode a packet from the `CacheIndex` flag in the data packet header, so a publisher that never sets it leaves subscribers decoding run-time IDs with the previous subscription's cache once the active index reaches 1. Run-time IDs are dense and start at zero in every cache, so nothing fails loudly -- measurements are simply attributed to the wrong signal IDs.

The test starts a `PublisherInstance` with two disjoint sets of signals on a loopback ephemeral port, subscribes with a filter selecting set A, resubscribes with a filter selecting set B (making cache index 1 active), then resubscribes to set A again (back to index 0). Every signal publishes a value that identifies itself, so a packet decoded against the wrong cache is caught as a signal ID whose value does not match. Both payload formats are covered by running the whole sequence with `SetPayloadDataCompressed` false (compact) and true (TSSC). No external STTP publisher is required.

Build with the included Visual Studio project or build.cmd on Windows. With CMake, build target PublisherCacheIndexTest and run Samples/PublisherCacheIndexTest. A successful run prints two PASS lines and exits with code 0.
