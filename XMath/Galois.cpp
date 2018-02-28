#include "Galois.h"

namespace CryptoCPP{
	namespace Math{
		size_t _ceil(double d)
		{
			return (size_t)d + (d > (size_t)d ? 1 : 0);
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

		GALOIS_API Galois::Galois(Galois & copy) :
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
			Galois * result = new Galois(*this);
			result->iadd(value);
			return result;
		}

		GALOIS_API Galois * Galois::sub(const Galois * value) const
		{ 
			Galois * result = new Galois(*this);
			result->isub(value);
			return result;
		}

		GALOIS_API Galois * Galois::mul(const Galois * value) const
		{ 
			Galois * result = new Galois(*this);
			result->imul(value);
			return result;
		}

		GALOIS_API Galois * Galois::inv(const Galois * value) const
		{ 
			Galois * result = new Galois(*this);
			result->iinv(value);
			return result;	
		}

		GALOIS_API void Galois::iadd(const Galois * value)
		{
			size_t min = data_size < value->data_size ? data_size : value->data_size;
			for(size_t t = 0; t<min; ++t)
			{
				size_t block = (get_value(t) + value->get_value(t)) % characteristic;
				set_value(t, block);
			}
		}

		GALOIS_API void Galois::isub(const Galois * value)
		{
			
		}

		GALOIS_API void Galois::imul(const Galois * value)
		{
			
		}

		GALOIS_API void Galois::iinv(cosnt Galois * value)
		{
			
		}

		GALOIS_API size_t Galois::_mask(size_t bits, bool side) const
		{
			size_t result = 0;
			for(size_t t = 0; t<bits; ++t) result = side?(result<<1)|1:(result>>1)|high_bit;
			return result;
		}

		GALOIS_API size_t Galois::get_value(size_t index) const
		{
			// Compute block/sub-block indexing
			size_t upper_bit_size = ((binary_block_size*index)%(8*sizeof(size_t)))%binary_block_size;
			size_t upper_block_index = (index*binary_block_size)/8;
			size_t lower_block_index = ((index - upper_bit_size)*binary_block_size)/8;
			
			// Boundary disparity check
			if(upper_block_index!=lower_block_index)
			{
				// Get block values
				size_t upper_block = data[upper_block_index] & _mask(upper_bit_size, true);
				size_t lower_block = data[lower_block_index] & _mask(binary_block_size-upper_bit_size, false);

				// Do alignment
				size_t block = (upper_block << (binary_block_size - upper_bit_size)) | (lower_block >> ((sizeof(size_t)*8)-upper_bit_size));
			
				return block;
			}
			else
			{
				// Passed: no boundary disparity
				size_t shift = (binary_block_size * index) % (8 * sizeof(size_t));
				size_t block_index = (binary_block_size * index) / (8 * sizeof(size_t));

				// Get and mask
				return data[block_index] >> shift & _mask(binary_block_size, true);
			}
		}

		GALOIS_API void Galois::set_value(size_t index, size_t value)
		{
			value = value % characteristic;
			
			// Compute block/sub-block indexing
			size_t upper_bit_size = ((binary_block_size*index)%(8*sizeof(size_t)))%binary_block_size;
			size_t upper_block_index = (index*binary_block_size)/8;
			size_t lower_block_index = ((index - upper_bit_size)*binary_block_size)/8;
			
			// Boundary disparity check
			if(upper_block_index!=lower_block_index)
			{
				// Mask bits
				data[upper_block_index] &= ~_mask(upper_bit_size, false);
				data[lower_block_index] &= ~_mask(binary_block_size-upper_bit_size, true);

				// Get block values
				data[upper_block_index] = value >> (binary_block_size - upper_bit_size);
				data[lower_block_index] = (value & _mask(binary_block_size-upper_bit_size, false)) << ((8 * sizeof(size_t)) - (binary_block_size - upper_bit_size));
			}
			else
			{
				// Passed: no boundary disparity
				size_t shift = (binary_block_size * index) % (8 * sizeof(size_t));
				size_t block_index = (binary_block_size * index) / (8 * sizeof(size_t));

				// Mask bits
				data[block_index] &= ~_mask(binary_block_size, false) << shift;

				// Apply shift
				data[block_index] = value << shift;
			}
		}
	}
}
