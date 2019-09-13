/** @file

MODULE				: X265v2Codec

TAG						: X265v2Codec

FILE NAME			: X265v2Codec.h

DESCRIPTION		: A 2nd generation of video codecs based on the H.265 standard
								implementation. The primary interface is ICodecv2 for access 
								and configuration.

COPYRIGHT			: (c)CSIR 2007-2015 all rights resevered

LICENSE				: Software License Agreement (BSD License)

RESTRICTIONS	: Redistribution and use in source and binary forms, with or without 
								modification, are permitted provided that the following conditions 
								are met:

								* Redistributions of source code must retain the above copyright notice, 
								this list of conditions and the following disclaimer.
								* Redistributions in binary form must reproduce the above copyright notice, 
								this list of conditions and the following disclaimer in the documentation 
								and/or other materials provided with the distribution.
								* Neither the name of the CSIR nor the names of its contributors may be used 
								to endorse or promote products derived from this software without specific 
								prior written permission.

								THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
								"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
								LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
								A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
								CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
								EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
								PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
								PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
								LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
								NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
								SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

===========================================================================
*/
#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include "ICodecv2.h"

struct x265_picture;
struct x265_param;
struct x265_nal;
struct x265_encoder;

class X265v2Codec : public ICodecv2
{
/// construction
public:
  X265v2Codec();
  virtual ~X265v2Codec();

/// ICodecv2 Interface Implementation
public:

	int		GetParameter(const char* type, int* length, void* value);
	void	GetParameterName(int ordinal, const char** name, int* length);
	int		SetParameter(const char* type, const char* value);

  char* GetErrorStr(void);
  int		Ready(void);
  int		GetCompressedBitLength(void);
  int		GetCompressedByteLength(void);
  void* GetReference(int refNum);

	void	Restart(void);
	int		Open(void);
	int		Close(void);
	int		Code(void* pSrc, void* pCmp, int codeParameter);
	int		Decode(void* pCmp, int bitLength, void* pDst);

/// Codec parameters.
private:

  // uint8_t* pBufferIn;
  x265_picture* pic_in;
  x265_picture* pic_out;
  x265_param* params;
  x265_nal* nals;
  x265_encoder* encoder;
  int num_nals;

  uint32_t m_uiWidth;
  uint32_t m_uiHeight;
  double m_dFps;
  bool m_bAnnexB;
  uint32_t m_uiIFramePeriod;
  uint32_t m_uiCurrentFrame;
  uint32_t m_uiFrameBitLimit;
  uint32_t m_uiCompressedByteLength;
  //uint32_t m_uiEncodingBufferSize;

  uint32_t m_uiTargetBitrate;
  std::string m_sTune;
  std::string m_sPreset;

  std::string m_sError;

  std::string m_sVps;
  std::string m_sSps;
  std::string m_sPps;
};//X265v2Codec class.
