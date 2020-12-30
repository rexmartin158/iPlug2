#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IPlugSOUL_DSP.h"

const int kNumPresets = 1;

using namespace iplug;
using namespace igraphics;

class IPlugSOUL final : public Plugin
{
public:
  IPlugSOUL(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  
  IPlugSOUL_DSP mDSP;
  int mSessionID = 0;
  IPlugQueue<int> mParamsToUpdate {IPlugSOUL_DSP::numParameters};
  std::vector<IPlugSOUL_DSP::Parameter> mSOULParams;
#endif
};
