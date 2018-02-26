#define MATRIX_API
#include "Matrix.h"

namespace CryptoCPP {
	namespace Math {

		MATRIX_API Vector::Vector(size_t count) : count(count), valueSet(new long long[count])
		{
			memset(valueSet, 0, count * sizeof(long long));
		}

		MATRIX_API Vector::Vector(size_t count, long long * values) : count(count), valueSet(values)
		{
		}

		MATRIX_API Vector::~Vector()
		{
			delete[] valueSet;
		}

		MATRIX_API long long Vector::at(size_t index) const
		{
			if (index < 0 || index >= count) throw new std::exception("Index out of bounds");
			return valueSet[index];
		}

		MATRIX_API long long Vector::at(size_t index, long long newval)
		{
			if (index < 0 || index >= count) throw new std::exception("Index out of bounds");
			long long l = valueSet[index];
			valueSet[index] = newval;
			return l;
		}


		DelegatingFPTR::DelegatingFPTR(Delegate impl, PDelegate point, Matrix* context) 
		{
			this->impl = impl;
			this->point = point;
			this->context = context;
		}

		MATRIX_API const DelegatingFPTR* DelegatingFPTR::operator()(const Vector & input, size_t index) const
		{
			return (context->*impl)(input, index);
		}

		MATRIX_API const DelegatingFPTR* DelegatingFPTR::operator()(Vector * input, size_t index) const
		{
			return (context->*point)(input, index);
		}

		Matrix::Matrix(size_t height, size_t width) : columns(new Vector*[width]), width(width), height(height)
		{
			for (size_t t = 0; t < width; ++t)
				columns[t] = new Vector(height);
		}

		Matrix::Matrix(const Matrix & copy) : columns(new Vector*[copy.width]), width(copy.width), height(copy.height)
		{
			for (size_t t = 0; t < width; ++t)
				for (size_t t1 = 0; t < height; ++t1)
					columns[t]->at(t1, copy.columns[t]->at(t1));
		}

		MATRIX_API Matrix::~Matrix() {
			delete ar;
			delete ac;

			for (size_t t = 0; t < width; ++t)
				delete columns[t];
			delete[] columns;
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_row(const Vector & row, size_t rowidx)
		{
			return set_row_r(row, rowidx);
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_col(const Vector & col, size_t colidx)
		{
			return set_col_r(col, colidx);
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_row(Vector * row, size_t rowidx)
		{
			return set_row_p(row, rowidx);
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_col(Vector * col, size_t colidx)
		{
			return set_col_p(col, colidx);
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_row_r(const Vector & row, size_t rowidx)
		{
			if (rowidx >= height) throw new std::exception("Row index out of bounds");
			size_t min = row.count < width ? row.count : width;
			for (size_t t = 0; t < min; ++t) columns[t]->at(rowidx, row.at(t));
			return ar;
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_col_r(const Vector & col, size_t colidx)
		{
			if (colidx >= width) throw new std::exception("Column index out of bounds");
			size_t min = col.count < height ? col.count : height;
			for (size_t t = 0; t < height; ++t) columns[colidx]->at(t, col.at(t));
			return ac;
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_row_p(Vector * row, size_t rowidx)
		{
			const DelegatingFPTR * chain = set_row((const Vector&) *row, rowidx);
			delete row;
			return chain;
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_col_p(Vector * col, size_t colidx)
		{
			const DelegatingFPTR * chain = set_col((const Vector&) *col, colidx);
			delete col;
			return chain;
		}

		MATRIX_API long long Matrix::set_at(size_t col, size_t row, long long value)
		{
			if (col < 0 || col >= width || row < 0 || row >= height) throw new std::exception("Index out of bounds");
			return columns[col]->at(row, value);
		}

		MATRIX_API long long Matrix::set_at(size_t index, bool rowMajor, long long value)
		{
			return set_at(rowMajor ? index % width : index / height, rowMajor ? index / width : index % height, value);
		}

		MATRIX_API Vector * Matrix::at_row(size_t index) const
		{
			if (index < 0 || index >= height) throw new std::exception("Index out of bounds");
			Vector * collect = new Vector(width);
			for (size_t t = 0; t < width; ++t)
				collect->at(t, columns[t]->at(index));
			return collect;
		}

		MATRIX_API Vector * Matrix::at_col(size_t index) const
		{
			if (index < 0 || index >= width) throw new std::exception("Index out of bounds");
			Vector * collect = new Vector(height);
			for (size_t t = 0; t < height; ++t)
				collect->at(t, columns[index]->at(t));
			return collect;
		}

		MATRIX_API long long Matrix::at(size_t col, size_t row) const
		{
			if (col < 0 || col >= width || row < 0 || row >= height) throw new std::exception("Index out of bounds");
			return columns[col]->at(row);
		}

		MATRIX_API long long Matrix::at(size_t index, bool rowMajor) const
		{
			return at(rowMajor ? index % width : index / height, rowMajor ? index / width : index % height);
		}

		MATRIX_API Matrix * Matrix::mul(const Matrix & factor) const
		{
			if (factor.height != width) throw new std::exception("Mismatched dimensions");
			Matrix* result = new Matrix(height, factor.width);
			for (size_t i = 0; i < factor.width; ++i)
				for (size_t j = 0; j < height; ++j)
					for (size_t k = 0; k < width; ++k)
						result->columns[i]->at(j, result->columns[i]->at(j) + (factor.columns[i]->at(k) * columns[k]->at(j)));
			return result;
		}

		MATRIX_API Matrix * Matrix::mul(long long scalar) const
		{
			Matrix* m = new Matrix(*this);
			for (size_t t = 0; t < width; ++t)
				for (size_t t1 = 0; t < height; ++t1)
					m->columns[t]->at(t1, m->columns[t]->at(t1) * scalar);
			return m;
		}

		MATRIX_API Matrix* Matrix::operator*(const Matrix & factor) const
		{
			return mul(factor);
		}

		MATRIX_API Matrix* Matrix::operator*(const Matrix * factor) const
		{
			return mul(*factor);
		}

		MATRIX_API Matrix* Matrix::operator*(long long scalar) const
		{
			return mul(scalar);
		}

		MATRIX_API Matrix * Matrix::minor(size_t row, size_t col) const
		{
			if (height == 0 || width == 0) return new Matrix(0, 0);
			if (row >= height || col >= width) throw new std::exception("Index out of bounds");
			Matrix* result = new Matrix(height - 1, width - 1);
			for (size_t i = 0; i < width; ++i) {
				if (i == col) continue;
				for (size_t j = 0; j < height; ++j) {
					if (j == row) continue;
					result->columns[i + ((i > col) ? -1 : 0)]->at(j + ((j > row) ? -1 : 0), columns[i]->at(j));
				}
			}
			return result;
		}

		MATRIX_API long long Matrix::det() const
		{
			// Matrix safety checks
			if (height != width) throw new std::exception("Matrix must be square to compute the determinant");
			if (!height) throw new std::exception("Zero-matrix does not have a determinant");

			// Compute determinant for 1x1 matrix
			if (height == 1) return columns[0]->at(0);

			// Compute determinant for higher-order matrices
			long long result = 0;
			for (size_t t = 0; t < width; ++t) {
				Matrix * smaller = minor(0, t);													// Compute minor
				result += smaller->det() * columns[t]->at(0) * (long long)((t % 2) ? -1 : 1);	// Compute partial determinant for the given minor
				delete smaller;																	// Delete allocated minor
			}
			return result;
		}

	}
}