#pragma once

namespace CryptoCPP {
	namespace Math {
		class Galois
		{
		public:
			Galois(size_t characteristic, size_t exponent, size_t irreducible);

			Galois * add(const Galois * value) const; // Add
			Galois * sub(const Galois * value) const; // Subtract
			Galois * mul(const Galois * value) const; // Multiply
			Galois * inv(const Galois * value) const; // Inverse multiply


		protected:
			size_t characteristic, exponent, irreducible;

			// Reduce the value of this galois to one that fits the field parameters
			void reduce();

			// Self-mutable operations
			void iadd(const Galois * value);
			void isub(const Galois * value);
			void imul(const Galois * value);
			void iinv(const Galois * value);
		};
	}
}