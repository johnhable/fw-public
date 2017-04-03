#pragma once

#include <CoreHelpers.h>

#include <Vec3.h>
#include <Vec4.h>

#include "FilmicToneCurve.h"

class FilmicColorGrading
{
public:

	struct UserParams
	{
		UserParams()
		{
			Reset();
		}

		void Reset()
		{
			m_colorFilter = Vec3(1,1,1);
			m_saturation = 1.0f;
			m_exposureBias = 0.0f;

			// no contrast midpoint, hardcoded to .18f
			// no contrast epislon, hardcoded to 1e-5f
			m_contrast = 1.0f;

			// filmic tonemapping
			m_filmicToeStrength = 0.0f;
			m_filmicToeLength = 0.5f;
			m_filmicShoulderStrength = 0.0f;
			m_filmicShoulderLength = 0.5f;
			m_filmicShoulderAngle = 0.0f;
			m_filmicGamma = 1.0f;


			m_postGamma = 1.0f;


			m_shadowColor = Vec3(1.0f,1.0f,1.0f);
			m_midtoneColor = Vec3(1.0f,1.0f,1.0f);
			m_highlightColor = Vec3(1.0f,1.0f,1.0f);

			m_shadowOffset = 0.0f;
			m_midtoneOffset = 0.0f;
			m_highlightOffset = 0.0f;

		}

		Vec3 m_colorFilter;
		float m_saturation;
		float m_exposureBias;

		// no contrast midpoint, hardcoded to .18f
		// no contrast epislon, hardcoded to 1e-5f
		float m_contrast;

		// filmic tonemapping
		float m_filmicToeStrength;
		float m_filmicToeLength;
		float m_filmicShoulderStrength;
		float m_filmicShoulderLength;
		float m_filmicShoulderAngle;
		float m_filmicGamma; // gamma to convolve into the filmic curve

		float m_postGamma; // after filmic curve, as a separate step

		Vec3 m_shadowColor;
		Vec3 m_midtoneColor;
		Vec3 m_highlightColor;

		float m_shadowOffset;
		float m_midtoneOffset;
		float m_highlightOffset;

	};


	// These params are roughly in the order they are applied.
	struct RawParams
	{
		RawParams()
		{
			Reset();
		}

		void Reset()
		{
			m_colorFilter = Vec3(1,1,1);
			m_saturation = 1.0f;

			m_luminanceWeights = Vec3(.25f,.50f,.25f);

			m_exposureBias = 0.0f; // in f stops
			m_contrastStrength = 1.0f;
			m_contrastMidpoint = 0.20f;
			m_contrastEpsilon = 1e-5f;

			m_filmicCurve.Reset();

			m_liftAdjust = Vec3(0,0,0);
			m_gammaAdjust = Vec3(0,0,0);
			m_gainAdjust = Vec3(0,0,0);

			// final adjustment to image, after all other curves
			m_postGamma = 1.0f;
		}

		// color filter
		Vec3 m_colorFilter;

		Vec3 m_luminanceWeights;

		// Saturation could be argued to go later, but if you do it later I feel like it gets in the way of log contrast. It's also
		// nice to be here so that everything after can be merged into a 1d curve for each channel.
		float m_saturation;

		// exposure and contrast
		float m_exposureBias;
		float m_contrastStrength;
		float m_contrastMidpoint;
		float m_contrastEpsilon;

		// filmic curve
		FilmicToneCurve::CurveParamsDirect m_filmicCurve;


		// gamma adjustment after filmic curve
		float m_postGamma;

		// lift/gamma/gain, aka highlights/midtones/shadows, aka slope/power/offset
		Vec3 m_liftAdjust;
		Vec3 m_gammaAdjust;
		Vec3 m_gainAdjust;

	};

	// modified version of the the raw params which has precalculated values
	struct EvalParams
	{
		EvalParams()
		{
			Reset();
		}

		void Reset()
		{
			m_linColorFilterExposure = Vec3(1,1,1);

			m_luminanceWeights = Vec3(.25f,.5f,.25f);
			m_saturation = 1.0f;

			m_contrastStrength = 1.0f;
			m_contrastLogMidpoint = log2f(.18f);
			m_contrastEpsilon = 1e-5f;

			m_filmicCurve.Reset();

			m_postGamma = 1.0f;

			m_liftAdjust = Vec3(0.0f);
			m_invGammaAdjust = Vec3(1.0f); // note that we invert gamma to skip the divide, also convolves the final gamma into it
			m_gainAdjust = Vec3(1.0f);
		}

		// performs all of these calculations in order
		Vec3 EvalFullColor(Vec3 v) const;

		Vec3 EvalExposure(Vec3 v) const;
		Vec3 EvalSaturation(Vec3 v) const;

		Vec3 EvalContrast(Vec3 v) const;
		Vec3 EvalFilmicCurve(Vec3 v) const; // also converts from linear to gamma

		Vec3 EvalLiftGammaGain(Vec3 v) const;

		// bake color filter and exposure bias together
		Vec3 m_linColorFilterExposure;

		Vec3 m_luminanceWeights;
		float m_saturation;

		float m_contrastStrength;
		float m_contrastLogMidpoint;
		float m_contrastEpsilon;

		FilmicToneCurve::FullCurve m_filmicCurve;

		float m_postGamma;

		Vec3 m_liftAdjust;
		Vec3 m_invGammaAdjust; // note that we invert gamma to skip the divide, also convolves the final gamma into it
		Vec3 m_gainAdjust;
	};

	enum eTableSpacing
	{
		kTableSpacing_Linear,
		kTableSpacing_Quadratic,
		kTableSpacing_Quartic,
		kTableSpacing_Num
	};

	struct BakedParams
	{
		BakedParams()
		{
			Reset();
		}

		void Reset()
		{
			m_linColorFilterExposure = Vec3(1,1,1);

			m_saturation = 1.0f;

			m_curveSize = 256;

			m_curveR.clear();
			m_curveG.clear();
			m_curveB.clear();

			m_spacing = kTableSpacing_Quadratic;
			m_luminanceWeights = Vec3(.25f,.5f,.25f);
		}

		static float SampleTable(const std::vector < float > & curve, float x);
		Vec3 EvalColor(const Vec3 x) const;

		// params
		Vec3 m_linColorFilterExposure;
		Vec3 m_luminanceWeights;

		float m_saturation;

		int m_curveSize;

		std::vector < float > m_curveR;
		std::vector < float > m_curveG;
		std::vector < float > m_curveB;

		eTableSpacing m_spacing;

	};



	static float ApplySpacing(float v, eTableSpacing spacing);
	static float ApplySpacingInv(float v, eTableSpacing spacing);

	static void RawFromUserParams(RawParams & rawParams, const UserParams & userParams);
	static void EvalFromRawParams(EvalParams & dstParams, const RawParams & rawParams);
	static void BakeFromEvalParams(BakedParams & dstCurve, const EvalParams & srcParams, const int curveSize, const eTableSpacing spacing);

	static float ApplyLiftInvGammaGain(const float lift, const float invGamma, const float gain, float v);
};

