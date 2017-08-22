# tempserver_rmt_cpp

Software for reading temperature from DS18B20 temperature sensor and sending it off to temperature server. Also contains functions for reading various system parameters and sending them off to the temperature server.

Although the software can be used stand-alone it is designed for communication with a temperature server. (See this #link for the server software)

Requires that a local mysql-server is running with the tempserver_remote database configured. Info #here

# To compile:

Clone the repo ~duh

Cd into repo:
```
$ cd tempserver_rmt_cpp
```
Then issue 
```
$ make -f makefile
```
to make all binaries, or use
```
$ make -f makefile read_temp 
```
to only build binary for reading and sending temperature or
```
$ make -f makefile update_status
```
to build binary for updating and sending status.
