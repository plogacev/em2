#include "MeasuresCRI.h"

#define RESERVE(objC, objR) 	SPROTECT( objR = NEW_INTEGER(mOutputVectorsLength)); \
				objC = INTEGER_POINTER(objR); \
				INIT_VEC(objC, mOutputVectorsLength)


CCRIMeasures::CCRIMeasures(SEXP positionsArg, SEXP fixationTimesArg, 
					SEXP fixationStartArg, SEXP fixationEndArg,
					SEXP trialIdArg, SEXP trialIInfoArg,
					SEXP nrOfROIsArg, SEXP nrOfTrials,
					SEXP cutoffArg, SEXP cutoffLengthArg,
					SEXP useTimeIntervalsArg):
				CFixationSequence(positionsArg, fixationTimesArg, 
						  fixationStartArg, fixationEndArg,
						  trialIdArg, trialIInfoArg,
						  nrOfROIsArg, nrOfTrials,
						  cutoffArg, cutoffLengthArg,
						  useTimeIntervalsArg)
	
{
	mOutputVectorsLength = NrOfTrials() * (NrOfROIs()*NrOfROIs());
	
	InitResultVectors();
	InitInfoVectors();
	
	UpdateToTrialOffset(0);	
}


CCRIMeasures::~CCRIMeasures() 
{
	SUNPROTECT_PTR(rCFT);
	SUNPROTECT_PTR(rCFC);
	SUNPROTECT_PTR(rsourcerid);
	SUNPROTECT_PTR(rrid);
	
	for(int i=0; i < InfoCol_Count(); i++) 
		SUNPROTECT_PTR( trialInfoR[i] );
				
	if(trialInfoR)
		free(trialInfoR);
	if(trialInfoVec)
		free(trialInfoVec);
}


void CCRIMeasures::InitInfoVectors()
{
	// trial info
	trialInfoR = (SEXP*)malloc(sizeof(SEXP)*InfoCol_Count());
	trialInfoVec = (void**)malloc(sizeof(void*)*InfoCol_Count());
	for(int i=0; i < InfoCol_Count(); i++) 
	{
		SPROTECT(trialInfoR[i] = allocVector(InfoCol_DataType(i), mOutputVectorsLength));
		switch( InfoCol_DataType(i) )
		{
			case LGLSXP:
				trialInfoVec[i] = (void*) LOGICAL( trialInfoR[i] );
				break;
			case INTSXP:
				trialInfoVec[i] = (void*) INTEGER( trialInfoR[i] );
				break;
			case REALSXP:
				trialInfoVec[i] = (void*) REAL( trialInfoR[i] );
				break;
			case CPLXSXP:
				trialInfoVec[i] = (void*) COMPLEX( trialInfoR[i] );
				break;
			case STRSXP:
				trialInfoVec[i] = NULL;
				break;
			default:
				trialInfoVec[i] = NULL;
				// NOT SUPPORTED
				break;
		}
	}
}


void CCRIMeasures::InitResultVectors()
{
	RESERVE(mCFT, rCFT);
	RESERVE(mCFC, rCFC);
	
	// init position vectors; not using RESERVE(), since it runs memset, which would be a waste of time
	SPROTECT( rsourcerid = NEW_INTEGER(mOutputVectorsLength));
	msourcerid = INTEGER_POINTER(rsourcerid); 
	SPROTECT( rrid = NEW_INTEGER(mOutputVectorsLength));
	mrid = INTEGER_POINTER(rrid); 
	
	for(int i=0; i < NrOfTrials(); i++) 
	{
		int trial_offset = NrOfROIs()*NrOfROIs()*i;
		
		for(int j=0; j < NrOfROIs(); j++) 
		{
			int sourcerid_offset = NrOfROIs()*j;
			int *sourcerid_CurTrial = msourcerid + trial_offset+sourcerid_offset;
			int *rid_CurTrial = mrid + trial_offset+sourcerid_offset;
			
			for(int k=0; k < NrOfROIs(); k++) 
			{
				sourcerid_CurTrial[k] = j+1;
				rid_CurTrial[k] = k+1;
			}
		}
	}
}


template<typename T>
inline void CCRIMeasures::TrialWrapUp_CompleteInfoForCol(int col, int offset) 
{
	T* infoVec = (T*)trialInfoVec[col] + offset;	
	T curContentDouble = InfoCol_LastTrialContent<T>(col);
	
	for(int j=0; j < NrOfROIs()*NrOfROIs(); j++) {
		infoVec[j] = curContentDouble;
	}

}
template<>
inline void CCRIMeasures::TrialWrapUp_CompleteInfoForCol<char*>(int col, int offset) 
{
    char* curContentString = InfoCol_LastTrialContent<char*>(col);
	for(int j=0; j < NrOfROIs()*NrOfROIs(); j++) 
		SET_STRING_ELT(trialInfoR[col], offset+j, mkChar(curContentString));
}


inline void CCRIMeasures::TrialWrapUp_CompleteInfo() 
{
	LOG_METHOD("CCRIMeasures","TrialWrapUp_CompleteInfo");

 	int offset = mTrialOffset*NrOfROIs()*NrOfROIs();	
	
	for(int i=0; i < InfoCol_Count(); i++) 
	{
		switch( InfoCol_DataType(i) ) 
		{
			case LGLSXP:
				TrialWrapUp_CompleteInfoForCol<int>(i, offset);
				break;
			case INTSXP:
				TrialWrapUp_CompleteInfoForCol<int>(i, offset);
				break;
			case REALSXP:
				TrialWrapUp_CompleteInfoForCol<double>(i, offset);
				break;
			case CPLXSXP:
				TrialWrapUp_CompleteInfoForCol<Rcomplex>(i, offset);
				break;
			case STRSXP:
				TrialWrapUp_CompleteInfoForCol<char*>(i, offset);
				break;
			default:
				// NOT SUPPORTED
				break;
		}
	}
}

void CCRIMeasures::TrialEndHook() 
{
	LOG(("trial finished\n"))
	TrialWrapUp_CompleteInfo();
	UpdateToTrialOffset(mTrialOffset+1);
}


