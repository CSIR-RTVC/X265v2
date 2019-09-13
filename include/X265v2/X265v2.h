// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the X265V2_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// X265V2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once

#ifdef _WIN32
#ifdef X265v2_EXPORTS
#define X265V2_API __declspec(dllexport)
#else
#define X265V2_API __declspec(dllimport)
#endif
#else
#define X265V2_API 
#endif

/// This dll has only one purpose to instantiate a X265v2Codec instance. The
/// obligation of scope management is left to the calling functions.
#include "X265v2Codec.h"

// This class is exported from the X265v2.dll
class X265V2_API X265v2Factory 
{
public:
	X265v2Factory(void);
	~X265v2Factory(void);

	/// Interface.
	X265v2Codec* GetCodecInstance(void);
	void				 ReleaseCodecInstance(ICodecv2* pInst);
};	///end X265v2Factory.
