# MCStatus

A command-line tool for viewing the current status of a Minecraft server, compatible with Windows and Unix-based systems.

## Building:

Requirements: CMake

```
git clone https://github.com/ReimarPB/MCStatus
git submodule init
mkdir build
cd build
cmake ..
```

## Usage:

```
mcstatus <hostname>
```
Replace `<hostname>` with the server IP or domain name.

The `-e` option lets you select between different editions of Minecraft. Possible values are:
- java (Modern Java Edition servers, 1.7 and above)
- legacy-java (Legacy Java Edition servers, 1.6 and below)
- ~~bedrock~~ (Bedrock Edition - Not implemented yet)

