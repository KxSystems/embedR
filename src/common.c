/*
 * common code for Q/R interface
 */

int kx_connection=0;

Z const int kdbDateOffset = 10957;
Z const J epoch_offset=kdbDateOffset*24*60*60*1000000000LL;

/* Custom type constant for integer64 (bit64 package) */
#define INT64SXP 64
#define INT64(x) ((J*) NUMERIC_POINTER(x))

/*
 * Make a data.frame from a named list by adding row.names, and class
 * attribute. Uses "1", "2", .. as row.names.
 */
static void make_data_frame(SEXP data)
{
	SEXP class_name, row_names; Sint n;
	PROTECT(data);
	PROTECT(class_name = NEW_CHARACTER((Sint) 1));
	SET_STRING_ELT(class_name, 0, COPY_TO_USER_STRING("data.frame"));
	/* Set the row.names. */
	n = GET_LENGTH(VECTOR_ELT(data,0));
    PROTECT(row_names=NEW_INTEGER(2)); INTEGER(row_names)[0]=NA_INTEGER; INTEGER(row_names)[1]=-n;
    setAttrib(data, R_RowNamesSymbol, row_names);
    SET_CLASS(data, class_name);
    UNPROTECT(3);
}

/**
 * set sxp as POSIXct (POSIX date-time) object
 * sxp will dispatch methods as a POSIX date-time object (e.g., printing/formatting methods, as.POSIXct methods, etc.), 
 * provided sxp is also a valid underlying representation for POSIXct 
 * (typically a numeric vector of seconds since the epoch, plus often a "tzone" attribute). 
 * If the underlying type/values aren’t consistent, R may print oddly or downstream functions may misbehave.
 * in r class(sxp) <- c("POSIXt", "POSIXct")
 */
static void setdatetimeclass(SEXP sxp)
{
	SEXP datetimeclass = PROTECT(allocVector(STRSXP,2));
	SET_STRING_ELT(datetimeclass, 0, mkChar("POSIXt"));
	SET_STRING_ELT(datetimeclass, 1, mkChar("POSIXct"));
	setAttrib(sxp, R_ClassSymbol, datetimeclass);           // set class attribute of sxp 
	UNPROTECT(1);
}

/**
 * annotates sxp it so it can be treated as a nanotime timestamp backed by an integer64 representation, 
 * and then returns an S4 version of that object
 */
static SEXP settimestampclass(SEXP sxp) {
    // Mark underlying storage as bit64-style integer64 
    SEXP s3class = PROTECT(mkString("integer64"));
    setAttrib(sxp, install(".S3Class"), s3class);
    // Set class to "nanotime" and attach the class's package attribute
    SEXP classValue = PROTECT(mkString("nanotime"));
    SEXP pkg        = PROTECT(mkString("nanotime"));
    setAttrib(classValue, install("package"), pkg);
    // Apply class to the object 
    classgets(sxp, classValue);
    // Return as an S4 object (and protect the return value) 
    SEXP ans = PROTECT(asS4(sxp, TRUE, 0));
    UNPROTECT(4); 
    return ans;
}

