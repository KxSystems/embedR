/*
 * This library provides an R server for Q
 */
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <R.h>
#include <Rdefines.h>
#include <Rembedded.h>
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

#define KXVER 3
#include "k.h"
#define INT64(x)   ((J*) REAL(x))

// Offsets used in conversion between R and q
static const J epoch_offset=10957*24*60*60*1000000000LL;
// Seconds in a day
static const int sec2day = 86400;
// Days+Seconds between 1970.01.01 & 2000.01.01
static const int kdbDateOffset = 10957;
static const int kdbSecOffset  = 946684800;

#include "common_R_interface/q2r.c"
#include "common_R_interface/r2q.c"
#include "feature_for_embedR/q2r_ex.c"
#include "feature_for_embedR/r2q_ex.c"

int R_SignalHandlers = 0;
