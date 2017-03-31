#include "FilmicColorGrading.h"

float FilmicColorGrading::ApplyLiftInvGammaGain(const float lift, const float invGamma, const float gain, float v)
{
	// lerp gain
	float lerpV = Saturate(powf(v,invGamma));
	float dst = gain*lerpV + lift*(1.0f-lerpV);
	return dst;
}

float FilmicColorGrading::ApplySpacing(float v, eTableSpacing spacing)
{
	if (spacing == kTableSpacing_Linear)
		return v;
	if (spacing == kTableSpacing_Quadratic)
		return v*v;
	if (spacing == kTableSpacing_Quartic)
		return v*v*v*v;

	// assert?
	return 0.0f;
}

float FilmicColorGrading::ApplySpacingInv(float v, eTableSpacing spacing)
{
	if (spacing == kTableSpacing_Linear)
		return v;
	if (spacing == kTableSpacing_Quadratic)
		return sqrtf(v);
	if (spacing == kTableSpacing_Quartic)
		return sqrtf(sqrtf(v));

	// assert?
	return 0.0f;
}


Vec3 FilmicColorGrading::EvalParams::EvalFullColor(Vec3 src) const
{
	Vec3 v = src;
	v = EvalExposure(v);
	v = EvalSaturation(v);
	v = EvalContrast(v);
	v = EvalFilmicCurve(v);
	v = EvalLiftGammaGain(v);
	return v;
}

Vec3 FilmicColorGrading::EvalParams::EvalExposure(Vec3 v) const
{
	return v * m_linColorFilterExposure;
}

Vec3 FilmicColorGrading::EvalParams::EvalSaturation(Vec3 v) const
{
	float grey = Vec3::Dot(v,m_luminanceWeights);
	Vec3 ret = Vec3(grey) + m_saturation*(v - Vec3(grey));
	return ret;
}


static float EvalLogContrastFunc(float x, float eps, float logMidpoint, float contrast)
{
	float logX = log2f(x+eps);
	float adjX = logMidpoint + (logX - logMidpoint) * contrast;
	float ret = MaxFloat(0.0f,exp2f(adjX) - eps);
	return ret;
}

// inverse of the log contrast function
static float EvalLogContrastFuncRev(float x, float eps, float logMidpoint, float contrast)
{
	// eps
	float logX = log2f(x+eps);
	float adjX = (logX - logMidpoint)/contrast + logMidpoint;
	float ret = MaxFloat(0.0f,exp2f(adjX) - eps);
	return ret;
}


Vec3 FilmicColorGrading::EvalParams::EvalContrast(Vec3 v) const
{
	Vec3 ret;
	ret.x = EvalLogContrastFunc(v.x,m_contrastEpsilon,m_contrastLogMidpoint,m_contrastStrength);
	ret.y = EvalLogContrastFunc(v.y,m_contrastEpsilon,m_contrastLogMidpoint,m_contrastStrength);
	ret.z = EvalLogContrastFunc(v.z,m_contrastEpsilon,m_contrastLogMidpoint,m_contrastStrength);
	return ret;
}

Vec3 FilmicColorGrading::EvalParams::EvalFilmicCurve(Vec3 v) const // don't forget, the filmic curve can include a gamma adjustment
{
	Vec3 ret;
	ret.x = m_filmicCurve.Eval(v.x);
	ret.y = m_filmicCurve.Eval(v.y);
	ret.z = m_filmicCurve.Eval(v.z);

	// also apply the extra gamma, which has not been convolved into the filmic curve
	ret.x = powf(ret.x,m_postGamma);
	ret.y = powf(ret.y,m_postGamma);
	ret.z = powf(ret.z,m_postGamma);

	return ret;
}

Vec3 FilmicColorGrading::EvalParams::EvalLiftGammaGain(Vec3 v) const
{
	Vec3 ret;
	ret.x = ApplyLiftInvGammaGain(m_liftAdjust.x,m_invGammaAdjust.x,m_gainAdjust.x,v.x);
	ret.y = ApplyLiftInvGammaGain(m_liftAdjust.y,m_invGammaAdjust.y,m_gainAdjust.y,v.y);
	ret.z = ApplyLiftInvGammaGain(m_liftAdjust.z,m_invGammaAdjust.z,m_gainAdjust.z,v.z);
	return ret;
}

