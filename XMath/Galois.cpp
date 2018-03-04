#define GALOIS_API

#include "Galois.h"
#include <string.h>
#include <vector>

namespace CryptoCPP{
	namespace Math{
		size_t _ceil(double d)
		{
			return (size_t)d + (d > (size_t)d ? 1 : 0);
		}

		size_t block_count(size_t bin_block_size, size_t bufs)
		{
			return (bufs * 8 * sizeof(BLOCK)) / bin_block_size;
		}

		void* do_copy(void* v, size_t size)
		{
			char * c = new char[size];
			memcpy(c, v, size);
			return c;
		}

		GALOIS_API Galois::Galois(
			BLOCK characteristic,
			BLOCK * irreducible,
			size_t irreducible_size,
			BLOCK * value,
			size_t value_size
			) :
				characteristic(characteristic),
				irreducible(irreducible),
				irreducible_size(irreducible_size),
				binary_block_size(_ceil(characteristic/2.0)),
				exponent(high_factor(irreducible, irreducible_size, _ceil(characteristic / 2.0), 0)),
				data_size(value_size)
		{
			data = value;
		}
		GALOIS_API Galois::Galois(
			BLOCK characteristic,
			BLOCK irreducible,
			BLOCK value
		) : Galois(characteristic, new BLOCK[1]{ irreducible }, 1, new BLOCK[1]{value}, 1)
		{ }

		GALOIS_API Galois::Galois(const Galois & copy) :
			characteristic(copy.characteristic),
			exponent(copy.exponent),
			irreducible(new BLOCK[copy.irreducible_size]),
			irreducible_size(copy.irreducible_size),
			binary_block_size(copy.binary_block_size),
			data_size(copy.data_size)
		{
			data = new BLOCK[data_size];
			memcpy(irreducible, copy.irreducible, irreducible_size * sizeof(BLOCK));
			memcpy(data, copy.data, data_size * sizeof(BLOCK));
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
			BLOCK * state = new BLOCK[state_size];
			memset(state, 0, state_size * sizeof(BLOCK));
			memcpy(state, imSmaller ? value->data : data, state_size * sizeof(BLOCK));

			iadd(imSmaller ? data : value->data, imSmaller ? data_size : value->data_size, binary_block_size, state, state_size, characteristic);
			
			return new Galois(characteristic, (BLOCK *)do_copy(irreducible, irreducible_size * sizeof(BLOCK)), irreducible_size, state, state_size);
		}

		GALOIS_API Galois * Galois::sub(const Galois * value) const
		{
			bool imSmaller = value->data_size > data_size;
			size_t state_size = imSmaller ? value->data_size : data_size;
			BLOCK * state = new BLOCK[state_size];
			memset(state, 0, state_size * sizeof(BLOCK));
			memcpy(state, data, data_size * sizeof(BLOCK));

			isub(value->data, value->data_size, binary_block_size, state, state_size, characteristic);

			return new Galois(characteristic, (BLOCK *)do_copy(irreducible, irreducible_size * sizeof(BLOCK)), irreducible_size, state, state_size);
		}

		GALOIS_API Galois * Galois::mul(const Galois * value) const
		{
			bool nb1, nb2;
			size_t
				h1 = high_factor(data, data_size, binary_block_size, &nb1),
				h2 = high_factor(value->data, value->data_size, value->binary_block_size, &nb2),
				h1_idx = h1 / (8 * sizeof(BLOCK)),
				h2_idx = h2 / (8 * sizeof(BLOCK));

			// If one of the values is 0, return a zero-Galois
			if (nb1 || nb2) return new Galois(characteristic, (BLOCK *)do_copy(irreducible, irreducible_size * sizeof(BLOCK)), irreducible_size, (BLOCK*)memset(new BLOCK[1], 0, sizeof(BLOCK)), 1);

			// The product of two values with the same base is represented as the sum of their exponents
			BLOCK * state = new BLOCK[h1_idx + h2_idx + 1];
			memset(state, 0, (h1_idx + h2_idx + 1) * sizeof(BLOCK));
			memcpy(state, this->data, this->data_size);

			BLOCK * cmp_exp = new BLOCK[(exponent/(8 * sizeof(BLOCK))) + 1];
			set_value(exponent, 1, binary_block_size, characteristic, cmp_exp);

			imul(value->data, value->data_size, binary_block_size, &state, h1_idx + h2_idx + 1, characteristic, h1_idx, h2_idx);
			ModResult * res = imod(state, h1_idx + h2_idx + 1, irreducible, irreducible_size, cmp_exp, (exponent / (8 * sizeof(BLOCK))) + 1, characteristic, binary_block_size);
			delete[] state;
			state = res->mod;
			size_t state_size = res->mod_size;
			delete[] res->div;
			delete res;

			return new Galois(characteristic, (BLOCK *)do_copy(irreducible, irreducible_size * sizeof(BLOCK)), irreducible_size, state, state_size);
		}

