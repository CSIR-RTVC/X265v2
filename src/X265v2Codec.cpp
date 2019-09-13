#include "X265v2Codec.h"
#include <cassert>
#include <vector>
#include <x265.h>
#include <CodecUtils/H265Util.h>
#include <GeneralUtils/Conversion.h>

const std::vector<std::string> X265Parameters = { "width", "height", "fps", "target_bitrate_kbps", "annexb", "tune", "preset", "parameters", "annexb_vps", "annexb_sps", "annexb_pps" };
const std::vector<std::string> TuneOptions = { "psnr", "ssim", "grain", "fastdecode", "zerolatency" };
const std::vector<std::string> PresetOptions = { "ultrafast", "superfast", "veryfast", "faster", "fast", "medium", "slow", "slower", "veryslow", "placebo" };

using vpp::convert;
using namespace vpp::h265;

X265v2Codec::X265v2Codec()
  :/*pBufferIn(nullptr),*/
  pic_in(nullptr),
  pic_out(nullptr),
  params(nullptr),
  nals(nullptr),
  encoder(nullptr),
  num_nals(0),
  m_uiWidth(0),
  m_uiHeight(0),
  m_dFps(0.0),
  m_bAnnexB(true),
  m_uiIFramePeriod(0),
  m_uiCurrentFrame(0),
  m_uiFrameBitLimit(0),
  m_uiCompressedByteLength(0),
  m_uiTargetBitrate(500),
  m_sTune(TuneOptions.at(4)),
  m_sPreset(PresetOptions.at(0))
{

}

X265v2Codec::~X265v2Codec()
{
  Close();
  x265_cleanup();
}

int	X265v2Codec::GetParameter(const char* type, int* length, void* value)
{
  std::string sName(type);
  if (sName == "width")
  {
    sprintf((char *)value, "%d", m_uiWidth);
  }
  else if (sName == "height")
  {
    sprintf((char *)value, "%d", m_uiHeight);
  }
  else if (sName == "fps")
  {
    sprintf((char *)value, "%f", m_dFps);
  }
  else if (sName == "target_bitrate_kbps")
  {
    sprintf((char *)value, "%d", m_uiTargetBitrate);
  }
  else if (sName == "annexb")
  {
    sprintf((char *)value, "%d", (int)m_bAnnexB);
  }
  else if (sName == "tune")
  {
    sprintf((char *)value, "%s", m_sTune.c_str());
  }
  else if (sName == "preset")
  {
    sprintf((char *)value, "%s", m_sPreset.c_str());
  }
  else if (sName == "parameters")
  {
    sprintf((char *)value, "%d", X265Parameters.size());
  }
  else if (sName == "annexb_vps")
  {
    memcpy(value, m_sVps.c_str(), m_sVps.length());
    *length = m_sVps.length();
    return(1);
  }
  else if (sName == "annexb_sps")
  {
    memcpy(value, m_sSps.c_str(), m_sSps.length());
    *length = m_sSps.length();
    return(1);
  }
  else if (sName == "annexb_pps")
  {
    memcpy(value, m_sPps.c_str(), m_sPps.length());
    *length = m_sPps.length();
    return(1);
  }
  else
  {
    m_sError = "[X265v2Codec::GetParameter] Read parameter not supported";
    return(0);
  }//end else...

  *length = (int)strlen((char *)value);
  return(1);
}

void X265v2Codec::GetParameterName(int ordinal, const char** name, int* length)
{
  if ((ordinal < 0) || ((uint32_t)ordinal >= X265Parameters.size()))
    return;
  *name = X265Parameters.at(ordinal).c_str();
  *length = X265Parameters.at(ordinal).length();
}

int	X265v2Codec::SetParameter(const char* type, const char* value)
{
  std::string sName(type);
  std::string sValue(value);
  bool success;
  if (sName == "width")
  {
    m_uiWidth = convert<uint32_t>(sValue, success);
    if (!success) { return 0; }
  }
  else if (sName == "height")
  {
    m_uiHeight = convert<uint32_t>(sValue, success);
    if (!success) { return 0; }
  }
  else if (sName == "fps")
  {
    m_dFps = convert<double>(sValue, success);
    if (!success) { return 0; }
  }
  else if (sName == "bitrate")
  {
    m_uiTargetBitrate = convert<uint32_t>(sValue, success);
    if (!success) { return 0; }
  }
  else if (sName == "annexb")
  {
    m_bAnnexB = convert<bool>(sValue, success);
    if (!success) { return 0; }
  }
  else if (sName == "tune")
  {
    auto it = find_if(TuneOptions.begin(), TuneOptions.end(), [sValue](const std::string& sOption){ return sOption == sValue; });
    if (it == TuneOptions.end()) { return 0; }
    else
    {
      m_sTune == *it;
    }
  }
  else if (sName == "preset")
  {
    auto it = find_if(PresetOptions.begin(), PresetOptions.end(), [sValue](const std::string& sOption){ return sOption == sValue; });
    if (it == PresetOptions.end())  { return 0; }
    else
    {
      m_sPreset == *it;
    }
  }
  else
  {
    return 0;
  }
  return 1;
}

