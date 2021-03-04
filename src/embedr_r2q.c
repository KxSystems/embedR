/*-----------------------------------------------------------*/
/*  File: r2q_ex.c                                           */
/*  Overview: Distinct code in embedR for R -> Q interface   */
/*-----------------------------------------------------------*/

/*-----------------------------------------------*/
/*                Load Libraries                 */
/*-----------------------------------------------*/

#include "socketpair.h"
#include "common.h"
#include "embedr.h"

/*-----------------------------------------------*/
/*                List of Functions              */
/*-----------------------------------------------*/

/*
 * User interface
 */

K ropen(K x);
K rclose(K x);
K rexec(K x);
K rget(K x);
K rset(K x,K y);
static K rcmd(int type,K x);

/*-----------------------------------------------*/
/*                Global Variable                */
/*-----------------------------------------------*/

__thread int ROPEN=-1; // initialise thread-local. Will fail in other threads. Ideally need to check if on q main thread.
__thread int RLOAD=0;

/*-----------------------------------------------*/
/*                  Functions                    */
/*-----------------------------------------------*/

/*
 * Conversion from R to Q
 */

K from_pairlist_robject(SEXP sxp) {
  K x = ktn(0,2*length(sxp));
  SEXP s = sxp;J i;
  for(i=0;i<x->n;i+=2) {
    kK(x)[i] = from_any_robject(CAR(s));
    kK(x)[i+1] = from_any_robject(TAG(s));
    s=CDR(s);
  }
  return attR(x,sxp);
}

/*
 * various utilities
 */

/* get k string or symbol name */
static char * getkstring(K x) {
  char *s=NULL;
  int len;
  switch (xt) {
    case -KC :
      s = calloc(2,1); s[0] = xg;
      break;
    case KC :
      s = calloc(1+xn,1); memmove(s, xG, xn);
      break;
    case -KS : // TODO: xs is already 0 terminated and fixed. can just return xs
      len = 1+strlen(xs);
      s = calloc(len,1); memmove(s, xs, len); break;
    default :
      krr("invalid name");
  }
  return s;
}

/*
 * The public interface used from Q.
 */

#ifdef _WIN32
static SOCKET spair[2];
#else
static int spair[2];
#endif

void* pingthread;

V* pingmain(V* v){
  while(1){
    nanosleep(&(struct timespec){.tv_sec=0, .tv_nsec=1000000}, NULL);
    send(spair[1], "M", 1, 0);
  }
}

K processR(I d){
  char buf[1024];
  /*MSG_DONTWAIT - set in sd1(-h,...) */
  while(0 < recv(d, buf, sizeof(buf), 0))
    ;
  R_ProcessEvents();
  return (K)0;
}

/*
 * ropen argument is empty, 0 or 1
 * empty,0   --slave (R is quietest)
 * 1         --verbose
 */

K ropen(K x) {
  if(!RLOAD) return krr("main thread only");
  if (ROPEN >= 0) return ki(ROPEN);
  int s,mode=0;	char *argv[] = {"R","--slave"};
  if (x && (-KI ==x->t || -KJ ==x->t)) mode=(x->t==-KI?x->i:x->j)!=0;
  if (mode) argv[1] = "--verbose";
  int argc = sizeof(argv)/sizeof(argv[0]);
  s=Rf_initEmbeddedR(argc, argv);
  if (s<0) return krr("open failed");
  if(dumb_socketpair(spair, 1) == -1)
    return krr("Init failed for socketpair");
  sd1(-spair[0], &processR);
  #ifndef WIN32
    pthread_t t;
    if(pthread_create(&t, NULL, pingmain, NULL))
      R krr("poller_thread");
    pingthread= &t;
  #else
    if(_beginthreadex(0,0,pingmain,NULL,0,0)==-1)
   	R krr("poller_thread");
  #endif
  ROPEN=mode;
  return ki(ROPEN);
}

// note that embedded R can be initialised once. No open/close/open supported 
// http://r.789695.n4.nabble.com/Terminating-and-restarting-an-embedded-R-instance-possible-td4641823.html
K rclose(K x){R NULL;}
K rexec(K x) { return rcmd(0,x); }
K rget(K x) { return rcmd(1,x); }

static char* ParseError[5]={"null","ok","incomplete","error","eof"};

K rcmd(int type,K x) {
  if(!RLOAD) return krr("main thread only");
  if (ROPEN < 0) ropen(NULL);
  SEXP e, p, r, xp;
  char rerr[256];extern char	R_ParseErrorMsg[256];
  int error;
  ParseStatus status;
  if(abs(x->t)==KS)
    e=from_symbol_kobject(x);
  else if(abs(x->t)==KC)
    e=from_string_kobject(x);
  else
    return krr("type");
  PROTECT(e);
  PROTECT(p=R_ParseVector(e, 1, &status, R_NilValue));
  if (status != PARSE_OK) {
    UNPROTECT(2);
    snprintf(rerr,sizeof(rerr),"%s: %s",ParseError[status], R_ParseErrorMsg);
    return krr(rerr);
  }
  PROTECT(xp=VECTOR_ELT(p, 0));
  r=R_tryEvalSilent(xp, R_GlobalEnv, &error);
  UNPROTECT(3);
  R_ProcessEvents();
  if (error) {
    snprintf(rerr,sizeof(rerr),"eval error: %s",R_curErrorBuf());
    return krr(rerr);
  }
  if (type==1)
    return from_any_robject(r);
  return (K)0;
}

K rset(K x,K y) {
  if(!RLOAD)
    return krr("main thread only");
  if (ROPEN < 0)
    ropen(NULL);
  ParseStatus status;
  SEXP txt, sym, val;
  char rerr[256];extern char	R_ParseErrorMsg[256];
  char *name = getkstring(x);
  /* generate symbol to check name is valid */
  PROTECT(txt=allocVector(STRSXP, 1));
  SET_STRING_ELT(txt, 0, mkChar(name));
  free(name);
  PROTECT(sym = R_ParseVector(txt, 1, &status,R_NilValue));
  if (status != PARSE_OK) {
    UNPROTECT(2);
    snprintf(rerr,sizeof(rerr),"%s: %s",ParseError[status], R_ParseErrorMsg);
    return krr(rerr);
  }
  if(SYMSXP != TYPEOF(VECTOR_ELT(sym,0))){
    UNPROTECT(2);
    return krr("nyi");
  }
  /* read back symbol string */
  const char *c = CHAR(PRINTNAME(VECTOR_ELT(sym,0)));
  PROTECT(val = from_any_kobject(y));
  defineVar(install(c),val,R_GlobalEnv);
  UNPROTECT(3);
  R_ProcessEvents();
  return (K)0;
}

__attribute__((constructor)) V __attach(V) {RLOAD=1;}
