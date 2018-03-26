#pragma once

/**
 * @file
 * @copydoc IPlugFaust
 */

#include "faust/gui/UI.h"
#include "faust/gui/MidiUI.h"
#include "assocarray.h"

#include "IPlugBase.h"

#include "Oversampler.h"

#if defined OS_MAC || defined OS_LINUX
#define DEFAULT_FAUST_LIBRARY_PATH "/usr/local/share/faust/"
#else
#define DEFAULT_FAUST_LIBRARY_PATH //TODO
#endif

/** This abstract interface is used by the IPlug FAUST architecture file and the IPlug libfaust JIT compiling class FaustGen
 * In order to provide a consistent interface to FAUST DSP whether using the JIT compiler or a compiled C++ class */
class IPlugFaust : public UI, public Meta
{
public:

  IPlugFaust(const char* name, int nVoices = 1)
  : mNVoices(nVoices)
  {
    mName.Set(name);
  }

  virtual ~IPlugFaust()
  {
    //delete mMidiUI;

    mParams.Empty(true);
  }

  virtual void Init(int recompileInterval = 0, int oversampling = 1, int maxNInputs = -1, int maxNOutputs = -1) = 0;

  /** In FaustGen this is implemented, so that the SVG files generated by a specific instance can be located. The path to the SVG file for process.svg will be returned.
   * There is a NO-OP implementation here so that when not using the JIT compiler, the same class can be used interchangeably
   * @param path The absolute path to process.svg for this instance. */
  virtual void GetDrawPath(WDL_String& path) {}

  /** Call this method with the JIT compiler in order to execute a shell command and compile the C++ code against the IPlugFaust_arch architecture file
   * There is a NO-OP implementation here so that when not using the JIT compiler, the same class can be used interchangeably
   * @return \c true on success */
  static bool CompileCPP() { return true; }

  void FreeDSP()
  {
    DELETE_NULL(mDSP);
  }

  // Unique methods
  void SetSampleRate(double sampleRate)
  {
    if (mDSP)
      mDSP->init((int) sampleRate);
  }

  void ProcessMidiMsg(const IMidiMsg& msg)
  {
//    TODO:
  }

  virtual void ProcessBlock(sample** inputs, sample** outputs, int nFrames)
  {
    if (mDSP)
      mDSP->compute(nFrames, inputs, outputs);
  }

  void SetParameterValue(int paramIdx, double normalizedValue)
  {
    assert(paramIdx < NParams());

    *(mZones.Get(paramIdx)) = normalizedValue;
  }

  void SetParameterValue(const char* labelToLookup, double normalizedValue)
  {
    FAUSTFLOAT* dest = nullptr;
    dest = mMap.Get(labelToLookup, nullptr);

    if(dest)
      *dest = normalizedValue;
    else
      DBGMSG("No parameter named %s\n", labelToLookup);
  }

  int CreateIPlugParameters(IPlugBase& plug, int startIdx = 0)
  {
    int plugParamIdx = startIdx;

    for (auto p = 0; p < NParams(); p++)
    {
      plugParamIdx += p;
      assert(plugParamIdx < plug.NParams());

      plug.GetParam(plugParamIdx)->Init(*mParams.Get(p));
    }

    return plugParamIdx;
  }

  int NParams()
  {
    return mParams.GetSize();
  }

  // Meta
  void declare(const char *key, const char *value) override
  {
    // TODO:
  }

  // UI

  // TODO:
  void openTabBox(const char *label) override {}
  void openHorizontalBox(const char *label) override {}
  void openVerticalBox(const char *label) override {}
  void closeBox() override {}

  void addButton(const char *label, FAUSTFLOAT *zone) override
  {
    IParam* pParam = new IParam();
    pParam->InitBool(label, 0);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }

  void addCheckButton(const char *label, FAUSTFLOAT *zone) override
  {
    IParam* pParam = new IParam();
    pParam->InitBool(label, 0);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }

  void addVerticalSlider(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    IParam* pParam = new IParam();
    pParam->InitDouble(label, init, min, max, step);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }

  void addHorizontalSlider(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    IParam* pParam = new IParam();
    pParam->InitDouble(label, init, min, max, step);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }

  void addNumEntry(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    IParam* pParam = new IParam();
    pParam->InitEnum(label, init, max - min);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }

  // TODO:
  void addHorizontalBargraph(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT min, FAUSTFLOAT max) override {}
  void addVerticalBargraph(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT min, FAUSTFLOAT max) override {}
  void addSoundfile(const char *label, const char *filename, Soundfile **sf_zone) override {}

protected:
  OverSampler<sample>* mOverSampler = nullptr;
  WDL_String mName;
  int mNVoices;
  ::dsp* mDSP = nullptr;
  MidiUI* mMidiUI = nullptr;
  WDL_PtrList<IParam> mParams;
  WDL_PtrList<FAUSTFLOAT> mZones;
  WDL_StringKeyedArray<FAUSTFLOAT*> mMap;
};




