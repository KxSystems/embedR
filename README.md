# embedR: Embedding R inside q

See <https://code.kx.com/v2/interfaces/with-r/#calling-r-from-q>

## Installation

### Download
Download the appropriate release archive from [releases](../../releases/latest) page. 

#### Unpack and install content of the archive 

environment     | action
----------------|---------------------------------------------------------------------------------------
Linux           | `tar xzvf embedr_linux-v*.tar.gz -C $QHOME --strip 1`
macOS           | `tar xzvf embedr_osx-v*.tar.gz -C $QHOME --strip 1`
Windows         | Open the archive and copy content of the `embedr` folder (`embedr\*`) to `%QHOME%` or `c:\q`<br/>Copy R_HOME/x64/*.dll or R_HOME/i386/*.dll to QHOME/w64 or QHOME/w32 respectively. 

### Building
For Linux/MacOS it is possible to build the library from source using the makefile provided. From the root of the directory:

```bash
// Create the `.so` binary
$ make

// Install into $QHOME
$ make install

// Remove the binary from current directory
$ make clean

// Or in a single command
$ make && make install && make clean
```

## Calling R

When calling R, you need to set `R_HOME`. This can be set as follows:

```bash
# Linux/macOS
export R_HOME=`R RHOME`
# Windows
for /f "delims=" %a in ('R RHOME') do @set R_HOME=%a
```

The library has four main methods:

- `.rk.open`: Initialise embedR. Optional to call. Allows to set verbose mode as `Ropen 1`.
- `.rk.exec`: execute an R command, do not return a result to q.
- `.rk.get`: execute an R command, return the result to q.
- `.rk.set`: set a variable in the R memory space


## Documentation

Documentation for this interface is available [here](https://code.kx.com/q/interfaces/r/embedr)

## Examples

A number of example scripts are provided in the [examples](examples) folder.

**Note:** Examples are kdb+ 3.5 or higher.

1. Plotting the 'moving window volatility' of returns
2. Analysis of corporate credit card transactions in the UK dataset available [here](https://ckan.publishing.service.gov.uk/dataset/corporate-credit-card-transactions-2014-152)

## Notes

1. If using interactive plotting with `lattice` and/or `ggplot2` you will need to call `print` on a chart object.
2. As of v2.0 the callable functions have migrated to the `.rk` namespace in line with the other fusion interfaces. The historic interface using `Rcmd`,`Ropen` etc will be deprecated in v2.1
