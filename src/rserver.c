/*
 * R server for Q
 */
/*
 * The public interface used from Q.
 * https://cran.r-project.org/doc/manuals/r-release/R-ints.pdf
 * https://cran.r-project.org/doc/manuals/r-release/R-exts.html
 */
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
ZK from_char_robject(SEXP);
ZK from_logical_robject(SEXP);
ZK from_integer_robject(SEXP);
ZK from_double_robject(SEXP);
ZK from_character_robject(SEXP);
ZK from_vector_robject(SEXP);
ZK from_raw_robject(SEXP sxp);
ZK from_nyi_robject(S m,SEXP sxp);

/**
 * S3 class membership test
 * @param class_ see if object has this class name
 * @param s object to test
 */
Rboolean isClass(const char *class_, SEXP s) {
  SEXP klass;
  int i;
  if(OBJECT(s)) {
    // for an S3 object, the class attribute is a character vector, e.g. c("lm", "oldClass").
    klass= getAttrib(s, R_ClassSymbol);
    // iterate through each class name & check if anything matching provided class
    for(i= 0; i < length(klass); i++)
      if(!strcmp(CHAR(STRING_ELT(klass, i)), class_))
        return TRUE;
  }
  return FALSE;
}

ZI get_date(I type,SEXP sxp,I i){
    if(type==INTSXP){
        i=INTEGER(sxp)[i];
        R i-(i==ni?0:kdbDateOffset);
    }
    F f=REAL(sxp)[i];
    R ISNA(f)?NA_INTEGER:(I)f-kdbDateOffset;
}

ZK from_date_robject(SEXP sxp) {
  J length= XLENGTH(sxp);
  int type= TYPEOF(sxp);
  if (length==1){
      return kd(get_date(type,sxp,0));
  }
  K x=ktn(KD,length);
  DO(length,kI(x)[i]=get_date(type,sxp,i))
  return x;
}

/**
 * @brief Build atom value dictionary.
 */
ZK atom_value_dict(J len, K v, SEXP keys){
  K k= ktn(KS, len);
  for(J i= 0; i < len; i++) {
    const char *keyName= CHAR(STRING_ELT(keys, i));
    kS(k)[i]= ss((S) keyName);
  }
  return xD(k,v);
}

ZI kdbSecOffset = 946684800;
ZI sec2day = 86400;

ZF get_datetime_ct(F f){
    return (f-kdbSecOffset)/sec2day;
}

ZK from_datetime_ct_robject(SEXP sxp) {
  K x;
  J length = XLENGTH(sxp);
  if (length==1){
      return kz(get_datetime_ct(REAL(sxp)[0]));
  }
  x = ktn(KZ,length);
  DO(length,kF(x)[i]=get_datetime_ct(REAL(sxp)[i]));
  return x;
}

ZF get_datetime_lt(K x,I i){
    //Relying on the order of tm key
    struct tm dttm;
    dttm.tm_sec  =kK(x)[0]->t==KF?kF(kK(x)[0])[i]:kK(x)[0]->f;
    dttm.tm_min  =kK(x)[1]->t==KI?kI(kK(x)[1])[i]:kK(x)[1]->i;
    dttm.tm_hour =kK(x)[2]->t==KI?kI(kK(x)[2])[i]:kK(x)[2]->i;
    dttm.tm_mday =kK(x)[3]->t==KI?kI(kK(x)[3])[i]:kK(x)[3]->i;
    dttm.tm_mon  =kK(x)[4]->t==KI?kI(kK(x)[4])[i]:kK(x)[4]->i;
    dttm.tm_year =kK(x)[5]->t==KI?kI(kK(x)[5])[i]:kK(x)[5]->i;
    dttm.tm_wday =kK(x)[6]->t==KI?kI(kK(x)[6])[i]:kK(x)[6]->i;
    dttm.tm_yday =kK(x)[7]->t==KI?kI(kK(x)[7])[i]:kK(x)[7]->i;
    dttm.tm_isdst=kK(x)[8]->t==KI?kI(kK(x)[8])[i]:kK(x)[8]->i;
    return (((F)mktime(&dttm)-kdbSecOffset)/sec2day);
}

