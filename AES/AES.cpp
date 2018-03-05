#define AES_API
#include "AES.h"
#include "Galois.h"
#include <string>

namespace CryptoCPP { namespace Crypto {
	AES_API AES::Digest::Digest(const char * message, const size_t size) : message(message), size(size)
	{ }
	AES_API AES::Digest::Digest(const char * message) : message(message), size(strlen(message))
	{ }

	AES_API AES::AES(const char * key, KeySize bitsize) : size(bitsize)
	{
		this->key = expand_key(key, bitsize);
	}

	AES_API AES::AES(const AES & copy) : size(copy.size)
	{
		this->key = new char[size == AES128 ? 16 : size == AES192 ? 24 : 32];
		memcpy(key, copy.key, size == AES128 ? 16 : size == AES192 ? 24 : 32);
	}

	AES_API AES::AES(char * key, KeySize size) : size(size)
	{
		this->key = key;
	}

	AES_API AES::Digest * AES::encrypt(Digest d)
	{
		// Allocate states
		size_t state_count = (d.size / 16) + (d.size % 16 == 0 ? 0 : 1);
		char * states = new char[state_count * 16];

		// Set up state properly
		memset(states + d.size, 0, (state_count * 16) - d.size);
		memcpy(states, d.message, d.size);

		// Encrypt states
		for (size_t t = 0; t < state_count; ++t) encrypt(states + (t * 16));
		
		return new Digest(states, state_count * 16);
	}

	AES_API AES::Digest * AES::decrypt(Digest d)
	{
		// Allocate states
		size_t state_count = d.size / 16;
		char * states = new char[state_count * 16];

		// Set up state properly
		memcpy(states, d.message, d.size);

		// Encrypt states
		for (size_t t = 0; t < state_count; ++t) decrypt(states + (t * 16));

		return new Digest(states, d.size);
	}

	AES_API AES::Serialized* AES::serialize()
	{
		AES::Serialized * s = new AES::Serialized();
		s->data_size = size == AES128 ? 16 : size == AES192 ? 24 : 32;
		s->data = new char[1 + s->data_size];
		memcpy(s->data + 1, key, s->data_size);
		s->data[0] = size == AES128 ? 0 : size == AES192 ? 1 : 2;
		return s;
	}

	AES_API AES * AES::deserialize(char* data)
	{
		KeySize size = data[0] == 0 ? AES128 : data[0] == 1 ? AES192 : AES256;
		char * key = new char[size == AES128 ? 16 : size == AES192 ? 24 : 32];
		return new AES(key, size);
	}

	AES_API AES * AES::deserialize(AES::Serialized* ser)
	{
		return deserialize(ser->data);
	}


	AES_API void AES::encrypt(char* state)
	{
		size_t last_round = size == AES128 ? 9 : size == AES192 ? 11 : 13;

		// Initial round. Just just xor the key for this round input the input
		add_round_key(state, key);

		// Rounds 1 - last
		for (size_t rounds = 1; rounds < 10; ++rounds)
		{
			sub_bytes(state);
			shift_rows(state);
			if (rounds != last_round) mix_columns(state);
			add_round_key(state, key + (rounds * 16));
		}
	}

	AES_API void AES::decrypt(char* state)
	{
		size_t last_round = size == AES128 ? 9 : size == AES192 ? 11 : 13;

		for (int rounds = last_round; rounds > 0; --rounds)
		{
			inv_add_round_key(state, key + (rounds * 16));
			if (rounds != last_round) inv_mix_columns(state);
			inv_shift_rows(state);
			inv_sub_bytes(state);
		}

		inv_add_round_key(state, key);
	}

	// Key expansion
	AES_API unsigned int AES::key_schedule_core(unsigned int input, size_t rcon_iteration)
	{
		char * convert = (char*)&input;
		for (size_t t = 0; t < 4; ++t)
			convert[t] = sbox(convert[t]);
		convert[3] ^= rcon(rcon_iteration);
		return input;
	}

