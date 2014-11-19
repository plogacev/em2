#ifndef __MEASURES_CRI_H__
#define __MEASURES_CRI_H__

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>
#include <stdio.h>

#include "eyemeasures.h"
#include "FixSeq.h"

class CCRIMeasures: public CFixationSequence
{
public:
	CCRIMeasures(SEXP positionsArg, SEXP fixationTimesArg, 
			SEXP fixationStartArg, SEXP fixationEndArg,
			SEXP trialIdArg, SEXP trialIInfoArg,
			SEXP nrOfROIsArg, SEXP nrOfTrials,
			SEXP cutoffArg, SEXP cutoffLengthArg,
			SEXP useTimeIntervalsArg);

	~CCRIMeasures();
	
	void computeCRI();
	
private:
	void UpdateToTrialOffset(unsigned int trialOffset); 

	void TrialWrapUp_CompleteInfo(); 	 // copy trial information
	
	template<typename T> void TrialWrapUp_CompleteInfoForCol(int col, int offset); 

	void InitResultVectors();
	void InitInfoVectors();
	
private:
	virtual void TrialEndHook();	
			
public:

	unsigned int mOutputVectorsLength;
	unsigned int mTrialOffset;

// measures
	SEXP  rCFT;
	SEXP  rCFC;
	int * mCFT;
	int * mCFC;

    // for adressing the current trial
	int * mCFT_Cur;
	int * mCFC_Cur;

// positions vectors
	SEXP rsourcerid;
	SEXP rrid;
	int* msourcerid;
	int* mrid;

// trial info
	SEXP  *trialInfoR;
	void **trialInfoVec;
};


#include "MeasuresCRI_Inl.h"

#endif // __MEASURES_CRI_H__
