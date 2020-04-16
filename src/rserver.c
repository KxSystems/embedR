/*
 * R server for Q
 */
/*
 * The public interface used from Q.
 * https://cran.r-project.org/doc/manuals/r-release/R-ints.pdf
 * https://cran.r-project.org/doc/manuals/r-release/R-exts.html
 */

K ropen(K x);
K rclose(K x);
K rcmd(K x);
K rget(K x);
K rset(K x,K y);

ZK rexec(int type,K x);
ZK klogicv(J len, int *val);
ZK klogica(J len, int rank, int *shape, int *val);
ZK kintv(J len, int *val);
ZK kinta(J len, int rank, int *shape, int *val);
ZK klonga(J len, int rank, int *shape, J*val);
ZK kdoublev(J len, double *val);
ZK kdoublea(J len, int rank, int *shape, double *val);
ZK atom_value_dict(J len, K v, SEXP keys);

__thread int ROPEN=-1; // initialise thread-local. Will fail in other threads. Ideally need to check if on q main thread.
__thread int RLOAD=0;

/*
 * convert R SEXP into K object.
 */
ZK from_any_robject(SEXP);
ZK error_broken_robject(SEXP);
ZK from_null_robject(SEXP);
ZK from_symbol_robject(SEXP);
ZK from_pairlist_robject(SEXP);
ZK from_closure_robject(SEXP);
ZK from_language_robject(SEXP);
ZK from_date_robject(SEXP);
ZK from_datetime_robject(SEXP);
ZK from_datetime_ct_robject(SEXP);
ZK from_datetime_lt_robject(SEXP);
ZK from_difftime_robject(SEXP);
ZK from_second_or_minute_robject(SEXP);
ZK from_days_robject(SEXP);
ZK from_char_robject(SEXP);
ZK from_logical_robject(SEXP);
ZK from_integer_robject(SEXP);
ZK from_double_robject(SEXP);
ZK from_character_robject(SEXP);
ZK from_vector_robject(SEXP);
ZK from_raw_robject(SEXP);
ZK from_nyi_robject(SEXP);
ZK from_frame_robject(SEXP);
ZK from_factor_robject(SEXP);

Rboolean isClass(const char *class_, SEXP s) {
  SEXP klass;
  int i;
  if(OBJECT(s)) {
    klass= getAttrib(s, R_ClassSymbol);
    for(i= 0; i < length(klass); i++)
      if(!strcmp(CHAR(STRING_ELT(klass, i)), class_))
        return TRUE;
  }
  return FALSE;
}

Rboolean isUnit(const char *units_, SEXP s){
  SEXP unit;
  unit=getAttrib(s, R_UnitsSymbol);
  if(!strcmp(CHAR(asChar(unit)), units_))
    return TRUE;
  return FALSE;
}

