

inline void CStandardMeasures::UpdateToTrialOffset(unsigned int trialOffset) 
{
	mTrialOffset = trialOffset;
	unsigned long offset = mTrialOffset*NrOfROIs();

	LOG(("setting to offset %d\n", offset))
	
	ffdVec  = ffdAllVec  + offset;
	ffpVec  = ffpAllVec  + offset;
	sfdVec  = sfdAllVec  + offset;
	fprtVec = fprtAllVec + offset;
	rbrtVec = rbrtAllVec + offset;
	tftVec  = tftAllVec  + offset;
	rpdVec  = rpdAllVec  + offset;
	crpdVec = crpdAllVec + offset;
	rrtVec  = rrtAllVec  + offset;
	rrtpVec = rrtpAllVec + offset;
	rrtrVec = rrtrAllVec + offset;
	rbrcVec = rbrcAllVec + offset;
	trcVec  = trcAllVec  + offset;
	lprtVec = lprtAllVec + offset;

}
