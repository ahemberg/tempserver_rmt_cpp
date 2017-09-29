# tempserver_rmt_cpp

Software for reading temperature from DS18B20 temperature sensor and sending it off to temperature server. Also contains functions for reading various system parameters and sending them off to the temperature server.

Although the software can be used stand-alone it is designed for communication with a temperature server. (See this #link for the server software)

Requires that a local mysql-server is running with the tempserver_remote database configured. Info #here

# To compile:

The code uses oracles mysql-connector to communicate with the mysql-database and this is required to compile.

for more information om installing see
https://dev.mysql.com/downloads/connector/cpp/

Or if you are using debian you can just issue:

```
$ sudo apt-get install libmysqlcppconn7
```
also install  install libcurl4-openssl-dev

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

# Make a secrets file
In order for the program to work it needs to be  able to access your local mysql-database. The authenitcation information should be stored in a file called **secrets**. It should be located **<WHERE?>** and have the following format

```
{
  "database_auth": {
    "host": "<DB HOST>",
    "user": "<DB USER>",
    "password": "<YOUR DB-USERs PASSWORD>",
    "database": "<DB NAME>"
  }
}
```

# TODO
Documentation, configuration etc.

# Remove this part
I'm just playing around with git anyway.
