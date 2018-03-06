#define SHA1_API
#include "SHA1.h"
#include <string>

namespace CryptoCPP {
	namespace SHA1 {
		SHA1_API char * digest(char * data, size_t data_size)
		{
			// Initialize buffers
			unsigned int h0 = 0x67452301;
			unsigned int h1 = 0xEFCDAB89;
			unsigned int h2 = 0x98BADCFE;
			unsigned int h3 = 0x10325476;
			unsigned int h4 = 0xC3D2E1F0;

			// Pad message
			size_t ml = data_size + 1;
			size_t full_size = ml + ((960 - (ml * 8 % 512)) % 512) / 8 + 8;
			char * msg = new char[full_size];
			memcpy(msg, data, data_size);
			memset(msg + data_size, 0, full_size - data_size);
			msg[data_size] = 0x80;
			size_t len = data_size * 8;
			for (int i = 0; i < 8; ++i) msg[full_size - 1 - i] = (len >> (i * 8)) & 255;

			unsigned int chunks = full_size / 64;

			// Perform hashing for each 512-bit block
			for (size_t i = 0; i<chunks; ++i)
			{

				// Split block into words
				unsigned int * w = new unsigned int[80];
				for (unsigned int j = 0; j<16; ++j)
					w[j] |= ((msg[i * 64 + j * 4] << 24) | (msg[i * 64 + j * 4 + 1] << 16) | (msg[i * 64 + j * 4 + 2] << 8) | (msg[i * 64 + j * 4 + 3] << 0));

				// Expand words
				for (size_t j = 16; j<80; ++j)
					w[j] = rot(w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16], 1);

				// Initialize chunk-hash
				unsigned int
					a = h0,
					b = h1,
					c = h2,
					d = h3,
					e = h4;

				// Do hash rounds
				for (size_t t = 0; t<80; ++t)
				{
					unsigned int tmp = rot(a, 5) + func(t, b, c, d) + e + K(t) + w[t];
					e = d;
					d = c;
					c = rot(b, 30);
					b = a;
					a = tmp;
				}
				h0 += a;
				h1 += b;
				h2 += c;
				h3 += d;
				h4 += e;
				delete[] w;
			}

			delete[] msg;

			char * result = new char[20];
			write_reverse_endian(result, h0, 0);
			write_reverse_endian(result, h1, 4);
			write_reverse_endian(result, h2, 8);
			write_reverse_endian(result, h3, 12);
			write_reverse_endian(result, h4, 16);

			return result;
		}


		SHA1_API unsigned int func(unsigned int t, unsigned int b, unsigned int c, unsigned int d)
		{
			return
				t < 20 ? (b & c) | ((~b) & d) :
				t < 40 ? b ^ c ^ d :
				t < 60 ? (b & c) | (b & d) | (c & d) :
				/*t<80*/ b ^ c ^ d;
		}

		SHA1_API unsigned int K(unsigned int t)
		{
			return
				t < 20 ? 0x5A827999 :
				t < 40 ? 0x6ED9EBA1 :
				t < 60 ? 0x8F1BBCDC :
				/*t<80*/ 0xCA62C1D6;
		}

		SHA1_API unsigned int rot(unsigned int value, size_t by)
		{
			return (value << by) | (value >> (32 - by));
		}
		SHA1_API void write_reverse_endian(char* to, unsigned int value, size_t offset)
		{
			for (size_t t = 0; t < 4; ++t) to[offset + 4 - t] = (value >> (t * 8)) & 255;
		}
	}
}