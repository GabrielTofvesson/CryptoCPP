#pragma once

#include "BigInteger.h"

namespace CryptoCPP {
	namespace Primes {
		bool fermat_prime_test(const BigInteger & value, size_t certainty);
		bool miller_rabin_prime_test(const BigInteger & value, size_t certainty);

		BigInteger * generate_prime(size_t byteCount, size_t certainty);
	}
}