# embedR: Embedding R inside q



See <https://code.kx.com/v2/interfaces/with-r/#calling-r-from-q>


## Installation

### Download
Download the appropriate release archive from [releases](../../releases/latest) page. 

### Unpack and install content of the archive 

environment     | action
----------------|---------------------------------------------------------------------------------------
Linux           | `tar xzvf embedr_linux-v*.tar.gz -C $QHOME --strip 1`
macOS           | `tar xzvf embedr_osx-v*.tar.gz -C $QHOME --strip 1`
Windows         | Open the archive and copy content of the `embedr` folder (`embedr\*`) to `%QHOME%` or `c:\q`<br/>Copy R_HOME/x64/*.dll or R_HOME/i386/*.dll to QHOME/w64 or QHOME/w32 respectively. 


## Calling R

When calling R, you need to set `R_HOME`. Required are:

```bash
# Linux/macOS
export R_HOME=`R RHOME`
# Windows
for /f "delims=" %a in ('R RHOME') do @set R_HOME=%a
```

The library has four main methods:

- `Ropen`: Initialise embedded R. Optional to call. Allows to set verbose mode as `Ropen 1`.
- `Rcmd`: run an R command, do not return a result
- `Rget`: run an R command, return the result to q
- `Rset`: set a variable in the R memory space


### Interactive plotting

If using interactive plotting with `lattice` and/or `ggplot2` you will need to call `print` on a chart object. 


## Examples

See [examples](examples) folder. 

Note: Examples are kdb+ 3.5 or higher.


### Example 1

`e4.q` is a simple example of plot 'moving window volatility' of returns. Converted from http://www.mayin.org/ajayshah/KB/R/html/p4.html


### Example 2

`pcd.q` is based on [Corporate credit card transactions 2014-15](https://data.gov.uk/dataset/corporate-credit-card-transaction-2014-15).

Download CSV from the link above and save it in the same folder as `pcd.q` under name `pcd2014v1.csv`.


### Example 3

<http://data.london.gov.uk/datastore/package/tubenetwork-performance-data>

Left for the reader :)
