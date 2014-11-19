#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#include <string.h>
#include <stdio.h>
#include "eyemeasures.h"
#include "FixSeq.h"
#include "MeasuresStandard.h"
#include "MeasuresCRI.h"

 
// allocates an R vector
// all vectors to be returned have the same length, which is specified here
#define VECRESERVEN( name, size ) SEXP name; SPROTECT( name = NEW_INTEGER(size))

// allocates a C vector of R vectors
#define VECVECRESERVE( name, size ) 	SEXP name[size]; for(int i=0; i < size; i++) SPROTECT( name[i] = NEW_INTEGER(maxPos))

	
#define APPEND_RET_VEC(name, vector) \
{ \
	SET_STRING_ELT(listNamesRet, rListAppendCnt,  mkChar(name)); \
	SET_ELEMENT(listRet, rListAppendCnt, vector); \
	rListAppendCnt++; \
} 

extern "C" {

/* just the interface function */	
SEXP computeStandardEyemeasuresExt(SEXP positionsArg, SEXP fixationTimesArg, 
				   SEXP fixationStartArg, SEXP fixationEndArg,
				   SEXP trialIdArg, SEXP trialInfoArg,
				   SEXP nrOfROIsArg, SEXP nrOfTrialsArg,
				   SEXP cutoffArg, SEXP cutoffLengthArg, 
				   SEXP regressiveFirstPassArg, SEXP useTimeIntervalsArg)
{
	LOG(("<computeStandardEyemeasuresExt>\n"))
	/* process arguments */
	CStandardMeasures m(positionsArg, fixationTimesArg, 
					fixationStartArg, fixationEndArg,
					trialIdArg, trialInfoArg,
					nrOfROIsArg, nrOfTrialsArg, 
					cutoffArg, cutoffLengthArg, 
					useTimeIntervalsArg );
	bool regressiveFirstPass = LOGICAL_VALUE(regressiveFirstPassArg);

	int nrOfTrials = INTEGER_VALUE(nrOfTrialsArg);
	if(nrOfTrials < 0) nrOfTrials = 0;
	
	/* do computations */
	m.computeStandardEyemeasures(regressiveFirstPass);

	// handle returning stuff
	SEXP listRet, listNamesRet;
	const int resultVectorsCnt = 1 + 14; // 1 extra for roi
	
	// set the names vector
	SPROTECT(listNamesRet = allocVector(STRSXP, resultVectorsCnt+length(trialInfoArg))); 
	SPROTECT(listRet = allocVector(VECSXP, resultVectorsCnt+length(trialInfoArg)));
	
	// add info elements
	SEXP trialInfoNames = getAttrib(trialInfoArg, R_NamesSymbol);
	int rListAppendCnt=0;
	for( int i=0; i < length(trialInfoArg); i++) 
		APPEND_RET_VEC ( CHAR(STRING_ELT(trialInfoNames, i)), m.trialInfoR[i]);

	// add result vectors
	APPEND_RET_VEC( "roi",  m.positionsR); // attaching ROIs vector to list
	APPEND_RET_VEC( "FFD",  m.ffdR);       // FFD (first fixation duration)
	APPEND_RET_VEC( "FFP",  m.ffpR);       // FFP (first fixation progressive)
	APPEND_RET_VEC( "SFD",  m.sfdR);       // SFD (single fixation duration)
	APPEND_RET_VEC( "FPRT", m.fprtR);      // FPRT (first pass reading time / gaze duration)
	APPEND_RET_VEC( "RBRT", m.rbrtR);      // RBRT (right bounded reading time)
	APPEND_RET_VEC( "TFT",  m.tftR);       // TFT (total fixation time)
	APPEND_RET_VEC( "RPD",  m.rpdR);       // RPD (regression path duration)
	APPEND_RET_VEC( "CRPD", m.crpdR);      // CRPD (cumulative regression path duration)
	APPEND_RET_VEC( "RRT",  m.rrtR);       // RRT (re-reading time)
	APPEND_RET_VEC( "RRTP", m.rrtpR);      // RRTP (re-reading time progressive)
	APPEND_RET_VEC( "RRTR", m.rrtrR);      // RRTR (re-reading time regressive)
	APPEND_RET_VEC( "RBRC", m.rbrcR);      // RBRC (first-pass regression count)
	APPEND_RET_VEC( "TRC",  m.trcR);       // TRC (total regression count)
	APPEND_RET_VEC( "LPRT", m.lprtR);      // LPRT (last pass reading time)
   	setAttrib(listRet, R_NamesSymbol, listNamesRet); //and attaching the vector names
   
	UNPROTECT_PTR(listRet);
	UNPROTECT_PTR(listNamesRet);

	LOG(("</computeStandardEyemeasuresExt>\n"))
	return(listRet);
}


/* just the interface function */	
SEXP computeCRIExt(SEXP positionsArg, SEXP fixationTimesArg, 
		   SEXP fixationStartArg, SEXP fixationEndArg,
		   SEXP trialIdArg, SEXP trialInfoArg,
		   SEXP maxPosArg, SEXP nrOfTrialsArg,
		   SEXP cutoffArg, SEXP cutoffLengthArg, 
		   SEXP regressiveFirstPassArg, SEXP useTimeIntervalsArg)
{
	int protectCnt = 0;

	LOG(("<computeCRIExt>\n"))

	// create the fixation sequence object
	CCRIMeasures m(  positionsArg, fixationTimesArg, 
					fixationStartArg, fixationEndArg,
					trialIdArg, trialInfoArg,
					maxPosArg, nrOfTrialsArg, 
					cutoffArg, cutoffLengthArg, 
					useTimeIntervalsArg );
	

	/* do computations */
	m.computeCRI();

	/* handle returning stuff*/
	SEXP listRet, listNamesRet;
	const int resultVectorsCnt = 4; // 1 extra for roi
	
	// set the names vector
	SPROTECT(listNamesRet = allocVector(STRSXP, resultVectorsCnt+length(trialInfoArg))); 
	SPROTECT(listRet = allocVector(VECSXP, resultVectorsCnt+length(trialInfoArg)));
	
	// add info elements
	SEXP trialInfoNames = getAttrib(trialInfoArg, R_NamesSymbol);
	int rListAppendCnt=0;
	for( int i=0; i < length(trialInfoArg); i++) 
		APPEND_RET_VEC( CHAR(STRING_ELT(trialInfoNames, i)), m.trialInfoR[i]);

	APPEND_RET_VEC( "sourcerid", m.rsourcerid);
	APPEND_RET_VEC( "rid", m.rrid);
	APPEND_RET_VEC( "CFC", m.rCFC);
	APPEND_RET_VEC( "CFT", m.rCFT);
    setAttrib(listRet, R_NamesSymbol, listNamesRet); //and attaching the vector names
   
   	SUNPROTECT_PTR(listNamesRet);
	SUNPROTECT_PTR(listRet);
   	
	LOG(("</computeCRIExt>\n"))
	return(listRet);
}



}
