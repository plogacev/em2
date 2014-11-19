#include <strings.h>
#include "eyemeasures.h"
#include "FixSeq.h"

	// TODO: implement regressiveFirstPass

CFixationSequence::CFixationSequence(SEXP positionsArg, SEXP fixationTimesArg, 
				     SEXP fixationStartArg, SEXP fixationEndArg,
				     SEXP trialIdArg, SEXP trialInfoArg,
				     SEXP nrOfROIsArg, SEXP nrOfTrialsArg, 
				     SEXP cutoffArg, SEXP cutoffLengthArg,
				     SEXP useTimeIntervalsArg )
{
	LOG_METHOD("CFixationSequence","CFixationSequence");

        SPROTECT(rPositionsArg 	  	= AS_INTEGER(positionsArg));
        SPROTECT(rFixationTimesArg 	= AS_INTEGER(fixationTimesArg));
        SPROTECT(rFixationStartArg 	= AS_INTEGER(fixationStartArg));
        SPROTECT(rFixationEndArg   	= AS_INTEGER(fixationEndArg));
        SPROTECT(rTrialIdArg   	  	= AS_INTEGER(trialIdArg));
        SPROTECT(rCutoffArg 	  	= AS_INTEGER(cutoffArg));

       	mPositions		= INTEGER_POINTER(rPositionsArg);
        mFixationTimes  = INTEGER_POINTER(rFixationTimesArg);
        mFixationStart  = INTEGER_POINTER(rFixationStartArg);
        mFixationEnd    = INTEGER_POINTER(rFixationEndArg);
        mTrialId	= (unsigned int*)INTEGER_POINTER(rTrialIdArg);
        mSequenceLength = length(positionsArg);
        mCutoff	        = INTEGER_POINTER(rCutoffArg);
        mCutoffLength   = INTEGER_VALUE(cutoffLengthArg);

        mNrOfTrials = INTEGER_VALUE(nrOfTrialsArg);
        // determine maximal position if it is not present
        mNrOfROIs = INTEGER_VALUE(nrOfROIsArg);
        if(! (mNrOfROIs > 0) )
        {
        		int curMax = mPositions[0];
        		for(int i=1; i < mSequenceLength; i++) {
        			if(mPositions[i] > curMax) {
                        	curMax = mPositions[i];
        			}
	        }
        		mNrOfROIs = curMax;
        }
        
        SPROTECT(rTrialInfoArg   = AS_VECTOR(trialInfoArg));
	mTrialInfoColumnCnt 	= length(trialInfoArg);
	rTrialInfoColumn = (SEXP*)malloc(sizeof(SEXP)*mTrialInfoColumnCnt);
	mTrialInfoColumn = (void**)malloc(sizeof(void*)*mTrialInfoColumnCnt);
	for(int i=0; i < mTrialInfoColumnCnt; i++) 
	{
		SPROTECT(rTrialInfoColumn[i] = VECTOR_ELT(rTrialInfoArg, i));
			
		switch( TYPEOF(rTrialInfoColumn[i]) ) {
				case LGLSXP:
					
					mTrialInfoColumn[i] = (void*) LOGICAL( rTrialInfoColumn[i] );
					break;
				case INTSXP:
					
					mTrialInfoColumn[i] = (void*) INTEGER( rTrialInfoColumn[i] );
					break;
				case REALSXP:
					
					mTrialInfoColumn[i] = (void*) REAL( rTrialInfoColumn[i] );
					break;
				case CPLXSXP:
					
					mTrialInfoColumn[i] = (void*) COMPLEX( rTrialInfoColumn[i] );
					break;
				case STRSXP:
					
					mTrialInfoColumn[i] = NULL;
					break;
				default:
					// NOT SUPPORTED
					break;
		}
	}

	mUseTimeIntervals = LOGICAL_VALUE(useTimeIntervalsArg);
	mBeenToTheRightSinceFirstFixation = new bool[mNrOfROIs];

	StartIteration();
	
	LOG(("number of trials %d\n", NrOfTrials()))

}

CFixationSequence::~CFixationSequence()
{
	LOG_METHOD("CFixationSequence","~CFixationSequence");

	delete mBeenToTheRightSinceFirstFixation;
	SUNPROTECT_PTR(rPositionsArg);
	SUNPROTECT_PTR(rFixationTimesArg);
	SUNPROTECT_PTR(rFixationStartArg);
	SUNPROTECT_PTR(rFixationEndArg);
	SUNPROTECT_PTR(rTrialIdArg);
	if(mCutoff != NULL)
		SUNPROTECT_PTR(rCutoffArg);

	
	for(int i=0; i < mTrialInfoColumnCnt; i++) {
		SUNPROTECT_PTR(rTrialInfoColumn[i]);
	}
	free(rTrialInfoColumn);
	free(mTrialInfoColumn);
	SUNPROTECT_PTR(rTrialInfoArg);
}


void CFixationSequence::StartIteration()
{
	LOG_METHOD("CFixationSequence","StartIteration");

	mCurrentIndex   = 0;
	UpdateTrialInfo();
	UpdateFixationInfo();
	
	if( !IsValidFixation() ) {
		NextFixation();
	} 
	
	mRightmostPositionSoFar = mCurrentPosition;

}

void CFixationSequence::TrialStart() 
{
	LOG_METHOD("CFixationSequence","TrialStart");
	
	for(int i=0; i < sizeof(mLastPositions)/sizeof(mLastPositions[0]); i++)
		mLastPositions[ i ] = -1;

	mCurrentPosition = -1;
	
	memset(mBeenToTheRightSinceFirstFixation, 0, sizeof(bool)*mNrOfROIs);
	mRightmostPositionSoFar = 0;
	
	TrialStartHook();

}

void CFixationSequence::TrialStartHook() 
{
	LOG_METHOD("CFixationSequence","TrialStartHook");
}

void CFixationSequence::TrialEnd() {
	LOG_METHOD("CFixationSequence","TrialEnd");		
	TrialEndHook();
}
void CFixationSequence::TrialEndHook() {
	LOG_METHOD("CFixationSequence","TrialEndHook");
		
}