ZK from_any_robject(SEXP sxp){
  if(isClass("data.frame", sxp)) {
    return from_frame_robject(sxp);
  }
  if(isClass("factor", sxp)) {
    return from_factor_robject(sxp);
  }
  if(isClass("Date", sxp)){
    return from_date_robject(sxp);
  }
  if(isClass("POSIXt", sxp)){
    return from_datetime_robject(sxp);
  }
  if(isClass("difftime", sxp)){
	return from_difftime_robject(sxp);
  }
  K result = 0;
  int type = TYPEOF(sxp);
  switch (type) {
	case NILSXP : 
	  return from_null_robject(sxp);
	  break; 		/* nil = NULL */
	case SYMSXP :
	  return from_symbol_robject(sxp);
	  break;    	/* symbols */
	case LISTSXP :
	  return from_pairlist_robject(sxp);
	  break; 		/* lists of dotted pairs */
	case CLOSXP : 
	  return from_closure_robject(sxp);
	  break;		/* closures */
	case LANGSXP :
	  return from_language_robject(sxp);
	  break; 	/* language constructs (special lists) */
	case CHARSXP : 
	  return from_char_robject(sxp);
	  break; 		/* "scalar" string type (internal only)*/
	case LGLSXP :
	  return from_logical_robject(sxp);
	  break; 		/* logical vectors */
	case RAWSXP :
	  return from_raw_robject(sxp);
	  break; 		/* raw bytes */
	case INTSXP :
	  return from_integer_robject(sxp);
	  break; 		/* integer vectors */
	case REALSXP :
	  return from_double_robject(sxp);
	  break; 		/* real variables */
	case STRSXP :
	  return from_character_robject(sxp);
	  break; 	/* string vectors */
	case VECSXP :
	  return from_vector_robject(sxp);
	  break; 		/* generic vectors */
	case FREESXP :
	  return error_broken_robject(sxp);
	  break;		/* node released by GC */
	case ANYSXP :
	  return error_broken_robject(sxp);
	  break; 		/* make "any" args work */
	case EXPRSXP :
	case BCODESXP :
	case EXTPTRSXP :
	case WEAKREFSXP :
	case S4SXP :
	case NEWSXP :
	case FUNSXP :
	case PROMSXP :
	case SPECIALSXP :
	case BUILTINSXP :
	case ENVSXP :
	case CPLXSXP :
	case DOTSXP :
	  return from_nyi_robject(sxp);
      break;
	}
	return result;
}

ZK dictpairlist(SEXP sxp){
  K k= knk(0);
  K v= knk(0);
  SEXP s= sxp;
  while(s!=R_NilValue){
    jk(&k,from_any_robject(TAG(s)));
    jk(&v,from_any_robject(CAR(s)));
    s= CDR(s);
  }
  return xD(k, v);
}

/* add attribute */
ZK addattR (K x,SEXP att)
{
	// attrs are pairlists: LISTSXP
	K u = from_pairlist_robject(att);
	return knk(2,u,x);
}

/* add attribute if any */
ZK attR(K x,SEXP sxp)
{
	SEXP att = ATTRIB(sxp);
	if (isNull(att)) return x;
	return addattR(x,att);
}

ZK atom_value_dict(J len, K v, SEXP keys){
   	K k= ktn(KS, len);
    for(J i= 0; i < len; i++) {
    	const char *keyName= CHAR(STRING_ELT(keys, i));
    	kS(k)[i]= ss((S) keyName);
    }
    return xD(k,v);
}

ZK error_broken_robject(SEXP sxp)
{
	return krr("Broken R object.");
}

ZK from_nyi_robject(SEXP sxp){
	return attR(kp((S)Rf_type2char(TYPEOF(sxp))),sxp);
}

ZK from_frame_robject(SEXP sxp) {
  J length= XLENGTH(sxp);
  if(length == 0)
    return from_null_robject(sxp);

  SEXP colNames= getAttrib(sxp, R_NamesSymbol);
  
  K k= ktn(KS, length), v= ktn(0, length);
  for(J i= 0; i < length; i++) {
    kK(v)[i]= from_any_robject(VECTOR_ELT(sxp, i));
    const char *colName= CHAR(STRING_ELT(colNames, i));
    kS(k)[i]= ss((S) colName);
  }

  K tbl= xT(xD(k, v));
  return tbl;
}

ZK from_factor_robject(SEXP sxp) {
  J length= XLENGTH(sxp);
  SEXP levels= asCharacterFactor(sxp);
  K x= ktn(KS, length);
  for(J i= 0; i < length; i++) {
    const char *sym= CHAR(STRING_ELT(levels, i));
    kS(x)[i]= ss((S) sym);
  }
  return x;
}

ZK from_raw_robject(SEXP sxp)
{
	K x = ktn(KG,XLENGTH(sxp));
	DO(xn,kG(x)[i]=RAW(sxp)[i])
	return x;
}

