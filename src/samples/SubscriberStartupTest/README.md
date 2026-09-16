# SubscriberStartupTest

A local loopback publisher checks that each connection produces one connection event and one metadata request/subscription sequence. No external STTP publisher is required.

Coverage includes outbound initial connection and automatic reconnect, reverse connections, protocol versions 2 and 3, metadata parsing enabled/disabled, delayed acceptance, rejection, and handshake timeout.

Build with the included Visual Studio project or build.cmd on Windows. With CMake, build target SubscriberStartupTest and run Samples/SubscriberStartupTest. A successful run prints twelve PASS lines and exits with code 0.
