/**
 * Common headers for integration with R.
 */

/*-----------------------------------------------*/
/*                Load Libraries                 */
/*-----------------------------------------------*/

#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <R.h>
#include <Rinternals.h>
#include "k.h"

/*-----------------------------------------------*/
/*                    Macro                      */
/*-----------------------------------------------*/

#define INT64(x)   ((J*) REAL(x))

/*-----------------------------------------------*/
/*                Global Variable                */
/*-----------------------------------------------*/

// Offsets used in conversion between R and q
extern const J epoch_offset;
// Seconds in a day
extern const int sec2day;
// Days between 1970.01.01 & 2000.01.01
extern const int kdbDateOffset;
// Seconds between 1970.01.01 & 2000.01.01
extern const int kdbSecOffset;

/**
 * @brief Attribute set to q time related types when it is converted to R object.
 */
extern SEXP R_UnitsSymbol;
/**
 * @brief Attribute set to q time related types when it is converted to R object.
 */
extern SEXP R_TzSymbol;

/*-----------------------------------------------*/
/*                  Functions                    */
/*-----------------------------------------------*/

// Utility //-------------------------------------/

/**
 * @brief Check if it is a leap year.
 */
bool is_leap(const int year);

/**
 * @brief Functions to derive day count since kdb+ epoch from month count
 */
int months2days(const int monthcount);

/**
 * @brief Functions to derive month count since kdb epoch from day count
 */
int days2months(const int daycount);

// R -> q //--------------------------------------/

/**
 * @brief Entry point of converting R object to q object.
 */
K from_any_robject(SEXP sxp);

/**
 * @brief Convert R pair-list object to q dictionary object.
 * @note
 * The definition varies between embedR and rkdb.
 */
K from_pairlist_robject(SEXP sxp);

/**
 * @brief add attribute if any.
 **/
K attR(K x,SEXP sxp);

// q -> R //--------------------------------------/

/**
 * @brief Entry point of converting q object to R object.
 */
SEXP from_any_kobject(K x);

/**
 * @brief Convert R string to q object.
 */
SEXP from_string_kobject(K);

/**
 * @brief Convert R symbol object to q object.
 */
SEXP from_symbol_kobject(K);
