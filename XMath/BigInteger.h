#pragma once

#include <vector>
#include <queue>

#ifdef BIGINT_API
#define BIGINT_API __declspec(dllexport)   
#else  
#define BIGINT_API __declspec(dllimport)   
#endif  

#define BYTE unsigned char


class BigInteger {
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


	BIGINT_API BigInteger* operator+(const BigInteger* val) const;
	BIGINT_API BigInteger* operator-(const BigInteger* val) const;
	BIGINT_API BigInteger* operator*(const BigInteger* val) const;
	BIGINT_API BigInteger* operator/(const BigInteger* val) const;
	BIGINT_API BigInteger* operator%(const BigInteger* val) const;
	BIGINT_API BigInteger* operator^(const BigInteger* val) const;
	BIGINT_API BigInteger* operator&(const BigInteger* val) const;
	BIGINT_API BigInteger* operator|(const BigInteger* val) const;


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
	void iadd(const BigInteger& val, bool swaptarget);
	void isub(const BigInteger& val, bool swaptarget);
	void imul(const BigInteger& val, bool swaptarget);
	BigInteger* idiv(const BigInteger& val, bool swaptarget);
	void imod(const BigInteger& val, bool swaptarget);
	void ixor(const BigInteger& val, bool swaptarget);
	void iand(const BigInteger& val, bool swaptarget);
	void ior(const BigInteger& val, bool swaptarget);
	void inot();
	void ishl(uint64_t shift);
	void ishr(uint64_t shift);
	void twos_complement();
	void set_bit(size_t index, bool value);
	void cpy(const BigInteger& val, bool withsign);

	char cmp(const BigInteger& other, bool grt) const;

	// Math helper functions
	char shift_mask(int64_t shift, bool left);

	// For sorting and whatnot
	void clip_zeroes();
	size_t highest_set_bit(bool* hasbits) const;
	size_t lowest_set_bit(bool* hasbits) const;
	size_t nth_set_bit(size_t index, bool minfirst, bool* hasbits) const;
	bool is_zero() const;
};