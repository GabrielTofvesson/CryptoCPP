#include <iostream>
#include "BigInteger.h"
#include "Matrix.h"
#include "Galois.h"
#include "AES.h"

using namespace CryptoCPP::Math;

int main()
{
	// Create a 2x2 matrix
	// |1 2|
	// |3 4|
	Matrix * m = new Matrix(2, 2);
	m->set_row
	(new Vector(2, new long long[2]{ 1, 2 }), 0)
	(new Vector(2, new long long[2]{ 3, 4 }), 1);

	// Create a 2x2 matrix
	// |5 6|
	// |7 8|
	Matrix * m1 = new Matrix(2, 2);
	m1->set_row
	(new Vector(2, new long long[2]{ 5, 6 }), 0)
	(new Vector(2, new long long[2]{ 7, 8 }), 1);

	// Multiply matrices
	Matrix * res = (*m) * m1;


	std::cout << "Matrix 'm':" << std::endl;
	for (size_t t = 0; t < 4; ++t) std::cout << m->at(t, true) << ((t % 2) ? '\n' : ' ');

	std::cout << "\nMatrix 'm1':" << std::endl;
	for (size_t t = 0; t < 4; ++t) std::cout << m1->at(t, true) << ((t % 2) ? '\n' : ' ');

	// Print result
	std::cout << "\nMatrix 'res':" << std::endl;
	for (size_t t = 0; t < 4; ++t) std::cout << res->at(t, true) << ((t%2) ? '\n' : ' ');
	std::cout << "\ndet(m) = " << m->det() << "\ndet(m1) = " << m->det() << "\ndet(res) = " << res->det() << std::endl;

	Galois * g1 = new Galois(2, 0b100011011, 0b10);
	Galois * g2 = new Galois(2, 0b100011011, 0b11);
	Galois * g3 = g1->mul(g2);
	std::cin.ignore();
	return 0;
}