ZK from_date_robject(SEXP sxp) {
  K x;
  J length= XLENGTH(sxp);
  x= ktn(KD,length);
  int type= TYPEOF(sxp);
  switch(type) {
    case INTSXP:
      DO(length,kI(x)[i]=INTEGER(sxp)[i]-kdbDateOffset);
      break;
    default:
      DO(length,kI(x)[i]=ISNA(REAL(sxp)[i])?NA_INTEGER:(I)REAL(sxp)[i]-kdbDateOffset);
  }
  return x;
}

ZK from_datetime_ct_robject(SEXP sxp){
  K x;
  J length = XLENGTH(sxp);
  x = ktn(KZ,length);
  DO(length,kF(x)[i]=(F)(((REAL(sxp)[i]-kdbSecOffset)/sec2day)));
  return x;
}

ZK from_datetime_lt_robject(SEXP sxp){
  K x;
  J i, key_length= XLENGTH(sxp);
  x= ktn(0, key_length);
  for(i= 0; i < key_length; ++i)
    kK(x)[i]= from_any_robject(VECTOR_ELT(sxp, i));
  J element_length=kK(x)[0]->n;
  K res=ktn(KZ, element_length);
  for(i=0; i < element_length; i++){
    //Relying on the order of tm key
    struct tm dttm;
    dttm.tm_sec  =kF(kK(x)[0])[i];
    dttm.tm_min  =kI(kK(x)[1])[i];
    dttm.tm_hour =kI(kK(x)[2])[i];
    dttm.tm_mday =kI(kK(x)[3])[i];
    dttm.tm_mon  =kI(kK(x)[4])[i];
    dttm.tm_year =kI(kK(x)[5])[i];
    dttm.tm_wday =kI(kK(x)[6])[i];
    dttm.tm_yday =kI(kK(x)[7])[i];
    dttm.tm_isdst=kI(kK(x)[8])[i];
    kF(res)[i]=(((F)mktime(&dttm)-kdbSecOffset)/sec2day);
  }
  return res;
}

//Wraper function of POSIXt
ZK from_datetime_robject(SEXP sxp){
  if(isClass("POSIXct", sxp))
    return from_datetime_ct_robject(sxp);
  else
    return from_datetime_lt_robject(sxp);
}

ZK from_second_or_minute_robject(SEXP sxp){
  K x;
  J length=XLENGTH(sxp);
  x=ktn(isUnit("secs", sxp)?KV:KU, length);
  int type=TYPEOF(sxp);
  switch(type){
	case INTSXP:
	  DO(length, kI(x)[i]=INTEGER(sxp)[i]);
	  break;
	default:
	  DO(length,kI(x)[i]=ISNA(REAL(sxp)[i])?NA_INTEGER:(I) REAL(sxp)[i]);
  }
  return x;
}

ZK from_days_robject(SEXP sxp){
  K x;
  J length= XLENGTH(sxp);
  x= ktn(KN,length);
  DO(length,kJ(x)[i]=(J) (REAL(sxp)[i]*sec2day)*1000000000LL)
  return x;
}

//Wrapper function of difftime
ZK from_difftime_robject(SEXP sxp){
  if(isUnit("secs", sxp) || isUnit("mins", sxp))
    return from_second_or_minute_robject(sxp);
  else if(isUnit("days", sxp))
    return from_days_robject(sxp);
  else /* hours */
    return from_nyi_robject(sxp);
}

// NULL in R(R_NilValue): often used as generic zero length vector
// NULL objects cannot have attributes and attempting to assign one by attr gives an error
ZK from_null_robject(SEXP sxp)
{
	return knk(0);
}

ZK from_symbol_robject(SEXP sxp)
{
	const char* t = CHAR(PRINTNAME(sxp));
	K x = ks((S)t);
	return attR(x,sxp);
}

