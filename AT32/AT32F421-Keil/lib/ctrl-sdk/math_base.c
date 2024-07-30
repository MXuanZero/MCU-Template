#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "math_base.h"
#include "arm_math.h"

#define FAST_MATH_TABLE_SIZE 512

static const float m_sin_table_f[FAST_MATH_TABLE_SIZE + 1] = {
	0.00000000f,  0.01227154f,  0.02454123f,  0.03680722f,	0.04906767f,
	0.06132074f,  0.07356456f,  0.08579731f,  0.09801714f,	0.11022221f,
	0.12241068f,  0.13458071f,  0.14673047f,  0.15885814f,	0.17096189f,
	0.18303989f,  0.19509032f,  0.20711138f,  0.21910124f,	0.23105811f,
	0.24298018f,  0.25486566f,  0.26671276f,  0.27851969f,	0.29028468f,
	0.30200595f,  0.31368174f,  0.32531029f,  0.33688985f,	0.34841868f,
	0.35989504f,  0.37131719f,  0.38268343f,  0.39399204f,	0.40524131f,
	0.41642956f,  0.42755509f,  0.43861624f,  0.44961133f,	0.46053871f,
	0.47139674f,  0.48218377f,  0.49289819f,  0.50353838f,	0.51410274f,
	0.52458968f,  0.53499762f,  0.54532499f,  0.55557023f,	0.56573181f,
	0.57580819f,  0.58579786f,  0.59569930f,  0.60551104f,	0.61523159f,
	0.62485949f,  0.63439328f,  0.64383154f,  0.65317284f,	0.66241578f,
	0.67155895f,  0.68060100f,  0.68954054f,  0.69837625f,	0.70710678f,
	0.71573083f,  0.72424708f,  0.73265427f,  0.74095113f,	0.74913639f,
	0.75720885f,  0.76516727f,  0.77301045f,  0.78073723f,	0.78834643f,
	0.79583690f,  0.80320753f,  0.81045720f,  0.81758481f,	0.82458930f,
	0.83146961f,  0.83822471f,  0.84485357f,  0.85135519f,	0.85772861f,
	0.86397286f,  0.87008699f,  0.87607009f,  0.88192126f,	0.88763962f,
	0.89322430f,  0.89867447f,  0.90398929f,  0.90916798f,	0.91420976f,
	0.91911385f,  0.92387953f,  0.92850608f,  0.93299280f,	0.93733901f,
	0.94154407f,  0.94560733f,  0.94952818f,  0.95330604f,	0.95694034f,
	0.96043052f,  0.96377607f,  0.96697647f,  0.97003125f,	0.97293995f,
	0.97570213f,  0.97831737f,  0.98078528f,  0.98310549f,	0.98527764f,
	0.98730142f,  0.98917651f,  0.99090264f,  0.99247953f,	0.99390697f,
	0.99518473f,  0.99631261f,  0.99729046f,  0.99811811f,	0.99879546f,
	0.99932238f,  0.99969882f,  0.99992470f,  1.00000000f,	0.99992470f,
	0.99969882f,  0.99932238f,  0.99879546f,  0.99811811f,	0.99729046f,
	0.99631261f,  0.99518473f,  0.99390697f,  0.99247953f,	0.99090264f,
	0.98917651f,  0.98730142f,  0.98527764f,  0.98310549f,	0.98078528f,
	0.97831737f,  0.97570213f,  0.97293995f,  0.97003125f,	0.96697647f,
	0.96377607f,  0.96043052f,  0.95694034f,  0.95330604f,	0.94952818f,
	0.94560733f,  0.94154407f,  0.93733901f,  0.93299280f,	0.92850608f,
	0.92387953f,  0.91911385f,  0.91420976f,  0.90916798f,	0.90398929f,
	0.89867447f,  0.89322430f,  0.88763962f,  0.88192126f,	0.87607009f,
	0.87008699f,  0.86397286f,  0.85772861f,  0.85135519f,	0.84485357f,
	0.83822471f,  0.83146961f,  0.82458930f,  0.81758481f,	0.81045720f,
	0.80320753f,  0.79583690f,  0.78834643f,  0.78073723f,	0.77301045f,
	0.76516727f,  0.75720885f,  0.74913639f,  0.74095113f,	0.73265427f,
	0.72424708f,  0.71573083f,  0.70710678f,  0.69837625f,	0.68954054f,
	0.68060100f,  0.67155895f,  0.66241578f,  0.65317284f,	0.64383154f,
	0.63439328f,  0.62485949f,  0.61523159f,  0.60551104f,	0.59569930f,
	0.58579786f,  0.57580819f,  0.56573181f,  0.55557023f,	0.54532499f,
	0.53499762f,  0.52458968f,  0.51410274f,  0.50353838f,	0.49289819f,
	0.48218377f,  0.47139674f,  0.46053871f,  0.44961133f,	0.43861624f,
	0.42755509f,  0.41642956f,  0.40524131f,  0.39399204f,	0.38268343f,
	0.37131719f,  0.35989504f,  0.34841868f,  0.33688985f,	0.32531029f,
	0.31368174f,  0.30200595f,  0.29028468f,  0.27851969f,	0.26671276f,
	0.25486566f,  0.24298018f,  0.23105811f,  0.21910124f,	0.20711138f,
	0.19509032f,  0.18303989f,  0.17096189f,  0.15885814f,	0.14673047f,
	0.13458071f,  0.12241068f,  0.11022221f,  0.09801714f,	0.08579731f,
	0.07356456f,  0.06132074f,  0.04906767f,  0.03680722f,	0.02454123f,
	0.01227154f,  0.00000000f,  -0.01227154f, -0.02454123f, -0.03680722f,
	-0.04906767f, -0.06132074f, -0.07356456f, -0.08579731f, -0.09801714f,
	-0.11022221f, -0.12241068f, -0.13458071f, -0.14673047f, -0.15885814f,
	-0.17096189f, -0.18303989f, -0.19509032f, -0.20711138f, -0.21910124f,
	-0.23105811f, -0.24298018f, -0.25486566f, -0.26671276f, -0.27851969f,
	-0.29028468f, -0.30200595f, -0.31368174f, -0.32531029f, -0.33688985f,
	-0.34841868f, -0.35989504f, -0.37131719f, -0.38268343f, -0.39399204f,
	-0.40524131f, -0.41642956f, -0.42755509f, -0.43861624f, -0.44961133f,
	-0.46053871f, -0.47139674f, -0.48218377f, -0.49289819f, -0.50353838f,
	-0.51410274f, -0.52458968f, -0.53499762f, -0.54532499f, -0.55557023f,
	-0.56573181f, -0.57580819f, -0.58579786f, -0.59569930f, -0.60551104f,
	-0.61523159f, -0.62485949f, -0.63439328f, -0.64383154f, -0.65317284f,
	-0.66241578f, -0.67155895f, -0.68060100f, -0.68954054f, -0.69837625f,
	-0.70710678f, -0.71573083f, -0.72424708f, -0.73265427f, -0.74095113f,
	-0.74913639f, -0.75720885f, -0.76516727f, -0.77301045f, -0.78073723f,
	-0.78834643f, -0.79583690f, -0.80320753f, -0.81045720f, -0.81758481f,
	-0.82458930f, -0.83146961f, -0.83822471f, -0.84485357f, -0.85135519f,
	-0.85772861f, -0.86397286f, -0.87008699f, -0.87607009f, -0.88192126f,
	-0.88763962f, -0.89322430f, -0.89867447f, -0.90398929f, -0.90916798f,
	-0.91420976f, -0.91911385f, -0.92387953f, -0.92850608f, -0.93299280f,
	-0.93733901f, -0.94154407f, -0.94560733f, -0.94952818f, -0.95330604f,
	-0.95694034f, -0.96043052f, -0.96377607f, -0.96697647f, -0.97003125f,
	-0.97293995f, -0.97570213f, -0.97831737f, -0.98078528f, -0.98310549f,
	-0.98527764f, -0.98730142f, -0.98917651f, -0.99090264f, -0.99247953f,
	-0.99390697f, -0.99518473f, -0.99631261f, -0.99729046f, -0.99811811f,
	-0.99879546f, -0.99932238f, -0.99969882f, -0.99992470f, -1.00000000f,
	-0.99992470f, -0.99969882f, -0.99932238f, -0.99879546f, -0.99811811f,
	-0.99729046f, -0.99631261f, -0.99518473f, -0.99390697f, -0.99247953f,
	-0.99090264f, -0.98917651f, -0.98730142f, -0.98527764f, -0.98310549f,
	-0.98078528f, -0.97831737f, -0.97570213f, -0.97293995f, -0.97003125f,
	-0.96697647f, -0.96377607f, -0.96043052f, -0.95694034f, -0.95330604f,
	-0.94952818f, -0.94560733f, -0.94154407f, -0.93733901f, -0.93299280f,
	-0.92850608f, -0.92387953f, -0.91911385f, -0.91420976f, -0.90916798f,
	-0.90398929f, -0.89867447f, -0.89322430f, -0.88763962f, -0.88192126f,
	-0.87607009f, -0.87008699f, -0.86397286f, -0.85772861f, -0.85135519f,
	-0.84485357f, -0.83822471f, -0.83146961f, -0.82458930f, -0.81758481f,
	-0.81045720f, -0.80320753f, -0.79583690f, -0.78834643f, -0.78073723f,
	-0.77301045f, -0.76516727f, -0.75720885f, -0.74913639f, -0.74095113f,
	-0.73265427f, -0.72424708f, -0.71573083f, -0.70710678f, -0.69837625f,
	-0.68954054f, -0.68060100f, -0.67155895f, -0.66241578f, -0.65317284f,
	-0.64383154f, -0.63439328f, -0.62485949f, -0.61523159f, -0.60551104f,
	-0.59569930f, -0.58579786f, -0.57580819f, -0.56573181f, -0.55557023f,
	-0.54532499f, -0.53499762f, -0.52458968f, -0.51410274f, -0.50353838f,
	-0.49289819f, -0.48218377f, -0.47139674f, -0.46053871f, -0.44961133f,
	-0.43861624f, -0.42755509f, -0.41642956f, -0.40524131f, -0.39399204f,
	-0.38268343f, -0.37131719f, -0.35989504f, -0.34841868f, -0.33688985f,
	-0.32531029f, -0.31368174f, -0.30200595f, -0.29028468f, -0.27851969f,
	-0.26671276f, -0.25486566f, -0.24298018f, -0.23105811f, -0.21910124f,
	-0.20711138f, -0.19509032f, -0.18303989f, -0.17096189f, -0.15885814f,
	-0.14673047f, -0.13458071f, -0.12241068f, -0.11022221f, -0.09801714f,
	-0.08579731f, -0.07356456f, -0.06132074f, -0.04906767f, -0.03680722f,
	-0.02454123f, -0.01227154f, -0.00000000f
};

