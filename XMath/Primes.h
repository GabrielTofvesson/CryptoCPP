#pragma once

#include "BigInteger.h"

namespace CryptoCPP {
	namespace Primes {
		bool fermat_prime_test(const Math::BigInteger & value, size_t certainty);
		bool miller_rabin_prime_test(const Math::BigInteger & value, size_t certainty);

		Math::BigInteger * generate_prime(size_t byteCount, size_t certainty);
	}
}