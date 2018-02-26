#define MATRIX_API
#include "Matrix.h"

namespace CryptoCPP {
	namespace Math {

		MATRIX_API Vector::Vector(size_t count) : count(count), valueSet(new long long[count])
		{
			memset(valueSet, 0, count * sizeof(long long));
		}

		MATRIX_API Vector::Vector(size_t count, long long * values) : count(count), valueSet(new long long[count])
		{
			memcpy(valueSet, values, count * sizeof(long long));
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


		DelegatingFPTR::DelegatingFPTR(Delegate impl, Matrix* context) 
		{
			this->impl = impl;
			this->context = context;
		}

		MATRIX_API const DelegatingFPTR* DelegatingFPTR::operator()(const Vector & input, size_t index)
		{
			return (context->*impl)(input, index);
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
			// TODO: Add other deletions
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_row(const Vector & row, size_t rowidx)
		{
			if (rowidx < 0 || rowidx >= height) throw new std::exception("Index out of bounds");
			size_t min = row.count < width ? row.count : width;
			for (size_t t = 0; t < min; ++t) columns[t]->at(rowidx, row.at(t));
			return ar;
		}

		MATRIX_API const DelegatingFPTR* Matrix::set_col(const Vector & col, size_t colidx)
		{
			return ac;
		}

		MATRIX_API long long Matrix::set_at(size_t col, size_t row, long long value)
		{
			if (col < 0 || col >= width || row < 0 || row >= height) throw new std::exception("Index out of bounds");
			columns[col]->at(row, value);
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
			for (int i = 0; i < factor.width; ++i)
				for (int j = 0; j < height; ++j) 
					for (int k = 0; k < width; ++k) 
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

		MATRIX_API Matrix * Matrix::minor(size_t row, size_t col) const
		{
			Matrix* result = new Matrix(height - 1, width - 1);
			for (int i = 0; i < width; ++i) {
				if (i == col) continue;
				for (int j = 0; j < height; ++j) {
					if (j == row) continue;
					result->columns[i + ((i > col) ? -1 : 0)]->at(j + ((j > row) ? -1 : 0), columns[i]->at(j));
				}
			}
			return result;
		}

		MATRIX_API long long Matrix::det() const
		{
			return 0;
		}

	}
}