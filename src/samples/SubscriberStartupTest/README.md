# SubscriberStartupTest

A local loopback publisher checks that each connection produces one connection event and one metadata request/subscription sequence. No external STTP publisher is required.

Coverage includes outbound initial connection and automatic reconnect, reverse connections, protocol versions 2 and 3, metadata parsing enabled/disabled, delayed acceptance, rejection, and handshake timeout.

Each accepted connection also checks that data published before signal index cache confirmation is delivered. A publisher starts publishing with its current cache index as soon as a subscription starts, while the cache sent to the subscriber is tagged with the next index and only becomes active once the publisher processes the confirmation. Data in this window uses the same signal mappings, so the test publishes one measurement flagged with the alternate cache index before reading the confirmation and requires that it arrives with the correct signal ID. Dropping this data delays time to first measurement by however long the window lasts, e.g., while a publisher establishes a large subscription, or until the next update when initial data is cached values for slowly updating signals.

Build with the included Visual Studio project or build.cmd on Windows. With CMake, build target SubscriberStartupTest and run Samples/SubscriberStartupTest. A successful run prints twelve PASS lines and exits with code 0.
