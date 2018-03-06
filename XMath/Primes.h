#pragma once
#define BIGINT_API
#include "BigInteger.h"

#if defined(__MINGW32__) || defined(_WIN32)

#if defined(PRIME_API)
#undef PRIME_API
#define PRIME_API __declspec(dllexport)
#else
#define PRIME_API __declspec(dllimport)
#endif

#endif

#ifndef PRIME_API
#if __GNUC__ >= 4
#define PRIME_API __attribute__ ((visibility ("default")))
#else
#define PRIME_API
#endif
#endif

namespace CryptoCPP {
	namespace Primes {
		PRIME_API typedef char(*RandomProvider)();
		PRIME_API typedef bool(*PrimalityTest)(RandomProvider provider, const Math::BigInteger & value, size_t certainty);

		// Fermat primality test
		PRIME_API bool fermat_prime_test(RandomProvider provider, const Math::BigInteger & value, size_t certainty);

		// Miller-Rabin primality test
		PRIME_API bool miller_rabin_prime_test(RandomProvider provider, const Math::BigInteger & value, size_t certainty);

		// Generate a probable prime
		PRIME_API Math::BigInteger * generate_prime(RandomProvider provider, size_t byteCount, size_t certainty, PrimalityTest test, bool & cancellation);

		// Generate a value < max
		PRIME_API char* generate_bounded_integer(RandomProvider provider, char * fill, char last, size_t max_size, size_t * gen_size, bool * allzeroes);
	}
}