#include <stdio.h>


#ifndef __DEBUG_H__


#define __DEBUG__ 0

class CMethodLogger
{
	public:
    		CMethodLogger(char* _class, char *method);
		~CMethodLogger();
    			
    	private:
    		char *mClass;
    		char *mMethod;
};

#ifdef __DEBUG__ 
	#if __DEBUG__ 
      		#define LOG(_x)	printf _x ;
		#define LOG_METHOD(_class, _method) CMethodLogger(_class, _method)
	#else
		#define LOG(_x)	
		#define LOG_METHOD(_class, _method) 
	#endif

#else 
   	#define LOG(_x)	
	#define LOG_METHOD(_class, _method) 
#endif

#endif //__DEBUG_H__
