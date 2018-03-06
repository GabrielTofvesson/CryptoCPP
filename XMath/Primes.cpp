#define PRIME_API
#include "Primes.h"

namespace CryptoCPP { namespace Primes {
	const CryptoCPP::Math::BigInteger * one = new CryptoCPP::Math::BigInteger(1);
	const CryptoCPP::Math::BigInteger * two = new CryptoCPP::Math::BigInteger(2);
	const CryptoCPP::Math::BigInteger * three = new CryptoCPP::Math::BigInteger(3);

	PRIME_API bool fermat_prime_test(RandomProvider provider, const Math::BigInteger & value, size_t certainty)
	{

		Math::BigInteger * oneless = value - 1;
		size_t raw_size = oneless->highest_nonzero_index();
		size_t set_bit = raw_size * 8;
		char last = oneless->highest_nonzero();
		for (size_t t1 = 0; t1 < 8; ++t1)
			if (last & (1 << t1))
			{
				set_bit += t1;
				break;
			}
		bool notprime = false;
		for (size_t t = 0; t < certainty && !notprime; ++t)
		{
			// Generate a random test value
			size_t gen_size = 0;
			bool allzeroes;
			char* gen = generate_bounded_integer(provider, 0, last, raw_size, &gen_size, &allzeroes);	// Make sure value is smaller than n-1
			if (allzeroes) gen[0] |= 2;																// Generated value must be greater than 1

			Math::BigInteger * res = Math::BigInteger::mod_pow(Math::BigInteger(gen, gen_size), *oneless, value);

			if (*res != *one) notprime = true;

			delete res;
			delete[] gen;
		}

		delete oneless;

		return !notprime;
	}

	PRIME_API bool miller_rabin_prime_test(RandomProvider provider, const Math::BigInteger & value, size_t certainty)
	{
		if (value == *two || value == *three) return true;
		if (value < *two) return false;

		// Get index of lowest set bit
		Math::BigInteger * oneless = value - 1;
		size_t raw_size = oneless->highest_nonzero_index();
		size_t set_bit = raw_size * 8;
		char last = oneless->highest_nonzero();
		for (size_t t1 = 0; t1 < 8; ++t1)
			if (last & (1 << t1))
			{
				set_bit += t1;
				break;
			}

		Math::BigInteger * pow1 = new Math::BigInteger(set_bit);
		Math::BigInteger * pow2 = *pow1 * 2;

		Math::BigInteger * cur = pow1;

		bool isPrime = true;

		for (size_t t = 0; t < certainty; ++t) {
			// Generate a random test value
			size_t gen_size = 0;
			bool allzeroes;
			char* gen = generate_bounded_integer(provider, 0, last, raw_size, &gen_size, &allzeroes);	// Make sure value is smaller than n-1
			if (allzeroes) gen[0] |= 2;																// Generated value must be greater than 1

			Math::BigInteger * res = Math::BigInteger::mod_pow(Math::BigInteger(gen, gen_size), *pow1, value);
			delete[] gen;
			if (*res == *oneless || *res == *one) {
				delete res;
				continue;
			}
			res = Math::BigInteger::mod_pow(Math::BigInteger(gen, gen_size), *pow2, value);
			if (*res == *oneless || *res == *one) {
				delete res;
				continue;
			}
			else
			{
				delete res;
				isPrime = false;
				break;
			}

		}

		delete pow2;
		delete pow1;
		delete oneless;

		return isPrime;
	}


	PRIME_API Math::BigInteger * generate_prime(RandomProvider provider, size_t byte_count, size_t certainty, PrimalityTest test, bool & cancellation)
	{
		char * fill = new char[byte_count];
		while (!cancellation)
		{
			bool zeroes;
			do {
				generate_bounded_integer(provider, fill, (char)128, byte_count, &byte_count, &zeroes); // Bounded by 128 so that the high bit never can be set
			} while (zeroes);
			fill[0] |= 1; // Allways odd

			Math::BigInteger * res = new Math::BigInteger(fill, byte_count);
			delete[] fill;
			if (test(provider, *res, certainty)) return res;
			delete res;
		}
		
		// Task was cancelled. No prime could be found
		return 0;
	}

	PRIME_API char* generate_bounded_integer(RandomProvider provider, char * fill, char last, size_t max_size, size_t * gen_size, bool * allzeroes)
	{
		// Generate a random test value
		if(!*gen_size) *gen_size = (((provider() << 24) | (provider() << 16) | (provider() << 8) | (provider())) % max_size) + 1;
		if(!fill) fill = new char[*gen_size];
		*allzeroes = true;
		for (size_t t = 0; t < *gen_size; ++t) {
			fill[t] = provider();
			*allzeroes |= !fill[t];
		}
		if (*gen_size == max_size) fill[*gen_size - 1] %= last; // Clip last if necessary
		return fill;
	}
}}