---
title: embedR, an interface for calling R from q
description: embedR is an interface that allows the R programming language to be invoked by q programs
keywords: interface, kdb+, q, r
hero: <i class="fab fa-superpowers"></i> Fusion for Kdb+
---

# <img src="../images/R_logo.png" width="50"/> embedR

## Introduction

Kdb+ and R are complementary technologies. Kdb+ is the world’s leading timeseries database and incorporates a programming language called q. [R](https://www.r-project.org/) is a programming language and environment for statistical computing and graphics. Both are tools used by data scientists to interrogate and analyze data. Their features sets overlap in that they both:

- are interactive development environments
- incorporate vector languages
- have a built-in set of statistical operations
- can be extended by the user
- are well suited for both structured and ad-hoc analysis

They do however have several differences:

- q can store and analyze petabytes of data directly from disk whereas R is limited to reading data into memory for analysis
- q has a larger set of datatypes including extensive temporal times (timestamp, timespan, time, second, minute, date, month) which make temporal arithmetic straightforward
- R contains advanced graphing capabilities whereas q does not
- built-in routines in q are generally faster than R
- R has a more comprehensive set of pre-built statistical routines

When used together, q and R provide an excellent platform for easily performing advanced statistical analysis and visualization on large volumes of data.

R can be called as a server from q, and vice-versa.

### q and R working together

Given the complementary characteristics of the two languages, it is important to utilize their respective strengths.
All the analysis could be done in q; the q language is sufficiently flexible and powerful to replicate any of the pre-built R functions.
Below are some best practice guidelines, although where the line is drawn between q and R will depend on both the system architecture and the strengths of the data scientists using the system.

- Do as much of the analysis as possible in q. This is because q analyzes the data directly from the disk and it is always most efficient to do as much work as possible as close to the data as possible. Whenever possible, avoid extracting large raw datasets from q and if extractions are required, use q to create smaller aggregated datasets
- Do not re-implement tried and tested R routines in q unless they either
  * can be written more efficiently in q and are going to be called often
  * require more data than is feasible to ship to the R installation
- Use R for data visualization

Considering the potential size of the data, it is probably more likely that the kdb+ installation containing the data will be hosted remotely from the user.

### What Does embedR Provide?

What **embedR** provides is to embed R inside q and invoke R routines. can only be used if the q and R installations are installed on the same server.

## Install

Work for both 32-bit and 64-bit kdb+. 

Pre-built packages are available from [here](https://github.com/KxSystems/embedR/releases/tag/v1.2.1) for:

-   <i class="fab fa-linux"></i> Linux
-   <i class="fab fa-apple"></i> macOS

If the appropriate build is not available on your target system, download from <i class="fab fa-github"></i> [KxSystems/embedR](https://github.com/KxSystems/embedR) and follow the installation instruction in `README.md`.

Optional additional environment variables are `R_SHARE_DIR`, `R_INCLUDE_DIR`, `LD_LIBRARY_PATH` (for libR.so).

## Testing

User can test embedR with the test script named `test.q` provided in `tests` folder of <i class="fab fa-github"></i> [KxSystems/embedR](https://github.com/KxSystems/embedR).

Procedure is simple. Go to `tests` directory and run `test.q`.

**Note:** If you don't have multi-threading environment eliminate `-s 2` flag.

```bash
$ cd embedR/tests
$ q test.q -test_data_frame true -s 2
0i

+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        Test Start!!
        Score:  0/0
        Fail:   0/0
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

...

+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        Completed!!
        Score:  78/78
        Fail:   0/78
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
```