ZK from_pairlist_robject(SEXP sxp)
{
	K x = ktn(0,2*length(sxp));
	SEXP s = sxp;J i;
	for(i=0;i<x->n;i+=2) {
		kK(x)[i] = from_any_robject(CAR(s));
		kK(x)[i+1] = from_any_robject(TAG(s));
		s=CDR(s);
	}
	return attR(x,sxp);
}

ZK from_closure_robject(SEXP sxp)
{
	K x = from_any_robject(FORMALS(sxp));
	K y = from_any_robject(BODY(sxp));
	return attR(knk(2,x,y),sxp);
}

ZK from_language_robject(SEXP sxp)
{
	K x = knk(0);
	SEXP s = sxp;
	while (0 < length(s)) {
		x = jk(&x,from_any_robject(CAR(s)));
		s = CDR(s);
	}
	return attR(x,sxp);
}

ZK from_char_robject(SEXP sxp)
{
	K x = kpn((S)CHAR(STRING_ELT(sxp,0)),LENGTH(sxp));
	return attR(x,sxp);
}

ZK from_logical_robject(SEXP sxp)
{
	K x;
	J len = XLENGTH(sxp);
	SEXP dim= getAttrib(sxp, R_DimSymbol);
	if (isNull(dim)) {
		//Process values
		x = klogicv(len,LOGICAL(sxp));
		//Dictionary with atom values
		SEXP keyNames= getAttrib(sxp, R_NamesSymbol);
		if(!isNull(keyNames)&&len==XLENGTH(keyNames)){
    		return atom_value_dict(len, x, keyNames);
  	  	}
	  	//Normal kdb+ list
		return attR(x,sxp);
	}
	x = klogica(len,length(dim),INTEGER(dim),LOGICAL(sxp));
	SEXP dimnames= getAttrib(sxp, R_DimNamesSymbol);
	if (!isNull(dimnames))
		return attR(x,sxp);
	SEXP e;
	PROTECT(e = duplicate(sxp));
	setAttrib(e, R_DimSymbol, R_NilValue);
	x = attR(x,e);
	UNPROTECT(1);
	return x;
}

ZK from_integer_robject(SEXP sxp){
	K x;
	J len = XLENGTH(sxp);
	SEXP dim= getAttrib(sxp, R_DimSymbol);
	if (isNull(dim)) {
		//Process values
		x = kintv(len,INTEGER(sxp));
		//Dictionary with atom values
		SEXP keyNames= getAttrib(sxp, R_NamesSymbol);
		if(!isNull(keyNames)&&len==XLENGTH(keyNames)){
    		return atom_value_dict(len, x, keyNames);
  		}
		//Normal kdb+ list
		return attR(x,sxp);
	}
	x = kinta(len,length(dim),INTEGER(dim),INTEGER(sxp));
	SEXP dimnames = getAttrib(sxp, R_DimNamesSymbol);
	if (!isNull(dimnames))
		return attR(x,sxp);
	SEXP e;
	PROTECT(e = duplicate(sxp));
	setAttrib(e, R_DimSymbol, R_NilValue);
	x = attR(x,e);
	UNPROTECT(1);
	return x;
}

ZK from_double_robject(SEXP sxp){
	K x;
	I nano, bit64=isClass("integer64",sxp);
	J len = XLENGTH(sxp);
	SEXP dim= getAttrib(sxp, R_DimSymbol);
	if (isNull(dim)) {
	  //Process values
	  nano = isClass("nanotime",sxp);
      if(nano || bit64) {
        x=ktn(nano?KP:KJ,len);
        DO(len,kJ(x)[i]=INT64(sxp)[i])
        if(nano)
          DO(len,if(kJ(x)[i]!=nj)kJ(x)[i]-=epoch_offset)
      }else{
		x= kdoublev(len, REAL(sxp));
	  }
	  //Dictionary with atom values
	  SEXP keyNames= getAttrib(sxp, R_NamesSymbol);
	  if(!isNull(keyNames)&&len==XLENGTH(keyNames)){
    	return atom_value_dict(len, x, keyNames);
  	  }else if(nano || bit64){
		//Class object
		return x;
	  }
	  //Normal kdb+ list
      return attR(x, sxp);  
	}
    if(bit64){
      x= klonga(len, length(dim), INTEGER(dim), (J*)REAL(sxp));
    }else{
      x= kdoublea(len, length(dim), INTEGER(dim), REAL(sxp));
    }
	SEXP dimnames = GET_DIMNAMES(sxp);
	if (!isNull(dimnames))
		return attR(x,sxp);
	SEXP e;
	PROTECT(e = duplicate(sxp));
	setAttrib(e, R_DimSymbol, R_NilValue);
	if(bit64) classgets(e,R_NilValue);
	x = attR(x,e);
	UNPROTECT(1);
	return x;
}

