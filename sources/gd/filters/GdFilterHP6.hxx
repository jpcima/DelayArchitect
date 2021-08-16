#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/* ------------------------------------------------------------
name: "GdFilters"
Code generated with Faust 2.33.1 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -mapp -es 1 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>


#ifndef FAUSTCLASS
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif


//[Before:class]
class mydsp {
	//[Begin:class]


 private:

	float fVec0[2];
	FAUSTFLOAT fHslider0;
	int fSampleRate;
	float fConst1;
	FAUSTFLOAT fHslider1;
	float fConst2;
	float fRec2[2];
	float fVec1[2];
	float fVec2[2];
	float fVec3[2];
	float fRec1[2];
	float fRec0[2];

 public:


	static constexpr int getNumInputs() {
		return 1;
	}
	static constexpr int getNumOutputs() {
		return 1;
	}

	static void classInit(int sample_rate) {
		//[Begin:classInit]
		//[End:classInit]
	}

	void instanceConstants(int sample_rate) {
		//[Begin:instanceConstants]
		fSampleRate = sample_rate;
		float fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = (6.28318548f / fConst0);
		fConst2 = (3.14159274f / fConst0);
		//[End:instanceConstants]
	}

	void instanceResetUserInterface() {
		//[Begin:instanceResetUserInterface]
		fHslider0 = FAUSTFLOAT(0.0f);
		fHslider1 = FAUSTFLOAT(500.0f);
		//[End:instanceResetUserInterface]
	}

	void instanceClear() {
		//[Begin:instanceClear]
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fVec0[l0] = 0.0f;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec2[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fVec1[l2] = 0.0f;
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fVec2[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fVec3[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec1[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec0[l6] = 0.0f;
		}
		//[End:instanceClear]
	}

	void init(int sample_rate) {
		//[Begin:init]
		classInit(sample_rate);
		instanceInit(sample_rate);
		//[End:init]
	}
	void instanceInit(int sample_rate) {
		//[Begin:instanceInit]
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
		//[End:instanceInit]
	}


	int getSampleRate() {
		return fSampleRate;
	}


	void compute(int count, FAUSTFLOAT const* const* inputs, FAUSTFLOAT* const* outputs) {
		//[Begin:compute]
		FAUSTFLOAT const* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = std::sqrt(float(fHslider0));
		float fSlow1 = float(fHslider1);
		float fSlow2 = (fConst1 * fSlow1);
		float fSlow3 = std::sin(fSlow2);
		float fSlow4 = (fSlow0 * fSlow3);
		float fSlow5 = (fSlow3 / fSlow0);
		float fSlow6 = (fSlow5 + 1.0f);
		float fSlow7 = ((1.0f - fSlow4) / fSlow6);
		float fSlow8 = std::tan((fConst2 * fSlow1));
		float fSlow9 = (1.0f / fSlow8);
		float fSlow10 = (fSlow9 + 1.0f);
		float fSlow11 = (0.0f - (1.0f / (fSlow8 * fSlow10)));
		float fSlow12 = (1.0f / fSlow10);
		float fSlow13 = (1.0f - fSlow9);
		float fSlow14 = ((1.0f - fSlow5) / fSlow6);
		float fSlow15 = (0.0f - (2.0f * std::cos(fSlow2)));
		float fSlow16 = (fSlow15 / fSlow6);
		float fSlow17 = (1.0f / fSlow6);
		float fSlow18 = (fSlow4 + 1.0f);
		for (int i0 = 0; (i0 < count); i0 = (i0 + 1)) {
			float fTemp0 = float(input0[i0]);
			fVec0[0] = fTemp0;
			fRec2[0] = ((fSlow11 * fVec0[1]) - (fSlow12 * ((fSlow13 * fRec2[1]) - (fSlow9 * fTemp0))));
			fVec1[0] = (fSlow7 * fRec2[0]);
			fVec2[0] = (fVec1[1] - (fSlow14 * fRec0[1]));
			fVec3[0] = (fSlow16 * fRec2[0]);
			fRec1[0] = ((fVec2[1] + fVec3[1]) - (fSlow17 * ((fSlow15 * fRec1[1]) - (fSlow18 * fRec2[0]))));
			fRec0[0] = fRec1[0];
			output0[i0] = FAUSTFLOAT(fRec0[0]);
			fVec0[1] = fVec0[0];
			fRec2[1] = fRec2[0];
			fVec1[1] = fVec1[0];
			fVec2[1] = fVec2[0];
			fVec3[1] = fVec3[0];
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
		}
		//[End:compute]
	}


	FAUSTFLOAT getCutoff() const { return fHslider1; }
	void setCutoff(FAUSTFLOAT value) { fHslider1 = value; }

	FAUSTFLOAT getResonance() const { return fHslider0; }
	void setResonance(FAUSTFLOAT value) { fHslider0 = value; }

	//[End:class]
};
//[After:class]


#endif
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#undef FAUSTFLOAT
#undef FAUSTCLASS
