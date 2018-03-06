#define RSA_API
#include "RSA.h"
#include "Primes.h"
#include <thread>

namespace CryptoCPP { namespace RSA {
	RSA_API RSA::RSA(KeyPair* keypair)
	{
		this->keypair = keypair;
	}

	RSA_API RSA::~RSA()
	{
		delete keypair->priv;
		delete keypair->pub->exp;
		delete keypair->pub->mod;
		delete keypair->pub;
		delete keypair;
	}



	RSA_API CipherData* RSA::encrypt(CipherData* data)
	{
		return crypto_compute(data, keypair->pub->exp, keypair->pub->mod);
	}

	RSA_API CipherData* RSA::sign(CipherData* data)
	{
		if (!can_decrypt()) throw new std::exception();
		return crypto_compute(data, keypair->priv, keypair->pub->mod);
	}


	RSA_API CipherData* RSA::decrypt(CipherData* data)
	{
		if (!can_decrypt()) throw new std::exception();
		return crypto_compute(data, keypair->priv, keypair->pub->mod);
	}

	RSA_API CipherData* RSA::check_sign(CipherData* data)
	{
		return crypto_compute(data, keypair->pub->exp, keypair->pub->mod);
	}

	RSA_API bool RSA::can_decrypt()
	{
		return keypair->priv != 0;
	}


	RSA_API CipherData* RSA::serialize_net()
	{
		unsigned int pk_size, mod_size;
		char * pk = keypair->pub->exp->to_array(&pk_size);
		char * mod = keypair->pub->mod->to_array(&mod_size);
		char* ser = new char[1 + (2 * 4) + pk_size + mod_size];
		ser[0] = 0; // Identifier: Shows that this is a public key packet
		memcpy(ser + 1, &pk_size, 4);
		memcpy(ser + 5, &mod_size, 4);
		memcpy(ser + 9, pk, pk_size);
		memcpy(ser + 9 + pk_size, mod, mod_size);
		delete[] mod;
		delete[] pk;
		CipherData* data = new CipherData();
		data->data = ser;
		data->size = 1 + (2 * 4) + pk_size + mod_size;
		return data;
	}

	RSA_API CipherData* RSA::serialize_all()
	{
		unsigned int pk_size, mod_size, priv_size;
		char * pk = keypair->pub->exp->to_array(&pk_size);
		char * mod = keypair->pub->mod->to_array(&mod_size);
		char * priv = keypair->priv->to_array(&priv_size);
		char* ser = new char[1 + (2 * 4) + pk_size + mod_size + priv_size];
		ser[0] = 1; // Identifier: Shows that this is a private key packet
		memcpy(ser + 1, &pk_size, 4);
		memcpy(ser + 1 + 4, &mod_size, 4);
		memcpy(ser + 1 + (2 * 4), &priv_size, 4);
		memcpy(ser + 1 + (3 * 4), pk, pk_size);
		memcpy(ser + 1 + (3 * 4) + pk_size, mod, mod_size);
		memcpy(ser + 1 + (3 * 4) + pk_size + mod_size, priv, priv_size);
		delete[] priv;
		delete[] mod;
		delete[] pk;
		CipherData* data = new CipherData();
		data->data = ser;
		data->size = 1 + (2 * 4) + pk_size + mod_size + priv_size;
		return data;
	}

