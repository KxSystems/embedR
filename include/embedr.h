/*
 * This library provides an R server for Q
 */

/*-----------------------------------------------*/
/*                Load Libraries                 */
/*-----------------------------------------------*/

#include <stdbool.h>
#include <Rdefines.h>
#include <Rembedded.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#define EXP __declspec(dllexport)
#else
#include <Rinterface.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#define EXP
#endif
#include <R_ext/Parse.h>
