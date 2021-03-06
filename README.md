# embedR: Embedding R inside q

See <https://code.kx.com/v2/interfaces/with-r/#calling-r-from-q>

## Installation

### Download Pre-built Binary

Download the appropriate release archive from [releases](../../releases/latest) page. 

- Linux/Mac

      $ install.sh

- Windows: Open the archive and copy content of the `embedr` folder (`embedr\*`) to `%QHOME%` or `c:\q`<br/>Copy R_HOME/x64/*.dll or R_HOME/i386/*.dll to QHOME/w64 or QHOME/w32 respectively.

### Install from Source

Building the library from source uses the CMake file provided.

#### Linux/ Mac

For successful installation you need to set a path to `lib` directory on `R_LIBRARY_DIR` and a path to `include` directory on `R_INCLUDE_DIR` with following commands:

```bash

]$ export R_LIBRARY_DIR=$(R RHOME)/lib
]$ export R_INCLUDE_DIR=$(R CMD config --cppflags | cut -c 3-)

```

Then execte the commands below at the root directory of this repository:

```bash

embedR]$ mkdir build && cd build
build]$ cmake ..
build]$ cmake --build . --target install

```

**Note:** `cmake --build . --target install` installs the required share object and q files to the `QHOME\[os]64` and `QHOME` directories respectively. If you do not wish to install these files directly, you can execute `cmake --build .` instead of `cmake --build . --target install` and move the files from their build location at `build/embedr`.

#### Windows

Set a path to `lib` directory on `R_LIBRARY_DIR` and a path to `include` directory on `R_INCLUDE_DIR` with following commands:

```bat

:: Example output as we don't know how to evaluate an expression...
> R RHOME
C:\PROGRA~1\R\R-40~1.4
> set R_HOME=C:\PROGRA~1\R\R-40~1.4
> set R_LIBRARY_DIR=%R_HOME%\bin\x64
:: looks like `include` is in the same layer as `lib`. Check your own environment. 
> set R_INCLUDE_DIR=%R_HOME%\include

```

Next you need to create `R.lib` from `R.dll` in `R_LIBRARY_DIR` 🔨🔨🔨. This `R.lib` will be used for linking the interface.

```bat

> cd %R_LIBRARY_DIR%
x64> echo EXPORTS > R.def
x64> for /f "usebackq tokens=4,* delims=_ " %i in (`dumpbin /exports "R.dll"`) do echo %i_%j >> R.def
:: Then delete 2-9 lines manually.
:: After the manual processing, create lib file.
x64> lib /def:R.def /out:R.lib /machine:x64

```

Then you need to create a symlink to `R.dll`.

```bat

x64> cd %QHOME%\w64
w64> MKLINK R.dll %R_HOME%\bin\R.dll

```

Then execte the commands below at the root directory of this repository on Visual Studio (assuming `-G "Visual Studio 15 2017 Win64"` is not necessary):

```bat

embedR>$ mkdir build && cd build
build> cmake --config Release ..
build> cmake --build . --config Release --target install

```

**Note:** `cmake --build . --config Release --target install` installs the required share object and q files to the `QHOME\[os]64` and `QHOME` directories respectively. If you do not wish to install these files directly, you can execute `cmake --build . --config Release` instead of `cmake --build . --config Release --target install` and move the files from their build location at `build/embedr`.

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
