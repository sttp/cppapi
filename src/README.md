# Implementation of the Streaming Telemetry Transport Protocol (STTP) in C++.

Code includes STTP functionality for both "subscribers" and "publishers".

## Compiling in Visual Studio

To properly compile in Visual Studio, you will need to download Boost:
    http://www.boost.org/users/download/

By default, the C++ STTP API project configuration adds an additional include
directory for the Boost libraries in a parallel location to the C++ STTP API
project in a folder called _boost_ regardless of version, for example:

STTP API project files:
```
    C:\projects\sttp\cppapi
                   \src
                   \build
                   etc...
```
Boost library files:
```
    C:\projects\sttp\boost
                     \boost
                     \libs
                     etc...
```

If you have an existing Boost installation you can simply create a symbolic
link to the folder, e.g., `mklink /D C:\projects\sttp\boost C:\boost_1_66_0`

Alternately you can adjust the additional include directories to your own
Boost installation location for each of the build configurations. The code
has been tested with version v1.66 of Boost.

Note that you will need to compile Boost in order to execute the sample
applications found in:
```
    sttp\cppapi\src\samples
```

The STTP API library uses zlib features of Boost, as a result compiling boost
requires zlib source code that can be downloaded separately: https://zlib.net/

After unzipping the zlib source code, set the following Boost compile script
environmental variables to the root of the zlib source code path, e.g.:
```
    set ZLIB_SOURCE="C:\zlib-1.2.11"
    set ZLIB_INCLUDE="C:\zlib-1.2.11"
```

Once environmental variables are set for zlib paths, compile Boost as normal.

## Compiling in Linux

The following information is intended to help developers build the STTP API
library on Linux platforms. Similar instructions may apply to other platforms.

### Dependencies

The STTP API library depends on the following libraries in order to build.
Earlier versions of the libraries listed may not work properly.

* CMake v2.8 (http://www.cmake.org/)

* GNU Make (http://www.gnu.org/software/make/)

* zlib Library, e.g.: `sudo apt install zlib1g-dev`

* bzip2 Library, e.g.: `sudo apt install libbz2-dev`

* Boost C++ Libraries v1.66.0 (http://www.boost.org/)
    - Boost.Asio
    - Boost.Bind
    - Boost.Iostreams
    - Boost.System
    - Boost.Thread
    - Boost.Uuid

Boost will need to be compiled:
https://www.boost.org/doc/libs/1_66_0/more/getting_started/unix-variants.html

For Ubuntu, here are some common steps (don't type `$`; that represents the shell's prompt):

```
$ sudo apt update
$ sudo apt install build-essential
$ sudo apt install cmake

$ sudo apt install zlib1g-dev
$ sudo apt install libbz2-dev

$ sudo mkdir /usr/local/boost_1_66_0
$ cd /usr/local/
$ wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.bz2
$ sudo tar -xvjf boost_1_66_0.tar.bz2
```

Start a new terminal session before building Boost:

```
$ cd /usr/local/boost_1_66_0
$ sudo ./bootstrap.sh
$ sudo ./b2 install
```

It may be necessary to add `/usr/local/lib`, the default path for boost libraries,
to the system library path before running any samples:

```
$ sudo ldconfig /usr/local/lib
```

### Configuration

From the command terminal, enter the source directory containing this
README file and type the following command:

```
    cmake .
```

Alternatively, you can create a build directory separate from the
source code you downloaded. Enter the build directory you created
and type the following command:

```
    cmake path/to/source
```

Using the CMake GUI, you can modify configuration options, such as
building as a shared library or changing the installation directory.

To make a debug build, use the following:

```
    cmake -DCMAKE_BUILD_TYPE=Debug
```

### Build

At the top level of the build directory, type the following command.

```
    make
```

In addition to the library itself, there are sample applications which
demonstrate the proper use of the STTP library API. To build all samples,
type the following command:

```
    make samples
```

Individual sample applications can be built as follows:

```
    make SimpleSubscribe
    make AdvancedSubscribe
    make AverageFrequencyCalculator
    make InstanceSubscribe
    make InstancePublish
    make DynamicMetadataPublish
    make FilterExpressionTests
    make SimplePublish
    make AdvancedPublish
```

### Installation

At the top level of the build directory, type the following command.

```
    make install
```

This will move the header files and the library file to the location
specified during configuration. Header files go under the 'include/'
subdirectory, and the library file goes under the 'lib/' subdirectory.