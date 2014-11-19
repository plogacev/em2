#include "MeasuresCRI.h"


/* computes CRI (Conditional Regression Information) for each position. CRI consists of CFC and CFT

   CFC (Conditional CFC): The CFC vector for a position n is an integer vector containing the number of fixations for all
	positions < n, computed from the portion of the fixation sequence, such that this portion starts with the
	*first* fixation (ever) on n, and ends with the first following fixation on n+k. Obviously the vector contains
	only fixations on positions < n.
	Any number of subsequent fixations on one position (i.e. without leaving to another position) is counted as one
	fixation.
	-> "conditional" because: the condition is that n was fixated and n+k was not

   CFT (Conditional Ftixation Times): The CFT vector contains the sum of fixation times for the same portion of the
	fixation sequence as CFC.
*/
void CCRIMeasures::computeCRI()
{
	int curPos, lastPos = 0;

LOG(("<compute CRI>\n"))

	// these pointers adress portions of CFTVec and CFCVec corresponding to the current rightmostPos
	int *curCFTVec = NULL;
	int *curCFCVec = NULL;

LOG(("init finished\n"))

	for(StartIteration(),curPos = 0; IsValidIndex(); NextFixation())
    {
		lastPos = curPos;
		curPos = CurrentPosition();
		int curFT = CurrentFixationTime();

		// set both cur* pointers to adress current righmostPos in CFTVec and CFCVec 
		int offset = RightmostPosition()*NrOfROIs();
		curCFTVec = mCFT_Cur + offset;
		curCFCVec = mCFC_Cur + offset;
		
LOG(("curPos = %d, rightmost position = %d\n", curPos, RightmostPosition()))

		if( RightmostPosition() != curPos) 
		{
		  // handle CFCs
			if( lastPos != curPos) { // if the position was changed
				++curCFCVec[ curPos ];
			}

		  // handle CFTs
			curCFTVec[ curPos ] += curFT;
		}
	}
LOG(("</compute CRI>\n"))
}

