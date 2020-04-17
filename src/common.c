/*
 * common code for Q/R interface
 */

int kx_connection=0;

/*
 * A (readable type name, R data type number) pair.
 */
struct data_types {
  char *name;
  Sint id;
};

/*
 * A mapping from readable names to R data type numbers.
 */
const struct data_types r_data_types[] = {
  {"unknown", -1},
  {"NULL", NILSXP},
  {"symbol", SYMSXP},
  {"pairlist", LISTSXP},
  {"closure", CLOSXP},
  {"environment", ENVSXP},
  {"promise", PROMSXP},
  {"language", LANGSXP},
  {"special", SPECIALSXP},
  {"builtin", BUILTINSXP},
  {"char", CHARSXP},
  {"logical", LGLSXP},
  {"integer", INTSXP},
  {"double", REALSXP},
  {"complex", CPLXSXP},
  {"character", STRSXP},
  {"...", DOTSXP},
  {"any", ANYSXP},
  {"expression", EXPRSXP},
  {"list", VECSXP},
  {"numeric", REALSXP},
  {"name", SYMSXP},
  {0, -1}
};

/*
 * Brute force search of R type table.
 * eg. 	get_type_name(LISTSXP)
 */
char* get_type_name(Sint type) {
  for (int i = 1; r_data_types[i].name != 0; i++){
    if(type == r_data_types[i].id)
      return r_data_types[i].name;
  }
  return r_data_types[0].name;
}

/*
 * Given the appropriate names, types, and lengths, create an R named list.
 */
SEXP make_named_list(char **names, SEXPTYPE *types, Sint *lengths, Sint n) {
  SEXP output, output_names, object = NULL_USER_OBJECT;
  Sint elements;
  PROTECT(output = NEW_LIST(n));
  PROTECT(output_names = NEW_CHARACTER(n));
  for(int i = 0; i < n; i++){
    elements = lengths[i];
    switch((int)types[i]) {
    case LGLSXP:
      PROTECT(object = NEW_LOGICAL(elements));
      break;
    case INTSXP:
      PROTECT(object = NEW_INTEGER(elements));
      break;
    case REALSXP:
      PROTECT(object = NEW_NUMERIC(elements));
      break;
    case STRSXP:
      PROTECT(object = NEW_CHARACTER(elements));
      break;
    case VECSXP:
      PROTECT(object = NEW_LIST(elements));
      break;
    default:
      error("Unsupported data type at %d %s\n", __LINE__, __FILE__);
    }
    SET_VECTOR_ELT(output, (Sint)i, object);
    SET_STRING_ELT(output_names, i, COPY_TO_USER_STRING(names[i]));
  }
  SET_NAMES(output, output_names);
  UNPROTECT(n+2);
  return output;
}

/*
 * Make a data.frame from a named list by adding row.names, and class
 * attribute. Uses "1", "2", .. as row.names.
 */
void make_data_frame(SEXP data) {
  SEXP class_name, row_names; Sint n;
  PROTECT(data);
  PROTECT(class_name = NEW_CHARACTER((Sint) 1));
  SET_STRING_ELT(class_name, 0, COPY_TO_USER_STRING("data.frame"));
  /* Set the row.names. */
  n = GET_LENGTH(VECTOR_ELT(data,0));
  PROTECT(row_names=NEW_INTEGER(2));
  INTEGER(row_names)[0]=NA_INTEGER; 
  INTEGER(row_names)[1]=-n;
  setAttrib(data, R_RowNamesSymbol, row_names);
  SET_CLASS(data, class_name);
  UNPROTECT(3);
}

/* for datetime */
static SEXP setdatetimeclass(SEXP sxp) {
  SEXP datetimeclass = PROTECT(allocVector(STRSXP,2));
  SET_STRING_ELT(datetimeclass, 0, mkChar("POSIXt"));
  SET_STRING_ELT(datetimeclass, 1, mkChar("POSIXct"));
  classgets(sxp, datetimeclass);
  UNPROTECT(1);
  return sxp;
}