ZK from_character_robject(SEXP sxp)
{
	K x;
	J i, length = XLENGTH(sxp);
	if (length == 1)
		x = kp((char*) CHAR(STRING_ELT(sxp,0)));
	else {
		x = ktn(0, length);
		for (i = 0; i < length; i++) {
			xK[i] = kp((char*) CHAR(STRING_ELT(sxp,i)));
		}
	}
  return attR(x,sxp);
}

ZK from_vector_robject(SEXP sxp)
{
	J i, length = LENGTH(sxp);
	K x = ktn(0, length);
	for (i = 0; i < length; i++) {
		kK(x)[i] = from_any_robject(VECTOR_ELT(sxp, i));
	}
    SEXP colNames= getAttrib(sxp, R_NamesSymbol);
 	if(!isNull(colNames)&&length==XLENGTH(colNames)){
    	K k= ktn(KS, length);
    	for(i= 0; i < length; i++) {
      		const char *colName= CHAR(STRING_ELT(colNames, i));
      		kS(k)[i]= ss((S) colName);
   		}
    	return xD(k,x);
 	}
  	return attR(x, sxp);
}

/*
 * various utilities
 */

/* get k string or symbol name */
static char * getkstring(K x)
{
	char *s=NULL;
	int len;
	switch (xt) {
	case -KC :
		s = calloc(2,1); s[0] = xg; break;
	case KC :
		s = calloc(1+xn,1); memmove(s, xG, xn); break;
	case -KS : // TODO: xs is already 0 terminated and fixed. can just return xs
		len = 1+strlen(xs);
		s = calloc(len,1); memmove(s, xs, len); break;
	default : krr("invalid name");
	}
	return s;
}

/*
 * convert R arrays to K lists
 * done for boolean, int, double
 */

ZK klogicv(J len, int *val) {
  K x= ktn(KB, len);
  DO(len, kG(x)[i]= (val)[i]);
  return x;
}

ZK klogica(J len, int rank, int *shape, int *val) {
  K x, y;
  J i, j, r, c, k;
  switch(rank) {
  case 1:
    x= kintv(len, val);
    break;
  case 2:
    r= shape[0];
    c= shape[1];
    x= knk(0);
    for(i= 0; i < r; i++) {
      y= ktn(KB, c);
      for(j= 0; j < c; j++)
        kG(y)[j]= val[i + r * j];
      x= jk(&x, y);
    };
    break;
  default:
    k= rank - 1;
    r= shape[k];
    c= len / r;
    x= knk(0);
    for(i= 0; i < r; i++)
      x= jk(&x, klogica(c, k, shape, val + c * i));
  }
  return x;
}

ZK kintv(J len, int *val)
{
	K x = ktn(KI, len);
	DO(len,kI(x)[i]=(val)[i]);
	return x;
}

