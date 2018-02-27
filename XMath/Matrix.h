#pragma once

#include <vector>

#if defined(__MINGW32__) || defined(_WIN32)

#if defined(MATRIX_API)
#define MATRIX_API __declspec(dllexport)
#else
#define MATRIX_API __declspec(dllimport)
#endif

#endif

#ifndef MATRIX_API
  #if __GNUC__ >= 4
    #define MATRIX_API __attribute__ ((visibility ("default")))
  #else
    #define MATRIX_API
  #endif
#endif


#define WITH ->operator()

namespace CryptoCPP {
	namespace Math {
		class Matrix;

		class Vector
		{
			friend class Matrix;
		public:
			MATRIX_API Vector(size_t count);
			MATRIX_API Vector(size_t count, long long * values);
			MATRIX_API ~Vector();

			MATRIX_API long long at(size_t index) const;
			MATRIX_API long long at(size_t index, long long newval);

		protected:
			const size_t count;
			long long * const valueSet;
		};


		class DelegatingFPTR;
		typedef const DelegatingFPTR(Matrix::*Delegate)(const Vector & input, size_t at);
		typedef const DelegatingFPTR(Matrix::*PDelegate)(const Vector * input, size_t at);

		class DelegatingFPTR {
		public:
			DelegatingFPTR(Delegate impl, PDelegate point, Matrix* context);

			MATRIX_API const DelegatingFPTR operator()(const Vector & input, size_t index) const;
			MATRIX_API const DelegatingFPTR operator()(Vector * input, size_t index) const;
		protected:
			Delegate impl;
			PDelegate point;
			Matrix* context;
		};

		class Matrix
		{
		public:
			MATRIX_API Matrix(size_t height, size_t width);
			MATRIX_API Matrix(const Matrix & copy);
			MATRIX_API ~Matrix();

			MATRIX_API const DelegatingFPTR set_row(const Vector & row, size_t rowidx);
			MATRIX_API const DelegatingFPTR set_col(const Vector & col, size_t colidx);
			MATRIX_API const DelegatingFPTR set_row(Vector * row, size_t rowidx);
			MATRIX_API const DelegatingFPTR set_col(Vector * col, size_t colidx);

			MATRIX_API const DelegatingFPTR set_row_r(const Vector & row, size_t rowidx);
			MATRIX_API const DelegatingFPTR set_col_r(const Vector & col, size_t colidx);
			MATRIX_API const DelegatingFPTR set_row_p(Vector * row, size_t rowidx);
			MATRIX_API const DelegatingFPTR set_col_p(Vector * col, size_t colidx);
			MATRIX_API long long set_at(size_t col, size_t row, long long value);
			MATRIX_API long long set_at(size_t index, bool rowMajor, long long value);

			MATRIX_API Vector* at_row(size_t index) const;
			MATRIX_API Vector* at_col(size_t index) const;
			MATRIX_API long long at(size_t col, size_t row) const;
			MATRIX_API long long at(size_t index, bool rowMajor) const;

			MATRIX_API Matrix* mul(const Matrix & factor) const;
			MATRIX_API Matrix* mul(long long scalar) const;

			MATRIX_API Matrix* operator*(const Matrix & factor) const;
			MATRIX_API Matrix* operator*(const Matrix * factor) const;
			MATRIX_API Matrix* operator*(long long scalar) const;

			MATRIX_API Matrix* get_minor(size_t row, size_t col) const;
			MATRIX_API long long det() const;

		protected:
			const DelegatingFPTR* ar = new DelegatingFPTR((Delegate)&Matrix::set_row_r, (PDelegate)&Matrix::set_row_p, this);
			const DelegatingFPTR* ac = new DelegatingFPTR((Delegate)&Matrix::set_col_r, (PDelegate)&Matrix::set_col_p, this);
			Vector * * const columns;
			const size_t height;
			const size_t width;
		};
	}
}
