/*-----------------------------------------------------------*/
/*  File: q2r_ex.c                                           */
/*  Overview: Distinct code in embedR for Q -> R interface   */
/*-----------------------------------------------------------*/

/*-----------------------------------------------*/
/*                Load Libraries                 */
/*-----------------------------------------------*/

#include "embedr.h"
#include "common.h"

/*-----------------------------------------------*/
/*                  Functions                    */
/*-----------------------------------------------*/

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