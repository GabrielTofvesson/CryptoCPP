#pragma once

#if defined(__MINGW32__) || defined(_WIN32)

#if defined(SHA1_API)
#undef SHA1_API
#define SHA1_API __declspec(dllexport)
#else
#define SHA1_API __declspec(dllimport)
#endif

#endif

#ifndef SHA1_API
#if __GNUC__ >= 4
#define SHA1_API __attribute__ ((visibility ("default")))
#else
#define SHA1_API
#endif
#endif

namespace CryptoCPP {
	namespace SHA {
		SHA1_API char * digest(char * data, size_t data_size);
		SHA1_API unsigned int rot(unsigned int val, size_t by);
		SHA1_API unsigned int func(unsigned int t, unsigned int b, unsigned int c, unsigned int d);
		SHA1_API unsigned int K(unsigned int t);
		SHA1_API void write_reverse_endian(char* to, unsigned int value, size_t offset);
	}
}