#pragma once

#if defined(__MINGW32__) || defined(_WIN32)

#if defined(GALOIS_API)
#undef GALOIS_API
#define GALOIS_API __declspec(dllexport)
#else
#define GALOIS_API __declspec(dllimport)
#endif

#endif

#ifndef GALOIS_API
  #if __GNUC__ >= 4
    #define GALOIS_API __attribute__ ((visibility ("default")))
  #else
    #define GALOIS_API
  #endif
#endif

#define BLOCK size_t

namespace CryptoCPP {
	namespace Math {
		class Galois
		{
		public:
			GALOIS_API Galois(
				BLOCK characteristic,
				BLOCK * irreducible,
				size_t irreducible_size,
				BLOCK * value,
				size_t value_size
				);
			GALOIS_API Galois(
				BLOCK characteristic,
				BLOCK irreducible,
				BLOCK value
			);
			GALOIS_API Galois(const Galois & copy);
			GALOIS_API ~Galois();

			// Addition
			GALOIS_API Galois * add(const Galois * value) const;

			// Subtraction
			GALOIS_API Galois * sub(const Galois * value) const;

			// Multiplication
			GALOIS_API Galois * mul(const Galois * value) const;

			// Inverse multiplication
			GALOIS_API Galois * inv() const;

			// Get internal value
			GALOIS_API BLOCK * to_array(size_t * size = 0);
			GALOIS_API BLOCK get_lowest();

		protected:
			static const BLOCK high_bit = 1 << ((sizeof(BLOCK) * 8) - 1);
			// GF parameters & value
			BLOCK characteristic, *irreducible, *data;
			// Storage params
			size_t binary_block_size, data_size, irreducible_size, exponent;


			struct ModResult {
				BLOCK * div;
				size_t div_size;
				BLOCK * mod;
				size_t mod_size;
			};


			// Logic
			GALOIS_API static void iadd(BLOCK * data, size_t data_size, size_t bin_size, BLOCK * state, size_t state_size, BLOCK characteristic);	// Addition
			GALOIS_API static void isub(BLOCK * data, size_t data_size, size_t bin_size, BLOCK * state, size_t state_size, BLOCK characteristic);	// Subtraction
			GALOIS_API static void imul(BLOCK * data, size_t data_size, size_t bin_size, BLOCK ** state, size_t state_size, BLOCK characteristic, size_t high1, size_t high2);	// Multiplication
			GALOIS_API static ModResult* imod(BLOCK * value, size_t value_size, BLOCK * modulo, size_t modulo_size, BLOCK * cmp, size_t cmp_size, BLOCK characteristic, size_t bin_size);
			GALOIS_API static void ilsh(BLOCK * state, size_t state_size, size_t bin_size, BLOCK characteristic, size_t shiftc);			// Left shift

			// Data management. Don't look at these unless you want a headache
			GALOIS_API static BLOCK _mask(size_t bits, bool side);
			GALOIS_API static BLOCK get_value(size_t idx, size_t block_size, BLOCK * from);
			GALOIS_API static void set_value(size_t idx, BLOCK value, size_t block_size, BLOCK characteristic, BLOCK * to);
			GALOIS_API static size_t high_factor(BLOCK * state, size_t state_size, size_t bin_size, bool * noBits);
		};
	}
}
