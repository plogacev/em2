#include "MeasuresStandard.h"

#define RESERVE(objC, objR) 	SPROTECT( objR = NEW_INTEGER(mOutputVectorsLength)); \
				objC = INTEGER_POINTER(objR); \
				INIT_VEC(objC, mOutputVectorsLength)


CStandardMeasures::CStandardMeasures(SEXP positionsArg, SEXP fixationTimesArg, 
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
	LOG_METHOD("CStandardMeasures","CStandardMeasures");

	mOutputVectorsLength = NrOfTrials()*NrOfROIs();
	
	InitResultVectors();
	InitInfoVectors();
	
	UpdateToTrialOffset(0);	
}


CStandardMeasures::~CStandardMeasures() 
{
	LOG_METHOD("CStandardMeasures","~CStandardMeasures");
	SUNPROTECT_PTR(ffdR);
	SUNPROTECT_PTR(ffpR);
	SUNPROTECT_PTR(sfdR);
	SUNPROTECT_PTR(fprtR);
	SUNPROTECT_PTR(rbrtR);
	SUNPROTECT_PTR(tftR);
	SUNPROTECT_PTR(rpdR);
	SUNPROTECT_PTR(crpdR);
	SUNPROTECT_PTR(rrtR);
	SUNPROTECT_PTR(rrtpR);
	SUNPROTECT_PTR(rrtrR);
	SUNPROTECT_PTR(rbrcR);
	SUNPROTECT_PTR(trcR);
	SUNPROTECT_PTR(lprtR);
	SUNPROTECT_PTR(positionsR);
	
	for(int i=0; i < InfoCol_Count(); i++) 
		SUNPROTECT_PTR( trialInfoR[i] );
				
	if(trialInfoR)
		free(trialInfoR);
	if(trialInfoVec)
		free(trialInfoVec);
}


void CStandardMeasures::InitInfoVectors()
{
	LOG_METHOD("CStandardMeasures","InitInfoVectors");

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


void CStandardMeasures::InitResultVectors()
{
	LOG_METHOD("CStandardMeasures","InitResultVectors");
	
	RESERVE(ffdAllVec,  ffdR);
	RESERVE(ffpAllVec,  ffpR);
	RESERVE(sfdAllVec,  sfdR);
	RESERVE(fprtAllVec, fprtR);
	RESERVE(rbrtAllVec, rbrtR);
	RESERVE(tftAllVec,  tftR);
	RESERVE(rpdAllVec,  rpdR);
	RESERVE(crpdAllVec, crpdR);
	RESERVE(rrtAllVec,  rrtR);
	RESERVE(rrtpAllVec, rrtpR);
	RESERVE(rrtrAllVec, rrtrR);
	RESERVE(rbrcAllVec, rbrcR);
	RESERVE(trcAllVec,  trcR);
	RESERVE(lprtAllVec, lprtR);

	// init positions vector; not using RESERVE(), since
	// it runs memset, which would be a waste of time
	SPROTECT( positionsR = NEW_INTEGER(mOutputVectorsLength));
	positionsVec = INTEGER_POINTER(positionsR); 
	for(int j=0; j < NrOfTrials(); j++) 
	{
		int *positionsCurTrial = positionsVec+(NrOfROIs()*j);
		for(int i=0; i < NrOfROIs(); i++) {
			positionsCurTrial[i] = i+1;
		}
	}
}



inline void CStandardMeasures::TrialWrapUp_CompleteMeasures() 
{
	LOG_METHOD("CStandardMeasures","TrialWrapUp_CompleteMeasures");

     // handle RRT, RRTP, RRTR 
	for(int i=0; i < NrOfROIs(); i++) {
		// RRT = TFT - FPRT
		rrtVec[i]  = tftVec[i] - fprtVec[i];
		// RRTR = TFT - RBRT
		rrtrVec[i] = tftVec[i] - rbrtVec[i];
		// RRTP = RBRT - FPRT
		rrtpVec[i] = rbrtVec[i] - fprtVec[i];
	
	}

     // handle CRPD
	crpdVec[0] = rpdVec[0];
	for(int i=1; i < NrOfROIs(); i++) {
		crpdVec[i] = rpdVec[i] + crpdVec[i-1];
	}
}


template<typename T>
inline void CStandardMeasures::TrialWrapUp_CompleteInfoForCol(int col, int offset) 
{
	LOG_METHOD("CStandardMeasures","TrialWrapUp_CompleteInfoForCol");
	
	T* infoVec = (T*)trialInfoVec[col] + offset;	
	T curContentDouble = InfoCol_LastTrialContent<T>(col);
	
	for(int j=0; j < NrOfROIs(); j++) {
		infoVec[j] = curContentDouble;
	}

}
template<>
inline void CStandardMeasures::TrialWrapUp_CompleteInfoForCol<char*>(int col, int offset) 
{
	LOG_METHOD("CStandardMeasures","TrialWrapUp_CompleteInfoForCol<char*>");

	char* curContentString = InfoCol_LastTrialContent<char*>(col);
	for(int j=0; j < NrOfROIs(); j++) 
		SET_STRING_ELT(trialInfoR[col], offset+j, mkChar(curContentString));
}


inline void CStandardMeasures::TrialWrapUp_CompleteInfo() 
{
	LOG_METHOD("CStandardMeasures","TrialWrapUp_CompleteInfo");

	int offset = mTrialOffset*NrOfROIs();	
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

void CStandardMeasures::TrialEndHook() 
{
	LOG_METHOD("CStandardMeasures","TrialEndHook");
	
	TrialWrapUp_CompleteInfo();
	TrialWrapUp_CompleteMeasures();
	UpdateToTrialOffset(mTrialOffset+1);
}



