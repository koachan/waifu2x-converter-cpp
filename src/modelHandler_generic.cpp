#include <thread>
#include <atomic>
#include "filters.hpp"
#include "sec.hpp"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define VPACK 8
struct v256_t {
	float v[VPACK] __attribute__((aligned(16)));
};

static inline v256_t
madd256(v256_t const &v0, v256_t const &v1, v256_t const &v2)
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = v0.v[i] * v1.v[i] + v2.v[i];
	}
	return ret;
}

static inline v256_t
load_broadcast(const float *p)
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = p[0];
	}
	return ret;
}

static inline v256_t
load256(const float *p)
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = p[i];
	}
	return ret;
}

static inline void
store256(float *p, v256_t const &v)
{
	for(int i = 0; i < VPACK; i++) {
		p[i] = v.v[i];
	}
}

static inline v256_t
zero()
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = 0;
	}
	return ret;
}

static inline v256_t
set1(float a)
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = a;
	}
	return ret;
}

static inline float
hadd8(v256_t const &v)
{
	float ret = 0;
	for(int i = 0; i < VPACK; i++) {
		ret += v.v[i];
	}
	return ret;
}

static inline v256_t
mul256(v256_t const &a, v256_t const &b)
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = a.v[i] * b.v[i];
	}
	return ret;
}

static inline v256_t
add256(v256_t const &a, v256_t const &b)
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = a.v[i] + b.v[i];
	}
	return ret;
}

static inline v256_t
max256(v256_t const &a, v256_t const &b)
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = MAX(a.v[i], b.v[i]);
	}
	return ret;
}

static inline v256_t
min256(v256_t const &a, v256_t const &b)
{
	v256_t ret;
	for(int i = 0; i < VPACK; i++) {
		ret.v[i] = MIN(a.v[i], b.v[i]);
	}
	return ret;
}

#undef VPACK

#include "modelHandler_avx_func.hpp"

#undef UNROLL
#define UNROLL 2

/* Generic */
#define VEC_NELEM 4

typedef struct {
	float v[VEC_NELEM] __attribute__((aligned(16)));;
} vreg_t;

static inline vreg_t
madd_vreg(vreg_t const &a, vreg_t const &b, vreg_t const &c)
{
	vreg_t ret;
	for(int i = 0; i < VEC_NELEM; i++) {
		ret.v[i] = a.v[i] * b.v[i] + c.v[i];
	}
	return ret;
}

static inline vreg_t
add_vreg(vreg_t const &a, vreg_t const &b)
{
	vreg_t ret;
	for(int i = 0; i < VEC_NELEM; i++) {
		ret.v[i] = a.v[i] + b.v[i];
	}
	return ret;
}

static inline vreg_t
zero_vreg()
{
	vreg_t ret;
	for(int i = 0; i < VEC_NELEM; i++) {
		ret.v[i] = 0;
	}
	return ret;
}

static inline vreg_t
min_vreg(vreg_t const &a, vreg_t const &b)
{
	vreg_t ret;
	for(int i = 0; i < VEC_NELEM; i++) {
		ret.v[i] = MIN(a.v[i], b.v[i]);
	}
	return ret;
}

static inline vreg_t
max_vreg(vreg_t const &a, vreg_t const &b)
{
	vreg_t ret;
	for(int i = 0; i < VEC_NELEM; i++) {
		ret.v[i] = MAX(a.v[i], b.v[i]);
	}
	return ret;
}

static inline vreg_t
set1_vreg(float a)
{
	vreg_t ret;
	for(int i = 0; i < VEC_NELEM; i++) {
		ret.v[i] = a;
	}
	return ret;
}

static inline void
store_vreg(const unsigned char *ptr, vreg_t const &val)
{
	float *p = (float*) ptr;
	vreg_t ret;
	for(int i = 0; i < VEC_NELEM; i++) {
		p[i] = val.v[i];
	}
}

static inline vreg_t
load_vreg(const unsigned char *ptr)
{
	vreg_t ret;
	float *p = (float*) ptr;
	for(int i = 0; i < VEC_NELEM; i++) {
		ret.v[i] = p[i];
	}
	return ret;
}

static inline vreg_t
load_vreg_broadcast(const unsigned char *ptr)
{
	vreg_t ret;
	float *p = (float*) ptr;
	for(int i = 0; i < VEC_NELEM; i++) {
		ret.v[i] = p[0];
	}
	return ret;
}

#define SIMD_OPLANE

#include "modelHandler_simd.hpp"


namespace w2xc {
void
filter_Generic_impl(ComputeEnv *env,
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