// convert from gamma space to linear space, and then normalize it
static Vec3 ColorLinearFromGammaNormalize(Vec3 val)
{
	// We aren't using a perceptually linear luminance because the entire point of this operation
	// is to change the color channels relative to each other. So it's fine if these adjustments
	// change the perceived luminance.

	Vec3 ret = val;
	val.x = powf(val.x,2.2f);
	val.y = powf(val.y,2.2f);
	val.z = powf(val.z,2.2f);

	float mid = (val.x+val.y+val.z)/3.0f;

	val.x /= mid;
	val.y /= mid;
	val.z /= mid;

	return val;
}

static Vec3 ColorLinearFromGamma(Vec3 val)
{
	Vec3 ret = val;
	val.x = powf(val.x,2.2f);
	val.y = powf(val.y,2.2f);
	val.z = powf(val.z,2.2f);

	return val;
}

void FilmicColorGrading::RawFromUserParams(RawParams & rawParams, const UserParams & userParams)
{
	rawParams.Reset();

	// convert color from gamma to linear space
	rawParams.m_colorFilter.x = powf(userParams.m_colorFilter.x,2.2f);
	rawParams.m_colorFilter.y = powf(userParams.m_colorFilter.y,2.2f);
	rawParams.m_colorFilter.z = powf(userParams.m_colorFilter.z,2.2f);

	// hardcode the luminance weights to something reasonable, but not perceptually correct
	rawParams.m_luminanceWeights = Vec3(.25f,.5f,.25f);

	// direct copy for saturation
	rawParams.m_saturation = userParams.m_saturation;

	// direct copy for strength, but midpoint/epsilon are hardcoded
	rawParams.m_exposureBias = userParams.m_exposureBias;
	rawParams.m_contrastStrength = userParams.m_contrast;
	rawParams.m_contrastEpsilon = 1e-5f;
	rawParams.m_contrastMidpoint = 0.18f;

	{
		FilmicToneCurve::CurveParamsUser filmicParams;
		filmicParams.m_gamma = userParams.m_filmicGamma;
		filmicParams.m_shoulderAngle = userParams.m_filmicShoulderAngle;
		filmicParams.m_shoulderLength = userParams.m_filmicShoulderLength;
		filmicParams.m_shoulderStrength = userParams.m_filmicShoulderStrength;
		filmicParams.m_toeLength = userParams.m_filmicToeLength;
		filmicParams.m_toeStrength = userParams.m_filmicToeStrength;

		FilmicToneCurve::CalcDirectParamsFromUser(rawParams.m_filmicCurve,filmicParams);
	}

	{
		Vec3 liftC = (userParams.m_shadowColor);
		Vec3 gammaC = (userParams.m_midtoneColor);
		Vec3 gainC = (userParams.m_highlightColor);

		{
			float avgLift = (liftC.x+liftC.y+liftC.z)/3.0f;
			liftC = liftC - avgLift;
		}

		{
			float avgGamma = (gammaC.x + gammaC.y + gammaC.z)/3.0f;
			gammaC = (gammaC - avgGamma);
		}

		{
			float avgGain = (gainC.x+gainC.y+gainC.z)/3.0f;
			gainC = (gainC - avgGain);
		}

		rawParams.m_liftAdjust  = 0.0f + (liftC  + userParams.m_shadowOffset   );
		rawParams.m_gainAdjust  = 1.0f + (gainC  + userParams.m_highlightOffset);

		{
			Vec3 midGrey = 0.5f + (gammaC + userParams.m_midtoneOffset  );
			Vec3 H = rawParams.m_gainAdjust;
			Vec3 S = rawParams.m_liftAdjust;
		
			rawParams.m_gammaAdjust.x = logf((0.5f-S.x)/(H.x-S.x))/logf(midGrey.x);
			rawParams.m_gammaAdjust.y = logf((0.5f-S.y)/(H.y-S.y))/logf(midGrey.y);
			rawParams.m_gammaAdjust.z = logf((0.5f-S.z)/(H.z-S.z))/logf(midGrey.z);
		}
	}
	
	// gamma after filmic curve to convert to display space
	rawParams.m_postGamma = userParams.m_postGamma;
}