		GALOIS_API Galois * Galois::inv() const
		{
			struct FactorItem {
				FactorItem(BLOCK * factor, size_t factor_size) { this->factor = factor; this->factor_size = factor_size; }
				BLOCK * factor;
				size_t factor_size;
			};

			size_t exp = ((exponent + 1) / (8 * sizeof(BLOCK))) + 1;
			BLOCK * compute = new BLOCK[exp];
			memset(compute, 0, exp * sizeof(size_t));
			memcpy(compute, irreducible, irreducible_size * sizeof(BLOCK));

			size_t compute_size = exp;

			BLOCK * temp = new BLOCK[exp];
			memset(temp, 0, exp * sizeof(BLOCK));
			memcpy(temp, data, data_size * sizeof(BLOCK));

			size_t temp_size = exp;

			std::vector<FactorItem> factors = std::vector<FactorItem>();
			struct ModResult * m;
			size_t high;
			size_t highest1 = 1, highest2 = 1;

			// TODO: Implement extended Euclidean algorithm
			bool nb;
			volatile bool x = false;
		Loop:
			high_factor(temp, temp_size, binary_block_size, &nb);
			if (nb) goto Next;

			m = //new ModResult(); m->div = new size_t[m->div_size = 1]{(size_t)(change?0:1)}; m->mod = new size_t[m->mod_size = 1]{0};
				imod(compute, compute_size, temp, temp_size, temp, temp_size, characteristic, binary_block_size);
			delete[] compute;
			compute = temp;
			temp = m->mod;
			compute_size = temp_size;
			temp_size = m->mod_size;
			factors.push_back(FactorItem(m->div, m->div_size));

			// Record highest values
			high = high_factor(m->div, m->div_size, binary_block_size, &nb);
			if (high > highest1)
			{
				highest2 = highest1;
				highest1 = high;
			}
			else if (high > highest2) highest2 = high;
			delete m;
			goto Loop;

		Next:
			// Free unneeded resources
			delete[] temp;
			delete[] compute;

			// Remove invalid computation result
			delete[] factors.at(factors.size() - 1).factor;
			factors.pop_back();
			
			if (factors.size() == 0) factors.push_back(FactorItem(new BLOCK[1]{1}, 1));

			// Initialize left result of the diophantine equation
			compute_size = highest1 * highest2;
			compute = new BLOCK[compute_size];
			memset(compute, 0, compute_size * sizeof(BLOCK));
			memcpy(compute, factors.at(factors.size() - 1).factor, factors.at(factors.size() - 1).factor_size * sizeof(BLOCK));
			delete[] factors.at(factors.size() - 1).factor;
			factors.pop_back();

			// Initialize the right result
			temp_size = compute_size;
			temp = new BLOCK[temp_size];
			memset(temp, 0, compute_size * sizeof(BLOCK));
			temp[0] |= 1;

			BLOCK * cmp = new BLOCK[exp];
			memset(cmp, 0, exp * sizeof(BLOCK));
			cmp[exponent] = 1;

			// Initialize a holder for performing intermediate computations on
			size_t holder_size = compute_size;
			BLOCK * holder = new BLOCK[holder_size];

			BLOCK * transfer;
			size_t transfer_size;

			// Continue computation of both sides
			while (factors.size() > 0)
			{
				FactorItem item = factors.at(factors.size() - 1);
				factors.pop_back();
				memcpy(memset(holder, 0, holder_size), item.factor, item.factor_size * sizeof(size_t));

				size_t f1 = high_factor(temp, temp_size, binary_block_size, 0), f2 = high_factor(holder, holder_size, binary_block_size, 0);

				imul(holder, holder_size, binary_block_size, &temp, temp_size, characteristic, f1 / (8 * sizeof(BLOCK)), f2 / (8 * sizeof(BLOCK)));
				temp_size = (f1 / (8 * sizeof(BLOCK))) + (f2 / (8 * sizeof(BLOCK))) + 1;
				ModResult * result = imod(temp, temp_size, irreducible, irreducible_size, cmp, exp, characteristic, binary_block_size);
				memcpy(memset(temp, 0, temp_size * sizeof(BLOCK)), result->mod, result->mod_size);
				delete[] result->mod;
				delete[] result->div;
				delete result;

				// Do swap
				transfer = compute;
				transfer_size = compute_size;

				compute = temp;
				compute_size = temp_size;

				temp = transfer;
				temp_size = transfer_size;
			}

			size_t result_size = ((high_factor(compute, compute_size, binary_block_size, &nb) + 1) / (8 * sizeof(BLOCK))) + 1;
			BLOCK * result = new BLOCK[result_size];
			memcpy(result, compute, result_size * sizeof(BLOCK));
			delete[] compute;
			delete[] temp;
			delete[] cmp;
			return new Galois(characteristic, (BLOCK*)do_copy(irreducible, irreducible_size * sizeof(BLOCK)), irreducible_size, result, result_size);
		}

