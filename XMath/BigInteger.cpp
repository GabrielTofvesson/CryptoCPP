#include "BigInteger.h"

BigInteger::BigInteger(int64_t initialValue)
{
	data = new std::vector<char>();

	// We know how big this should be and we know the size won't change
	static const size_t bytes = sizeof(int64_t);
	for (size_t t = 0; t < bytes; ++t) data->push_back((initialValue >> (t * 8)) & 255);
}

BigInteger::BigInteger(BigInteger & initialvalue)
{
	size_t size = initialvalue.data->size();
	data = new std::vector<char>(size);
	for (size_t t = 0; t < size; ++t) data->push_back(initialvalue.data->at(t));
	sign = initialvalue.sign;
}

BigInteger * BigInteger::operator+(const BigInteger & val)
{
	return nullptr;
}

BigInteger * BigInteger::operator*(const BigInteger & val)
{
	return nullptr;
}

BigInteger * BigInteger::operator/(const BigInteger & val)
{
	return nullptr;
}

BigInteger * BigInteger::operator^(const BigInteger & val)
{
	return nullptr;
}

BigInteger * BigInteger::operator&(const BigInteger & val)
{
	return nullptr;
}

BigInteger * BigInteger::operator|(const BigInteger & val)
{
	return nullptr;
}

BigInteger * BigInteger::operator~()
{
	return nullptr;
}

BigInteger * BigInteger::operator<<(int64_t shiftcount)
{
	return nullptr;
}

BigInteger * BigInteger::operator>>(int64_t shiftcount)
{
	return nullptr;
}

void BigInteger::iadd(const BigInteger & val)
{
}

void BigInteger::isub(const BigInteger & val)
{
}

void BigInteger::imul(const BigInteger & val)
{
}

void BigInteger::idiv(const BigInteger & val)
{
}

void BigInteger::ixor(const BigInteger & val)
{
}

void BigInteger::iand(const BigInteger & val)
{
}

void BigInteger::ior(const BigInteger & val)
{
}

void BigInteger::inot(const BigInteger & val)
{
}

void BigInteger::ishl(int64_t shift)
{
}

void BigInteger::ishr(int64_t shift)
{
}

char BigInteger::shift_mask(int64_t shift, bool left)
{
	return 0;
}

void BigInteger::clip_zeroes()
{
}

size_t BigInteger::highest_set_bit(bool & hasbits)
{
	return size_t();
}

size_t BigInteger::lowest_set_bit(bool & hasbits)
{
	return size_t();
}
