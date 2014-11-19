#include "debug.h"


CMethodLogger::CMethodLogger(char* _class, char *_method) 
{
	mClass  = _class;
	mMethod = _method;
	LOG(("<%s::%s, call id: 0x%x>\n", mClass, mMethod, this))
}

CMethodLogger::~CMethodLogger() 
{
	LOG(("</%s::%s, call id: 0x%x>\n", mClass, mMethod, this))
}
