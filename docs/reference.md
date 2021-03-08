---
title: embedR User Guide
date: April 2020
description: Lists functions available for use within the embedR
keywords: interface, kdb+, q, r, 
---

# :fontawesome-brands-r-project: embedR User Guide

<div class="fusion" markdown="1">
:fontawesome-brands-superpowers: [Fusion for kdb+](../fusion.md)
</div>
:fontawesome-brands-github:
[kxsystems/embedr](https://github.com/kxsystems/embedr)

A shared library can be loaded which brings R into the q memory space,
meaning all the R statistical routines and graphing capabilities can be invoked directly from q.
Using this method means data is not passed between remote processes.

The library has five methods:

```txt
  //Connection
  .rk.open:            Initialise embedR. Optional to call. Allows to set verbose mode as Ropen 1
  .rk.close:           Close internal R connection

  //Execution
  .rk.exec:            Run an R command, do not return a result
  .rk.get:             Run an R command, return the result to q
  .rk.set:             Set a variable in the R memory space

  //Graphic
  .rk.dev              Open plot window with noRStudioGD=TRUE (Normally R will open a new device automatically)
  .rk.off              Close plot window

  //Utility
  .rk.install          Install package in embeded R process over the connection

```

### Connection

#### .rk.open

_Initialise embedR. Optional to call. Allows to set verbose mode as Ropen 1_

Syntax: `.rk.open[signal]`

Where

- `signal` is an integer which is one of empty, 0 or 1.
     * 0 or empty: quiet mode
     * 1: verbose mode

```q
q).rk.open[]
q)//or
q).rk.open[1]
```

!!! note "Note"

     As of version 2.0 <b>Ropen</b> was migrated to <b>.rk.open</b>. <b>Ropen</b> will be depricated in version 2.1.

#### .rk.close

_Close internal R connection_

Syntax: `.rk.close[]`

!!! note "Note"

      As of version 2.0 <b>Rclose</b> was migrated to <b>.rk.close</b>. <b>Rclose</b> will be depricated in version 2.1.

### Execution

#### .rk.exec

_Run an R command, do not return a result_

Synatax: `.rk.exec[command]`

Where

- `command` is an R expression to execute in R process

```q
q).rk.exec "Sys.setenv(TZ = 'UTC')"
q).rk.exec "library(xts)"
Loading required package: zoo

Attaching package: ‘zoo’

The following objects are masked from ‘package:base’:

    as.Date, as.Date.numeric
```

#### .rk.get

_Run an R command, return the result to q_

Synatax: `.rk.get[rexp]`

Where

- `rexp` is a string denoting an R expression to execute in R process

```q
q).rk.exec "today <- as.Date('2020-04-01')"
q).rk.get "today"
,2019.04.01
```

!!! note "Note"

     As of version 2.0 <b>Rget</b> was migrated to <b>.rk.get</b>. <b>Rget</b> will be depricated in version 2.1.

#### .rk.set

_Set a variable in the R memory space_

Synatax: `.rk.set[rvar; qvar]`

Where

- `rvar` is a string denoting a name of R variable used in R process
- `qvar` is any q object used to assign to the R variable in R process

```q
q).rk.set["mnth"; `month$/:2010.01.29 2020.04.02]
q).rk.get "mnth"
2010.01 2020.04m
```

!!! note "Note"

     As of version 2.0 <b>Rset</b> was migrated to <b>.rk.set</b>. <b>Rset</b> will be depricated in version 2.1.

### Graphic

#### .rk.new

_Open plot window with noRStudioGD=TRUE (Normally R will open a new device automatically)_

Syntax: `.rk.new[]`

!!! note "Note"

     As of version 2.0 <b>Rnew</b> was migrated to <b>.rk.new</b>. <b>Rnew</b> will be depricated in version 2.1.

#### .rk.off

_Close plot window_

Syntax: `.rk.off[]`

To close the graphics window, use `dev.off()` rather than the close button on the window.

!!! note "Note"

     As of version 2.0 <b>Roff</b> was migrated to <b>.rk.off</b>. <b>Roff</b> will be depricated in version 2.1.

### Utility

#### .rk.install

_Install package in embeded R process over the connection_

Syntax: `.rk.install[package]`

Where

- `package` is a symbol denoting the name of a package to install in the R process

You must be a super user who has an access to the library directory.

The result is any information regarding install if the package is installed for the first time; otherwise nothing is returned.

```q
q).rk.install["psy"]
Installing package into ‘/usr/lib64/R/library’
(as ‘lib’ is unspecified)
trying URL 'https://cloud.r-project.org/src/contrib/psy_1.1.tar.gz'
Content type 'application/x-gzip' length 36107 bytes (35 KB)
==================================================
downloaded 35 KB

* installing *source* package ‘psy’ ...
** package ‘psy’ successfully unpacked and MD5 sums checked
** using staged installation
** R
** data
** byte-compile and prepare package for lazy loading
** help
*** installing help indices
  converting help for package ‘psy’
    finding HTML links ... done
    ckappa                                  html  
    cronbach                                html  
    ehd                                     html  
    expsy                                   html  
    fpca                                    html  
    icc                                     html  
    lkappa                                  html  
    mdspca                                  html  
    mtmm                                    html  
    psy-package                             html  
    scree.plot                              html  
    sleep                                   html  
    sphpca                                  html  
    wkappa                                  html  
** building package indices
** testing if installed package can be loaded from temporary location
** testing if installed package can be loaded from final location
** testing if installed package keeps a record of temporary installation path
* DONE (psy)
Making 'packages.html' ... done

The downloaded source packages are in
        ‘/tmp/Rtmp8Pnq5n/downloaded_packages’
Updating HTML index of packages in '.Library'
```

!!! note "Note"

     As of version 2.0 <b>Rinstall</b> was migrated to <b>.rk.install</b>. <b>Rinstall</b> will be depricated in version 2.1.


Simple examples of embedR are available in [Examples](examples.md) page.