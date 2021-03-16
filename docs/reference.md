---
title: embedR User Guide
date: April 2020
description: Lists functions available for use within the embedR
keywords: interface, kdb+, q, r, 
---

# embedR User Guide

A shared library can be loaded which brings R into the q memory space,
meaning all the R statistical routines and graphing capabilities can be invoked directly from q.
Using this method means data is not passed between remote processes.

The library has five methods:

```txt
  //Connection
  .r.open:            Initialise embedR. Optional to call. Allows to set verbose mode as Ropen 1
  .r.close:           Close internal R connection

  //Execution
  .r.exec:            Run an R command, do not return a result
  .r.get:             Run an R command, return the result to q
  .r.set:             Set a variable in the R memory space

  //Graphic
  .r.dev              Open plot window with noRStudioGD=TRUE (Normally R will open a new device automatically)
  .r.off              Close plot window

  //Utility
  .r.install          Install package in embeded R process over the connection

```

## Connection

### .r.open

_Initialise embedR. Optional to call. Allows to set verbose mode as Ropen 1_

Syntax: `.r.open[signal]`

Where

- `signal` is an integer which is one of empty, 0 or 1.
  * 0 or empty: quiet mode
  * 1: verbose mode

```q
q).r.open[]
q)//or
q).r.open[1]
```

!!! note "Note"

     As of version 2.0 <b>Ropen</b> was migrated to <b>.r.open</b>. <b>Ropen</b> will be depricated in version 2.1.

### .r.close

_Close internal R connection_

Syntax: `.r.close[]`

!!! note "Note"

      As of version 2.0 <b>Rclose</b> was migrated to <b>.r.close</b>. <b>Rclose</b> will be depricated in version 2.1.

## Execution

### .r.exec

_Run an R command, do not return a result_

Synatax: `.r.exec[command]`

Where

- `command` is an R expression to execute in R process

```q
q).r.exec "Sys.setenv(TZ = 'UTC')"
q).r.exec "library(xts)"
Loading required package: zoo

Attaching package: ‘zoo’

The following objects are masked from ‘package:base’:

    as.Date, as.Date.numeric
```

### .r.get

_Run an R command, return the result to q_

Synatax: `.r.get[rexp]`

Where

- `rexp` is a string denoting an R expression to execute in R process

```q
q).r.exec "today <- as.Date('2020-04-01')"
q).r.get "today"
,2019.04.01
```

!!! note "Note"

     As of version 2.0 <b>Rget</b> was migrated to <b>.r.get</b>. <b>Rget</b> will be depricated in version 2.1.

### .r.set

_Set a variable in the R memory space_

Synatax: `.r.set[rvar; qvar]`

Where

- `rvar` is a string denoting a name of R variable used in R process
- `qvar` is any q object used to assign to the R variable in R process

```q
q).r.set["mnth"; `month$/:2010.01.29 2020.04.02]
q).r.get "mnth"
2010.01 2020.04m
```

!!! note "Note"

     As of version 2.0 <b>Rset</b> was migrated to <b>.r.set</b>. <b>Rset</b> will be depricated in version 2.1.

## Graphic

### .r.new

_Open plot window with noRStudioGD=TRUE (Normally R will open a new device automatically)_

Syntax: `.r.new[]`

!!! note "Note"

     As of version 2.0 <b>Rnew</b> was migrated to <b>.r.new</b>. <b>Rnew</b> will be depricated in version 2.1.

### .r.off

_Close plot window_

Syntax: `.r.off[]`

To close the graphics window, use `dev.off()` rather than the close button on the window.

!!! note "Note"

     As of version 2.0 <b>Roff</b> was migrated to <b>.r.off</b>. <b>Roff</b> will be depricated in version 2.1.

## Utility

### .r.install

_Install package in embeded R process over the connection_

Syntax: `.r.install[package]`

Where

- `package` is a symbol denoting the name of a package to install in the R process

You must be a super user who has an access to the library directory.

The result is any information regarding install if the package is installed for the first time; otherwise nothing is returned.

```q
q).r.install["psy"]
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

     As of version 2.0 <b>Rinstall</b> was migrated to <b>.r.install</b>. <b>Rinstall</b> will be depricated in version 2.1.


Simple examples of embedR are available in [Examples](examples.md) page.