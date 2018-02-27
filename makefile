start:
	$(info No recipe specified!)

xmath:
	mkdir -p build/include
	mkdir -p build/lib
	mkdir -p XMath/intermediate
	g++ -c XMath/BigInteger.cpp -o XMath/intermediate/BigInteger.o
	g++ -c XMath/Matrix.cpp -o XMath/intermediate/Matrix.o
	ar rcs build/lib/libxmath.a XMath/intermediate/*
	cp XMath/*.h* build/include/
	rm -r XMath/intermediate/

clean:
	mkdir -p build
	rm -r build
	mkdir -p XMath/intermediate
	rm -r XMath/intermediate
