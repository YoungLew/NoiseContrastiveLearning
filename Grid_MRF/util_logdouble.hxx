/*
*     File Name           :     util_logdouble.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-18 14:12]
*     Last Modified       :     [2016-01-24 12:24]
*     Description         :     some util function for logDouble Type 
**/

#ifndef __UTIL_LOGDOUBLE_H__
#define __UTIL_LOGDOUBLE_H__
#include <math.h>

#define LOGDOUBLE double
const LOGDOUBLE LOGDOUBLE_ZERO = (LOGDOUBLE)(-2e20);

LOGDOUBLE LOGDOUBLE_SIGMOD_UP(LOGDOUBLE tmp){
    if (tmp >(LOGDOUBLE)(50)) return tmp;
    if (tmp < (LOGDOUBLE)(-50))  return (LOGDOUBLE)(tmp);
    return log(exp(tmp) + 1.0);
}

LOGDOUBLE exp_add(LOGDOUBLE a, LOGDOUBLE b){
    if (a < b){
        LOGDOUBLE t = a; a = b; b = t;
    }
    if (b <=LOGDOUBLE_ZERO) return a;
    LOGDOUBLE tmp = a - b;
    return b + LOGDOUBLE_SIGMOD_UP(tmp);
}
#endif
