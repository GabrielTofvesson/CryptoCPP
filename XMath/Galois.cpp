#define GALOIS_API

#include "Galois.h"
#include <string.h>

namespace CryptoCPP{
	namespace Math{
		size_t _ceil(double d)
		{
			return (size_t)d + (d > (size_t)d ? 1 : 0);
		}

		size_t block_count(size_t bin_block_size, size_t bufs)
		{
			return (bufs * 8) / bin_block_size;
		}

		void* do_copy(void* v, size_t size)
		{
			char * c = new char[size];
			memcpy(c, v, size);
			return c;
		}

		GALOIS_API Galois::Galois(
				size_t characteristic,
				size_t exponent,
				size_t * irreducible,
				size_t irreducible_size,
				size_t * value
			) :
				characteristic(characteristic),
				exponent(exponent),
				irreducible(irreducible),
				irreducible_size(irreducible_size),
				binary_block_size(_ceil(characteristic/2.0)),
				data_size(binary_block_size * exponent)
		{
			data = value;
		}

		GALOIS_API Galois::Galois(const Galois & copy) :
			characteristic(copy.characteristic),
			exponent(copy.exponent),
			irreducible(new size_t[copy.irreducible_size]),
			irreducible_size(copy.irreducible_size),
			binary_block_size(copy.binary_block_size),
			data_size(copy.data_size)
		{
			data = new size_t[data_size];
			memcpy(irreducible, copy.irreducible, irreducible_size * sizeof(size_t));
			memcpy(data, copy.data, data_size * sizeof(size_t));
		}

		GALOIS_API Galois::~Galois()
		{
			delete[] irreducible;
			delete[] data;
		}

		GALOIS_API Galois * Galois::add(const Galois * value) const
		{
			bool imSmaller = value->data_size > data_size;
			size_t state_size = imSmaller ? value->data_size : data_size;
			size_t * state = new size_t[state_size];
			memset(state, 0, state_size * sizeof(size_t));
			memcpy(state, imSmaller ? value->data : data, state_size * sizeof(size_t));

			iadd(imSmaller ? data : value->data, imSmaller ? data_size : value->data_size, binary_block_size, state, state_size, characteristic);
			
			return new Galois(characteristic, exponent, (size_t *)do_copy(irreducible, irreducible_size * sizeof(size_t)), irreducible_size, state);
		}

		GALOIS_API Galois * Galois::sub(const Galois * value) const
		{
			bool imSmaller = value->data_size > data_size;
			size_t state_size = imSmaller ? value->data_size : data_size;
			size_t * state = new size_t[state_size];
			memset(state, 0, state_size * sizeof(size_t));
			memcpy(state, data, data_size * sizeof(size_t));

			isub(value->data, value->data_size, binary_block_size, state, state_size, characteristic);

			return new Galois(characteristic, exponent, (size_t *)do_copy(irreducible, irreducible_size * sizeof(size_t)), irreducible_size, state);
		}

		GALOIS_API Galois * Galois::mul(const Galois * value) const
		{
			bool nb1, nb2;
			size_t
				h1 = high_factor(data, data_size, binary_block_size, &nb1),
				h2 = high_factor(value->data, value->data_size, value->binary_block_size, &nb2);

			// If one of the values is 0, return a zero-Galois
			if (nb1 || nb2) return new Galois(characteristic, exponent, (size_t *)do_copy(irreducible, irreducible_size * sizeof(size_t)), irreducible_size, (size_t*)memset(new size_t[1], 0, sizeof(size_t)));

			// The product of two values with the same base is represented as the sum of their exponents
			size_t * state = new size_t[h1 + h2];
			memset(state, 0, (h1 + h2) * sizeof(size_t));

			imul(value->data, value->data_size, binary_block_size, state, h1 + h2, characteristic, h1, h2);

			return new Galois(characteristic, exponent, (size_t *)do_copy(irreducible, irreducible_size * sizeof(size_t)), irreducible_size, state);
		}

		GALOIS_API Galois * Galois::inv() const
		{

		}

		// These internal functions assume that an adequate state size has been chose
		GALOIS_API void Galois::iadd(size_t * data, size_t data_size, size_t bin_size, size_t * state, size_t state_size, size_t characteristic)
		{
				for (size_t t = block_count(bin_size, data_size); t > 0; --t)
					set_value(
						t - 1,
						(
							get_value(t-1, bin_size, state) +
							get_value(t - 1, bin_size, data)
						) % characteristic,
						bin_size,
						characteristic, 
						state
					);
		}

		GALOIS_API void Galois::isub(size_t * data, size_t data_size, size_t bin_size, size_t * state, size_t state_size, size_t characteristic)
		{
			for (size_t t = block_count(bin_size, data_size); t > 0; --t)
				set_value(
					t - 1,
					(
						characteristic +
						get_value(t - 1, bin_size, state) -
						get_value(t - 1, bin_size, data)
						) % characteristic,
					bin_size,
					characteristic,
					state
				);
		}

