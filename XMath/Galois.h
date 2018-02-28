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
			GALOIS_API Galois * inv(const Galois * value) const;


		protected:
			static const size_t high_bit = 1 << ((sizeof(size_t)*8)-1);
			// GF parameters
			const size_t characteristic, exponent, irreducible;
			// Effective storage params
			const size_t binary_block_size, data_size;
			// Value of this GF object
			const size_t * data;


			// Reduce the value of this galois to fit characteristic
			GALOIS_API void reduce();

			// Self-mutable operations
			GALOIS_API void iadd(const Galois * value);
			GALOIS_API void isub(const Galois * value);
			GALOIS_API void imul(const Galois * value);
			GALOIS_API void iinv(const Galois * value);

			GALOIS_API size_t _mask(size_t bits, bool side) const;
			GALOIS_API size_t get_value(size_t idx) const;
			GALOIS_API void set_value(size_t idx, size_t value);
		};
	}
}
