///
/// @file        DoubleHashingSchemeTest.cpp
/// @brief       CPPUnit-Tests for class DoubleHashingScheme
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
#include <setsync/sha1.h>
#include <stdlib.h>
#include <limits>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( DoubleHashingSchemeTest);

/*========================== tests below ==========================*/

/*=== BEGIN tests for class 'DoubleHashingScheme' ===*/

void DoubleHashingSchemeTest::testCount() {
	/* test signature () const */

	bloom::DoubleHashingScheme hashProvider;
	CPPUNIT_ASSERT(std::numeric_limits<size_t>::max() == hashProvider.count());

}

void DoubleHashingSchemeTest::testHash() {
	/* test signature (const unsigned char* begin, std::size_t remaining_length) const */

	bloom::DoubleHashingScheme ProviderA;
	bloom::DoubleHashingScheme ProviderB;
	bloom::DoubleHashingScheme ProviderC;
	unsigned char cad1[SHA_DIGEST_LENGTH];
	unsigned char cad2[SHA_DIGEST_LENGTH];
	std::string s1 = "example";
	std::string s2 = "sample";
	for (std::size_t i = 0; i < 10; i++) {
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, SHA_DIGEST_LENGTH, i) == ProviderB.hash(
						cad1, SHA_DIGEST_LENGTH, i));
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, SHA_DIGEST_LENGTH, i) != ProviderC.hash(
						cad2, SHA_DIGEST_LENGTH, i));
	}
	// Check, if all generated hash functions are different
	for (std::size_t i = 1; i < 10; i++) {
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, SHA_DIGEST_LENGTH, 0) != ProviderB.hash(
						cad1, SHA_DIGEST_LENGTH, i));
	}
}

/*=== END   tests for class 'DoubleHashingScheme' ===*/

void DoubleHashingSchemeTest::setUp() {
}

void DoubleHashingSchemeTest::tearDown() {
}

