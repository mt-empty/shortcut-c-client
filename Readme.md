# Shortcut

A command line client for [shortcut-pages](), written in C.

![](/shortcut.gif)

## Installing
**Note**, I'm fairly new to Linux, this tool is in it's early stages, contribution is greatly appreciated.

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

The default prefix for installation is `/usr/local/bin`.

### Requirements

- `clang`/`gcc`


## Usage

```
usage: shortcut [OPTION] page

available commands:
    -V, --version           print verbose output
    -h, --help              print this help and exit
```


## TODO
* Auto completion
* Update option
* List all pages option
* Verbose option


## Contributing

Contributions are most welcome!

Bugs: open an issue here.

New features: open an issue here, feel free to send a pull request with the included feature.
