#pragma once

#if defined(__MINGW32__) || defined(_WIN32)

#if defined(GALOIS_API)
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


namespace CryptoCPP {
	namespace Math {
		class Galois
		{
		public:
			GALOIS_API Galois(
				size_t characteristic,
				size_t exponent,
				size_t * irreducible,
				size_t irreducible_size,
				size_t * value
				);
			GALOIS_API ~Galois();

			// Addition
			GALOIS_API Galois * add(const Galois * value) const;

			// Subtraction
			GALOIS_API Galois * sub(const Galois * value) const;

			// Multiplication
			GALOIS_API Galois * mul(const Galois * value) const;

			// Inverse multiplication
			GALOIS_API Galois * inv() const;


		protected:
			static const size_t high_bit = 1 << ((sizeof(size_t)*8)-1);
			// GF parameters
			size_t characteristic, exponent, *irreducible, irreducible_size;
			// Effective storage params
			size_t binary_block_size, data_size;
			// Value of this GF object
			size_t * data;


			// Reduce the value of this galois to fit characteristic
			GALOIS_API void reduce();


			// Logic
			GALOIS_API static void iadd(size_t * data, size_t data_size, size_t bin_size, size_t * state, size_t state_size, size_t characteristic);	// Addition
			GALOIS_API static void isub(size_t * data, size_t data_size, size_t bin_size, size_t * state, size_t state_size, size_t characteristic);	// Subtraction
			GALOIS_API static void imul(size_t * data, size_t data_size, size_t bin_size, size_t * state, size_t state_size, size_t characteristic, size_t high1, size_t high2);	// Multiplication
			GALOIS_API static void iinv(size_t * state, size_t state_size);							// Multiplicative inverse
			GALOIS_API static void ilsh(size_t * state, size_t state_size, size_t bin_size, size_t characteristic, size_t shiftc);			// Left shift

			// Data management. Don't look at these unless you want a headache
			GALOIS_API static size_t _mask(size_t bits, bool side);
			GALOIS_API static size_t get_value(size_t idx, size_t block_size, size_t * from);
			GALOIS_API static void set_value(size_t idx, size_t value, size_t block_size, size_t characteristic, size_t * to);
			GALOIS_API static size_t high_factor(size_t * state, size_t state_size, size_t bin_size, bool * noBits);
		};
	}
}
