#pragma once
#include "Primes.h"
#include "BigInteger.h"


#if defined(__MINGW32__) || defined(_WIN32)

#if defined(RSA_API)
#undef RSA_API
#define RSA_API __declspec(dllexport)
#else
#define RSA_API __declspec(dllimport)
#endif

#endif

#ifndef RSA_API
#if __GNUC__ >= 4
#define RSA_API __attribute__ ((visibility ("default")))
#else
#define RSA_API
#endif
#endif


namespace CryptoCPP { namespace RSA {
	struct PublicKey
	{
		Math::BigInteger * mod;
		Math::BigInteger * exp;
	};
	typedef Math::BigInteger PrivateKey;
	struct KeyPair {
		PublicKey * pub;
		PrivateKey * priv;
	};
	struct CipherData {
		char* data;
		size_t size;
	};
	
	class RSA
	{
	public:
		RSA_API RSA(KeyPair* pair);
		RSA_API ~RSA();
		
		RSA_API CipherData* encrypt(CipherData* data);		// Encrypt with public key
		RSA_API CipherData* sign(CipherData* data);			// Encrypt with private key

		RSA_API CipherData* decrypt(CipherData* data);		// Decrypt with private key
		RSA_API CipherData* check_sign(CipherData* data);	// Decrypt with public key

		RSA_API bool can_decrypt();							// Checks whether or not we have a private key

		RSA_API CipherData* serialize_net();				// Serializes public key
		RSA_API CipherData* serialize_all();				// Complete serialization (public + private key). NOTE: Should NEVER be transmitted over an insecure channel. This should preferably be kept to the local file system

		RSA_API static RSA * deserialize(CipherData* ser);	// Deserializes a serialized RSA object. Autodetects whether or not a private key is available

	protected:
		KeyPair * keypair;

		RSA_API static CipherData* crypto_compute(CipherData* data, Math::BigInteger * exp, Math::BigInteger * mod); // Since the encryption/decryption is symmetric (operation-wise), the operation is generalized here
	};

	typedef char(*RandomProvider)();
	RSA_API KeyPair* generate_key_pair(RandomProvider provider, size_t approximate_byte_count, size_t byte_margin, size_t certainty);
}}