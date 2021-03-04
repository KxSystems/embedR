/*
 * This library provides an R server for Q
 */

/*-----------------------------------------------*/
/*                Load Libraries                 */
/*-----------------------------------------------*/

#include <stdbool.h>
#include <Rdefines.h>
#include <Rembedded.h>
#ifndef WIN32
#include <Rinterface.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#else
#include <windows.h>
#include <process.h> 
#endif
#include <R_ext/Parse.h>
