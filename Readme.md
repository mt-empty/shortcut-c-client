# Shortcut

A command line client for [shortcut-pages](https://github.com/mt-empty/shortcut-pages), written in C.

![](/shortcut.gif)


## Installing

Install from source:
```
git clone https://github.com/mt-empty/shortcut-c-client
cd shortcut-c-client
make install
```

Install non-GUI cheatsheet pages(extra):
```
git clone 
cd shortcut-c-client
install-extra
```

The default prefix for installation is `/usr/local/bin`. Pages are located in `/opt`, these can be overwritten in the makefile or simply pass the variables when making `make PREFIX=YOUR_PATH DEST_DIR=YOUR_PATH install`.


### Requirements

* `clang`/`gcc`
* `git`


## Usage

```
usage: shortcut [OPTION] page

available commands:
    -V, --version           print verbose output
    -h, --help              print this help and exit
```


## TODO

* [x] List all pages option
* [ ] Auto completion
* [ ] Update option


## Contributing

Contributions are most welcome!

Bugs: open an issue here.

New features: open an issue here or feel free to send a pull request with the included feature.
