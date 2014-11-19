# create dataset

em2.dset <- function() 
{
    testdset <- c()
    ids <- list( c(1,1), c(1,2), c(1,3),
  	     c(2,1), c(2,2), c(2,3),
	     c(3,1), c(3,2), c(3,3))
     
     for(id in ids) {
	len <- as.integer(rnorm(1, 20, 5))
	dset <- data.frame(subject=id[1], 
			   trial=id[2],
			   fixationdur=as.integer(rchisq(len,200)),
			   response=(rnorm(1) < 0),
			   rid=as.integer(runif(len, min=1, max=len/1.5))
			  )
	start <- c(); end <- c()
	time <- 0
	for(fixdur in dset$fixationdur) {
		time <- time + as.integer(rchisq(1, 10))
		start <- append(start, time)
		time <- time + fixdur 
		end  <- append(end, time)
	}
	dset$fixationstart <- start
	dset$fixationend   <- end

     testdset <- rbind(testdset, dset)
     }
     

     return(testdset)
}

