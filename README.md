# Gex 2 PS1 Sound Exporter
This is a simple command line application written in C++ that exports Gex 2 PS1 sound files to .flac sound files.

## Usage
Currently the program only takes in the input file name and the output folder name.

The 1 needed file is the **input file**, the _.smp_ file from Gex 2.

The 1 additional flag is the **output folder**, specified by _-o_ or _--out_. This is the folder where the sounds will be output.

Usage on the command line is as follows:
```
> gex2ps1soundexporter file [-o --out folder]
```

## Getting the Sound Files
To get the sound files you will need:
* A dumped ROM of Gex: Enter the Gecko on PS1
* A program to open .bin files such as [PowerISO](https://www.poweriso.com)
* [Soul Spiral](https://github.com/TheSerioliOfNosgoth/SoulSpiral-Official)

First, open the Gex 2 .bin file using PowerISO. Extract the file called _BIGFILE.DAT_.

Open Soul Spiral and click on the button "Open a BigFile". Navigate to where _BIGFILE.DAT_ is and select it. Use the automatic settings. Export the SMP files that you want.

## Compilation & Building
Currently the program is built only for Linux, but I plan to port it to other systems.

Same sort of stuff as the model exporter. This process assumes you have a Linux distribution installed and are familiar with its package manager. If you are uncomfortable with your system's package manager and instead use something like Snap or Flatpak, I cannot help you here.

### Install the Needed Packages
This program requires CMake to build. Install the cmake package on your distribution with its package manager.

To retrieve the source code, you will need git. Install the git package.

You will also need to install libflac. These may be named differently depending on your distribution. You may also need to install the developer packages for the build process, depending on your distribution.

Lastly, you will need a compiler and a standard C library. The ones I chose and are confirmed to work are the GNU Compiler Collection (gcc and g++) and the GNU C Library (glibc). Install these.

### Build
Clone the repository into a folder of your choice (e.g. /home/[user]/src).

Navigate to the root of the repository folder and run the following command:
```
> cmake --preset [preset]
```

The available presets are _amd64-release-linux_ and _amd64-debug-linux_. These presets use the compilers, generators, and etc that I use when building for Linux (you can find their exact details in CMakePresets.json). If you would like to change some of the attributes, add more flags to the cmake command as necessary.

You may also want to include a `-DCMAKE_INSTALL_PREFIX=[install prefix]` flag, if you wish to install the program in a location other than /usr/local. (By default CMake installs it into /usr/local, this is the conventional location on Linux for programs that are not managed by any package manager)

Change directory to where it was built:
```
> cd out/build/[preset]
```

...and compile the programs:
```
> cmake --build .
```

If all has went well, gex2ps1soundexporter will be built in the folder with no errors.

## Additional Notes
These are some other things that are important to keep in mind when using the program.
* Due to its small scale and lack of wide usage, the program is relatively untested. Use caution when selecting the import file. Only try to export sounds from files that you know are from a properly dumped version of the game.

## Credits
* Crystal Dynamics for their amazing game
* [Martin Korth](https://problemkaputt.de/psx-spx.htm) for painstakingly detailing the Playstation 1's architecture, including its SPU
* [Duckstation](https://github.com/stenzek/duckstation) for their implementation of the SPU ADPCM decompressor which I used as a partial reference
* [The Gex Discord](https://discord.gg/TeA7D4f) for their work in documenting and modding the Gex games
