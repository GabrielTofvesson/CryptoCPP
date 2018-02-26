#pragma once

#include <vector>

#ifdef MATRIX_API
#define MATRIX_API __declspec(dllexport)   
#else  
#define MATRIX_API __declspec(dllimport)   
#endif


namespace CryptoCPP {
	namespace Math {
		class Matrix
		{
		public:
			MATRIX_API typedef void(*ChainFunction) (Vector & valueSet);

			MATRIX_API Matrix(size_t height, size_t width);
			MATRIX_API Matrix(Matrix & copy);

			MATRIX_API ChainFunction set_row(const Vector & row);
			MATRIX_API ChainFunction set_col(const Vector & col);

			MATRIX_API Vector* at_row(size_t index) const;
			MATRIX_API Vector* at_col(size_t index) const;
			MATRIX_API long long at(size_t row, size_t col) const;

			MATRIX_API Matrix* mul(const Matrix & factor) const;
			MATRIX_API Matrix* mul(long long scalar) const;

			MATRIX_API Matrix* minor(size_t row, size_t col) const;
			MATRIX_API long long det() const;

		protected:
			Vector * columns;
			size_t height;
			size_t width;

		};

		class Vector
		{
		public:
			MATRIX_API Vector(size_t count);
			MATRIX_API Vector(size_t count, long long * values);

			MATRIX_API long long at(size_t index) const;
			MATRIX_API long long at(size_t index, long long newval);

		protected:
			long double * valueSet;
			size_t count;
		};
	}
}