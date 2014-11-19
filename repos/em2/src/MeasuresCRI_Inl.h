

inline void CCRIMeasures::UpdateToTrialOffset(unsigned int trialOffset) 
{
	mTrialOffset = trialOffset;
	unsigned long offset = mTrialOffset*NrOfROIs()*NrOfROIs();

	mCFT_Cur  = mCFT  + offset;
	mCFC_Cur  = mCFC  + offset;
}
