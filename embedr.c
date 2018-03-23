/*
 * This library provides an R server for Q
 */
#include <errno.h>
#include <string.h>
#include <R.h>
#include <Rdefines.h>
#include <Rembedded.h>
#include <time.h>
#ifndef WIN32
#include <Rinterface.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <unistd.h>
#else
#include <windows.h>
#include <process.h> 
#endif

#include <R_ext/Parse.h>
#include "src/socketpair.c"

#define KXVER 3
#include "src/k.h"

#include "src/common.c"
#include "src/rserver.c"

int R_SignalHandlers = 0;
