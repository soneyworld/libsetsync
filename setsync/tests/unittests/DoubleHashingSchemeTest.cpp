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
#include <setsync/sha1.h>
#include <stdlib.h>
#include <limits>

using namespace std;
namespace bloom {

/*=== BEGIN tests for class 'DoubleHashingScheme' ===*/

void DoubleHashingSchemeTest::testCount() {
	bloom::DoubleHashingScheme hashProvider;
	CPPUNIT_ASSERT(std::numeric_limits<size_t>::max() == hashProvider.count());

}

void DoubleHashingSchemeTest::testHash() {
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

/*=== BEGIN tests for class 'ExtendedDoubleHashingScheme' ===*/

void DoubleHashingSchemeTest::testExtendedCount() {
	bloom::ExtendedDoubleHashingScheme hashProvider;
	CPPUNIT_ASSERT(std::numeric_limits<size_t>::max() == hashProvider.count());

}

void DoubleHashingSchemeTest::testExtendedHash() {
	bloom::ExtendedDoubleHashingScheme ProviderA;
	bloom::ExtendedDoubleHashingScheme ProviderB;
	bloom::ExtendedDoubleHashingScheme ProviderC;
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
};