	AES_API char* AES::expand_key(const char* key, KeySize size)
	{
		size_t n = size == AES128 ? 16 : size == AES192 ? 24 : 32;
		size_t b = size == AES128 ? 176 : size == AES192 ? 208 : 240;

		char* output = new char[b];
		//memset(output + n, 0, b - n);
		memcpy(output, key, n);

		size_t rcon_iter = 1;

		size_t accruedBytes = n;
		while (accruedBytes < b)
		{
			// Generate 4 new bytes of extended key
			unsigned int _t = key_schedule_core(*(unsigned int *)(output + accruedBytes - 4), rcon_iter);
			char * t = (char*)&_t;
			++rcon_iter;
			for (size_t i = 0; i < 4; ++i) t[i] ^= output[accruedBytes - n + i];
			memcpy(output + accruedBytes, t, 4);
			accruedBytes += 4;

			// Generate 12 new bytes of extended key
			for (int i = 0; i < 3; ++i)
			{
				memcpy(t, output + accruedBytes - 4, 4);
				for (size_t j = 0; j < 4; ++j) t[j] ^= output[accruedBytes - n + j];
				memcpy(output + accruedBytes, t, 4);
				accruedBytes += 4;
			}

			// Special processing for 256-bit key schedule
			if (size == AES256)
			{
				memcpy(t, output + accruedBytes - 4, 4);
				for (size_t j = 0; j < 4; ++j) t[j] = (sbox(t[j]) ^ output[accruedBytes - n + j]);
				memcpy(output + accruedBytes, t, 4);
				accruedBytes += 4;
			}
			// Special processing for 192-bit key schedule
			if (size != AES128)
				for (int i = size == AES192 ? 1 : 2; i >= 0; --i)
				{
					memcpy(t, output + accruedBytes - 4, 4);
					for (int j = 0; j < 4; ++j) t[j] ^= output[accruedBytes - n + j];
					memcpy(output + accruedBytes, t, 4);
					accruedBytes += 4;
				}
		}

		return output;
	}

	AES_API char AES::rcon(unsigned int iteration)
	{
		return iteration == 0 ? 0x8d : Math::Galois(2, 283, iteration).get_lowest() & 255;
	}

	AES_API char AES::sbox(char c)
	{
		Math::Galois * inv = Math::Galois(2, 283, c).inv();
		c = affine(inv->get_lowest() & 255);
		delete inv;
		return c;
	}

	// Encryption functions
	AES_API void AES::add_round_key(char* state, char* roundKey)
	{
		for (size_t t = 0; t < 16; ++t) state[t] ^= roundKey[t];
	}

	AES_API void AES::mix_columns(char* state)
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				for (int k = 0; k < 4; ++k)
				{
					Math::Galois g = Math::Galois(2, 283, state[k + i * 4]);
					Math::Galois mix = Math::Galois(2, 283, mix_matrix[(k + 4 - j) % 4]);
					Math::Galois * result = g.mul(&mix);
					state[j + i * 4] ^= result->get_lowest() & 255;
					delete result;
				}
	}

	AES_API void AES::shift_rows(char* state)
	{
		for (size_t i = 1; i < 4; ++i)
		{
			unsigned int value = get_row(state, i);
			for (size_t j = 0; j < i; ++j) value = rotate(value);
			write_to_row(value, state, i);
		}
	}

	AES_API void AES::sub_bytes(char* state)
	{
		for (size_t t = 0; t < 16; ++t)
			state[t] = sbox(state[t]);
	}

	AES_API char AES::affine(char value)
	{
		return (value ^ rot(value, 1) ^ rot(value, 2) ^ rot(value, 3) ^ rot(value, 4) ^ 0b01100011);
	}


	// Inverse encryption functions
	AES_API void AES::inv_add_round_key(char* state, char* roundKey)
	{
		add_round_key(state, roundKey);
	}

	AES_API void AES::inv_mix_columns(char* state)
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				for (int k = 0; k < 4; ++k)
				{
					Math::Galois g = Math::Galois(2, 283, state[k + i * 4]);
					Math::Galois mix = Math::Galois(2, 283, unmix_matrix[(k + 4 - j) % 4]);
					Math::Galois * result = g.mul(&mix);
					state[j + i * 4] ^= result->get_lowest() & 255;
					delete result;
				}
	}

	AES_API void AES::inv_shift_rows(char* state)
	{
		for (size_t i = 1; i < 4; ++i)
		{
			unsigned int value = get_row(state, i);
			for (size_t j = 3; j >= i; --j) value = rotate(value);
			write_to_row(value, state, i);
		}
	}

	AES_API void AES::inv_sub_bytes(char* state)
	{
		for (size_t t = 0; t < 16; ++t)
		{
			Math::Galois * inv = Math::Galois(2, 283, inv_affine(state[t])).inv();
			state[t] = inv->get_lowest() & 255;
			delete inv;
		}
	}

	AES_API char AES::inv_affine(char value)
	{
		return rot(value, 1) ^ rot(value, 3) ^ rot(value, 6) ^ 0b00000101;
	}


	// Helper functions
	AES_API void AES::write_to_row(unsigned int value, char* to, size_t row)
	{
		to[row] = value & 255;
		to[row + 4] = (value >> 8) & 255;
		to[row + 8] = (value >> 16) & 255;
		to[row + 12] = (value >> 24) & 255;
	}

	AES_API unsigned int AES::rotate(unsigned int i)
	{
		return (i >> 24) | (i << 8);
	}

	AES_API unsigned int AES::get_row(char* from, size_t row)
	{
		return (from[row] | (from[row + 4] << 8) | (from[row + 8] << 16) | (from[row + 12] << 24));
	}

	AES_API char AES::rot(char value, size_t by)
	{
		return (value << by) | (value >> (8 - by));
	}
}}