/* for date */
static SEXP setdateclass(SEXP sxp) {
  SEXP timeclass= PROTECT(allocVector(STRSXP, 1));
  SET_STRING_ELT(timeclass, 0, mkChar("Date"));
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
 * An array of functions that deal with kdbplus data types. Note that the order
 * is very important as we index it based on the kdb+ type number in the K object.
 */
typedef SEXP(*conversion_function)(K);

conversion_function kdbplus_types[] = {
	from_list_of_kobjects,
	from_bool_kobject,
	from_guid_kobject,
	error_broken_kobject,
	from_byte_kobject,
	from_short_kobject,
	from_int_kobject,
	from_long_kobject,
	from_float_kobject,
	from_double_kobject,
	from_string_kobject,
	from_symbol_kobject,
	from_timestamp_kobject,
	from_month_kobject,
	from_date_kobject,
	from_datetime_kobject,
	from_timespan_kobject,
	from_minute_kobject,
	from_second_kobject,
	from_time_kobject
};

/**
 * Convert K object to R object
 */
static SEXP from_any_kobject(K x)
{
	SEXP result;
	int type = abs(x->t);
	if (XT == type)
		result = from_table_kobject(x);
	else if (XD == type)
		result = from_dictionary_kobject(x);
	else if (105 == type || 101 == type)
        // composition and unary primitive
		result = from_int_kobject(ki(0));
	else if (type <= KT)
        // basic data types
		result = kdbplus_types[type](x);
	else if (KT < type && type < 77) {
        // enums
		K t = k(0,"value",r1(x),(K)0);
		if(t && t->t!=-128) {
			result = from_any_kobject(t);
			r0(t);
		}else 
			result = error_broken_kobject(x);
	} else if(77 <= type && type < XT){
		K t = k(0,"{(::) each x}",r1(x),(K)0);
		if(t && t->t!=-128) {
			result = from_any_kobject(t);
			r0(t);
		}else 
			result = error_broken_kobject(x);
	}
	else
		result = error_broken_kobject(x);
	return result;
}

/**
 * Convert K columns to R object
 */
static SEXP from_columns_kobject(K x)
{
  SEXP col, result;
  int i, type, length = x->n;
  K c;
  PROTECT(result = NEW_LIST(length));
  for (i = 0; i < length; i++) {
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
static SEXP error_broken_kobject(K broken)
{
	error("Value is not a valid kdb+ object; unknown type %d\n", broken->t);
	return mkChar("unknown");
}

/*
 * An R list from a K list object.
 */
static SEXP from_list_of_kobjects(K x)
{
	SEXP result;
	int i, length = x->n;
	PROTECT(result = NEW_LIST(length));
	for (i = 0; i < length; i++) {
		SET_VECTOR_ELT(result, i, from_any_kobject(xK[i]));
	}
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

#define scalar(x) (x->t < 0)

/**
 * Create logical R type from kdb boolean (atom and vector)
 */
static SEXP from_bool_kobject(K x)
{
    SEXP result;
    if(scalar(x)) return ScalarLogical(x->g);
    PROTECT(result= allocVector(LGLSXP,x->n));
    for(int i= 0; i < x->n; i++)
        LOGICAL(result)[i]= kG(x)[i];
    UNPROTECT(1);
    return result;
}

/**
 * Create raw R type from kdb byte (atom and vector)
 */
static SEXP from_byte_kobject(K x)
{
    SEXP result;G*r;
    if(scalar(x)) return ScalarRaw(x->g);
    PROTECT(result= allocVector(RAWSXP,x->n));
    r=RAW(result);
    for(int i= 0; i < x->n; i++)
        r[i]= kG(x)[i];
    UNPROTECT(1);
    return result;
}

/**
 * Function used in the conversion of kdb guid to R char array
 */
static K guid_2_char(G* x){
    K y= ktn(KC,37);
    G*gv= x;
    sprintf(kC(y),"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",gv[ 0],gv[ 1],gv[ 2],gv[ 3],gv[ 4],gv[ 5],gv[ 6],gv[ 7],gv[ 8],gv[ 9],gv[10],gv[11],gv[12],gv[13],gv[14],gv[15]);
    y->n= 36;
    return(y);
}

/**
 * Create character R type from kdb guid (atom and vector)
 */ 
static SEXP from_guid_kobject(K x)
{
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

/**
 * Create integer R type from kdb short (atom and vector)
 */
static SEXP from_short_kobject(K x)
{
    SEXP result;
    if(scalar(x)) return ScalarInteger(x->h==nh?NA_INTEGER:(int)x->h);
    PROTECT(result= allocVector(INTSXP,x->n));
    for(int i= 0; i < x->n; i++)
        INTEGER(result)[i]= kH(x)[i]==nh?NA_INTEGER:kH(x)[i];
    UNPROTECT(1);
    return result;
}

/**
 * Create integer R type from kdb integer (atom and vector)
 */
static SEXP from_int_kobject(K x)
{
    SEXP result;
    if(scalar(x)) return ScalarInteger(x->i);
    PROTECT(result= allocVector(INTSXP,x->n));
    for(int i= 0; i < x->n; i++)
        INTEGER(result)[i]= kI(x)[i];
    UNPROTECT(1);
    return result;
}

/**
 * Create integer64 R type from kdb long (atom and vector)
 */
static SEXP from_long_kobject(K x)
{
    SEXP result;
    J i,n=scalar(x)?1:x->n;
    PROTECT(result = allocVector(REALSXP,n));
    if(scalar(x))
        INT64(result)[0]= x->j;
    else
        for(i = 0; i < n; i++)
            INT64(result)[i]= kJ(x)[i];
    SEXP cls = PROTECT(mkString("integer64"));
    classgets(result, cls);
    UNPROTECT(2);
    return result;
}

/**
 * Create numeric R type from kdb real (atom and vector)
 */
static SEXP from_float_kobject(K x)
{
    SEXP result;
    if(scalar(x)) return ScalarReal(ISNAN(x->e)?R_NaN:x->e);
    PROTECT(result= allocVector(REALSXP,x->n));
    for(int i= 0; i < x->n; i++)
        REAL(result)[i]= (double) ISNAN(kE(x)[i])?R_NaN:kE(x)[i];
    UNPROTECT(1);
    return result;
}

/**
 * Create numeric R type from kdb float (atom and vector)
 */
static SEXP from_double_kobject(K x)
{
    SEXP result;
  if(scalar(x)) return ScalarReal(ISNAN(x->f)?R_NaN:x->f);
  PROTECT(result= allocVector(REALSXP,x->n));
  for(int i= 0; i < x->n; i++)
    REAL(result)[i]= ISNAN(kF(x)[i])?R_NaN:kF(x)[i];
  UNPROTECT(1);
  return result;
}

/** 
 * Create character R type from kdb char vector
 */ 
static SEXP from_string_kobject(K x)
{
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

static SEXP from_string_column_kobject(K x)
{
  SEXP result;
  int i, length = x->n;
  PROTECT(result = NEW_CHARACTER(length));
  for(i = 0; i < length; i++) {
    SET_STRING_ELT(result, i, mkCharLen((S)&kC(x)[i],1));
  }
  UNPROTECT(1);
  return result;
}

/**
 * Create character R type from kdb symbol (atom and vector)
 */
static SEXP from_symbol_kobject(K x)
{
    SEXP result;
    if(scalar(x)) return mkString(x->s);
    PROTECT(result= allocVector(STRSXP,x->n));
    for(int i= 0; i < x->n; i++)
        SET_STRING_ELT(result, i, mkChar(kS(x)[i]));
    UNPROTECT(1);
    return result;
}

/**
 * Create integer R type from kdb month (atom and vector)
 */
static SEXP from_month_kobject(K object)
{
	return from_int_kobject(object);
}

/**
 * Create date R type from kdb date (atom and vector)
 */
static SEXP from_date_kobject(K x)
{
    SEXP result=PROTECT(from_int_kobject(x));
    // add days between 1970 and 2000
    for(int i= 0; i < XLENGTH(result); i++)
        if(INTEGER(result)[i]!=NA_INTEGER)
            INTEGER(result)[i]+=kdbDateOffset;
    setdateclass(result);
    UNPROTECT(1);
    return result;
}

/**
 * Create date R type from kdb datetime (atom and vector)
 */
static SEXP from_datetime_kobject(K x)
{
	SEXP result;
	int i, length = x->n;
    // add days between 1970 and 2000, multiply by seconds in a day
	if (scalar(x)) {
		PROTECT(result = NEW_NUMERIC(1));
		NUMERIC_POINTER(result)[0] = (x->f + kdbDateOffset) * 86400; 
	}
	else {
		PROTECT(result = NEW_NUMERIC(length));
		for(i = 0; i < length; i++)
			NUMERIC_POINTER(result)[i] = (kF(x)[i] + kdbDateOffset) * 86400;
	}
    setdatetimeclass(result);
    UNPROTECT(1);
	return result;
}

/**
 * Create integer R type from kdb minute (atom and vector)
 */
static SEXP from_minute_kobject(K object)
{
	return from_int_kobject(object);
}

/**
 * Create integer R type from kdb second (atom and vector)
 */
static SEXP from_second_kobject(K object)
{
	return from_int_kobject(object);
}

/**
 * Create integer R type from kdb time (atom and vector)
 */
static SEXP from_time_kobject(K object)
{
	return from_int_kobject(object);
}

/**
 * Create numeric R type from kdb timespan (atom and vector)
 */
static SEXP from_timespan_kobject(K x)
{
	SEXP result;
	int i, length = x->n;
    // remove nanoseconds
	if (scalar(x)) {
		PROTECT(result = NEW_NUMERIC(1));
		NUMERIC_POINTER(result)[0] = x->j / 1e9;
	}
	else {
		PROTECT(result = NEW_NUMERIC(length));
		for(i = 0; i < length; i++)
			NUMERIC_POINTER(result)[i] = xJ[i] / 1e9;
	}
	UNPROTECT(1);
	return result;
}

/**
 * Create nanotime R type from kdb timespan (atom and vector)
 */
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

static SEXP from_dictionary_kobject(K x)
{
	SEXP names, result;
	K table;

	/* if keyed, try to create a simple table */
	/* ktd will free its argument if successful */
	/* if fails, x is still valid */
	if (XT==xx->t && XT==xy->t) {
		r1(x);
		if ((table = ktd(x))) {
			result = from_table_kobject(table);
			r0(table);
			return result;
		}
		r0(x);
	}

	PROTECT(names = from_any_kobject(xx));
	PROTECT(result = from_any_kobject(xy));
	SET_NAMES(result, names);
	UNPROTECT(2);
	return result;
}

static SEXP from_table_kobject(K x)
{
  SEXP names, result;
  PROTECT(names = from_any_kobject(kK(x->k)[0]));
  PROTECT(result = from_columns_kobject(kK(x->k)[1]));
  SET_NAMES(result, names);
  UNPROTECT(2);
  make_data_frame(result);
  return result;
}

