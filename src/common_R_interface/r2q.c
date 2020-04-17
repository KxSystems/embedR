/*-----------------------------------------------*/
/*  File: r2q.c                                  */
/*  Overview: common code for R -> Q interface   */
/*-----------------------------------------------*/

/*
 * The public interface used from Q.
 * https://cran.r-project.org/doc/manuals/r-release/R-ints.pdf
 * https://cran.r-project.org/doc/manuals/r-release/R-exts.html
 */

ZK klogicv(J len, int *val);
ZK klogica(J len, int rank, int *shape, int *val);
ZK kintv(J len, int *val);
ZK kinta(J len, int rank, int *shape, int *val);
ZK klonga(J len, int rank, int *shape, J*val);
ZK kdoublev(J len, double *val);
ZK kdoublea(J len, int rank, int *shape, double *val);
ZK atom_value_dict(J len, K v, SEXP keys);

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

/*
 * Functions to derive month count since kdb epoch from day count
 */

extern bool is_leap(const int year);

int days2months(const int daycount){
  int year=2000, months=0, days=0;
  const int mdays[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  while(true){
    if(daycount < days+(is_leap(year)?366:365))
      break;
    days+=is_leap(year)?366:365;
    months+=12;
    year++;
  }
  for(int i= 0; i < 12; i++){
    if(days < daycount){
      if(i==1)
        days+=is_leap(year)?29:28;
      else
        days+=mdays[i];
      months+=1;
    }
    else
      break;
  }
  return months;
}

/*
 * Utility functions to identify class and unit
 */

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

/*
 * Utility functions to handle attribute
 */

//extern ZK from_pairlist_robject(SEXP sxp);

/* add attribute */
ZK addattR (K x,SEXP att) {
  // attrs are pairlists: LISTSXP
  K u = from_pairlist_robject(att);
  return knk(2,u,x);
}

/* add attribute if any */
ZK attR(K x,SEXP sxp) {
  SEXP att = ATTRIB(sxp);
  if (isNull(att))
    return x;
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

/*
 * Conversopn from R to Q
 */

 ZK from_any_robject(SEXP sxp){
  if(isClass("data.frame", sxp))
    return from_frame_robject(sxp);
  if(isClass("factor", sxp))
    return from_factor_robject(sxp);
  if(isClass("Date", sxp))
    return from_date_robject(sxp);
  if(isClass("POSIXt", sxp))
    return from_datetime_robject(sxp);
  if(isClass("difftime", sxp))
    return from_difftime_robject(sxp);
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

ZK error_broken_robject(SEXP sxp) {
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

ZK from_raw_robject(SEXP sxp) {
  K x = ktn(KG,XLENGTH(sxp));
  DO(xn,kG(x)[i]=RAW(sxp)[i])
  return x;
}


ZK from_date_robject(SEXP sxp) {
  K x;
  J length= XLENGTH(sxp);
  x= ktn(isClass("month", sxp)?KM:KD,length);
  int type= TYPEOF(sxp);
  switch(type) {
    case INTSXP:
      if(isClass("month", sxp)){
        DO(length,kI(x)[i]=days2months(INTEGER(sxp)[i]-kdbDateOffset));
      }
      else{
        DO(length,kI(x)[i]=INTEGER(sxp)[i]-kdbDateOffset);
      }
      break;
    default:
      if(isClass("month", sxp)){
        DO(length,kI(x)[i]=ISNA(REAL(sxp)[i])?NA_INTEGER:days2months((I)REAL(sxp)[i]-kdbDateOffset));
      }
      else{
        DO(length,kI(x)[i]=ISNA(REAL(sxp)[i])?NA_INTEGER:(I)REAL(sxp)[i]-kdbDateOffset);
      }
  }
  return x;
}

ZK from_datetime_ct_robject(SEXP sxp) {
  K x;
  J length = XLENGTH(sxp);
  x = ktn(KZ,length);
  DO(length,kF(x)[i]=(F)(((REAL(sxp)[i]-kdbSecOffset)/sec2day)));
  return x;
}

ZK from_datetime_lt_robject(SEXP sxp) {
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
ZK from_datetime_robject(SEXP sxp) {
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

/* Wrapper function of difftime */
ZK from_difftime_robject(SEXP sxp){
  if(isUnit("secs", sxp) || isUnit("mins", sxp))
    return from_second_or_minute_robject(sxp);
  else if(isUnit("days", sxp))
    return from_days_robject(sxp);
  else /* hours */
    return from_nyi_robject(sxp);
}

/*
 * NULL in R(R_NilValue): often used as generic zero length vector
 * NULL objects cannot have attributes and attempting to assign one by attr gives an error
 */
ZK from_null_robject(SEXP sxp) {
  return knk(0);
}

ZK from_symbol_robject(SEXP sxp) {
  const char* t = CHAR(PRINTNAME(sxp));
  K x = ks((S)t);
  return attR(x,sxp);
}

ZK from_closure_robject(SEXP sxp) {
  K x = from_any_robject(FORMALS(sxp));
  K y = from_any_robject(BODY(sxp));
  return attR(knk(2,x,y),sxp);
}

ZK from_language_robject(SEXP sxp) {
  K x = knk(0);
  SEXP s = sxp;
  while (0 < length(s)) {
    x = jk(&x,from_any_robject(CAR(s)));
    s = CDR(s);
  }
  return attR(x,sxp);
}

ZK from_char_robject(SEXP sxp) {
  K x = kpn((S)CHAR(STRING_ELT(sxp,0)),LENGTH(sxp));
  return attR(x,sxp);
}

ZK from_logical_robject(SEXP sxp) {
  K x;
  J len = XLENGTH(sxp);
  SEXP dim= getAttrib(sxp, R_DimSymbol);
  if (isNull(dim)) {
    //Process values
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

ZK from_integer_robject(SEXP sxp) {
  K x;
  J len = XLENGTH(sxp);
  SEXP dim= getAttrib(sxp, R_DimSymbol);
  if (isNull(dim)) {
    //Process values
    x = kintv(len,INTEGER(sxp));
    //Dictionary with atom values
    SEXP keyNames= getAttrib(sxp, R_NamesSymbol);
    if(!isNull(keyNames)&&len==XLENGTH(keyNames))
      return atom_value_dict(len, x, keyNames);
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
  I nano, span, bit64=isClass("integer64",sxp);
  J len = XLENGTH(sxp);
  SEXP dim= getAttrib(sxp, R_DimSymbol);
  if (isNull(dim)) {
    //Process values
    nano = isClass("nanotime",sxp);
    span = isClass("timespan",sxp);
    if(nano || span || bit64) {
      x=ktn(nano?KP:(span?KN:KJ),len);
      DO(len,kJ(x)[i]=INT64(sxp)[i])
      if(nano)
        DO(len,if(kJ(x)[i]!=nj)kJ(x)[i]-=epoch_offset)
    }
    else
      x= kdoublev(len, REAL(sxp));
    //Dictionary with atom values
    SEXP keyNames= getAttrib(sxp, R_NamesSymbol);
    if(!isNull(keyNames)&&len==XLENGTH(keyNames))
      return atom_value_dict(len, x, keyNames);
    else if(nano || span || bit64)
      return x;
    //Normal kdb+ list
    return attR(x, sxp);  
  }
  if(bit64)
    x= klonga(len, length(dim), INTEGER(dim), (J*)REAL(sxp));
  else
    x= kdoublea(len, length(dim), INTEGER(dim), REAL(sxp));
  SEXP dimnames= getAttrib(sxp, R_DimNamesSymbol);
  if (!isNull(dimnames))
    return attR(x,sxp);
  SEXP e;
  PROTECT(e = duplicate(sxp));
  setAttrib(e, R_DimSymbol, R_NilValue);
  if(bit64)
    classgets(e,R_NilValue);
  x = attR(x,e);
  UNPROTECT(1);
  return x;
}

ZK from_character_robject(SEXP sxp) {
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

ZK from_vector_robject(SEXP sxp) {
  J i, length = LENGTH(sxp);
  K x = ktn(0, length);
  for (i = 0; i < length; i++)
    kK(x)[i] = from_any_robject(VECTOR_ELT(sxp, i));
  SEXP colNames= getAttrib(sxp, R_NamesSymbol);
  if(!isNull(colNames)&&length==XLENGTH(colNames)) {
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

ZK kintv(J len, int *val) {
  K x = ktn(KI, len);
  DO(len,kI(x)[i]=(val)[i]);
  return x;
}

ZK kinta(J len, int rank, int *shape, int *val) {
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

ZK kdoublev(J len, double *val) {
  K x = ktn(KF, len);
  DO(len,kF(x)[i]=(val)[i]);
  return x;
}

ZK kdoublea(J len, int rank, int *shape, double *val) {
  K x,y;
  J i,j,r,c,k;
  switch (rank) {
    case 1 : 
      x = kdoublev(len,val);
      break;
    case 2 :
      r = shape[0];
      c = shape[1];
      x = knk(0);
      for (i=0;i<r;i++) {
        y = ktn(KF,c);
        for (j=0;j<c;j++)
          kF(y)[j] = val[i+r*j];
        x = jk(&x,y);
      };
      break;
    default :
      k = rank-1;
      r = shape[k];
      c = len/r;
      x = knk(0);
      for (i=0;i<r;i++)
        x = jk(&x,kdoublea(c,k,shape,val+c*i));
  }
  return x;
}