///
/// @file        DoubleHashingSchemeTest.cpp
/// @brief       CPPUnit-Tests for class DoubleHashingScheme and ExtendedDoubleHashingScheme
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 

#include "DoubleHashingSchemeTest.h"
#include <setsync/bloom/DoubleHashingScheme.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <math.h>
#include <list>
#include <stdlib.h>
#include <limits>

using namespace std;
namespace bloom {

/*=== BEGIN tests for class 'DoubleHashingScheme' ===*/

void DoubleHashingSchemeTest::testCount() {
	bloom::DoubleHashingScheme hashProvider(hash.getHashSize());
	CPPUNIT_ASSERT(std::numeric_limits<size_t>::max() == hashProvider.count());

}

void DoubleHashingSchemeTest::testHash() {
	bloom::DoubleHashingScheme ProviderA(hash.getHashSize());
	bloom::DoubleHashingScheme ProviderB(hash.getHashSize());
	bloom::DoubleHashingScheme ProviderC(hash.getHashSize());
	unsigned char cad1[hash.getHashSize()];
	unsigned char cad2[hash.getHashSize()];
	std::string s1 = "example";
	std::string s2 = "sample";
	for (std::size_t i = 0; i < 10; i++) {
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, hash.getHashSize(), i) == ProviderB.hash(
						cad1, hash.getHashSize(), i));
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, hash.getHashSize(), i) != ProviderC.hash(
						cad2, hash.getHashSize(), i));
	}
	// Check, if all generated hash functions are different
	for (std::size_t i = 1; i < 10; i++) {
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, hash.getHashSize(), 0) != ProviderB.hash(
						cad1, hash.getHashSize(), i));
	}
}

/*=== END   tests for class 'DoubleHashingScheme' ===*/

/*=== BEGIN tests for class 'ExtendedDoubleHashingScheme' ===*/

void DoubleHashingSchemeTest::testExtendedCount() {
	bloom::ExtendedDoubleHashingScheme hashProvider(hash.getHashSize());
	CPPUNIT_ASSERT(std::numeric_limits<size_t>::max() == hashProvider.count());

}

void DoubleHashingSchemeTest::testExtendedHash() {
	bloom::ExtendedDoubleHashingScheme ProviderA(hash.getHashSize());
	bloom::ExtendedDoubleHashingScheme ProviderB(hash.getHashSize());
	bloom::ExtendedDoubleHashingScheme ProviderC(hash.getHashSize());
	unsigned char cad1[hash.getHashSize()];
	unsigned char cad2[hash.getHashSize()];
	std::string s1 = "example";
	std::string s2 = "sample";
	for (std::size_t i = 0; i < 10; i++) {
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, hash.getHashSize(), i) == ProviderB.hash(
						cad1, hash.getHashSize(), i));
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, hash.getHashSize(), i) != ProviderC.hash(
						cad2, hash.getHashSize(), i));
	}
	// Check, if all generated hash functions are different
	for (std::size_t i = 1; i < 10; i++) {
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, hash.getHashSize(), 0) != ProviderB.hash(
						cad1, hash.getHashSize(), i));
	}
}

/*=== END   tests for class 'DoubleHashingScheme' ===*/

void DoubleHashingSchemeTest::setUp() {
}

void DoubleHashingSchemeTest::tearDown() {
}
};