char* X265v2Codec::GetErrorStr(void)
{
  // HACK
  return (char*)m_sError.c_str();
}

int	X265v2Codec::Ready(void)
{
  return encoder != nullptr;
}

int	X265v2Codec::GetCompressedBitLength(void)
{
  return m_uiCompressedByteLength * 8;
}

int		X265v2Codec::GetCompressedByteLength(void)
{
  return m_uiCompressedByteLength;
}

void* X265v2Codec::GetReference(int refNum)
{
  // Not implemented
  return 0;
}

void X265v2Codec::Restart(void)
{
#pragma chMSH(TODO: reset relevant members)
}

int X265v2Codec::Open(void)
{
  std::ostringstream error;
  m_sError = "";

  if (encoder)
  {
    error << "Encoder needs to be closed before re-opening";
    m_sError = error.str();
    return 0; // NB return here
  }

  do
  {
    if (m_uiHeight == 0 || m_uiWidth == 0 || m_dFps == 0.0)
    {
      error << "Invalid settings for open - width: " << m_uiWidth << " height: " << m_uiHeight << " fps: " << m_dFps;
      m_sError = error.str();
      break;
    }

    params = x265_param_alloc();
    if (!params)
    {
      error << "Failed to allocate x65 params";
      m_sError = error.str();
      break;
    }

    int res = x265_param_default_preset(params, m_sPreset.c_str(), m_sTune.c_str());
    if (res < 0)
    {
      error << "Invalid preset/tune name";
      m_sError = error.str();
      break;
    }

    // optionally set profile
    res = x265_param_apply_profile(params, "main");
    if (res < 0)
    {
      error << "Invalid profile name";
      m_sError = error.str();
      break;
    }

    // params->ti_threads = 1;
    params->sourceWidth = m_uiWidth;
    params->sourceHeight = m_uiHeight;
    // HACK for now: we use doubles (won't work for 12.5)
    params->fpsNum = (int)m_dFps;
    params->fpsDenom = 1;
    params->internalCsp = X265_CSP_I420;
    params->bRepeatHeaders = true;
    params->bEnableAccessUnitDelimiters = false;
    params->bAnnexB = m_bAnnexB;
    // for testing
#if 1
    params->bOpenGOP = false;
    params->keyframeMax = 30;
#endif
#if 1
    params->rc.bitrate = m_uiTargetBitrate;
    params->rc.rateControlMode = X265_RC_ABR;
    params->rc.vbvBufferSize = static_cast<int>(m_uiTargetBitrate / m_dFps);
    //params->rc.bitrate = 0;
    //params->rc.rateControlMode = X265_RC_CQP;
#endif

    encoder = x265_encoder_open(params);
    if (!encoder)
    {
      error << "Failed to x265_encoder_open";
      m_sError = error.str();
      break;
    }

    uint32_t uiNalCount = 0;
    int header_size = x265_encoder_headers(encoder, &nals, &uiNalCount);
    char* pNalus = new char[header_size];
    uint32_t uiCurrentPos = 0;
    for (size_t i = 0; i < uiNalCount; ++i)
    {
      int iLength = nals[i].sizeBytes;
      memcpy(pNalus + uiCurrentPos, nals[i].payload, iLength);

      // we're only storing annex-B style start codes for now so overwrite the length until we solve this
      if (!m_bAnnexB)
      {
#if 0
        pStart[uiCurrentPos] = (iLength >> 24);
        pStart[uiCurrentPos + 1] = (iLength >> 16);
        pStart[uiCurrentPos + 2] = (iLength >> 8);
        pStart[uiCurrentPos + 3] = (iLength & 0xFF);
#else
        pNalus[uiCurrentPos] = 0;
        pNalus[uiCurrentPos + 1] = 0;
        pNalus[uiCurrentPos + 2] = 0;
        pNalus[uiCurrentPos + 3] = 1;
#endif
      }

      if (isVps(pNalus[uiCurrentPos + 4]))
      {
        m_sVps = std::string((const char*)&pNalus[uiCurrentPos], iLength);
      }
      else if (isSps(pNalus[uiCurrentPos + 4]))
      {
        m_sSps = std::string((const char*)&pNalus[uiCurrentPos], iLength);
      }
      else if (isPps(pNalus[uiCurrentPos + 4]))
      {
        m_sPps = std::string((const char*)&pNalus[uiCurrentPos], iLength);
      }
      uiCurrentPos += (nals[i].sizeBytes);
    }

    delete[] pNalus;

    pic_in = x265_picture_alloc();
    x265_picture_init(params, pic_in);
    // pic.planes
    pic_in->stride[0] = m_uiWidth;
    pic_in->stride[1] = pic_in->stride[2] = m_uiWidth >> 1;
    pic_in->bitDepth = 8;
    pic_in->colorSpace = X265_CSP_I420;
    pic_in->framesize = static_cast<uint64_t>(m_uiWidth * m_uiHeight * 1.5);
    pic_in->height = m_uiHeight;
#if 0
    pic_in->planes[0] = (uint8_t*)pBufferIn;
    pic_in->planes[1] = (uint8_t*)pic_in->planes[0] + pic_in->stride[0] * m_in.getHeight();
    pic_in->planes[2] = (uint8_t*)pic_in->planes[1] + ((m_in.getWidth() * m_in.getHeight()) >> 2);
#endif

    // not currently using out pic
    pic_out = x265_picture_alloc();
    x265_picture_init(params, pic_out);
    break;
  } while (true); // error loop

  if (m_sError != "")
  {
    Close();
    return 0;
  }
  return 1;
}

