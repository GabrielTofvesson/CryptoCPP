#pragma once

#include <vector>
#include <tuple>

#if defined(__MINGW32__) || defined(_WIN32)

#if defined(BIGINT_API)
#undef BIGINT_API
#define BIGINT_API __declspec(dllexport)
#else
#define BIGINT_API __declspec(dllimport)
#endif

#endif

#ifndef BIGINT_API
  #if __GNUC__ >= 4
    #define BIGINT_API __attribute__ ((visibility ("default")))
  #else
    #define BIGINT_API
  #endif
#endif

#define BYTE unsigned char


namespace CryptoCPP {
	namespace Math {
		class BigInteger
		{
		public:
			BIGINT_API BigInteger(long long initialValue);
			BIGINT_API BigInteger(const BigInteger& initialvalue);
			BIGINT_API BigInteger(const char * value, size_t size);
			BIGINT_API ~BigInteger();

			// These should just create a new bigint and call the internal functions on it
			BIGINT_API BigInteger* operator+(const BigInteger& val) const;
			BIGINT_API BigInteger* operator-(const BigInteger& val) const;
			BIGINT_API BigInteger* operator*(const BigInteger& val) const;
			BIGINT_API BigInteger* operator/(const BigInteger& val) const;
			BIGINT_API BigInteger* operator%(const BigInteger& val) const;
			BIGINT_API BigInteger* operator^(const BigInteger& val) const;
			BIGINT_API BigInteger* operator&(const BigInteger& val) const;
			BIGINT_API BigInteger* operator|(const BigInteger& val) const;
			BIGINT_API BigInteger* operator~() const;
			BIGINT_API BigInteger* operator<<(size_t shiftcount) const;
			BIGINT_API BigInteger* operator>>(size_t shiftcount) const;

			BIGINT_API BigInteger* operator+=(const BigInteger& val);
			BIGINT_API BigInteger* operator-=(const BigInteger& val);
			BIGINT_API BigInteger* operator*=(const BigInteger& val);
			BIGINT_API BigInteger* operator/=(const BigInteger& val);
			BIGINT_API BigInteger* operator%=(const BigInteger& val);
			BIGINT_API BigInteger* operator^=(const BigInteger& val);
			BIGINT_API BigInteger* operator&=(const BigInteger& val);
			BIGINT_API BigInteger* operator|=(const BigInteger& val);
			BIGINT_API BigInteger* operator<<=(size_t shiftcount);
			BIGINT_API BigInteger* operator>>=(size_t shiftcount);

			BIGINT_API bool operator<(const BigInteger& val) const;
			BIGINT_API bool operator>(const BigInteger& val) const;
			BIGINT_API bool operator<=(const BigInteger& val) const;
			BIGINT_API bool operator>=(const BigInteger& val) const;
			BIGINT_API bool operator==(const BigInteger& val) const;
			BIGINT_API bool operator!=(const BigInteger& val) const;

			BIGINT_API BigInteger * pow(const size_t exp) const;
			BIGINT_API BigInteger * pow(const BigInteger & exp) const;

			BIGINT_API char lowest() const;
			BIGINT_API char highest_nonzero() const;
			BIGINT_API size_t highest_nonzero_index() const;
			BIGINT_API char* to_array(size_t * size_out) const;
			BIGINT_API char* to_string() const;

			BIGINT_API static BigInteger* mul_inv(const BigInteger & v1, const BigInteger & v2);
			BIGINT_API static BigInteger* mod_pow(const BigInteger* base, const BigInteger* exp, const BigInteger* mod);
			BIGINT_API static BigInteger* mod_pow(const BigInteger & base, const BigInteger & exp, const BigInteger & mod);
			BIGINT_API static BigInteger* gcd(const BigInteger* i1, const BigInteger* i2);

		protected:
			std::vector<BYTE>* data;
			bool sign;

			// Internal functions: manipulate the object they are called on
			BIGINT_API void iadd(const BigInteger& val, bool swaptarget);
			BIGINT_API void isub(const BigInteger& val, bool swaptarget);
			BIGINT_API void imul(const BigInteger& val, bool swaptarget);
			BIGINT_API BigInteger* idiv(const BigInteger& val, bool swaptarget);
			BIGINT_API void imod(const BigInteger& val, bool swaptarget);
			BIGINT_API void ixor(const BigInteger& val, bool swaptarget);
			BIGINT_API void iand(const BigInteger& val, bool swaptarget);
			BIGINT_API void ior(const BigInteger& val, bool swaptarget);
			BIGINT_API void inot();
			BIGINT_API void ishl(size_t shift);
			BIGINT_API void ishr(size_t shift);
			BIGINT_API void twos_complement();
			BIGINT_API void set_bit(size_t index, bool value);
			BIGINT_API void cpy(const BigInteger& val, bool withsign);

			BIGINT_API char cmp(const BigInteger& other, bool grt) const;

			// Math helper functions
			BIGINT_API char shift_mask(size_t shift, bool left);

			// For sorting and whatnot
			BIGINT_API void clip_zeroes();
			BIGINT_API size_t highest_set_bit(bool* hasbits) const;
			BIGINT_API size_t lowest_set_bit(bool* hasbits) const;
			BIGINT_API size_t nth_set_bit(size_t index, bool minfirst, bool* hasbits) const;
			BIGINT_API bool is_zero() const;
		};
	}
}

