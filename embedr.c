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
#include <sys/socket.h>
#include <sys/types.h>
//#include <unistd.h>
#else
#include <process.h> 
#define closesocket(x) close(x)
#endif

#include <R_ext/Parse.h>
#include "src/socketpair.c"

#define KXVER 3
#include "src/k.h"

#include "src/common.c"
#include "src/rserver.c"

int R_SignalHandlers = 0;