		// These internal functions assume that an adequate state size has been chose
		GALOIS_API void Galois::iadd(BLOCK * data, size_t data_size, size_t bin_size, BLOCK * state, size_t state_size, BLOCK characteristic)
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

		GALOIS_API void Galois::isub(BLOCK * data, size_t data_size, size_t bin_size, BLOCK * state, size_t state_size, BLOCK characteristic)
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

		GALOIS_API void Galois::imul(BLOCK * data, size_t data_size, size_t bin_size, BLOCK ** state, size_t state_size, BLOCK characteristic, size_t high1, size_t high2)
		{
			BLOCK * temp = new BLOCK[high1 + high2 + 1];
			//memset(temp, 0, (high1 + high2 + 1) * sizeof(size_t));

			BLOCK * res = new BLOCK[high1 + high2 + 1];
			memset(res, 0, (high1 + high2 + 1) * sizeof(BLOCK));

			size_t data_blocks = block_count(bin_size, data_size);

			for (size_t t = block_count(bin_size, state_size); t > 0; --t)
			{
				memset(temp, 0, (high1 + high2 + 1) * sizeof(BLOCK));
				//memcpy(temp, data, data_size * sizeof(size_t));
				//ilsh(temp, data_size, bin_size, characteristic, t-1);

				for (size_t tblk = 0; tblk < data_blocks; ++tblk) // Multiply each element
					set_value(tblk + t - 1, get_value(tblk, bin_size, data) * get_value(t - 1, bin_size, *state), bin_size, characteristic, temp);

				// Add shifted and multiplied value to state
				iadd(temp, high1 + high2 + 1, bin_size, res, high1 + high2 + 1, characteristic);
			}
			delete[] *state;
			*state = res;
		}

		GALOIS_API Galois::ModResult* Galois::imod(BLOCK * value, size_t value_size, BLOCK * modulo, size_t modulo_size, BLOCK * cmp, size_t cmp_size, BLOCK characteristic, size_t bin_size)
		{
			bool nb;
			size_t mod_max = high_factor(modulo, modulo_size, bin_size, &nb);
			if (nb) return 0;
			size_t cmp_max = high_factor(cmp, cmp_size, bin_size, &nb);
			if (nb) return 0;

			BLOCK * aligned = 0;
			ModResult * result = new ModResult();
			result->mod = new BLOCK[value_size];
			result->mod_size = value_size;
			memcpy(result->mod, value, value_size * sizeof(BLOCK));

			result->div = 0;

		LoopStart:	// Loop start
					// Loop evaluation
			size_t idx = high_factor(result->mod, result->mod_size, bin_size, &nb);
			if (nb || idx < cmp_max || (idx == cmp_max && result->mod[idx / (8 * sizeof(BLOCK))] < cmp[cmp_max / (8 * sizeof(BLOCK))])) goto LoopEnd; // Break

					// Loop body
			if (aligned == 0) {
				aligned = new BLOCK[value_size];
				result->div_size = (((idx - mod_max) * bin_size) / (8 * sizeof(BLOCK))) + 1;
				result->div = new BLOCK[result->div_size];
				memset(result->div, 0, result->div_size * sizeof(BLOCK));
			}
			memcpy(memset(aligned, 0, value_size * sizeof(BLOCK)), modulo, modulo_size * sizeof(BLOCK));
			ilsh(aligned, value_size, bin_size, characteristic, idx - mod_max);
			isub(aligned, value_size, bin_size, result->mod, value_size, characteristic);

			set_value(idx - mod_max, (get_value(idx - mod_max, bin_size, result->div) + 1) % characteristic, bin_size, characteristic, result->div);

					// End of loop body
			goto LoopStart;
		LoopEnd:

			if (result->div == 0) {
				result->div = new BLOCK[1]{0};
				result->div_size = 1;
			}
			delete[] aligned;

			// Stuff after loop
			return result;
		}

