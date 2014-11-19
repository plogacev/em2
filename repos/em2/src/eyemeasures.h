#ifndef __EYEMEASURES_H__
#define __EYEMEASURES_H__

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#include <string.h>
#include "debug.h"

static long int protectCnt=0;
	
// smart protect, keeps count of how many object have been protected
// *and* allows for logging
#define SPROTECT( expr ) 	{ PROTECT( expr ); protectCnt++; }
#define SUNPROTECT( expr ) 	{ UNPROTECT( expr ); protectCnt=0; }
#define SUNPROTECT_PTR( expr ) 	{ UNPROTECT_PTR( expr ); protectCnt=0; }

#define INIT_VEC( vec, vecLen )		memset(vec, 0, sizeof(*vec)*vecLen)
#define INIT_VECVEC( vec, size )	for(int i=0; i < size; i++) { memset(vec[i], 0, sizeof(*vec)*measuresVecLen); }


#endif //__EYEMEASURES_H__
