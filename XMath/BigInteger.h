#pragma once

#include <vector>

class BigInteger {
public:
	BigInteger(int64_t initialValue);
	BigInteger(BigInteger& initialvalue);

	// These should just create a new bigint and call the internal functions on it
	BigInteger* operator+(const BigInteger& val);
	BigInteger* operator-(const BigInteger& val);
	BigInteger* operator*(const BigInteger& val);
	BigInteger* operator/(const BigInteger& val);
	BigInteger* operator^(const BigInteger& val);
	BigInteger* operator&(const BigInteger& val);
	BigInteger* operator|(const BigInteger& val);
	BigInteger* operator~();
	BigInteger* operator<<(int64_t shiftcount);
	BigInteger* operator>>(int64_t shiftcount);

protected:
	std::vector<char>* data;
	bool sign;

	// Internal functions: manipulate the object they are called on
	void iadd(const BigInteger& val);
	void isub(const BigInteger& val);
	void imul(const BigInteger& val);
	void idiv(const BigInteger& val);
	void ixor(const BigInteger& val);
	void iand(const BigInteger& val);
	void ior(const BigInteger& val);
	void inot(const BigInteger& val);
	void ishl(int64_t shift);
	void ishr(int64_t shift);

	// Math helper functions
	char shift_mask(int64_t shift, bool left);

	// For sorting and whatnot
	void clip_zeroes();
	size_t highest_set_bit(bool& hasbits);
	size_t lowest_set_bit(bool& hasbits);
};