		GALOIS_API void Galois::ilsh(BLOCK * state, size_t state_size, size_t bin_size, BLOCK characteristic, size_t shiftc)
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

		GALOIS_API BLOCK Galois::get_value(size_t index, size_t block_size, BLOCK * from)
		{
			// Compute block/sub-block indexing
			size_t upper_bit_size = ((block_size*index)%(8*sizeof(BLOCK)))% block_size;
			size_t upper_block_index = (index*block_size)/(8 * sizeof(BLOCK));
			size_t lower_block_index = ((index - upper_bit_size)*block_size)/ (8 * sizeof(BLOCK));
			
			// Boundary disparity check
			if(upper_block_index!=lower_block_index)
			{
				// Get block values
				size_t upper_block = from[upper_block_index] & _mask(upper_bit_size, true);
				size_t lower_block = from[lower_block_index] & _mask(block_size -upper_bit_size, false);

				// Do alignment
				BLOCK block = (upper_block << (block_size - upper_bit_size)) | (lower_block >> ((sizeof(BLOCK)*8)-upper_bit_size));
			
				return block;
			}
			else
			{
				// Passed: no boundary disparity
				size_t shift = (block_size * index) % (8 * sizeof(BLOCK));
				size_t block_index = (block_size * index) / (8 * sizeof(BLOCK));

				// Get and mask
				return from[block_index] >> shift & _mask(block_size, true);
			}
		}

		GALOIS_API void Galois::set_value(size_t index, BLOCK value, size_t block_size, BLOCK characteristic, BLOCK * to)
		{
			value = value % characteristic;
			
			// Compute block/sub-block indexing
			size_t upper_bit_size = ((block_size*index)%(8*sizeof(BLOCK)))%block_size;
			size_t upper_block_index = (index*block_size)/8;
			size_t lower_block_index = ((index - upper_bit_size)*block_size)/8;
			
			// Boundary disparity check
			if(upper_block_index!=lower_block_index)
			{
				// Mask bits
				to[upper_block_index] &= ~_mask(upper_bit_size, true);
				to[lower_block_index] &= ~_mask(block_size - upper_bit_size, false);

				// Get block values
				to[upper_block_index] |= value >> (block_size - upper_bit_size);
				to[lower_block_index] |= (value & _mask(block_size -upper_bit_size, false)) << ((8 * sizeof(BLOCK)) - (block_size - upper_bit_size));
			}
			else
			{
				// Passed: no boundary disparity
				size_t shift = (block_size * index) % (8 * sizeof(BLOCK));
				size_t block_index = (block_size * index) / (8 * sizeof(BLOCK));

				// Mask bits
				to[block_index] &= ~(_mask(block_size, true) << shift);

				// Apply shift
				to[block_index] |= value << shift;
			}
		}

		GALOIS_API size_t Galois::high_factor(BLOCK * state, size_t state_size, size_t bin_size, bool * noBits)
		{
			if(noBits!=0) *noBits = false;
			for (size_t t = block_count(bin_size, state_size); t > 0; --t)
				if (get_value(t - 1, bin_size, state))
					return t - 1;
			if (noBits != 0) *noBits = true;
			return 0;
		}
	}
}
