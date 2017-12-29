#include <emmintrin.h>

namespace Qrack {

	struct ComplexSimd {
		__m128d _val;

		ComplexSimd() {
		}
		
		ComplexSimd(__m128d v) {
			_val = v;
		}
		ComplexSimd(double real, double imag) {
			_val = _mm_set_pd(imag, real);
		}

		ComplexSimd operator+(const ComplexSimd& other) const {
			return ComplexSimd(_mm_add_pd(_val, other._val));
		}
		ComplexSimd operator+=(const ComplexSimd& other) {
			_val = _mm_add_pd(_val, other._val);
			return ComplexSimd(_val);
		}
		ComplexSimd operator-(const ComplexSimd& other) const {
			return ComplexSimd(_mm_sub_pd(_val, other._val));
		}
		ComplexSimd operator-=(const ComplexSimd& other) {
			_val = _mm_sub_pd(_val, other._val);
			return ComplexSimd(_val);
		}
		ComplexSimd operator*(const ComplexSimd& other) const {
			__v2df realComps = (__v2df)_mm_mul_pd(other._val, _val);
			__v2df imagComps  = (__v2df)_mm_mul_pd(other._val,
				_mm_move_sd(
					_mm_castsi128_pd(_mm_bslli_si128(_mm_castpd_si128(_val), 8)),
					_mm_castsi128_pd(_mm_bsrli_si128(_mm_castpd_si128(_val), 8))
				)
			);			
			return ComplexSimd(realComps[0] - realComps[1], imagComps[0] + imagComps[1]);
		}
		ComplexSimd operator*=(const ComplexSimd& other) {
			__v2df realComps = (__v2df)_mm_mul_pd(other._val, _val);
			__v2df imagComps  = (__v2df)_mm_mul_pd(other._val,
				_mm_move_sd(
					_mm_castsi128_pd(_mm_bslli_si128(_mm_castpd_si128(_val), 8)),
					_mm_castsi128_pd(_mm_bsrli_si128(_mm_castpd_si128(_val), 8))
				)
			);	
			_val = _mm_set_pd(realComps[0] - realComps[1], imagComps[0] + imagComps[1]);
			return ComplexSimd(_val);
		}
		ComplexSimd operator*(const double rhs) const {
			return _mm_mul_pd(_val, _mm_set1_pd(rhs));
		}
		ComplexSimd operator/(const double rhs) const {
			return _mm_div_pd(_val, _mm_set1_pd(rhs));
		}
		ComplexSimd operator/=(const double rhs) {
			_val = _mm_div_pd(_val, _mm_set1_pd(rhs));
			return ComplexSimd(_val);
		}
	};

	static ComplexSimd operator*(const double lhs, const ComplexSimd& rhs) {
		return _mm_mul_pd(_mm_set1_pd(lhs), rhs._val);
	}
	double real(const ComplexSimd& cmplx) {
		return ((__v2df)(cmplx._val))[0];
	}
	double imag(const ComplexSimd& cmplx) {
		return ((__v2df)(cmplx._val))[1];
	}
	double normSqrd(const ComplexSimd& cmplx) {
		__v2df temp = (__v2df)_mm_mul_pd(cmplx._val, cmplx._val);
		return (temp[0] + temp[1]); 
	}
}
