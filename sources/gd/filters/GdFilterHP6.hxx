#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/* ------------------------------------------------------------
name: "GdFilters"
Code generated with Faust 2.33.1 (https://faust.grame.fr)
Compilation options: -lang cpp -os0 -mapp -es 1 -single -ftz 0
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

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

#define FAUST_INT_CONTROLS 0
#define FAUST_REAL_CONTROLS 19


//[Before:class]
class mydsp : public one_sample_dsp {
	//[Begin:class]


 private:

	FAUSTFLOAT fHslider0;
	int fSampleRate;
	float fConst1;
	FAUSTFLOAT fHslider1;
	float fConst2;
	float fVec0[2];
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

	void control(int* RESTRICT iControl, FAUSTFLOAT* RESTRICT fControl) {
		fControl[0] = std::sqrt(float(fHslider0));
		fControl[1] = float(fHslider1);
		fControl[2] = (fConst1 * fControl[1]);
		fControl[3] = std::sin(fControl[2]);
		fControl[4] = (fControl[0] * fControl[3]);
		fControl[5] = (fControl[3] / fControl[0]);
		fControl[6] = (fControl[5] + 1.0f);
		fControl[7] = ((1.0f - fControl[4]) / fControl[6]);
		fControl[8] = std::tan((fConst2 * fControl[1]));
		fControl[9] = (1.0f / fControl[8]);
		fControl[10] = (fControl[9] + 1.0f);
		fControl[11] = (0.0f - (1.0f / (fControl[8] * fControl[10])));
		fControl[12] = (1.0f / fControl[10]);
		fControl[13] = (1.0f - fControl[9]);
		fControl[14] = ((1.0f - fControl[5]) / fControl[6]);
		fControl[15] = (0.0f - (2.0f * std::cos(fControl[2])));
		fControl[16] = (fControl[15] / fControl[6]);
		fControl[17] = (1.0f / fControl[6]);
		fControl[18] = (fControl[4] + 1.0f);
	}

	int getNumIntControls() { return 0; }
	int getNumRealControls() { return 19; }

	void compute(FAUSTFLOAT const* RESTRICT inputs, FAUSTFLOAT* RESTRICT outputs, int const* RESTRICT iControl, FAUSTFLOAT const* RESTRICT fControl) {
		//[Begin:compute]
		float fTemp0 = float(inputs[0]);
		fVec0[0] = fTemp0;
		fRec2[0] = ((fControl[11] * fVec0[1]) - (fControl[12] * ((fControl[13] * fRec2[1]) - (fControl[9] * fTemp0))));
		fVec1[0] = (fControl[7] * fRec2[0]);
		fVec2[0] = (fVec1[1] - (fControl[14] * fRec0[1]));
		fVec3[0] = (fControl[16] * fRec2[0]);
		fRec1[0] = ((fVec2[1] + fVec3[1]) - (fControl[17] * ((fControl[15] * fRec1[1]) - (fControl[18] * fRec2[0]))));
		fRec0[0] = fRec1[0];
		outputs[0] = FAUSTFLOAT(fRec0[0]);
		fVec0[1] = fVec0[0];
		fRec2[1] = fRec2[0];
		fVec1[1] = fVec1[0];
		fVec2[1] = fVec2[0];
		fVec3[1] = fVec3[0];
		fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];
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