// 快速开平方的倒数
// https://reprap.org/forum/read.php?147,219210
// https://en.wikipedia.org/wiki/Fast_inverse_square_root
float m_rsqrtf(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y; // evil floating point bit level hacking
	i = 0x5f375a86 - (i >> 1); // what the fuck?
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y)); // 1st iteration
	// y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration,
	// this can be removed

	return y;
}

float m_sqrtf(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y; // evil floating point bit level hacking
	i = 0x5f375a86 - (i >> 1); // what the fuck?
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y)); // 1st iteration
	// y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration,
	// this can be removed

	return y * number;
}

// 查表法算sin值，从dsp库中复制出来的
float m_sinf(float x)
{
	float val, fract, in; /* Temporary variables for input, output */
	uint16_t index; /* Index variable */
	float a, b; /* Two nearest output values */
	int32_t n;
	float findex;

	/* input x is in radians */
	/* Scale the input to [0 1] range from [0 2*PI], divide input by 2*pi */
	in = x * 0.159154943092f;

	/* Calculation of floor value of input */
	n = (int32_t)in;

	/* Make negative values towards -infinity */
	if (x < 0.0f) {
		n--;
	}

	/* Map input value to [0 1] */
	in = in - (float)n;

	/* Calculation of index of the table */
	findex = (float)FAST_MATH_TABLE_SIZE * in;
	index = (uint16_t)findex;

	/* when "in" is exactly 1, we need to rotate the index down to 0 */
	if (index >= FAST_MATH_TABLE_SIZE) {
		index = 0;
		findex -= (float)FAST_MATH_TABLE_SIZE;
	}

	/* fractional value calculation */
	fract = findex - (float)index;

	/* read two nearest values of input value from the sin table */
	a = m_sin_table_f[index];
	b = m_sin_table_f[index + 1];

	/* Linear interpolation process */
	val = (1.0f - fract) * a + fract * b;

	/* Return the output value */
	return (val);
}

