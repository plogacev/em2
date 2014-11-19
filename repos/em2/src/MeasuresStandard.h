#ifndef __MEASURES_STANDARD_H__
#define __MEASURES_STANDARD_H__

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>
#include <stdio.h>

#include "eyemeasures.h"
#include "FixSeq.h"

class CStandardMeasures: public CFixationSequence
{
public:
	CStandardMeasures(SEXP positionsArg, SEXP fixationTimesArg, 
			  SEXP fixationStartArg, SEXP fixationEndArg,
			  SEXP trialIdArg, SEXP trialIInfoArg,
			  SEXP nrOfROIsArg, SEXP nrOfTrials,
			  SEXP cutoffArg, SEXP cutoffLengthArg,
			  SEXP useTimeIntervalsArg);
	~CStandardMeasures();

	void computeStandardEyemeasures(bool regressiveFirstPass);

private:
	void UpdateToTrialOffset(unsigned int trialOffset); 

	void TrialWrapUp_CompleteMeasures(); // compute the measures which are computed directly
			      	   					 // from other measures (i.e. RRT, RRTP, RRTR, CRPD)
	void TrialWrapUp_CompleteInfo(); 	 // copy trial information
	
	template<typename T> void TrialWrapUp_CompleteInfoForCol(int col, int offset); 

	void InitResultVectors();
	void InitInfoVectors();
	
private:
	virtual void TrialEndHook();	
			
public:
// R objects
	
	SEXP ffdR;
	SEXP ffpR;
	SEXP sfdR;
	SEXP fprtR;
	SEXP rbrtR;
	SEXP tftR;
	SEXP rpdR;
	SEXP crpdR;
	SEXP rrtR;
	SEXP rrtpR;
	SEXP rrtrR;
	SEXP rbrcR;
	SEXP trcR;
	SEXP lprtR;

// for adressing in C
	int * ffdAllVec;
	int * ffpAllVec;
	int * sfdAllVec;
	int * fprtAllVec;
	int * rbrtAllVec;
	int * tftAllVec;
	int * rpdAllVec;
	int * crpdAllVec;
	int * rrtAllVec;
	int * rrtpAllVec;
	int * rrtrAllVec;
	int * rbrcAllVec;
	int * trcAllVec;
	int * lprtAllVec;

// for adressing the current trial
	int * ffdVec;
	int * ffpVec;
	int * sfdVec;
	int * fprtVec;
	int * rbrtVec;
	int * tftVec;
	int * rpdVec;
	int * crpdVec;
	int * rrtVec;
	int * rrtpVec;
	int * rrtrVec;
	int * rbrcVec;
	int * trcVec;
	int * lprtVec;

	unsigned int mOutputVectorsLength;
	unsigned int mTrialOffset;

	// positions vector
	SEXP positionsR;
	int* positionsVec;

	SEXP  *trialInfoR;
	void **trialInfoVec;
};


#include "MeasuresStandard_Inl.h"

#endif // __MEASURES_STANDARD_H__
