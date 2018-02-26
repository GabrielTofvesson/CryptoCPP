#define BIGINT_API

#include "BigInteger.h"

namespace CryptoCPP {
	namespace Math {
		BIGINT_API BigInteger::BigInteger(int64_t initialValue)
		{
			data = new std::vector<BYTE>();

			// We know how big this should be and we know the size won't change
			static const size_t bytes = sizeof(int64_t);
			for (size_t t = 0; t < bytes; ++t) data->push_back((initialValue >> (t * 8)) & 255);

			sign = false;

			if (initialValue < 0) {
				sign = 1;
				twos_complement();  // Compute two's complement of this value
			}

			clip_zeroes();
		}

		BIGINT_API BigInteger::BigInteger(const BigInteger & initialvalue)
		{
			size_t size = initialvalue.data->size();
			data = new std::vector<BYTE>(size);
			for (size_t t = 0; t < size; ++t) (*data)[t] = (*initialvalue.data)[t];
			sign = initialvalue.sign;
		}

		BIGINT_API BigInteger * BigInteger::operator+(const BigInteger & val) const
		{
			BigInteger* create = new BigInteger(*this);
			create->iadd(val, false);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator-(const BigInteger & val) const
		{
			BigInteger* create = new BigInteger(*this);
			create->isub(val, false);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator*(const BigInteger & val) const
		{
			BigInteger* create = new BigInteger(*this);
			create->imul(val, false);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator/(const BigInteger & val) const
		{
			BigInteger* create = new BigInteger(*this);
			create->idiv(val, false);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator%(const BigInteger & val) const
		{
			BigInteger* create = new BigInteger(*this);
			create->imod(val, false);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator^(const BigInteger & val) const
		{
			BigInteger* create = new BigInteger(*this);
			create->ixor(val, false);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator&(const BigInteger & val) const
		{
			BigInteger* create = new BigInteger(*this);
			create->iand(val, false);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator|(const BigInteger & val) const
		{
			BigInteger* create = new BigInteger(*this);
			create->ior(val, false);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator~() const
		{
			BigInteger* create = new BigInteger(*this);
			create->inot();
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator<<(uint64_t shiftcount) const
		{
			BigInteger* create = new BigInteger(*this);
			create->ishl(shiftcount);
			return create;
		}

		BIGINT_API BigInteger * BigInteger::operator>>(uint64_t shiftcount) const
		{
			BigInteger* create = new BigInteger(*this);
			create->ishr(shiftcount);
			return create;
		}



		BIGINT_API BigInteger* BigInteger::operator+=(const BigInteger& val)
		{
			iadd(val, false);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator-=(const BigInteger& val)
		{
			isub(val, false);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator*=(const BigInteger& val)
		{
			imul(val, false);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator/=(const BigInteger& val)
		{
			idiv(val, false);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator%=(const BigInteger& val)
		{
			imod(val, false);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator^=(const BigInteger& val)
		{
			ixor(val, false);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator&=(const BigInteger& val)
		{
			iand(val, false);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator|=(const BigInteger& val)
		{
			ior(val, false);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator<<=(uint64_t shiftcount)
		{
			ishl(shiftcount);
			return this;
		}

		BIGINT_API BigInteger* BigInteger::operator>>=(uint64_t shiftcount)
		{
			ishr(shiftcount);
			return this;
		}



		BIGINT_API bool BigInteger::operator<(const BigInteger& val) const
		{
			return cmp(val, false) == 1;
		}

		BIGINT_API bool BigInteger::operator>(const BigInteger& val) const
		{
			return cmp(val, true) == 1;
		}

		BIGINT_API bool BigInteger::operator<=(const BigInteger& val) const
		{
			return cmp(val, false) != 0;
		}

		BIGINT_API bool BigInteger::operator>=(const BigInteger& val) const
		{
			return cmp(val, true) != 0;
		}

		BIGINT_API bool BigInteger::operator==(const BigInteger& val) const
		{
			if (sign != val.sign || data->size() != val.data->size()) return false;
			for (size_t i = 0; i < data->size(); ++i) if ((*data)[i] != (*val.data)[i]) return false;
			return true;
		}

		BIGINT_API bool BigInteger::operator!=(const BigInteger& val) const
		{
			return !(*this == val);
		}


		BIGINT_API char* BigInteger::toString()
		{
			char* string = new char[data->size() * 2 + 3];
			string[0] = '0';
			string[1] = 'x';
			string[data->size() * 2 + 2] = 0;
			for (size_t t = 0; t < data->size(); ++t) {
				string[(data->size() - 1 - t) * 2 + 3] = (data->at(t) & 15) + ((data->at(t) & 15) > 9 ? 87 : 48);
				string[(data->size() - 1 - t) * 2 + 2] = (data->at(t) >> 4) + ((data->at(t) >> 4) > 9 ? 87 : 48);
			}
			return string;
		}


		BIGINT_API void BigInteger::iadd(const BigInteger & other, bool swaptarget)
		{
			if ((other.sign != sign) ^ swaptarget)
			{
				if (other.sign ^ swaptarget)
				{
					isub(other, true);
				}
				else
				{
					BigInteger tmp = BigInteger(other);
					tmp.isub(*this, false);
					cpy(tmp, true);
				}
				return;
			}
			bool carry = false;
			bool greater = other.data->size() > data->size();
			size_t min = greater ? data->size() : other.data->size();
			const BigInteger* larger = greater ? &other : other.data->size() < data->size() ? this : nullptr;

			for (size_t i = 0; i < min; ++i)
			{
				int res = (*data)[i] + (*other.data)[i] + (carry ? 1 : 0);
				carry = res > 255;
				(*data)[i] = (res % 256);
			}
			if (larger == &other)
			{
				for (size_t i = min; i < larger->data->size(); ++i)
				{
					int res = larger->data->at(i) + (carry ? 1 : 0);
					carry = res < larger->data->at(i);
					data->push_back(res % 256);
				}
			}
			else
			{
				int at = min;
				while (carry)
				{
					if (at == data->size())
					{
						data->push_back(1);
						break;
					}
					int res = data->at(at) + 1;
					carry = res == 256;
					(*data)[at] = res % 256;
					++at;
				}
			}
			clip_zeroes();
			if (!data->size()) data->push_back(0);
		}

		BIGINT_API void BigInteger::isub(const BigInteger & other, bool swaptarget)
		{
			if (other.sign ^ sign ^ swaptarget) // this - (-other) = this + other
			{
				bool swap = !sign;
				sign = false;
				iadd(other, swap);
				sign = !swap;
				return;
			}
			if (sign) // -this - (-other) = -this + other = other - this
			{
				BigInteger res = BigInteger(other);
				res.sign = false;
				sign = false;
				res.isub(*this, false);
				cpy(res, true);
			}
			else if ((*this) < other) // this - other (where other>this)
			{
				BigInteger res = BigInteger(other);
				res.isub(*this, false);
				sign = true;
				cpy(res, false);
			}
			else // this - other (where other<=this)
			{
				// Get two's complement of the other value
				BigInteger tc = BigInteger(other);
				tc.twos_complement();
				tc.iadd(*this, false);
				bool hb;
				size_t idx = tc.nth_set_bit(0, false, &hb);
				if (hb) (*tc.data)[idx / 8] &= ~(1 << (int)(idx % 8));
				tc.clip_zeroes();
				cpy(tc, false);
			}
			if (!data->size()) data->push_back(0);
		}

		BIGINT_API void BigInteger::imul(const BigInteger & val, bool swaptarget)
		{
			bool hb, passed = false;
			size_t track = -1;
			size_t bit, prev = 0;

			BigInteger tmp = BigInteger(*this);

			data->clear();

		Loop:
			bit = val.nth_set_bit(++track, true, &hb);
			if (!hb || (passed && track == 0)) return;
			passed = true;

			tmp.ishl(bit - prev);
			iadd(tmp, false);

			prev = bit;
			goto Loop;
		}

		BIGINT_API BigInteger* BigInteger::idiv(const BigInteger & val, bool swaptarget)
		{
			if (val.is_zero()) throw new std::exception("Divide by zero!");
			BigInteger* rem = new BigInteger(0);
			BigInteger quot = BigInteger(0);

			rem->data->push_back(0);
			bool hb;
			for (size_t t = highest_set_bit(&hb) + 1; hb && t > 0; --t)
			{
				rem->ishl(1);
				(*rem->data)[0] |= ((*data)[(t - 1) / 8] >> ((t - 1) % 8)) & 1;

				if (*rem >= val)
				{
					rem->isub(val, false);
					quot.set_bit((t - 1), 1);
				}
			}
			cpy(quot, false);
			sign = val.sign ^ sign ^ swaptarget;
			if (!data->size()) data->push_back(0);
			return rem;
		}

		BIGINT_API void BigInteger::imod(const BigInteger & val, bool swaptarget)
		{
			BigInteger* modres = idiv(val, swaptarget);
			cpy(*modres, true);
			delete modres;
		}

		BIGINT_API void BigInteger::ixor(const BigInteger & val, bool swaptarget)
		{
			size_t s1 = val.data->size(), s2 = data->size();
			for (size_t t = 0; t < s1; ++t)
				if (t < s2)
					(*data)[t] ^= val.data->at(t);
				else
					data->push_back(val.data->at(t));
		}

		BIGINT_API void BigInteger::iand(const BigInteger & val, bool swaptarget)
		{
			size_t s1 = data->size(), s2 = val.data->size();
			for (size_t t = 0; t < s1; ++t)
				if (t < s2)
					(*data)[t] &= val.data->at(t);
				else
					break;
		}

		BIGINT_API void BigInteger::ior(const BigInteger & val, bool swaptarget)
		{
			size_t s1 = val.data->size(), s2 = data->size();
			for (size_t t = 0; t < s1; ++t)
				if (t < s2)
					(*data)[t] |= val.data->at(t);
				else
					data->push_back(val.data->at(t));
		}

		BIGINT_API void BigInteger::inot()
		{
			for (size_t t = 0; t < data->size(); ++t) (*data)[t] = ~(*data)[t];
		}

		BIGINT_API void BigInteger::ishl(uint64_t shift)
		{
			size_t set = shift / 8;
			char sub = shift % 8;
			BYTE bm = shift_mask(sub, true);
			BYTE ibm = ~bm;
			BYTE carry = 0;
			bool hb;
			size_t fsb1 = highest_set_bit(&hb);
			if (!hb) return;
			BYTE fsb = (fsb1 % 8);
			size_t cl = data->size() + set + ((fsb + sub) % 8 == 0 ? 0 : 1);
			BYTE* create = new BYTE[cl];
			create[cl - 1] = 0;
			for (size_t i = set; i - set < data->size(); ++i)
			{
				create[i] = ((((*data)[i - set] & ibm) << sub) | carry);
				carry = (((*data)[i - set] & bm) >> (8 - sub));
			}
			create[cl - 1] |= carry;
			data->clear();
			data->resize(set);
			for (size_t t = 0; t < cl; ++t) data->push_back(create[t]);
			delete[] create;
			clip_zeroes();
		}

		BIGINT_API void BigInteger::ishr(uint64_t shift)
		{
			size_t offset = shift / 8;
			char sub = shift % 8;
			BYTE bm = shift_mask(sub, false);
			BYTE ibm = ~bm;
			bool hb;
			size_t fsb1 = highest_set_bit(&hb);
			if (hb) return;
			BYTE carry = 0;

			std::vector<BYTE> collect = std::vector<BYTE>();
			for (size_t t = 0; t < data->size() - offset; ++t) collect.push_back(data->at(t + offset));
			if (collect.at(collect.size() - 1) == 0) collect.pop_back();
			data->clear();
			data->resize(collect.size());

			for (size_t t = 0; t < collect.size(); ++t)
			{
				(*data)[t] = (collect[t] >> sub);
				if (t + 1 < collect.size()) (*data)[t] |= (collect[t + 1] & bm) << (8 - sub);
			}
			if (!data->size()) data->push_back(0);
		}

		BIGINT_API void BigInteger::twos_complement()
		{
			for (size_t i = 0; i < data->size(); ++i) (*data)[i] = ~(*data)[i];
			iadd(BigInteger(1), false);
		}

		BIGINT_API void BigInteger::set_bit(size_t index, bool value)
		{
			BYTE fill = sign ? 0xFF : 0x00;
			while (index >= data->size() * 8) data->push_back(fill);
			(*data)[index / 8] = ((*data)[index / 8] & ~(1 << (index % 8))) | ((value ? 1 : 0) << (index % 8));
		}

		BIGINT_API void BigInteger::cpy(const BigInteger& val, bool withsign)
		{
			if (withsign) sign = val.sign;
			data->clear();
			for (BYTE b : *val.data) data->push_back(b);
		}

		BIGINT_API char BigInteger::cmp(const BigInteger & other, bool grt) const
		{
			// If the other number is less than zero and this is a positive number, this number is larger and vice versa
			if (other.sign && !sign && other.data->size() != 0) return grt ? 1 : 0;
			if (sign && !other.sign && data->size() != 0) return grt ? 0 : 1;
			size_t l1, l2;
			size_t idx = 0;
			bool hb1, hb2;
			while ((l1 = nth_set_bit(idx, false, &hb1)) == (l2 = other.nth_set_bit(idx, false, &hb2))) {
				++idx;
				if (hb1 ^ hb2) return hb2 ^ grt ? 1 : 0;
				if (!hb1) return 2;
			}
			return ((l1 > l2 && (!sign == grt)) || ((sign == grt) && l1 < l2)) ? 1 : 0;
		}

		BIGINT_API char BigInteger::shift_mask(int64_t shift, bool left)
		{
			BYTE res = 0;
			for (uint64_t i = shift; i > 0; --i) res = left ? (res >> 1) | 128 : (res << 1) | 1;
			return res;
		}

		BIGINT_API void BigInteger::clip_zeroes()
		{
			while (data->size() > 1 && !data->at(data->size() - 1)) data->pop_back();
		}

		BIGINT_API size_t BigInteger::highest_set_bit(bool * hasbits) const
		{
			*hasbits = true;
			for (size_t t = data->size(); t > 0; --t)
				if (data->at(t - 1))
					for (size_t t1 = 8; t1 > 0; --t1)
						if (data->at(t - 1) & (1 << (t1 - 1)))
							return (t1 - 1) + ((t - 1) * 8);
			*hasbits = false;
			return 0;
		}

		BIGINT_API size_t BigInteger::lowest_set_bit(bool * hasbits) const
		{
			*hasbits = true;
			for (size_t t = 0; t < data->size(); ++t)
				if (data->at(t))
					for (size_t t1 = 0; t1 < 8; ++t1)
						if (data->at(t1) & (1 << t1))
							return t1 + (t * 8);
			*hasbits = false;
			return 0;
		}

		BIGINT_API size_t BigInteger::nth_set_bit(size_t index, bool minfirst, bool * hasbits) const
		{
			*hasbits = true;
			size_t target = index + 1;
			for (size_t l = minfirst ? 1 : (data->size() * 8); (minfirst && l <= data->size() * 8) || (!minfirst && l > 0); l += minfirst ? 1 : -1)
				if (((*data)[(int)((l - 1) / 8)] & (1 << (int)((l - 1) % 8))) && --target == 0)
					return l - 1;
			*hasbits = false;
			return 0;
		}

		BIGINT_API bool BigInteger::is_zero() const
		{
			for (size_t t = 0; t < data->size(); ++t)
				if (data->at(t))
					return false;
			return true;
		}

	}
}