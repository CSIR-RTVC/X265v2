// X265v2.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "X265v2.h"

#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif

X265v2Factory::X265v2Factory(void)
{
}//end constructor.

X265v2Factory::~X265v2Factory(void)
{
}//end destructor.

X265v2Codec* X265v2Factory::GetCodecInstance(void)
{
	return(new X265v2Codec());
}//end GetCodecInstance.

void X265v2Factory::ReleaseCodecInstance(ICodecv2* pInst)
{
	if(pInst != NULL)
	{
		delete pInst;
		pInst = NULL;
	}//end if pInst...
}//end ReleaseCodecInstance.