/* for timestamp */
static SEXP settimestampclass(SEXP sxp) {
  SEXP classValue;
  SEXP tag = PROTECT(mkString(".S3Class"));
  SEXP val = PROTECT(mkString("integer64"));
  setAttrib(sxp, tag, val);
  UNPROTECT(2);
  classValue= PROTECT(mkString("nanotime"));
  tag = PROTECT(mkString("package"));
  val = PROTECT(mkString("nanotime"));
  setAttrib(classValue, tag, val);
  classgets(sxp, classValue);
  UNPROTECT(3);
  return asS4(sxp,TRUE,0);
}

static SEXP R_UnitsSymbol = NULL;
static SEXP R_TzSymbol = NULL;

/* for timespan, minute, second */
//Available units: "secs", "mins", "hours", "days", "weeks"
static SEXP setdifftimeclass(SEXP sxp, char* units) {
  SEXP difftimeclass= PROTECT(allocVector(STRSXP, 1));
  SET_STRING_ELT(difftimeclass, 0, mkChar("difftime"));
  classgets(sxp, difftimeclass);
  if (R_UnitsSymbol == NULL)
    R_UnitsSymbol = install("units");
  SEXP difftimeunits= PROTECT(allocVector(STRSXP, 1));
  SET_STRING_ELT(difftimeunits, 0, mkChar(units));
  setAttrib(sxp, R_UnitsSymbol, difftimeunits);
  UNPROTECT(2);
  return sxp;
}

/* for setting timezone */
static SEXP settimezone(SEXP sxp, char* tzone) {
  SEXP timezone= PROTECT(allocVector(STRSXP, 1));
  SET_STRING_ELT(timezone, 0, mkChar(tzone));
  if (R_TzSymbol == NULL)
    R_TzSymbol = install("tzone");
  setAttrib(sxp, R_TzSymbol, timezone);
  UNPROTECT(1);
  return sxp;
}

/* for date */
static SEXP setdateclass(SEXP sxp) {
  SEXP timeclass= PROTECT(allocVector(STRSXP, 1));
  SET_STRING_ELT(timeclass, 0, mkChar("Date"));
  classgets(sxp, timeclass);
  UNPROTECT(1);
  return sxp;
}

/* month */
static SEXP setmonthclass(SEXP sxp){
  SEXP timeclass= PROTECT(allocVector(STRSXP, 2));
  SET_STRING_ELT(timeclass, 0, mkChar("Date"));
  SET_STRING_ELT(timeclass, 1, mkChar("month"));
  classgets(sxp, timeclass);
  UNPROTECT(1);
  return sxp;
}

/* for timespan */
static SEXP settimespanclass(SEXP sxp) {
  SEXP timeclass= PROTECT(allocVector(STRSXP, 2));
  SET_STRING_ELT(timeclass, 0, mkChar("integer64"));
  SET_STRING_ELT(timeclass, 1, mkChar("timespan"));
  classgets(sxp, timeclass);
  UNPROTECT(1);
  return sxp;
}

/*
 * We have functions that turn any K object into the appropriate R SEXP.
 */
static SEXP from_any_kobject(K object);
static SEXP error_broken_kobject(K);
static SEXP from_list_of_kobjects(K);
static SEXP from_bool_kobject(K);
static SEXP from_byte_kobject(K);
static SEXP from_guid_kobject(K);
static SEXP from_string_kobject(K);
static SEXP from_string_column_kobject(K);
static SEXP from_short_kobject(K);
static SEXP from_int_kobject(K);
static SEXP from_long_kobject(K);
static SEXP from_float_kobject(K);
static SEXP from_double_kobject(K);
static SEXP from_symbol_kobject(K);
static SEXP from_month_kobject(K);
static SEXP from_date_kobject(K);
static SEXP from_datetime_kobject(K);
static SEXP from_minute_kobject(K);
static SEXP from_second_kobject(K);
static SEXP from_time_kobject(K);
static SEXP from_timespan_kobject(K);
static SEXP from_timestamp_kobject(K);
static SEXP from_columns_kobject(K object);
static SEXP from_dictionary_kobject(K);
static SEXP from_table_kobject(K);

/*
 * Function used in the conversion of kdb guid to R char array
 */
static K guid_2_char(K);

/*
 * Functions to derive day count since kdb+ epoch from month count
 */

bool is_leap(const int year){
  return (year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0);
}

