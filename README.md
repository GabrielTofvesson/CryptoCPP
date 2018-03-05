# CryptoCPP
My attempt at a small crypto library

## Current projects
* AES
* RSA
* SHA
* XMath
* CryptoTests



### AES
An implementation of AES designed to show how AES encrypts. It uses mathematical functions rather than lookup tables in order to clarify how the encryption actally works and why the operations are as they are.
Status:
* Implemented

Dependencies:
* XMath


### RSA
Small RSA implementation with key generation delegated partially to XMath. The implementation supports message signing, seralization and deserialization.
Status:
* Headers: Implemented
* Code: Not implemented

Dependencies:
* XMath


### SHA
Secure hashing algorithms. Implementations: SHA1.
Status:
* Not implemented

Dependencies:
None


### XMath
A library with a lot of helper functions and classes for keeping other code clean.
Status:
* BigInteger: Implemented
* Galois Implemented
* Matrix Implemented
* Primes: Not implemented

Dependencies:
None


### CryptoTests
A project for testing all the other projects and their implementations.
Status:
N/A

Dependencies:
All