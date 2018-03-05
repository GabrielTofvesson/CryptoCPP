#pragma once

#if defined(__MINGW32__) || defined(_WIN32)

#if defined(AES_API)
#undef AES_API
#define AES_API __declspec(dllexport)
#else
#define AES_API __declspec(dllimport)
#endif

#endif

#ifndef AES_API
#if __GNUC__ >= 4
#define AES_API __attribute__ ((visibility ("default")))
#else
#define AES_API
#endif
#endif


namespace CryptoCPP { namespace Crypto {

	// MixColumns matrix basis
	static const char mix_matrix[4] = { 2, 3, 1, 1 };
	static const char unmix_matrix[4] = { 14, 11, 13, 9 };

	class AES {
	public:
		struct Digest {
			AES_API Digest(const char * message, const size_t size);
			AES_API Digest(const char * message);
			const char* message;
			const size_t size;
		};
		enum KeySize{ AES128, AES192, AES256 };

		AES_API AES(const char * key, KeySize bitsize = AES128);
		AES_API AES(const AES & copy);

		AES_API Digest * encrypt(Digest d);
		AES_API Digest * decrypt(Digest d);

		// Serialization
		struct Serialized {
			char* data;
			size_t data_size;
		};
		AES_API Serialized* serialize();
		AES_API static AES * deserialize(char* data);
		AES_API static AES * deserialize(Serialized* ser);

	protected:
		char* key;
		const KeySize size;
		
		AES_API AES(char * key, KeySize size);

		AES_API void encrypt(char* state);
		AES_API void decrypt(char* state);

		AES_API static unsigned int key_schedule_core(unsigned int input, size_t rcon_iteration);
		AES_API static char* expand_key(const char* key, KeySize size);
		AES_API static char rcon(unsigned int iteration);
		AES_API static char sbox(char c);

		AES_API static void add_round_key(char* state, char* roundKey);
		AES_API static void mix_columns(char* state);
		AES_API static void shift_rows(char* state);
		AES_API static void sub_bytes(char* state);
		AES_API static char affine(char value);

		AES_API static void inv_add_round_key(char* state, char* roundKey);
		AES_API static void inv_mix_columns(char* state);
		AES_API static void inv_shift_rows(char* state);
		AES_API static void inv_sub_bytes(char* state);
		AES_API static char inv_affine(char value);

		AES_API static void write_to_row(unsigned int value, char* to, size_t row);
		AES_API static unsigned int rotate(unsigned int i);
		AES_API static unsigned int get_row(char* from, size_t row);
		AES_API static char rot(char value, size_t by);
	};
}}