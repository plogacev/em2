#include "MeasuresStandard.h"

/* 
Key:
   SA = saccade assimilation (none, within ROI, )

Measures:
   FFD (First Fixation Duration): 
   	duration of the first fixation on that position iff the fixation was progressive (i.e. no material
	downstream was viewed before this fixation); 
	and FFD=0 otherwise
      SA: none
	
   SFD (Single Fixation Duration): 
   	duration of the first SINGLE fixation on that position (i.e. no subsequent fixations to follow)
	= FFD for single fixations
      SA: none
	
   FPRT (First Pass Reading Time; = Gaze Duration):
   	Sum of all fixations on n starting with the first and ending with a fixation on k!=n
	i.e. all fixations prior to a left- or rightward movement
	(there can be several initial fixations, because different letters of a word can be focused on)
      SA: within n

   RBRT (Right Bounded Reading Time):
   	Sum of all fixations on n starting with the first and ending with a fixation on n+k for any k>0
      SA: within n

   RPD (Regression Path Duration):
   	sum of fixations on n and all n-k as long as no n+i has not been fixated AFTER THE FIRST FIXATION ON n (i,k > 0)

   	i.e. for each fixation on n the current RT is added to all j >= n such that
		a) j has already been fixated at least once AND
	        b) no fixation on j+l has taken place since this first fixation (i.e. the RBRT of j is not terminated)
      SA: assimilation to j iff saccade n -> x, with x <= j  

   CRPD (Cumulative Regression Path Duration):
	Sum of all preceding RPDs.
	TODO: Actually CRPD may be tricky to interpret if one of the positions have only been fixated on
		second pass. Think about a clearer definition!!!
   
   TFT (Total Fixation Time):
   	sum of all fixations on word n
      SA: within n

   RRT (ReReading Time):
   	TFT - FPRT
      SA: within n

   RRTP (ReReading Time Progressive):
	RBRT - FPRT
      SA: within n
	
   RRTR (ReReading Time Regressive):
	TFT - RBRT
      SA: within n
	
------------------------------------

   FFP (First Fixation Progressive):
   	1 if material downstream was viewed before the first fixation on this position

   NFP (Next Fixation Progressive):
	1 if the next saccade is a progression
	
   RBRC (Right Bounded Regression Count, =ROC,Regression Out Count):
 
   TRC (Total Regression Count, =ROFC,Regression Out Full Count):

   FPRP (First Pass Regression Probability):
	1 iff RBRC > 0

   TRP (Regression Probability):
	1 iff TRC > 0
*/

/* NOTES:
 * 	- The design does not allow this code to handle fixations with time = 0 correctly. This should not be a problem
 * 	  however.
 *	- *valid* ROI indices have to be > 0. Any ROI <= 0 is handled as an invalid region.
 * 	- RPD: Shravan's definition says: "if the word is skipped entirely during first-pass reading then 
 * 	       the regression path duration is 0"
 * 	       -> Here it is not zero, just like RBRT. This seems to match the Michigan data, although I don't see the point
 * 	          of such a measure, just like with this kind of definition of RBRT.
 *	- right now saccades can only be assimilated if the following fixation is on a valid region.
 */

/* TODO: 
 * 	- Try not to use rtVec directly, since often "rtVec[curPos]" sneaks in instead of "rtVec[i]"
 * 	- Change 0 to NAs where more appropriate (0 can have an effect on the mean in contrast to NAs; which option makes
 * 	  more sense ?)
 */

#define FIXATED_EARLIER( pos )	(tftVec[pos] > 0)
#define ELIGIBLE_FOR_FIRST_FIXATION( pos ) ( regressiveFirstPass ?  (tftVec[pos] == 0) : (rightmostPos <= pos && (tftVec[pos] == 0)) )