		GALOIS_API void Galois::imul(size_t * data, size_t data_size, size_t bin_size, size_t * state, size_t state_size, size_t characteristic, size_t high1, size_t high2)
		{
			size_t * temp = new size_t[high1 + high2];
			memset(temp, 0, (high1 + high2) * sizeof(size_t));

			size_t data_blocks = block_count(bin_size, data_size);

			for (size_t t = block_count(bin_size, state_size); t > 0; --t)
			{
				memcpy(temp, data, data_size * sizeof(size_t));
				ilsh(temp, data_size + (t*bin_size), t);

				for (size_t tblk = 0; tblk < data_blocks; ++tblk) // Multiply each element
					set_value(tblk, get_value(tblk, bin_size, data) * get_value(t - 1, bin_size, state), bin_size, characteristic, temp);

				// Add shifted and multiplied value to state
				iadd(state, data_size + (t*bin_size), bin_size, state, state_size, characteristic);
			}
		}

		GALOIS_API void Galois::iinv(size_t * state, size_t state_size)
		{
			
		}

		GALOIS_API void Galois::ilsh(size_t * state, size_t state_size, size_t bin_size, size_t characteristic, size_t shiftc)
		{
			for (size_t t = block_count(bin_size, state_size); t > shiftc; --t)
				set_value(t - 1, get_value(t - 1 - shiftc, bin_size, state), bin_size, characteristic, state);
			for (size_t t = shiftc; t > 0; --t)
				set_value(t - 1, 0, bin_size, characteristic, state);
		}

		GALOIS_API size_t Galois::_mask(size_t bits, bool side)
		{
			size_t result = 0;
			for(size_t t = 0; t<bits; ++t) result = side?(result<<1)|1:(result>>1)|high_bit;
			return result;
		}

		GALOIS_API size_t Galois::get_value(size_t index, size_t block_size, size_t * from)
		{
			// Compute block/sub-block indexing
			size_t upper_bit_size = ((block_size*index)%(8*sizeof(size_t)))% block_size;
			size_t upper_block_index = (index*block_size)/8;
			size_t lower_block_index = ((index - upper_bit_size)*block_size)/8;
			
			// Boundary disparity check
			if(upper_block_index!=lower_block_index)
			{
				// Get block values
				size_t upper_block = from[upper_block_index] & _mask(upper_bit_size, true);
				size_t lower_block = from[lower_block_index] & _mask(block_size -upper_bit_size, false);

				// Do alignment
				size_t block = (upper_block << (block_size - upper_bit_size)) | (lower_block >> ((sizeof(size_t)*8)-upper_bit_size));
			
				return block;
			}
			else
			{
				// Passed: no boundary disparity
				size_t shift = (block_size * index) % (8 * sizeof(size_t));
				size_t block_index = (block_size * index) / (8 * sizeof(size_t));

				// Get and mask
				return from[block_index] >> shift & _mask(block_size, true);
			}
		}

		GALOIS_API void Galois::set_value(size_t index, size_t value, size_t block_size, size_t characteristic, size_t * to)
		{
			value = value % characteristic;
			
			// Compute block/sub-block indexing
			size_t upper_bit_size = ((block_size*index)%(8*sizeof(size_t)))%block_size;
			size_t upper_block_index = (index*block_size)/8;
			size_t lower_block_index = ((index - upper_bit_size)*block_size)/8;
			
			// Boundary disparity check
			if(upper_block_index!=lower_block_index)
			{
				// Mask bits
				to[upper_block_index] &= ~_mask(upper_bit_size, false);
				to[lower_block_index] &= ~_mask(block_size -upper_bit_size, true);

				// Get block values
				to[upper_block_index] = value >> (block_size - upper_bit_size);
				to[lower_block_index] = (value & _mask(block_size -upper_bit_size, false)) << ((8 * sizeof(size_t)) - (block_size - upper_bit_size));
			}
			else
			{
				// Passed: no boundary disparity
				size_t shift = (block_size * index) % (8 * sizeof(size_t));
				size_t block_index = (block_size * index) / (8 * sizeof(size_t));

				// Mask bits
				to[block_index] &= ~_mask(block_size, false) << shift;

				// Apply shift
				to[block_index] = value << shift;
			}
		}

		GALOIS_API size_t Galois::high_factor(size_t * state, size_t state_size, size_t bin_size, bool * noBits)
		{
			*noBits = false;
			for (size_t t = block_count(bin_size, state_size); t > 0; --t)
				if (get_value(t - 1, bin_size, state))
					return t - 1;
			*noBits = true;
			return 0;
		}
	}
}
