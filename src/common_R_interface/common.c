/**
 * Common source for R integration with kdb+. Define global variables.
 */

/*-----------------------------------------------*/
/*                Load Libraries                 */
/*-----------------------------------------------*/

#include "common.h"

/*-----------------------------------------------*/
/*                Global Variable                */
/*-----------------------------------------------*/

// Offsets used in conversion between R and q
const J epoch_offset=10957*24*60*60*1000000000LL;
// Seconds in a day
const int sec2day = 86400;
// Days between 1970.01.01 & 2000.01.01
const int kdbDateOffset = 10957;
// Seconds between 1970.01.01 & 2000.01.01
const int kdbSecOffset  = 946684800;

SEXP R_UnitsSymbol=NULL;
SEXP R_TzSymbol=NULL;

/*-----------------------------------------------*/
/*                Utility Functions              */
/*-----------------------------------------------*/

/**
 * @brief Check if it is a leap year.
 */
bool is_leap(const int year){
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/**
 * @brief Functions to derive day count since kdb+ epoch from month count
 */
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

/**
 * @brief Functions to derive month count since kdb epoch from day count
 */
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
