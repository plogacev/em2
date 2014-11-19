
inline void CFixationSequence::DoCutoff() 
{
	int i;
	for(i=0; IsValidIndex() && i < mCutoffLength; mCurrentIndex++ )
       	{
		if(IsValidFixation() && mPositions[ mCurrentIndex] == mCutoff[i]) {
			i++;
		}
	}
	
	// cut all subsequent occurences of the last specified ROI
	while( IsValidIndex() && mPositions[ mCurrentIndex ] == mCutoff[i] )
		mCurrentIndex++;
}


inline void CFixationSequence::NextFixation()
{
	do {
		mCurrentIndex++;
		UpdateTrialInfo();
	} while( IsValidIndex() && !IsValidFixation() );
	UpdateFixationInfo();

	if(mRightmostPositionSoFar < mCurrentPosition ) {
		mRightmostPositionSoFar = mCurrentPosition;
	}
}

/*
 * inline void CFixationSequence::PrevFixation() {
 *	do {
 *		mCurrentIndex--;
 *	} while( IsValidIndex() && !IsValidFixation() );
 *	UpdatePositions();
 *}
 */

inline void CFixationSequence::UpdateTrialInfo()
{
	LOG_METHOD("CFixationSequence","UpdateTrialInfo");
	if( mCurrentIndex == 0 )		// start of sequence ?
	{
		TrialStart();
	}
	else if( mCurrentIndex >= mSequenceLength )		// end of sequence ?
	{
		TrialEnd();
	}
	else if(	mTrialId[ mCurrentIndex] != mTrialId[ mCurrentIndex-1]) // new trial ?
	{
		TrialEnd();
		TrialStart();
	}
}

inline void CFixationSequence::UpdateFixationInfo()
{
	if( IsValidIndex() ) 
	{
		mLastPositions[1] = mLastPositions[0];
		mLastPositions[0] = mCurrentPosition;
		mCurrentPosition = mPositions[ mCurrentIndex ];
	}
}

inline bool CFixationSequence::IsValidIndex() {
	return IsValidIndex( mCurrentIndex );
}

inline bool CFixationSequence::IsValidIndex(int index) {
	return !(index < 0) && (index < mSequenceLength);
}
inline bool CFixationSequence::IsValidPosition(int index) {
	return IsValidIndex(index) && (mPositions[ index ] > 0);
}

inline bool CFixationSequence::IsValidFixation() {
	return IsValidIndex() && 
	       IsValidPosition( mCurrentIndex ) &&
       	       (mFixationTimes[ mCurrentIndex ] > 0);		       
}

// positions are always ROI-1, so they can be used as indices directly
inline int CFixationSequence::CurrentPosition() {
	return mCurrentPosition - 1 ;
}

inline int CFixationSequence::LastPosition(int howFarBack) 
{ 
	if( ((unsigned) howFarBack) < sizeof(mLastPositions)/sizeof(mLastPositions[0]) )
		return mLastPositions[howFarBack] - 1 ; 
	else // invalid index
		return -1;
}
inline int CFixationSequence::NextPosition(int howFarForward) 
{ 
	int nextIndex = mCurrentIndex+howFarForward;
	if( IsValidIndex(nextIndex) ) 
		return  mPositions[nextIndex] - 1 ;

	else // invalid index
		return -1;
}

inline int CFixationSequence::RightmostPosition() 
	{ return mRightmostPositionSoFar - 1 ; }

inline bool CFixationSequence::BeenToTheRight() 
	{ return (mRightmostPositionSoFar > mCurrentPosition ); }

inline bool CFixationSequence::PositionChange() 
	{ return (mLastPositions[0] != mCurrentPosition); }

inline bool CFixationSequence::PositionChange_Progression() 
	{ return (mLastPositions[0] < mCurrentPosition); }

inline bool CFixationSequence::PositionChange_Regression()  
	{ 
	if (mLastPositions[0] > mCurrentPosition) {
		LOG(("regression from %d to %d\n", mLastPositions[0], mCurrentPosition  ))
	}
	return (mLastPositions[0] > mCurrentPosition); 
	}

inline int CFixationSequence::CurrentFixationTime(bool assimilateSaccade)
{
	// if saccade assimilation is enabled & requested & the next fixation is a valid one
	if( mUseTimeIntervals && assimilateSaccade && NextPosition(1) >= 0 )
		return mFixationStart[ mCurrentIndex+1 ] - mFixationStart[ mCurrentIndex ];
	else
		return mFixationTimes[ mCurrentIndex ]; 
}

inline int CFixationSequence::NrOfROIs()   
	{ return mNrOfROIs; }

inline int CFixationSequence::NrOfTrials() 
	{ return mNrOfTrials; }

inline int  CFixationSequence::InfoCol_Count()
	{ return mTrialInfoColumnCnt;}

inline int  CFixationSequence::InfoCol_DataType(int col)
	{ return TYPEOF(rTrialInfoColumn[col]); }

// get content of the info column for the last trial 
// ... for simple data types ...
template<typename T>
inline T CFixationSequence::InfoCol_LastTrialContent(int col)
{
	return ((T*)mTrialInfoColumn[col])[mCurrentIndex-1];
}
// ... and for strings
template<>
inline char* CFixationSequence::InfoCol_LastTrialContent<char*>(int col)
{
	return (char*) CHAR(STRING_ELT(rTrialInfoColumn[col], mCurrentIndex-1));
}