// 查表法算cos值，从dsp库中复制出来的
float m_cosf(float x)
{
	float val, fract, in; /* Temporary variables for input, output */
	uint16_t index; /* Index variable */
	float a, b; /* Two nearest output values */
	int32_t n;
	float findex;

	/* input x is in radians */
	/* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi,
	 * add 0.25 (pi/2) to read sine table */
	in = x * 0.159154943092f + 0.25f;

	/* Calculation of floor value of input */
	n = (int32_t)in;

	/* Make negative values towards -infinity */
	if (in < 0.0f) {
		n--;
	}

	/* Map input value to [0 1] */
	in = in - (float)n;

	/* Calculation of index of the table */
	findex = (float)FAST_MATH_TABLE_SIZE * in;
	index = (uint16_t)findex;

	/* when "in" is exactly 1, we need to rotate the index down to 0 */
	if (index >= FAST_MATH_TABLE_SIZE) {
		index = 0;
		findex -= (float)FAST_MATH_TABLE_SIZE;
	}

	/* fractional value calculation */
	fract = findex - (float)index;

	/* read two nearest values of input value from the cos table */
	a = m_sin_table_f[index];
	b = m_sin_table_f[index + 1];

	/* Linear interpolation process */
	val = (1.0f - fract) * a + fract * b;

	/* Return the output value */
	return (val);
}