	RSA_API RSA * RSA::deserialize(CipherData* data)
	{
		bool isprivate = data->data[0];
		size_t pk_size, mod_size, priv_size = 0;
		pk_size = *(unsigned int*)(data->data + 1);
		mod_size = *(unsigned int*)(data->data + 1 + 4);
		if(isprivate) priv_size = *(unsigned int*)(data->data + 1 + (2 * 4));
		if (
			pk_size >= data->size ||
			mod_size >= data->size ||
			priv_size >= data->size ||
			pk_size + mod_size >= data->size ||
			pk_size + priv_size >= data->size ||
			pk_size + mod_size + priv_size >= data->size ||
			mod_size + pk_size >= data->size
			)
			throw new std::exception(); // Index out of bounds

		char * pk = new char[pk_size];
		char * mod = new char[mod_size];
		char * priv = isprivate ? new char[priv_size] : 0;
		memcpy(pk, data->data + 1 + (3 * 4), pk_size);
		memcpy(mod, data->data + 1 + (3 * 4) + pk_size, mod_size);
		if (isprivate) memcpy(priv, data->data + 1 + (3 * 4) + pk_size + mod_size, priv_size);

		KeyPair* pair = new KeyPair();
		pair->priv = isprivate ? new Math::BigInteger(priv, priv_size) : 0;
		pair->pub = new PublicKey();
		pair->pub->mod = new Math::BigInteger(mod, mod_size);
		pair->pub->exp = new Math::BigInteger(pk, pk_size);
		if (isprivate) delete[] priv;
		delete[] mod;
		delete[] pk;
		return new RSA(pair);
	}


	RSA_API CipherData* RSA::crypto_compute(CipherData* data, Math::BigInteger * exp, Math::BigInteger * mod)
	{
		CipherData* out = new CipherData();
		char* c = new char[data->size + 1];
		c[data->size] = 0;
		memcpy(c, data->data, data->size);
		Math::BigInteger base = Math::BigInteger(c, data->size + 1);
		Math::BigInteger * encrypted = Math::BigInteger::mod_pow(&base, exp, mod);
		out->data = encrypted->to_array(&out->size);
		delete encrypted;
		return out;
	}


	RSA_API KeyPair* generate_key_pair(RandomProvider provider, size_t approximate_byte_count, size_t byte_margin, size_t certainty)
	{
		bool cancellation = false;
		char* c = new char[sizeof(size_t)];
		for (size_t t = sizeof(size_t); t > 0; --t) c[t] = provider();
		size_t margin = *(size_t*)c;
		margin %= byte_margin;
		Math::BigInteger * p = Primes::generate_prime(provider, provider() > 128 ? (approximate_byte_count + margin) : (approximate_byte_count - margin), certainty, Primes::miller_rabin_prime_test, cancellation);
		for (size_t t = sizeof(size_t); t > 0; --t) c[t] = provider();
		size_t margin = *(size_t*)c;
		margin %= byte_margin;
		Math::BigInteger * q = Primes::generate_prime(provider, provider() > 128 ? (approximate_byte_count + margin) : (approximate_byte_count - margin), certainty, Primes::miller_rabin_prime_test, cancellation);
		delete[] c;

		// Compute n
		Math::BigInteger * n = *p * *q;

		// Compute totient n
		Math::BigInteger * tmp1 = *p - 1;
		Math::BigInteger * tmp2 = *q - 1;

		Math::BigInteger * gcd = Math::BigInteger::gcd(tmp1, tmp2);
		Math::BigInteger * mul = *tmp1 * *tmp2;
		delete tmp1;
		delete tmp2;
		Math::BigInteger * m = *mul / *gcd; // Totient n
		delete gcd;
		delete mul;

		bool nonzero;
		bool zeroes;
		char * gen = 0;
		size_t gen_size;
		char last = m->highest_nonzero();
		size_t idx = m->highest_nonzero_index();
		do {
			if (gen != 0) delete[] gen;
			nonzero = false;
			gen = Primes::generate_bounded_integer(provider, 0, last, idx, &gen_size, &zeroes);
			for (size_t t = 1; t < gen_size; ++t)
				if (nonzero = gen[t])
					break;
		} while (zeroes || (!nonzero && gen[0]==1));

		Math::BigInteger * e = new Math::BigInteger(gen, gen_size);
		delete[] gen;
		Math::BigInteger * inverse = Math::BigInteger::mul_inv(*e, *n);
		delete m;

		PublicKey * pk = new PublicKey();
		pk->exp = e;
		pk->mod = n;
		
		KeyPair * kp = new KeyPair();
		kp->priv = inverse;
		kp->pub = pk;

		return kp;
	}
}}