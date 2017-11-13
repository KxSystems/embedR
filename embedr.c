/*
 * This library provides an R server for Q
 */
#include <errno.h>
#include <string.h>
#include <R.h>
#include <Rdefines.h>
#include <Rembedded.h>
#ifndef WIN32
#include <Rinterface.h>
#endif
#ifdef WIN32
#define closesocket(x) close(x)
#endif
#include <R_ext/Parse.h>
#define KXVER 3
#include "src/k.h"

#include "src/common.c"
#include "src/rserver.c"

int R_SignalHandlers = 0;
