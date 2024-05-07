# DONE
_Docker Orchestrator for Network Emulation_

**DONE** is a simple **network emulator**, inspired by the [IMUNES](https://github.com/imunes/imunes) project. Starting from the features IMUNES offered, we decided to recreate some of them, improving both software reliability and the UX while reducing software dependencies to the bare minimum.

<!-- SCREENSHOTS HERE -->

## Prerequisites
In order for DONE to properly work, make sure you have the following dependencies installed:
- [`raylib`](https://github.com/raysan5/raylib) for graphical purposes;
- [`docker`](https://docs.docker.com/engine/install/) to virtualize nodes and routers;
- [`openVSwitch`](https://www.openvswitch.org/) to virtualize switches.

<!-- 
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
cd raylib/src/
make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED # To make the dynamic shared version.
sudo make install RAYLIB_LIBTYPE=SHARED

-->

You also need to make sure you have a C compiler on your machine.

## Installation
To install DONE, simply clone the repository and navigate to the `DONE` folder, then run the `make` commands from there:
```
git clone https://github.com/IPoAC-SMT/DONE.git
cd DONE/DONE
make clean
make
make exec
```

## Usage:
All the possible `make` options are the following:
```
make clean      # removes objects and executable
make exec       # if executable exists executes the GUI
make cli        # if executable exists runs the CLI
make            # compiles
make prod       # compiles and removes the intermediate files
make compile    # compiles the DoneScript files inside ./DONE/DoneScript/sources/
make import     # imports the DoneScript compiled files inside ./DONE/saves/
```
### Starting a simulation
Once you have created your topology, you can run the simulation by pressing on the **Start simulation!** button.
It is advisable to stop it via the **Stop simulation** button before closing the application, however everything will be deleted even if you do not stop it before exiting, therefore no pending docker containers will remain on your machine.

### Saving configurations to file
TODO

<!-- -->


## DoneScript
`DoneScript` is the official **scripting language** for DONE.

All DoneScript files should be named without spaces and with the extension `.ds`.

Instructions are the following, where `shortname` is a string shorter than 20 chars, `x`, `y`, `r`, `g` and`b` are integers, `varname` is a shortname that begins with `var`. `command` can be any valid linux command.

```
create (host|router|switch|external interface|external natted interface|internet) at <x> <y> as <shortname>
link <shortname> and <shortname>
draw rectangle between <x> <y> and <x> <y> with color <r> <g> <b>
send command to <shortname> <command>
add text "a very long string I want to add" at <x> <y>

// you can comment things out, and it ignores empty lines as the ones around it

// YOU CAN ALSO WRITE EVERYTHING IN UPPERCASE!

for <varname> in (host|router|switch|external interface|external natted interface|internet)
do
    // you can write here any instruction, even other loops! wonderful, isn't it?
done
for <varname> in <min> <max> step <step>
do
    // whatever you want
done
```


## Command Line Interface
The CLI is an instance of `python3` with network emulation functions which offers the possibility to interact with the underlying network infrastructure by using a simple command line instead of the graphical interface.
To use the CLI instead of the GUI, as said earlier, launch DONE with the following `make` command:
```
make cli
```
