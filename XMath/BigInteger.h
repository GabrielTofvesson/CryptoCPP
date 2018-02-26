#pragma once

#include <vector>

#ifdef BIGINT_API
#define BIGINT_API __declspec(dllexport)   
#else  
#define BIGINT_API __declspec(dllimport)   
#endif  

#define BYTE unsigned char


namespace CryptoCPP {
	namespace Math {
		class BigInteger
		{
		public:
			BIGINT_API BigInteger(int64_t initialValue);
			BIGINT_API BigInteger(const BigInteger& initialvalue);

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
			BIGINT_API BigInteger* operator<<(uint64_t shiftcount) const;
			BIGINT_API BigInteger* operator>>(uint64_t shiftcount) const;

			BIGINT_API BigInteger* operator+=(const BigInteger& val);
			BIGINT_API BigInteger* operator-=(const BigInteger& val);
			BIGINT_API BigInteger* operator*=(const BigInteger& val);
			BIGINT_API BigInteger* operator/=(const BigInteger& val);
			BIGINT_API BigInteger* operator%=(const BigInteger& val);
			BIGINT_API BigInteger* operator^=(const BigInteger& val);
			BIGINT_API BigInteger* operator&=(const BigInteger& val);
			BIGINT_API BigInteger* operator|=(const BigInteger& val);
			BIGINT_API BigInteger* operator<<=(uint64_t shiftcount);
			BIGINT_API BigInteger* operator>>=(uint64_t shiftcount);

			BIGINT_API bool operator<(const BigInteger& val) const;
			BIGINT_API bool operator>(const BigInteger& val) const;
			BIGINT_API bool operator<=(const BigInteger& val) const;
			BIGINT_API bool operator>=(const BigInteger& val) const;
			BIGINT_API bool operator==(const BigInteger& val) const;
			BIGINT_API bool operator!=(const BigInteger& val) const;

			BIGINT_API char* toString();

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
			BIGINT_API void ishl(uint64_t shift);
			BIGINT_API void ishr(uint64_t shift);
			BIGINT_API void twos_complement();
			BIGINT_API void set_bit(size_t index, bool value);
			BIGINT_API void cpy(const BigInteger& val, bool withsign);

			BIGINT_API char cmp(const BigInteger& other, bool grt) const;

			// Math helper functions
			BIGINT_API char shift_mask(int64_t shift, bool left);

			// For sorting and whatnot
			BIGINT_API void clip_zeroes();
			BIGINT_API size_t highest_set_bit(bool* hasbits) const;
			BIGINT_API size_t lowest_set_bit(bool* hasbits) const;
			BIGINT_API size_t nth_set_bit(size_t index, bool minfirst, bool* hasbits) const;
			BIGINT_API bool is_zero() const;
		};
	}
}