int X265v2Codec::Close(void)
{
  if (params)
  {
    x265_param_free(params);
    params = NULL;
  }

  if (encoder)
  {
    x265_encoder_close(encoder);
    encoder = NULL;
  }

  if (pic_in)
  {
    x265_picture_free(pic_in);
    pic_in = NULL;
  }

  if (pic_out)
  {
    x265_picture_free(pic_out);
    pic_out = NULL;
  }
  x265_cleanup();
  return 0;
}

int	X265v2Codec::Code(void* pSrc, void* pCmp, int codeParameter)
{
  assert(encoder);
#if 0
  // we only handle one sample at a time
  assert(in.size() == 1);
  const MediaSample& mediaIn = in[0];
  // const uint8_t* pBufferIn = mediaIn.getDataBuffer().data();
  memcpy(pBufferIn, mediaIn.getDataBuffer().data(), m_uiEncodingBufferSize);
#else
  // avoid copy, reset pointers
  pic_in->planes[0] = (uint8_t*)pSrc;
  pic_in->planes[1] = (uint8_t*)pic_in->planes[0] + pic_in->stride[0] * m_uiHeight;
  pic_in->planes[2] = (uint8_t*)pic_in->planes[1] + ((m_uiWidth * m_uiHeight) >> 2);

#endif

  uint32_t uiNalCount = 0;
  int frame_size = x265_encoder_encode(encoder, &nals, &uiNalCount, pic_in, NULL);
  if (frame_size > 0)
  {
    uint32_t uiLen = 0;
    for (size_t i = 0; i < uiNalCount; ++i)
    {
      uiLen += nals[i].sizeBytes;
#if 0
      VLOG(2) << "Transform complete: "
        << " i: " << i
        << " NALU type: " << nals[i].type
        << " frame size: " << nals[i].sizeBytes;
#endif
      if (!m_bAnnexB)
      {
        uiLen += 4; // length prefix
      }
    }

    if ((int)uiLen > codeParameter)
    {
      std::ostringstream error;
      error << "Destination buffer (" << codeParameter << ") to small for encoded data: " << frame_size;
      m_sError = error.str();
      return 0;
    }
    m_uiCompressedByteLength = uiLen;
    uint32_t uiCurrentPos = 0;
    
    int iPreNalOffset = m_bAnnexB ? 0 : 4; // length prefixes
    for (size_t i = 0; i < uiNalCount; ++i)
    {
      int iLength = nals[i].sizeBytes;
      uint8_t* pStart = (uint8_t*)pCmp;
      memcpy((char*)pCmp + uiCurrentPos + iPreNalOffset, nals[i].payload, iLength);

      if (!m_bAnnexB)
      {
        pStart[uiCurrentPos] = (iLength >> 24);
        pStart[uiCurrentPos + 1] = (iLength >> 16);
        pStart[uiCurrentPos + 2] = (iLength >> 8);
        pStart[uiCurrentPos + 3] = (iLength & 0xFF);
      }

      uiCurrentPos += (nals[i].sizeBytes + iPreNalOffset);
    }
    return 1;
  }
  else
  {
    if (frame_size == 0)
    {
      return 1;
    }
    else
    {
      m_sError = "Transform failed";
      return 0;
    }
  }
}

int X265v2Codec::Decode(void* pCmp, int bitLength, void* pDst)
{
  assert(false);
  return -1;
}
