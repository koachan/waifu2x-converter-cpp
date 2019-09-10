#include <thread>
#include <atomic>
#include <altivec.h>
#include "filters.hpp"
#include "sec.hpp"

struct v256_t {
	vector float v0, v1;
};

static inline v256_t
madd256(v256_t const &v0, v256_t const &v1, v256_t const &v2)
{
	v256_t ret;
	ret.v0 = vec_madd(v0.v0, v1.v0, v2.v0);
	ret.v1 = vec_madd(v0.v1, v1.v1, v2.v1);
	return ret;
}

static inline v256_t
load_broadcast(const float *p)
{
	v256_t ret;
	ret.v0 = vec_splats(p[0]);
	ret.v1 = vec_splats(p[0]);
	return ret;
}

static inline v256_t
load256(const float *p)
{
	v256_t ret;
	ret.v0 = vec_ld(0, p);
	ret.v1 = vec_ld(16, p);
	return ret;
}

static inline void
store256(float *p, v256_t const &v)
{
	vec_st(v.v0, 0, p);
	vec_st(v.v1, 16, p);
}

static inline v256_t
zero()
{
	v256_t ret;
	ret.v0 = vec_splats(0.f);
	ret.v1 = vec_splats(0.f);
	return ret;
}

static inline v256_t
set1(float a)
{
	v256_t ret;
	ret.v0 = vec_splats(a);
	ret.v1 = vec_splats(a);
	return ret;
}

static inline float
hadd8(v256_t const &v)
{
	vector float sum4;
	sum4 = vec_add(v.v0, v.v1);

	return vec_extract(sum4, 0) + vec_extract(sum4, 1)
		+ vec_extract(sum4, 2) + vec_extract(sum4, 3);
}

static inline v256_t
mul256(v256_t const &a, v256_t const &b)
{
	v256_t ret;
	ret.v0 = vec_madd(a.v0, b.v0, vec_splats(0.f));
	ret.v1 = vec_madd(a.v1, b.v1, vec_splats(0.f));
	return ret;
}

static inline v256_t
add256(v256_t const &a, v256_t const &b)
{
	v256_t ret;
	ret.v0 = vec_add(a.v0, b.v0);
	ret.v1 = vec_add(a.v1, b.v1);
	return ret;
}

static inline v256_t
max256(v256_t const &a, v256_t const &b)
{
	v256_t ret;
	ret.v0 = vec_max(a.v0, b.v0);
	ret.v1 = vec_max(a.v1, b.v1);
	return ret;
}

static inline v256_t
min256(v256_t const &a, v256_t const &b)
{
	v256_t ret;
	ret.v0 = vec_min(a.v0, b.v0);
	ret.v1 = vec_min(a.v1, b.v1);
	return ret;
}

#include "modelHandler_avx_func.hpp"

#undef UNROLL
#define UNROLL 5

/* PowerPC AltiVec */
#define VEC_NELEM 8

#define vreg_t    v256_t
#define madd_vreg madd256
#define add_vreg  add256
#define zero_vreg zero
#define min_vreg  min256
#define max_vreg  max256
#define set1_vreg set1

static inline void
store_vreg(const unsigned char *ptr, vreg_t const &val)
{
	float *p = (float*) ptr;
	store256(p, val);
}

static inline vreg_t
load_vreg(const unsigned char *ptr)
{
	float *p = (float*) ptr;
	return load256(p);
}

static inline vreg_t
load_vreg_broadcast(const unsigned char *ptr)
{
	float *p = (float*) ptr;
	return load_broadcast(p);
}

#define SIMD_OPLANE

#include "modelHandler_simd.hpp"


namespace w2xc {
void
filter_AltiVec_impl(ComputeEnv *env,
		const float *packed_input,
		float *packed_output,
		int nInputPlanes,
		int nOutputPlanes,
		const float *fbiases,
		const float *weight,
		int ip_width,
		int ip_height,
		int nJob)
{
	if (simd_available(nInputPlanes, nOutputPlanes)) {
		filter_simd_impl0(env,
				packed_input,
				packed_output,
				nInputPlanes,
				nOutputPlanes,
				fbiases,
				weight,
				ip_width,
				ip_height,
				nJob);
	} else {
		filter_AVX_impl0(env,
				packed_input,
				packed_output,
				nInputPlanes,
				nOutputPlanes,
				fbiases,
				weight,
				ip_width,
				ip_height,
				nJob);
	}
}
}
