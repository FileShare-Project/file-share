# FileShare

This repository is for the GUI and CLI versions of FileShare.  
Currently, it is only a test program that showcases a basic usage of the LibFileShareProtocol.

## Usage 

This is currently only a test program, and the current usage does not reflect the final one in any shape or form.

There is 2 test modes : a client (sends a file to the server), and a server (receive the file and downloads it).  
If you start the program with 0 arguments, it will start the server, if you give an argument it will start the client.

The Client will send a few dummy requests (not implemented on server side) and finish with a SendFile `/tmp/to_send`.  
You need to create that file on your computer with the content you intend to send to the server.

The Server will respond to the Client requests, but only act on the SendFile. It will download it in `~/Downloads/TODO/to_send`.

All this behavior can be altered by editing `source/main.cpp`. Changing the default Config for the server allows to change the download path for instance.

This Client/Server distinction on the test program is only for testing purposes. In reality, both are Server **AND** Client at the same time.  
The test program only sends requests from the "Client" end, but that is purely for ease of testing.

## Build from source

First you need to clone the submodules :

``` sh
git submodule update --init --recursive
```

Then you can build using CMake :

```sh
cmake -B ./build  && cmake --build ./build
# Or
cmake -B ./build  && make -C ./build
```

You will get the binary symlinked at `./file-share`

## Running tests

```sh
cmake -B ./build  && cmake --build ./build --target test
# Or
cmake -B ./build  && make -C ./build test
```