ZK kinta(J len, int rank, int *shape, int *val)
{
	K x,y;
	J i,j,r,c,k;
	switch (rank) {
		case 1 :
		  x = kintv(len,val);
		  break;
		case 2 :
		  r = shape[0];
		  c = shape[1];
		  x = knk(0);
		  for (i=0;i<r;i++) {
			y = ktn(KI,c);
			for (j=0;j<c;j++)
			  kI(y)[j] = val[i+r*j];
			x = jk(&x,y);
		  };
		  break;
		default :
		  k = rank-1;
		  r = shape[k];
		  c = len/r;
		  x = knk(0);
		  for (i=0;i<r;i++)
			x = jk(&x,kinta(c,k,shape,val+c*i));
	}
	return x;
}

ZK klonga(J len, int rank, int *shape, J*val) {
  K x, y;
  J i, j, r, c, k;
  switch(rank) {
  case 1:
    x= ktn(KJ,len);
    DO(len, kJ(x)[i]=val[i])
    break;
  case 2:
    r= shape[0];
    c= shape[1];
    x= ktn(0,r);
    for(i= 0; i < r; i++) {
      y= ktn(KJ, c);
      for(j= 0; j < c; j++)
        kJ(y)[j]= val[i + r * j];
      kK(x)[i]=y;
    };
    break;
  default:
    k= rank - 1;
    r= shape[k];
    c= len / r;
    x= ktn(0,r);
    for(i= 0; i < r; i++)
      kK(x)[i] = klonga(c, k, shape, val + c * i);
  }
  return x;
}

ZK kdoublev(J len, double *val)
{
	K x = ktn(KF, len);
	DO(len,kF(x)[i]=(val)[i]);
	return x;
}

ZK kdoublea(J len, int rank, int *shape, double *val)
{
	K x,y;
	J i,j,r,c,k;
	switch (rank) {
		case 1 : x = kdoublev(len,val); break;
		case 2 :
			r = shape[0]; c = shape[1]; x = knk(0);
			for (i=0;i<r;i++) {
				y = ktn(KF,c);
				for (j=0;j<c;j++)
					kF(y)[j] = val[i+r*j];
				x = jk(&x,y);
			}; break;
		default :
			k = rank-1; r = shape[k]; c = len/r; x = knk(0);
			for (i=0;i<r;i++)
				x = jk(&x,kdoublea(c,k,shape,val+c*i));
	}
	return x;
}

/*
 * The public interface used from Q.
 */
static I spair[2];
void* pingthread;

V* pingmain(V* v){
	while(1){
		nanosleep(&(struct timespec){.tv_sec=0,.tv_nsec=1000000}, NULL);
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
K ropen(K x)
{
	if(!RLOAD) return krr("main thread only");
	if (ROPEN >= 0) return ki(ROPEN);
	int s,mode=0;	char *argv[] = {"R","--slave"};
	if (x && (-KI ==x->t || -KJ ==x->t)) mode=(x->t==-KI?x->i:x->j)!=0;
	if (mode) argv[1] = "--verbose";
	int argc = sizeof(argv)/sizeof(argv[0]);
	s=Rf_initEmbeddedR(argc, argv);
	if (s<0) return krr("open failed");
	if(dumb_socketpair(spair, 1) == -1){
    return krr("Init failed for socketpair");
  }
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
K rcmd(K x) { return rexec(0,x); }
K rget(K x) { return rexec(1,x); }

static char* ParseError[5]={"null","ok","incomplete","error","eof"};


K rexec(int type,K x)
{
	if(!RLOAD) return krr("main thread only");
	if (ROPEN < 0) ropen(NULL);
	SEXP e, p, r, xp;
	char rerr[256];extern char	R_ParseErrorMsg[256];
	int error;
	ParseStatus status;
	if(abs(x->t)==KS) e=from_symbol_kobject(x);
	else if(abs(x->t)==KC) e=from_string_kobject(x);
	else return krr("type");
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
	if (type==1) return from_any_robject(r);
	return (K)0; //return knk(0) for cmd success?
}

K rset(K x,K y) {
	if(!RLOAD) return krr("main thread only");
	if (ROPEN < 0) ropen(NULL);
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
