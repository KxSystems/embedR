## Compiling locally

The Makefile works on l64 and l32.

It requires that `R` be available as a shared object, `libR.so` (compile R using --enable-R-shlib, or install a package with the shared object, e.g. `Rserve`). 
For all platforms it is advisable to set `R_HOME`

Linux/macOS
```
export R_HOME=`R RHOME`
```

Windows
```
R.exe RHOME
set R_HOME=<output of above>
```


Linux and macOS

Just run `make` in this folder. Do remember to set QHOME beforehand.

Windows

Install R tools (https://cran.r-project.org/bin/windows/Rtools/). It uses mingw to compile package and has been tested and used by R package authors.

Set your `QHOME` and add `R` to the `PATH` on your system. 
For 32bit dll run 
```
sh w32.sh
```

For 64bit
```
sh w64.sh
```

Copy  DLLs(`w32/embedr.dll` and `%R_HOME%\bin\i386\*.dll` for 32 bit and `w64/embedr.dll` and `%R_HOME%\bin\x64\*.dll` for 64 bit) to `%QHOME%/w32` or `%QHOME%/w64` respectively.
Copy `rinit.q` and `rtest.q` to `%QHOME%`

When using 32bit or 64bit `R` make sure you have appropriate R version in your path.
