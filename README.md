# DONE
Docker Orchestrator for Network Emulation

## Usage:
```
make clean      # removes objects and executable
make exec       # if executable exists executes the GUI
make cli        # if executable exists runs the CLI
make            # compiles
make prod       # compiles and removes the intermediate files
make compile    # compiles the DoneScript files inside ./DONE/DoneScript/sources/
make import     # imports the DoneScript compiled files inside ./DONE/saves/
```

## DoneScript
DoneScript is the official scripting language for DONE.

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


## CLI
The CLI is an interactive instance of python3 with network emulation functions
