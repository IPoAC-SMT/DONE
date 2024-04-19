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

Instructions are the following:

```

```


## CLI
The CLI is an interactive instance of python3 with network emulation functions
