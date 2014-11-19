  \name{em2.dset}
\alias{em2.dset}
\title{ Compute Eye Fixation Measures }
\description{
  Generate fixation measures relevant for sentence processing research.
}
\usage{
em2.dset()
}
\details{

The em2.dset() function creates a toy dataset to illustrate the functionality of em2().
}
\author{ Pavel Logacev <pavel.logacev@gmail.com> }
%\seealso{ \code{\link{summary.maxchi}}, \code{\link{phylpro}}  }
\examples{ 
# generate a toy dataset
data <- em2.dset()

# generate standard measures
data.em <- em2(data$rid, data$fixationdur, data[,c('subject','trial','response')])
	
}
%\keyword{ htest }
