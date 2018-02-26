#pragma once

#include <vector>

#ifdef MATRIX_API
#define MATRIX_API __declspec(dllexport)   
#else  
#define MATRIX_API __declspec(dllimport)   
#endif


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
		typedef const DelegatingFPTR*(Matrix::*Delegate)(const Vector & input, size_t at);

		class DelegatingFPTR {
		public:

			DelegatingFPTR(Delegate impl, Matrix* context);

			MATRIX_API const DelegatingFPTR* operator()(const Vector & input, size_t index);
		protected:
			Delegate impl;
			Matrix* context;
		};

		class Matrix
		{
			friend class Vector;
			friend class DelegatingFPTR;
		public:
			MATRIX_API Matrix(size_t height, size_t width);
			MATRIX_API Matrix(const Matrix & copy);
			MATRIX_API ~Matrix();

			MATRIX_API const DelegatingFPTR* set_row(const Vector & row, size_t rowidx);
			MATRIX_API const DelegatingFPTR* set_col(const Vector & col, size_t colidx);
			MATRIX_API long long set_at(size_t col, size_t row, long long value);
			MATRIX_API long long set_at(size_t index, bool rowMajor, long long value);

			MATRIX_API Vector* at_row(size_t index) const;
			MATRIX_API Vector* at_col(size_t index) const;
			MATRIX_API long long at(size_t col, size_t row) const;
			MATRIX_API long long at(size_t index, bool rowMajor) const;

			MATRIX_API Matrix* mul(const Matrix & factor) const;
			MATRIX_API Matrix* mul(long long scalar) const;

			MATRIX_API Matrix* minor(size_t row, size_t col) const;
			MATRIX_API long long det() const;

		protected:
			const DelegatingFPTR* ar = new DelegatingFPTR(add_row, this);
			const DelegatingFPTR* ac = new DelegatingFPTR(add_col, this);
			Vector * * const columns;
			const size_t height;
			const size_t width;
		};
	}
}