// 快速算atan，从dsp库中复制出来的
#define ATANHALFF32 0.463648f
#define PIHALFF32 1.5707963267948966192313f
#define ATAN2_NB_COEFS_F32 10
static const float m_atan2_coefs_f[ATAN2_NB_COEFS_F32] = {
	0.0f,
	1.0000001638308195518f,
	-0.0000228941363602264f,
	-0.3328086544578890873f,
	-0.004404814619311061f,
	0.2162217461808173258f,
	-0.0207504842057097504f,
	-0.1745263362250363339f,
	0.1340557235283553386f,
	-0.0323664125927477625f
};

static inline float m_atan_limited_f(float x)
{
	float res = m_atan2_coefs_f[ATAN2_NB_COEFS_F32 - 1];
	int i = 1;
	for (i = 1; i < ATAN2_NB_COEFS_F32; i++) {
		res = x * res + m_atan2_coefs_f[ATAN2_NB_COEFS_F32 - 1 - i];
	}

	return (res);
}

inline float m_atanf(float x)
{
	int sign = 0;
	float res = 0.0f;

	if (x < 0.0f) {
		sign = 1;
		x = -x;
	}

	if (x > 1.0f) {
		x = 1.0f / x;
		res = PIHALFF32 - m_atan_limited_f(x);
	} else {
		res += m_atan_limited_f(x);
	}

	if (sign) {
		res = -res;
	}

	return (res);
}

float m_atan2f(float y, float x)
{
	float result = 0.0f;
	if (x > 0.0f) {
		result = m_atanf(y / x);
	} else if (x < 0.0f) {
		if (y > 0.0f) {
			result = m_atanf(y / x) + DEF_PI;
		} else if (y < 0.0f) {
			result = m_atanf(y / x) - DEF_PI;
		} else {
			if (signbit(y)) {
				result = -DEF_PI;
			} else {
				result = DEF_PI;
			}
		}
	} else {
		if (y > 0.0f) {
			result = PIHALFF32;
		}
		if (y < 0.0f) {
			result = -PIHALFF32;
		}
	}
	return result;
}

inline float m_tanf(float x)
{
	return tanf(x);
}

inline float m_floorf(float x)
{
	return floorf(x);
}

inline float m_ceilf(float x)
{
	return ceilf(x);
}

inline float m_fmodf(float x, float y)
{
	return fmodf(x, y);
}

inline float m_ilog2f(float x)
{
	union {
		float val;
		int32_t x;
	} u = { x };
	return (float)(((u.x >> 23) & 0xff) - 127);
}

inline float m_log2f(float x)
{
	union {
		float val;
		int32_t x;
	} u = { x };
	float ilog2 = (float)(((u.x >> 23) & 0xff) - 128);
	u.x = (u.x & 0x007fffff) | 0x3f800000;
	return ilog2 + (-0.34484843f * u.val + 2.02466578f) * u.val - 0.67487759f;
}
