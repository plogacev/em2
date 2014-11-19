#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#include <string.h>
#include <stdio.h>

#ifndef __FIX_SEQ_H__
#define __FIX_SEQ_H__

class CFixationSequence 
{
	public:
		CFixationSequence(SEXP positionsArg, SEXP fixationTimesArg, 
				  SEXP fixationStartArg, SEXP fixationEndArg,
				  SEXP trialIdArg, SEXP trialIInfoArg,
				  SEXP nrOfROIsArg, SEXP nrOfTrials,
				  SEXP cutoffArg, SEXP cutoffLengthArg,
				  SEXP useTimeIntervalsArg);
		~CFixationSequence();
	
		int NrOfROIs();
		int NrOfTrials();

	public: // iteration issues
		void StartIteration();
		void NextFixation();
		void PrevFixation(); // NOTE: so far this function is not compatible 
				     // with RightmostPosition()
		bool IsValidFixation();
		bool IsValidIndex();

	private:
		bool IsValidPosition(int index);
		bool IsValidIndex(int index);

	public: // member access
		int CurrentPosition();
		int LastPosition(int howFarBack);
		int NextPosition(int howFarForward);
		int CurrentFixationTime(bool assimilateSaccade=true);//assimilateSaccade is only taken into account if
								     //useTimeIntervalsArg is set to true in the constructor
		
		int RightmostPosition();
		bool BeenToTheRight();

		bool PositionChange();
		bool PositionChange_Progression();
		bool PositionChange_Regression();

//		bool IsNewTrial();
//		unsigned int CurrentTrialId();

		int  InfoCol_Count();
		int  InfoCol_DataType(int col);
		template<typename T> T InfoCol_LastTrialContent(int col);

	private:
		void UpdateFixationInfo();
		void UpdateTrialInfo();

		void DoCutoff();

		void TrialStart();	
		void TrialEnd();	

	// hooks
	private: 
		virtual void TrialStartHook();	
		virtual void TrialEndHook();	
		
//	private: // data members		
	public:	
		int *mPositions;
		int *mFixationTimes;
		int *mFixationStart;
		int *mFixationEnd;
		int mSequenceLength;
		unsigned int *mTrialId;

		int *mCutoff;
		int mCutoffLength;
		bool mUseTimeIntervals;
		bool mRegressiveFirstPass;

		int mNrOfROIs;
		int mNrOfTrials;

		// for iterating
		int mCurrentIndex; 	// current index in the fixation sequence
		int mCurrentPosition;	// index of the current ROI, IMPORTANT: the pos nr is always: (ROI - 1)
		int mLastPositions[2];

		/*
		// for noticing trial switching
		unsigned int mCurrentTrialId;
		bool 	     mNewTrial;
		int	     mLastTrialIndex; // this variable only points to the last trial *iff* 
					      // mNewTrial is true
*/
		
		// for keeping track of the trial so far
		bool *mBeenToTheRightSinceFirstFixation;
		int mRightmostPositionSoFar;


		

	private: // R interface members
		SEXP rPositionsArg;
		SEXP rFixationTimesArg;
		SEXP rFixationStartArg;
		SEXP rFixationEndArg;
		SEXP rTrialIdArg;
		SEXP rCutoffArg;
		SEXP rTrialInfoArg;

		SEXP* rTrialInfoColumn;
		void** mTrialInfoColumn;
		int   mTrialInfoColumnCnt;
			
};

#include "FixSeq_Inl.h"

#endif // __FIX_SEQ_H__