void CStandardMeasures::computeStandardEyemeasures(bool regressiveFirstPass) 
{
	LOG_METHOD("CStandardMeasures","computeStandardEyemeasures");
/*	StartIteration();
	IsValidIndex(); NextFixation();
	IsValidIndex(); NextFixation();
	IsValidIndex(); NextFixation();
	IsValidIndex(); NextFixation();
*/
	bool firstFixationSequence = true;

	for(StartIteration(); IsValidIndex(); NextFixation())
    {
				
		int curPos  = CurrentPosition();
		int nextPos = NextPosition(1);
		LOG(("fixation on <%d>\n", curPos))

		// If the saccade assimilation mode is disabled, then all three values are 
		/// equal, i.e. without saccades. If it is enabled, then:
		// * curFT = the given fixation time
		// * curFTwithSaccade = difference between the starts of the next and this fixation
		// * curFTwithSaccadeWithin = curFTwithSaccade if the next fixation is on the same ROI
		int curFT, curFTwithSaccade, curFTwithSaccadeWithin;
		curFT = CurrentFixationTime(false);
		curFTwithSaccade = CurrentFixationTime(true);
		if( nextPos == curPos)
			curFTwithSaccadeWithin = curFTwithSaccade;
		else
			curFTwithSaccadeWithin = curFT;

		int rightmostPos = RightmostPosition();		
		LOG(("tftVec==<%d>\n", tftVec))
		bool firstFixation = ELIGIBLE_FOR_FIRST_FIXATION(curPos);
		

	   // handle TFT ( this *has* to be done *exactly* here, because TFT is used for indicating 
	   // that a fixation on n has already taken place and RPD relies on this info )
	   	tftVec[curPos] += curFTwithSaccadeWithin;


	   // handle position change
		if ( PositionChange() ) // fixation on a new position ?
		{
			LOG(("fixating on a new position\n"))
			// if we came to a new position and this is not the first fixation overall, 
			// then this is not the first fixation sequence either
			firstFixationSequence = firstFixation;
			
			for(int j=0; j < curPos; j++) {
				if( !ELIGIBLE_FOR_FIRST_FIXATION(j) )
					mBeenToTheRightSinceFirstFixation[j] = true;
			}

	   // handle LPRT (a)
			// reset LPRT in the beginning of every new fixation sequence on a position
			lprtVec[curPos] = 0;


	   // handle TRC
	    		if( PositionChange_Regression() ) { // did we move to the left ?
				// then TRC for the last position is incremented
				++trcVec[LastPosition(0)]; 
			} 

	   // handle RBRC
	    		if(PositionChange_Regression() && // did we move to the left ?
			   ! mBeenToTheRightSinceFirstFixation[ LastPosition(0) ] ) // ... and never been to the right since the first fixation ?
			{ 
				++rbrcVec[LastPosition(0)]; // then RBRC for the last position is incremented
			} 
			
			/*
			 * else { // since a position change took place it has to be to the right
				if( lastPos[1] > lastPos[0] && // was the move before a regression
				    ! mBeenToTheRightSinceFirstFixation[ lastPos[1] ] )
			       	{ 
					++rocVec[lastPos[1]];
				}
			  }
			*/
		} // else: keep this value untill the position is switched again, since untill then nothing should change


	   // handle LPRT (b)
		lprtVec[curPos] += curFTwithSaccadeWithin;
			
	
	   // handle (fp)FFD, FFP, SFD & FPRT
		if( firstFixation )  // first fixation encontered ?	
		{
		LOG(("first fixation\n"))
			// set FFD, SFD & FPRT to this first fixation
			sfdVec[curPos] = curFT;
			fprtVec[curPos] = curFTwithSaccadeWithin;

			// if the current curPosition is the rightmost one up to now, then FFP is 1; else 0 by default
			if( ! BeenToTheRight() ) {
			       	// so the fixation came from the left
				ffpVec[curPos] = 1;
				ffdVec[curPos] = curFT;
			}

		} else { // not the first fixation on that curPosition that is encountered
		LOG(("NOT first fixation\n"))
			sfdVec[curPos] = 0; // delete SFD value
		
			// if no other word has been fixated between this fixation and the last one on this word 	
			if( firstFixationSequence )
				fprtVec[curPos] += curFTwithSaccadeWithin;
		}

	   // handle RBRT
		if( ! mBeenToTheRightSinceFirstFixation[curPos] )
	   		rbrtVec[curPos] += curFTwithSaccadeWithin;
#if __DEBUG__
		else
			LOG(("been to the right\n"))
#endif
		
	   // handle RPD
	   /* - for all n < curPos: the RT on the current position is not part of n's RPD 
	    *   (since since either n has not yet been fixated upon or n's right bound has already been passed)
	    * - for all n <= curPos: the RT on the current position is part of n's RPD *iff* n has already been
	    *   fixated upon and no n+k has been fixated (for any k > 0)
	    */
		for(int j=curPos; j < NrOfROIs(); j++) 
		{
			// if j has been fixated and no position to the right of j has been fixated since the first
			// fixation on j
			if( FIXATED_EARLIER(j) & !mBeenToTheRightSinceFirstFixation[j] ) 
			{
				// if the next saccade targets to the right of j, then do not assimilate the saccade length
				if(nextPos > j)
		   			rpdVec[j] += curFT;
				else // ... else do assimilate it
		   			rpdVec[j] += curFTwithSaccade;
			}
		}
	}
}
