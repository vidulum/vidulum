Vidulum 2.1.0
=============

What is Vidulum (VDL)?
--------------
- One minute block time
- 10 VDL total per block
- 3.2 VDL for masternodes per block
- 4.8 VDL for proof of work miners per block
- 2.0 VDL for Vidulum Reward System V-Staking
- 1440 Estimated blocks per day
- Trading on STEX and CREX24


Security Warnings
-----------------

**Vidulum is experimental and a work-in-progress.** Use at your own risk.

Deprecation Policy
------------------

This release is considered deprecated 16 weeks after the release day. There
is an automatic deprecation shutdown feature which will halt the node some
time after this 16 week time period. The automatic feature is based on block
height and can be explicitly disabled.

Building
-----------------

### Install dependencies

On Ubuntu/Debian-based systems:

```
$ sudo apt-get install \
      build-essential pkg-config libc6-dev m4 g++-multilib \
      autoconf libtool ncurses-dev unzip git python python-zmq \
      zlib1g-dev wget bsdmainutils automake curl
```

On Fedora-based systems:

```
$ sudo dnf install \
      git pkgconfig automake autoconf ncurses-devel python \
      python-zmq wget gtest-devel gcc gcc-c++ libtool patch curl
```

Windows:
```
sudo apt-get install \
    build-essential pkg-config libc6-dev m4 g++-multilib \
    autoconf libtool ncurses-dev unzip git python \
    zlib1g-dev wget bsdmainutils automake mingw-w64
```

On Mac systems:

```
brew tap discoteq/discoteq; brew install flock
brew install autoconf autogen automake
brew install gcc5
brew install binutils
brew install protobuf
brew install coreutils
brew install wget llvm
```

### Check GCC version

gcc/g++ 4.9 or later is required. Vidulum has been successfully built using gcc/g++ versions 4.9 to 7.x inclusive. Use ```g++ --version``` to check which version you have.

On Ubuntu Trusty, if your version is too old then you can install gcc/g++ 4.9 as follows:

```
$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test
$ sudo apt-get update
$ sudo apt-get install g++-4.9
```

### Fetch the software and parameter files

Fetch our repository with git and run ```fetch-params.sh``` like so:
```
$ ./zcutil/fetch-params.sh
```

### Build Linux/MAC

Ensure you have successfully installed all system package dependencies as described above. Then run the build, e.g.:
```
$ git clone https://github.com/vidulum/vidulum.git
$ cd vidulum/
$ ./zcutil/build.sh
```

This should compile our dependencies and build vidulumd

### Build Windows

Ensure you have successfully installed all system package dependencies as described above. Then run the build, e.g.:
```
$ git clone https://github.com/vidulum/vidulum.git
$ cd vidulum/
HOST=x86_64-w64-mingw32 ./zcutil/build.sh
```

--------
### Need Help?

Join our Discord: https://discord.gg/bF2P5bV

Participation in the Vidulum project is subject to a
[Code of Conduct](code_of_conduct.md).

License
-------

For license information see the file [COPYING](COPYING).
