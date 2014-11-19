.packageName <- "em2"

discontinuity <- function(x) {
	return (length(unique(x)) != length(rle(x)[['values']]))
}


em2 <- function( rid, fixationdur, trialinfo, measures="standard",
				assimilateSaccades=FALSE, fixationstart=NA, fixationend=NA,
				regressiveFirstPass=FALSE)
{
	rid.len = length(rid)
	stopifnot(rid.len == length(fixationdur))
	stopifnot(is.data.frame(trialinfo) )
  	stopifnot(rid.len == nrow(trialinfo) )
 	stopifnot( (is.na(fixationstart) && length(fixationstart)) || rid.len ==  nrow(trialinfo))

	cutoff <- NA

	maxrid <- max(rid, na.rm=T)

	# trialIndexCmd <- paste( "trialid[[", 1:length(trialid), "]]", collapse=",")
	# trialIndexCmd <- paste("paste(", trialIndexCmd, ", sep='-')") 
	# trialIndex <- factor(eval(parse(text=trialIndexCmd)))
	trialIndex <- plyr::id(trialinfo)

	if(discontinuity(as.integer(trialIndex))) {
		stop("Trial numbering is discontinuous.")
		return(NULL);
	}

	cutoffLen <- length(cutoff);
	if( is.na(cutoff[1]) ) {
		cutoff <- 0; cutoffLen <- 0;
	}

	if(measures == "standard") { cFunctionName <- "computeStandardEyemeasuresExt" }
	else if(measures == "CRI") { cFunctionName <- "computeCRIExt" }

	# change offset to make the values fit the type "integer" instead of "double" 
	fixationstart <- (fixationstart - min(fixationstart))
	fixationend <- (fixationend - min(fixationend))

	ret <- .Call(cFunctionName, 
		as.integer(rid), as.integer(fixationdur), 
		as.integer(fixationstart), as.integer(fixationend),
		as.integer(trialIndex), as.list(trialinfo),
		as.integer(maxrid), as.integer(length(unique(trialIndex))),
		as.integer(cutoff), as.integer(cutoffLen), as.logical(regressiveFirstPass), 
		as.logical(assimilateSaccades), as.integer(NA), PACKAGE="em2" )

	# copy attributes, so that factors are displayed properly
	for( name in names(trialinfo)) {
		attributes(ret[[name]]) <- attributes(trialinfo[[name]])
	}
	ret <- as.data.frame(ret)

	return(ret)
}