void FilmicColorGrading::EvalFromRawParams(EvalParams & dstParams, const RawParams & rawParams)
{
	// bake color filter and exposure bias together
	dstParams.m_linColorFilterExposure = rawParams.m_colorFilter * exp2f(rawParams.m_exposureBias);

	dstParams.m_luminanceWeights = rawParams.m_luminanceWeights;
	dstParams.m_saturation = rawParams.m_saturation;

	dstParams.m_contrastStrength = rawParams.m_contrastStrength;
	dstParams.m_contrastLogMidpoint = log2f(rawParams.m_contrastMidpoint);
	dstParams.m_contrastEpsilon = rawParams.m_contrastEpsilon;

	FilmicToneCurve::CreateCurve(dstParams.m_filmicCurve,rawParams.m_filmicCurve);

	dstParams.m_postGamma = rawParams.m_postGamma;

	dstParams.m_liftAdjust = rawParams.m_liftAdjust;
	dstParams.m_invGammaAdjust.x = 1.0f/(rawParams.m_gammaAdjust.x);
	dstParams.m_invGammaAdjust.y = 1.0f/(rawParams.m_gammaAdjust.y);
	dstParams.m_invGammaAdjust.z = 1.0f/(rawParams.m_gammaAdjust.z);
	dstParams.m_gainAdjust = rawParams.m_gainAdjust;
}

void FilmicColorGrading::BakeFromEvalParams(BakedParams & dstCurve, const EvalParams & srcParams, const int curveSize, const eTableSpacing spacing)
{

	// in the curve, we are baking the following steps:
	// v = EvalContrast(v);
	// v = EvalFilmicCurve(v);
	// v = EvalLiftGammaGain(v);

	// So what is the maximum value to bake into the curve? It's filmic W with inverse contrast applied
	float maxTableValue = EvalLogContrastFuncRev(srcParams.m_filmicCurve.m_W,srcParams.m_contrastEpsilon,srcParams.m_contrastLogMidpoint,srcParams.m_contrastStrength);

	dstCurve.Reset();
	dstCurve.m_curveSize = curveSize;
	dstCurve.m_spacing = spacing;

	dstCurve.m_saturation = srcParams.m_saturation;
	dstCurve.m_linColorFilterExposure = srcParams.m_linColorFilterExposure * (1.0f / maxTableValue);
	dstCurve.m_luminanceWeights = srcParams.m_luminanceWeights;

	dstCurve.m_curveB.resize(curveSize);
	dstCurve.m_curveG.resize(curveSize);
	dstCurve.m_curveR.resize(curveSize);

	for (int i = 0; i < curveSize; i++)
	{
		float t = float(i)/float(curveSize-1);

		t = ApplySpacing(t,spacing) * maxTableValue;

		Vec3 rgb = Vec3(t,t,t);
		rgb = srcParams.EvalContrast(rgb);
		rgb = srcParams.EvalFilmicCurve(rgb);
		rgb = srcParams.EvalLiftGammaGain(rgb);

		dstCurve.m_curveR[i] = rgb.x;
		dstCurve.m_curveG[i] = rgb.y;
		dstCurve.m_curveB[i] = rgb.z;
	}
}

// Tex1D, more or less. NormX is in [0.0f,1.0f], not [0,size]
float FilmicColorGrading::BakedParams::SampleTable(const std::vector < float > & curve, float normX)
{
	int size = curve.size();

	float x = normX * float(size-1) + .5f;

	// Tex2d-ish. When implementing in a shader, make sure to do the pad above, but everything below will be in the Tex2d call.
	int baseIndex = MaxInt(0,x-.5f);
	float t = (x-.5f) - float(baseIndex);

	int x0 = MaxInt(0,MinInt(baseIndex,size-1));
	int x1 = MaxInt(0,MinInt(baseIndex+1,size-1));

	float v0 = curve[x0];
	float v1 = curve[x1];

	float ret = v0*(1.0f-t) + v1*t;
	return ret;
}

Vec3 FilmicColorGrading::BakedParams::EvalColor(const Vec3 srcColor) const
{
	Vec3 rgb = srcColor;

	// exposure and color filter
	rgb = rgb * m_linColorFilterExposure;

	// saturation
	float grey = Vec3::Dot(rgb,m_luminanceWeights);
	rgb = Vec3(grey) + m_saturation*(rgb - Vec3(grey));

	rgb.x = ApplySpacingInv(rgb.x,m_spacing);
	rgb.y = ApplySpacingInv(rgb.y,m_spacing);
	rgb.z = ApplySpacingInv(rgb.z,m_spacing);

	// contrast, filmic curve, gamme 
	rgb.x = SampleTable(m_curveR,rgb.x);
	rgb.y = SampleTable(m_curveG,rgb.y);
	rgb.z = SampleTable(m_curveB,rgb.z);

	return rgb;
}