ZK from_datetime_lt_robject(SEXP sxp) {
  K x,res;
  J i, key_length= XLENGTH(sxp);
  x= ktn(0, key_length);
  for(i= 0; i < key_length; ++i)
    kK(x)[i]= from_any_robject(VECTOR_ELT(sxp, i));
  J element_length=kK(x)[0]->t==KF?kK(x)[0]->n:1;
  if(element_length==1){
      res=kz(get_datetime_lt(x,0));
  }else{
      res=ktn(KZ, element_length);
      for(i=0; i < element_length; i++){
          kF(res)[i]=get_datetime_lt(x,i);
      }
  }
  r0(x);
  return res;
}

ZK from_datetime_robject(SEXP sxp) {
  if(isClass("POSIXct", sxp))
    return from_datetime_ct_robject(sxp);
  else
    return from_datetime_lt_robject(sxp);
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

/**
 * Convert R object to K object
 */
ZK from_any_robject(SEXP sxp)
{
	K result = 0;
	int type = TYPEOF(sxp);
    if(isClass("data.frame", sxp)){
        return from_frame_robject(sxp);
    }
    if(isClass("Date", sxp)){
        return from_date_robject(sxp);
    }
    if(isClass("POSIXt", sxp)){
        return from_datetime_robject(sxp);
    }
    if(isClass("factor", sxp)){
        return from_factor_robject(sxp);
    }
	switch (type) {
	case NILSXP : return from_null_robject(sxp); break; 		/* nil = NULL */
	case SYMSXP : return from_symbol_robject(sxp); break;    	/* symbols */
	case LISTSXP : return from_pairlist_robject(sxp); break; 		/* lists of dotted pairs */
	case CLOSXP : return from_closure_robject(sxp); break;		/* closures */
	case ENVSXP : return from_nyi_robject("environment",sxp); break;	/* environments */
	case PROMSXP : return from_nyi_robject("promise",sxp); break; 		/* promises: [un]evaluated closure arguments */
	case LANGSXP : return from_language_robject(sxp); break; 	/* language constructs (special lists) */
	case SPECIALSXP : return from_nyi_robject("special",sxp); break; 	/* special forms */
	case BUILTINSXP : return from_nyi_robject("builtin",sxp); break; 	/* builtin non-special forms */
	case CHARSXP : return from_char_robject(sxp); break; 		/* "scalar" string type (internal only)*/
	case LGLSXP : return from_logical_robject(sxp); break; 		/* logical vectors */
	case INTSXP : return from_integer_robject(sxp); break; 		/* integer vectors */
	case REALSXP : return from_double_robject(sxp); break; 		/* real variables */
	case CPLXSXP : return from_nyi_robject("complex", sxp); break; 		/* complex variables */
	case STRSXP : return from_character_robject(sxp); break; 	/* string vectors */
	case DOTSXP : return from_nyi_robject("dot",sxp); break; 		/* dot-dot-dot object */
	case ANYSXP : return error_broken_robject(sxp); break; 		/* make "any" args work */
	case VECSXP : return from_vector_robject(sxp); break; 		/* generic vectors */
	case EXPRSXP : return from_nyi_robject("exprlist",sxp); break; 	/* sxps vectors */
	case BCODESXP : return from_nyi_robject("bcode",sxp); break; 	/* byte code */
	case EXTPTRSXP : return from_nyi_robject("external",sxp); break; 	/* external pointer */
	case WEAKREFSXP : return error_broken_robject(sxp); break; 	/* weak reference */
	case RAWSXP : return from_raw_robject(sxp); break; 		/* raw bytes */
	case S4SXP : return from_nyi_robject("s4",sxp); break; 		/* S4 non-vector */

	case NEWSXP : return error_broken_robject(sxp); break;		/* fresh node created in new page */
    case FREESXP : return error_broken_robject(sxp); break;		/* node released by GC */
	case FUNSXP : return from_nyi_robject("fun",sxp); break; 		/* Closure or Builtin */
	}
	return result;
}

/**
 * Create a 2 element list, first element is a dict of attributes, send element is the value
 */
ZK addattR (K x,SEXP att)
{
	// attrs are pairlists: LISTSXP
	K u = from_pairlist_robject(att);
	return knk(2,u,x);
}

/**
 * If x has an attribute associated, return 2 element list, first element is a dict of attributes, send element is the value
 * otherwise return x
 */
ZK attR(K x,SEXP sxp)
{
    // fetches the attributes pairlist attached to the R object
	SEXP att = ATTRIB(sxp);
	if (isNull(att)) return x;
	return addattR(x,att);
}

ZK error_broken_robject(SEXP sxp)
{
	return krr("Broken R object.");
}

/**
 * Convert R object to K (one that isnt properly processed yet e.g. promise)
 * @param marker name given to data type
 * @param sxp r object
 */
ZK from_nyi_robject(S marker, SEXP sxp){
	return attR(kp((S)Rf_type2char(TYPEOF(sxp))),sxp);
}

/**
 * Convert R bytes to K byte vector
 */
ZK from_raw_robject(SEXP sxp)
{
    if (XLENGTH(sxp)==1){
        return kg(RAW(sxp)[0]);
    }
	K x = ktn(KG,XLENGTH(sxp));
	DO(xn,kG(x)[i]=RAW(sxp)[i])
	return x;
}

/**
 * Convert R null to K empty list
 * NULL in R(R_NilValue): often used as generic zero length vector
 * NULL objects cannot have attributes and attempting to assign one by attr gives an error
 */
ZK from_null_robject(SEXP sxp)
{
	return knk(0);
}

/**
 * Convert R symbol to K symbol
 */
ZK from_symbol_robject(SEXP sxp)
{
	const char* t = CHAR(PRINTNAME(sxp));
	K x = ks((S)t);
	return attR(x,sxp);
}

/**
 * Convert R list of dotted pairs to K list
 */
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

/**
 * Convert R close to K list
 */
ZK from_closure_robject(SEXP sxp)
{
	K x = from_any_robject(FORMALS(sxp));
	K y = from_any_robject(BODY(sxp));
	return attR(knk(2,x,y),sxp);
}

/**
 * Convert R language object to K list
 */
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

/**
 * Convert R char to K char vector
 */
ZK from_char_robject(SEXP sxp)
{
	K x = kpn((S)CHAR(STRING_ELT(sxp,0)),LENGTH(sxp));
	return attR(x,sxp);
}

/**
 * Convert to boolean vector
 */
ZK klogicv(J len, int *val) {
  if(len==1){
      R kb(*val);
  }
  K x= ktn(KB, len);
  DO(len, kG(x)[i]= (val)[i]);
  return x;
}

ZK klogica(J len, int rank, int *shape, int *val) {
  K x, y;
  J i, j, r, c, k;
  switch(rank) {
    case 1:
      x= klogicv(len, val);
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

/**
 * Convert R logical  to K boolean vector
 */
ZK from_logical_robject(SEXP sxp)
{
	K x;
	J len = XLENGTH(sxp);
    SEXP dim= getAttrib(sxp, R_DimSymbol);
	if (isNull(dim)) {
        // convert to boolean vector
        x = klogicv(len,LOGICAL(sxp));
        //Dictionary with atom values
        SEXP keyNames= getAttrib(sxp, R_NamesSymbol);
        if(!isNull(keyNames)&&len==XLENGTH(keyNames))
            return atom_value_dict(len, x, keyNames);
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

/**
 * Convert to integer vector
 */
ZK kintv(J len, int *val)
{
    if(len==1){
        R ki(*val);
    }
	K x = ktn(KI, len);
	DO(len,kI(x)[i]=(val)[i]);
	return x;
}

ZK kinta(J len, int rank, int *shape, int *val)
{
	K x,y;
	J i,j,r,c,k;
	switch (rank) {
		case 1 : x = kintv(len,val); break;
		case 2 :
			r = shape[0]; c = shape[1]; x = knk(0);
			for (i=0;i<r;i++) {
				y = ktn(KI,c);
				for (j=0;j<c;j++)
					kI(y)[j] = val[i+r*j];
				x = jk(&x,y);
			}; break;
		default :
			k = rank-1; r = shape[k]; c = len/r; x = knk(0);
			for (i=0;i<r;i++)
				x = jk(&x,kinta(c,k,shape,val+c*i));
	}
	return x;
}

/**
 * Convert R integer object to K integer vector
 */
ZK from_integer_robject(SEXP sxp)
{
	K x;
	J len = XLENGTH(sxp);
	SEXP dim = GET_DIM(sxp);
	if (isNull(dim)) {
		x = kintv(len,INTEGER(sxp));
        //Dictionary with atom values
        SEXP keyNames= getAttrib(sxp, R_NamesSymbol);
        if(!isNull(keyNames)&&len==XLENGTH(keyNames))
            return atom_value_dict(len, x, keyNames);
        //Normal kdb+ list
		return attR(x,sxp);
	}
	x = kinta(len,length(dim),INTEGER(dim),INTEGER(sxp));
	SEXP dimnames = GET_DIMNAMES(sxp);
	if (!isNull(dimnames))
		return attR(x,sxp);
	SEXP e;
	PROTECT(e = duplicate(sxp));
	SET_DIM(e, R_NilValue);
	x = attR(x,e);
	UNPROTECT(1);
	return x;
}

/**
 * Convert to long vector
 */
ZK klongv(J len, J*val)
{
    if(len==1){
        R kj(*val);
    }
    K x = ktn(KJ, len);
    DO(len,kJ(x)[i]=(val)[i]);
    return x;
}

ZK klonga(J len, int rank, int *shape, J*val) {
  K x, y;
  J i, j, r, c, k;
  switch(rank) {
  case 1:
    x=klongv(len,val);
    break;
  case 2:
    r= shape[0]; c= shape[1]; x= ktn(0,r);
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
    x= knk(0);
    for(i= 0; i < r; i++)
      kK(x)[i] = klonga(c, k, shape, val + c * i);
  }
  return x;
}

/**
 * Convert to float vector
 */
ZK kdoublev(J len, double *val)
{
    if(len==1){
        R kf(*val);
    }
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

ZJ get_nano(J t){
    R t-(t==nj?0:epoch_offset);
}

/**
 * Convert R real (double) object to K float/long/timestamp objects
 */
ZK from_double_robject(SEXP sxp)
{
	K x;
    I bit64=isClass("integer64",sxp);
	J len = XLENGTH(sxp);
    SEXP dim = GET_DIM(sxp);
    if (isNull(dim)) {
        if (bit64)
            return klongv(len,(J*)REAL(sxp));
        if (isClass("nanotime",sxp)){
            if(len==1){
                R ktj(-KP,get_nano(INT64(sxp)[0]));
            }
            x=ktn(KP,len);
            DO(len,kJ(x)[i]=get_nano(INT64(sxp)[i]));
            return x;
        }
        x = kdoublev(len,REAL(sxp));
        return attR(x,sxp);
    }
    if (bit64) {
        /* For arrays with dimensions - not commonly used with integer64 */
        return klonga(len, length(dim), INTEGER(dim), (J*)REAL(sxp));  // Don't attach R attributes for integer64
    }
    // convert to float
	x = kdoublea(len,length(dim),INTEGER(dim),REAL(sxp));
	SEXP dimnames = GET_DIMNAMES(sxp);
	if (!isNull(dimnames))
		return attR(x,sxp);
	SEXP e;
	PROTECT(e = duplicate(sxp));
	SET_DIM(e, R_NilValue);
	x = attR(x,e);
	UNPROTECT(1);
	return x;
}

/**
 * Convert R char to K char or list
 */
ZK from_character_robject(SEXP sxp)
{
	K x;
	J i, length = XLENGTH(sxp);
	if (length == 1)
		x = kp((char*) CHAR(STRING_ELT(sxp,0)));
	else {
		x = ktn(0, length);
		for (i = 0; i < length; i++)
			kK(x)[i] = kp((char*) CHAR(STRING_ELT(sxp,i)));
	}
  return attR(x,sxp);
}

/**
 * Convert R generic vector to K list
 */
ZK from_vector_robject(SEXP sxp)
{
	J i, length = LENGTH(sxp);
	K x = ktn(0, length);
	for (i = 0; i < length; i++) {
		xK[i] = from_any_robject(VECTOR_ELT(sxp, i));
	}
    SEXP colNames= getAttrib(sxp, R_NamesSymbol);
    if(!isNull(colNames)&&length==XLENGTH(colNames)) {
        K k= ktn(KS, length);
        for(i= 0; i < length; i++) {
            const char *colName= CHAR(STRING_ELT(colNames, i));
            kS(k)[i]= ss((S) colName);
        }
    return xD(k,x);
    }
    return attR(x,sxp);
}

/**
 * get k string or symbol name 
 */
static char * getkstring(K x)
{
	char *s=NULL;
	switch (xt) {
	case -KC :
		s = calloc(2,1); s[0] = xg; break;
	case KC :
		s = calloc(1+xn,1); memmove(s, xG, xn); break;
	case -KS : // TODO: xs is already 0 terminated and fixed. can just return xs
		int len = 1+strlen(xs);
		s = calloc(len,1); memmove(s, xs, len); break;
	default : krr("invalid name");
	}
	return s;
}

/*
 * The public interface used from Q.
 */
#ifdef WIN32
static SOCKET spair[2];
#else
static int spair[2];
#endif
void* pingthread;

V* pingmain(V* v){
	while(1){
#ifdef WIN32
        Sleep(100);
#elif __APPLE__
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        uint64_t nanoseconds = 100000000; 
        mach_wait_until(mach_absolute_time() + (nanoseconds * timebase.denom / timebase.numer));
#else
        struct timespec deadline;
        deadline.tv_sec = 0;
        deadline.tv_nsec = 100000000;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &deadline, NULL);
#endif
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
 * note that embedded R can be initialised once. No open/close/open supported
 * http://r.789695.n4.nabble.com/Terminating-and-restarting-an-embedded-R-instance-possible-td4641823.html
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

static char* ParseError[5]={"null","ok","incomplete","error","eof"};

/**
 * Execute the expression 
 * @param type 1 will return the result
 * @param x expression to evaluate (string,char,sym)
 */
ZK rexec(int type,K x)
{
	if(!RLOAD) return krr("main thread only");
	if (ROPEN < 0) ropen(NULL);
	SEXP e, p, r, xp;
	char rerr[256];extern char	R_ParseErrorMsg[256];
	int error;
	ParseStatus status;
    // create R object from K symbol/string/char
	if(abs(x->t)==KS) e=from_symbol_kobject(x);
	else if(abs(x->t)==KC) e=from_string_kobject(x);
	else return krr("type");
	PROTECT(e);
    // parse 1 expression from e (if contains multiple, only do first)
	PROTECT(p=R_ParseVector(e, 1, &status, R_NilValue));
	if (status != PARSE_OK) {
		UNPROTECT(2);
		snprintf(rerr,sizeof(rerr),"%s: %s",ParseError[status], R_ParseErrorMsg);
		return krr(rerr);
	}
	PROTECT(xp=VECTOR_ELT(p, 0));
    // evaluate the first expression
	r=R_tryEvalSilent(xp, R_GlobalEnv, &error);
	UNPROTECT(3);
	R_ProcessEvents();
	if (error) {
		snprintf(rerr,sizeof(rerr),"eval error: %s",R_curErrorBuf());
		return krr(rerr);
	}
	if (type==1) return from_any_robject(r); // convert result to q, return result
	return (K)0; //return knk(0) for cmd success?
}

K rcmd(K x) { return rexec(0,x); }
K rget(K x) { return rexec(1,x); }

/**
 * set the R variable named by x (char,char vector,sym), to the value y
 */
K rset(K x,K y) {
	if(!RLOAD) return krr("main thread only");
	if (ROPEN < 0) ropen(NULL);
	ParseStatus status;
	SEXP txt, sym, val;
	char rerr[256];extern char	R_ParseErrorMsg[256];
	char *name = getkstring(x);
	// generate symbol to check name is valid 
	PROTECT(txt=allocVector(STRSXP, 1));
	SET_STRING_ELT(txt, 0, mkChar(name));
	free(name);
    // parse the string as R code to validate it is a symbol
	PROTECT(sym = R_ParseVector(txt, 1, &status,R_NilValue));
	if (status != PARSE_OK) {
		UNPROTECT(2);
		snprintf(rerr,sizeof(rerr),"%s: %s",ParseError[status], R_ParseErrorMsg);
		return krr(rerr);
	}
    // check the parsed code is a symbol (not "a+b", "foo()", etc)
	if(SYMSXP != TYPEOF(VECTOR_ELT(sym,0))){
		UNPROTECT(2);
		return krr("nyi");
	}
	// use the official print name
	const char *c = CHAR(PRINTNAME(VECTOR_ELT(sym,0)));
    // convert K object into R object
	PROTECT(val = from_any_kobject(y));
    // create variable in global env (install interns the sym, bind to value)
	defineVar(install(c),val,R_GlobalEnv);
	UNPROTECT(3);
	R_ProcessEvents();
	return (K)0;
}

static K vec_list(K x,I i){return r1(kK(x)[i]);}
static K vec_bool(K x,I i){return kb(kG(x)[i]);}
static K vec_guid(K x,I i){return ku(kU(x)[i]);}
static K vec_byte(K x,I i){return kg(kG(x)[i]);}
static K vec_short(K x,I i){return kh(kH(x)[i]);}
static K vec_int(K x,I i){return ki(kI(x)[i]);}
static K vec_long(K x,I i){return kj(kJ(x)[i]);}
static K vec_real(K x,I i){return ke(kE(x)[i]);}
static K vec_double(K x,I i){return kf(kF(x)[i]);}
static K vec_char(K x,I i){return kc(kC(x)[i]);}
static K vec_sym(K x,I i){return ks(kS(x)[i]);}
static K vec_timestamp(K x,I i){return ktj(-KP,kJ(x)[i]);}
static K vec_month(K x,I i){K r=ki(kI(x)[i]);r->t=-KM;return r;}
static K vec_date(K x,I i){return kd(kI(x)[i]);}
static K vec_datetime(K x,I i){return kz(kF(x)[i]);}
static K vec_timespan(K x,I i){return ktj(-KN,kJ(x)[i]);}
static K vec_minute(K x,I i){K r=ki(kI(x)[i]);r->t=-KU;return r;}
static K vec_second(K x,I i){K r=ki(kI(x)[i]);r->t=-KV;return r;}
static K vec_time(K x,I i){return kt(kI(x)[i]);}

typedef K(*k_vec_function)(K,I);

static k_vec_function k_types[] = {
    vec_list,
    vec_bool,
    vec_guid,
    vec_byte, // not used, placeholder
    vec_byte,
    vec_short,
    vec_int,
    vec_long,
    vec_real,
    vec_double,
    vec_char,
    vec_sym,
    vec_timestamp,
    vec_month,
    vec_date,
    vec_datetime,
    vec_timespan,
    vec_minute,
    vec_second,
    vec_time
};

K rfunc(K x,K y){
    if(!RLOAD) return krr("main thread only");
    if (ROPEN < 0) ropen(NULL);
    if (y->t!=XT&&y->t!=XD&&abs(y->t)>KT) return krr("type");
    SEXP call,res,args = R_NilValue;
    int error,p=0;
    char rerr[256];
    char *name = getkstring(x);
    if (y->t<0||y->t==XT||y->t==XD){
        PROTECT(args = from_any_kobject(y));
        PROTECT(call = Rf_lang2(Rf_install(name), args));
        p=2;
    }else{
        if (y->n) {
            PROTECT(args = Rf_allocList(y->n));
            p++;
            SEXP node = args;
            for (R_xlen_t i = 0; i < y->n; ++i) {
                K tt=k_types[y->t](y,i);
                SEXP val = PROTECT(from_any_kobject(tt));
                r0(tt);
                SETCAR(node, val);
                node = CDR(node);
                UNPROTECT(1); // val
            }
        }
        PROTECT(call = Rf_lcons(Rf_install(name), args));
        p++;
    }
    PROTECT(res = R_tryEval(call, R_GlobalEnv, &error));
    p++;
    if (error) {
        snprintf(rerr,sizeof(rerr),"run error: %s",R_curErrorBuf());
        UNPROTECT(p);
        return krr(rerr);
    }
    K r=from_any_robject(res);
    UNPROTECT(p);
    R_ProcessEvents();
    return r;
}

__attribute__((constructor)) V __attach(V) {RLOAD=1;}