int months2days(const int monthcount){
  int days=0;
  const int mdays[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  const int years = monthcount / 12;
  for(int i= 0; i < years; i++)
    days+=is_leap(2000+i)?366:365;
  const int this_year = 2000+years;
  const int months= monthcount % 12;
  for(int i=0; i < months; i++){
    if(i == 1)
      days+=is_leap(this_year)?29:28;
    else
      days+=mdays[i];
  }
  return days;
}

/*
 * An array of functions that deal with kdbplus data types. Note that the order
 * is very important as we index it based on the kdb+ type number in the K object.
 */
typedef SEXP(*conversion_function)(K);

conversion_function kdbplus_types[] = {
  from_list_of_kobjects,  from_bool_kobject,     from_guid_kobject,
  error_broken_kobject,   from_byte_kobject,     from_short_kobject,
  from_int_kobject,       from_long_kobject,     from_float_kobject,
  from_double_kobject,    from_string_kobject,   from_symbol_kobject,
  from_timestamp_kobject, from_month_kobject,    from_date_kobject,
  from_datetime_kobject,  from_timespan_kobject, from_minute_kobject,
  from_second_kobject,    from_time_kobject
};

/*
 * Convert K object to R object
 */
static SEXP from_any_kobject(K x) {
  SEXP result;
  int type = abs(x->t);
  if (XT == type)
    result = from_table_kobject(x);
  else if (XD == type)
    result = from_dictionary_kobject(x);
  else if(101 == type)
    result= R_NilValue;
  else if(105 == type)
    result= from_int_kobject(ki(0));
  else if (type <= KT)
    result = kdbplus_types[type](x);
  else if (KT<type && type < 77){
    K t = k(0,"value",r1(x),(K)0);
    if(t && t->t!=-128) {
      result = from_any_kobject(t);
      r0(t);
    }
    else 
      result = error_broken_kobject(x);
  }
  else if(77<type && type < XT){
    K t = k(0,"{(::) each x}",r1(x),(K)0);
    if(t && t->t!=-128) {
      result = from_any_kobject(t);
      r0(t);
    }
    else 
      result = error_broken_kobject(x);
  }
  else
    result = error_broken_kobject(x);
  return result;
}

/*
 * Convert K columns to R object
 */
static SEXP from_columns_kobject(K x) {
  SEXP col, result;
  int type, length = x->n;
  K c;
  PROTECT(result = NEW_LIST(length));
  for (int i = 0; i < length; i++) {
    c = xK[i];
    type = abs(c->t);
    if (type == KC)
      col = from_string_column_kobject(c);
    else
      col = from_any_kobject(c);
    SET_VECTOR_ELT(result, i, col);
  }
  UNPROTECT(1);
  return result;
}

/*
 * Complain that the given K object is not valid and return "unknown".
 */
static SEXP error_broken_kobject(K broken) {
  error("Value is not a valid kdb+ object; unknown type %d\n", broken->t);
  return mkChar(r_data_types[0].name);
}

/*
 * An R list from a K list object.
 */
static SEXP from_list_of_kobjects(K x) {
  SEXP result;
  K y;
  J i, length= x->n, utype;
  PROTECT(result= allocVector(VECSXP,length));
  utype= length > 0 ? kK(x)[0]->t : 0;
  for(i= 0; i < length; i++) {
    y= kK(x)[i];
    utype= utype == y->t ? utype : 0;
    SET_VECTOR_ELT(result, i, from_any_kobject(y));
  }
  if(utype == KC)
    result= coerceVector(result, STRSXP);
  UNPROTECT(1);
  return result;
}

/*
 * These next functions have 2 main control flow paths. One for scalars and
 * one for vectors. Because of the way the data is laid out in k objects, its
 * not possible to combine them.
 *
 * We always decrement the reference count of the object as it will have been
 * incremented in the initial dispatch.
 *
 * We promote shorts and floats to larger types when converting to R (ints and
 * doubles respectively).
 */

static I scalar(K x) { return x->t < 0; }

static SEXP from_bool_kobject(K x) {
  SEXP result;
  if(scalar(x)) return ScalarLogical(x->g);
  PROTECT(result= allocVector(LGLSXP,x->n));
  for(int i= 0; i < x->n; i++)
    LOGICAL(result)[i]= kG(x)[i];
  UNPROTECT(1);
  return result;
}

static SEXP from_byte_kobject(K x) {
  SEXP result;G*r;
  if(scalar(x)) return ScalarRaw(x->g);
  PROTECT(result= allocVector(RAWSXP,x->n));
  r=RAW(result);
  for(int i= 0; i < x->n; i++)
    r[i]= kG(x)[i];
  UNPROTECT(1);
  return result;
}

static SEXP from_guid_kobject(K x){
  SEXP r;K y,z= ktn(0,x->n);
  if(scalar(x)){
    y= guid_2_char(kG(x));
    r= from_any_kobject(y);
    r0(y);
    return r;
  }
  for(J i=0;i<x->n;i++){
    y= guid_2_char((G*)(&kU(x)[i]));
    kK(z)[i]= kp(kC(y));
    r0(y);
  }
  r = from_any_kobject(z);
  r0(z);
  return r;
}

static SEXP from_short_kobject(K x) {
  SEXP result;
  if(scalar(x)) return ScalarInteger(x->h==nh?NA_INTEGER:(int)x->h);
  PROTECT(result= allocVector(INTSXP,x->n));
  for(int i= 0; i < x->n; i++)
    INTEGER(result)[i]= kH(x)[i]==nh?NA_INTEGER:kH(x)[i];
  UNPROTECT(1);
  return result;
}

static SEXP from_int_kobject(K x) {
  SEXP result;
  if(scalar(x)) return ScalarInteger(x->i);
  PROTECT(result= allocVector(INTSXP,x->n));
  for(int i= 0; i < x->n; i++)
    INTEGER(result)[i]= kI(x)[i];
  UNPROTECT(1);
  return result;
}

static SEXP from_long_kobject(K x) {
  SEXP result;
  long n=scalar(x)?1:x->n;
  PROTECT(result= allocVector(REALSXP,n));
  if(scalar(x))
    INT64(result)[0]= x->j;
  else {
    for(int i= 0; i < n; i++)
      INT64(result)[i]= kJ(x)[i];
  }
  classgets(result, mkString("integer64"));
  UNPROTECT(1);
  return result;
}

static SEXP from_float_kobject(K x) {
  SEXP result;
  if(scalar(x)) return ScalarReal(ISNAN(x->e)?R_NaN:x->e);
  PROTECT(result= allocVector(REALSXP,x->n));
  for(int i= 0; i < x->n; i++)
    REAL(result)[i]= (double) ISNAN(kE(x)[i])?R_NaN:kE(x)[i];
  UNPROTECT(1);
  return result;
}

static SEXP from_double_kobject(K x) {
  SEXP result;
  if(scalar(x)) return ScalarReal(ISNAN(x->f)?R_NaN:x->f);
  PROTECT(result= allocVector(REALSXP,x->n));
  for(int i= 0; i < x->n; i++)
    REAL(result)[i]= ISNAN(kF(x)[i])?R_NaN:kF(x)[i];
  UNPROTECT(1);
  return result;
}

static SEXP from_string_kobject(K x) {
  SEXP result;
  long n=scalar(x)?1:x->n;
  PROTECT(result= allocVector(STRSXP,1));
  if(scalar(x))
    SET_STRING_ELT(result, 0, mkCharLen((S) &x->g, 1));
  else
    SET_STRING_ELT(result, 0, mkCharLen((S) kC(x), n));
  UNPROTECT(1);
  return result;
}

static SEXP from_string_column_kobject(K x) {
  SEXP result;
  long n=scalar(x)?1:x->n;
  PROTECT(result= allocVector(STRSXP,n));
  for(int i = 0; i < n; i++)
    SET_STRING_ELT(result, i, mkCharLen((S)&kC(x)[i],1));
  UNPROTECT(1);
  return result;
}

static SEXP from_symbol_kobject(K x) {
  SEXP result;
  if(scalar(x)) return mkString(x->s);
  PROTECT(result= allocVector(STRSXP,x->n));
  for(int i= 0; i < x->n; i++)
    SET_STRING_ELT(result, i, mkChar(kS(x)[i]));
  UNPROTECT(1);
  return result;
}

static SEXP from_month_kobject(K x) {
  SEXP result=PROTECT(from_int_kobject(x));
  for(J i= 0; i < XLENGTH(result); i++)
    if(INTEGER(result)[i]!=NA_INTEGER)
      INTEGER(result)[i]=months2days(INTEGER(result)[i])+kdbDateOffset;
  setmonthclass(result);
  UNPROTECT(1);
  return result;
}

static SEXP from_date_kobject(K x) {
  SEXP result=PROTECT(from_int_kobject(x));
  for(int i= 0; i < XLENGTH(result); i++)
    if(INTEGER(result)[i]!=NA_INTEGER)
      INTEGER(result)[i]+=kdbDateOffset;
  setdateclass(result);
  UNPROTECT(1);
  return result;
}

static SEXP from_datetime_kobject(K x) {
  SEXP result=PROTECT(from_double_kobject(x));
  for(int i= 0; i < XLENGTH(result); i++)
    REAL(result)[i]= REAL(result)[i]* sec2day + kdbDateOffset * sec2day;
  setdatetimeclass(result);
  settimezone(result,"GMT");
  UNPROTECT(1);
  return result;
}

static SEXP from_minute_kobject(K object) { 
  SEXP result=PROTECT(from_int_kobject(object));
  setdifftimeclass(result,"mins");
  UNPROTECT(1); 
  return result; 
}

static SEXP from_second_kobject(K object) { 
  SEXP result=PROTECT(from_int_kobject(object));
  setdifftimeclass(result,"secs");
  UNPROTECT(1); 
  return result;
}

static SEXP from_time_kobject(K object) {
  return from_int_kobject(object);
}

static SEXP from_timespan_kobject(K x) {
  SEXP result=from_long_kobject(x);
  J i,n=XLENGTH(result);
  PROTECT(result);

  //judge timespan or days
  int isDay=1;
  const int examine=n<5?n:5;
  for(int j= 0; j < examine; j++)
    isDay= (((INT64(result)[j] % (sec2day*1000000000LL))==0) < isDay)?0:isDay;
  
  if(isDay){
    //difftime days
    SEXP realresult;
    PROTECT(realresult=allocVector(REALSXP, n));
    for(i= 0; i < n; i++)
      REAL(realresult)[i]=(INT64(result)[i]!=nj)?((INT64(result)[i]/1000000000LL)/sec2day):NA_REAL;
    setdifftimeclass(realresult,"days");
    UNPROTECT(2);
    return realresult;
  }
  else{
    //timespan
    settimespanclass(result);
    UNPROTECT(1);
    return result;
  }
}

static SEXP from_timestamp_kobject(K x) {
  SEXP result=from_long_kobject(x);
  long n=XLENGTH(result);
  PROTECT(result);
  for(int i= 0; i < n; i++)
    if(INT64(result)[i]!=nj)INT64(result)[i]+=epoch_offset;
  settimestampclass(result);
  UNPROTECT(1);
  return result;
}

static SEXP from_dictionary_kobject(K x) {
  SEXP names, result;
  K table, k= kK(x)[0], v= kK(x)[1];
  /* if keyed, try to create a simple table */
  /* ktd will free its argument if successful */
  /* if fails, x is still valid */
  if (XT==k->t && XT==v->t) {
    r1(x);
    if ((table = ktd(x))) {
      result = from_table_kobject(table);
      r0(table);
      return result;
    }
    r0(x);
  }
  PROTECT(names = from_any_kobject(k));
  PROTECT(result = from_any_kobject(v));
  setAttrib(result, R_NamesSymbol, names);
  UNPROTECT(2);
  return result;
}

static SEXP from_table_kobject(K x) {
  SEXP names, result;
  PROTECT(names = from_any_kobject(kK(x->k)[0]));
  PROTECT(result = from_columns_kobject(kK(x->k)[1]));
  SET_NAMES(result, names);
  UNPROTECT(2);
  make_data_frame(result);
  return result;
}

/*
 * Util function
 */

static K guid_2_char(K x){
    K y= ktn(KC,37);
    G*gv= x;
    sprintf(kC(y),"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",gv[ 0],gv[ 1],gv[ 2],gv[ 3],gv[ 4],gv[ 5],gv[ 6],gv[ 7],gv[ 8],gv[ 9],gv[10],gv[11],gv[12],gv[13],gv[14],gv[15]);
    y->n= 36;
    return(y);
}
