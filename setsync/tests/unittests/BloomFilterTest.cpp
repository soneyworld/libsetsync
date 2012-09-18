///
/// @file        BloomFilterTest.cpp
/// @brief       CPPUnit-Tests for class BloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 

#include "BloomFilterTest.h"
#include <setsync/bloom/BloomFilter.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <math.h>
#include <list>
#include <setsync/sha1.h>
#include <stdlib.h>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( BloomFilterTest);

/*========================== tests below ==========================*/

/*=== BEGIN tests for class 'DefaultHashProvider' ===*/

void BloomFilterTest::testCount() {
	/* test signature () const */

	bloom::SaltedHashFunction Provider(10);
	CPPUNIT_ASSERT(10 == Provider.count());

}

void BloomFilterTest::testHash() {
	/* test signature (const unsigned char* begin, std::size_t remaining_length) const */

	bloom::SaltedHashFunction ProviderA(5);
	bloom::SaltedHashFunction ProviderB(5);
	bloom::SaltedHashFunction ProviderC(5);
	unsigned char cad1[SHA_DIGEST_LENGTH];
	unsigned char cad2[SHA_DIGEST_LENGTH];
	std::string s1 = "example";
	std::string s2 = "sample";
	for (std::size_t i = 0; i < ProviderA.count(); i++) {
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, SHA_DIGEST_LENGTH, i) == ProviderB.hash(
						cad1, SHA_DIGEST_LENGTH, i));
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, SHA_DIGEST_LENGTH, i) != ProviderC.hash(
						cad2, SHA_DIGEST_LENGTH, i));
	}
	// Check, if all generated hash functions are different
	for (std::size_t i = 1; i < ProviderA.count(); i++) {
		CPPUNIT_ASSERT(
				ProviderA.hash(cad1, SHA_DIGEST_LENGTH, 0) != ProviderB.hash(
						cad1, SHA_DIGEST_LENGTH, i));
	}
}

/*=== END   tests for class 'DefaultHashProvider' ===*/

/*=== BEGIN tests for class 'BloomFilter' ===*/
void BloomFilterTest::testLoad() {
	throw "not yet implemented";

}

void BloomFilterTest::testInsert() {
	bloom::BloomFilter Filter1(8196);

	//	 test signature (const unsigned char* key)

	unsigned char cad1[SHA_DIGEST_LENGTH];
	SHA1((const unsigned char*) "ejemplo", 7, cad1);
	Filter1.add(cad1);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.contains(cad1));

	//	 test signature (const std::string& key)

	string strin;
	strin = "hello";
	Filter1.add(strin);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.contains(strin));

}

void BloomFilterTest::testContains() {
	/* test signature (const std::string& key) const */
	/* test signature (const char* data, const std::size_t& length) const */

	bloom::BloomFilter Filter1(8196);
	char word[8];
	for (int j = 0; j <= 127; j++) {
		for (int i = 0; i <= 7; i++) {
			word[i] = 33 + rand() % (126 - 23);
		}
		Filter1.add(word);
		CPPUNIT_ASSERT_EQUAL(true, Filter1.contains(word));
	}
	Filter1.clear();
	CPPUNIT_ASSERT_EQUAL(false, Filter1.contains(word));

}

void BloomFilterTest::testContainsAll() {
	bloom::BloomFilter Filter2(8196);
	unsigned char hashes[100 * SHA_DIGEST_LENGTH];
	for (int j = 0; j < 100; j++) {
		unsigned char word[8];
		for (int i = 0; i <= 7; i++) {
			word[i] = 33 + rand() % (126 - 23);
		}
		SHA1(word, 8, hashes + SHA_DIGEST_LENGTH * j);
	}
	for (int i = 0; i < 100; i++) {
		Filter2.add(hashes + i * SHA_DIGEST_LENGTH);
	}
	Filter2.containsAll(hashes, 100);
	CPPUNIT_ASSERT(Filter2.containsAll(hashes, 100));

}

void BloomFilterTest::testOperatorAndAndAssign() {
	/* test signature (const BloomFilter& filter) */

	bloom::BloomFilter FilterA(8196);
	bloom::BloomFilter FilterB(8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.add(strin1);
	FilterA.add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin2));

	strin3 = "hola";
	strin4 = "byebye";
	FilterB.add(strin3);
	FilterB.add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.contains(strin4));

	FilterA.operator &=(FilterB);
	CPPUNIT_ASSERT_EQUAL(false, FilterA.contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin2));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin4));

}

void BloomFilterTest::testOperatorInclusiveOrAndAssign() {
	/* test signature (const BloomFilter& filter) */
	bloom::BloomFilter FilterA(8196);
	bloom::BloomFilter FilterB(8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.add(strin1);
	FilterA.add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin2));

	strin3 = "hola";
	strin4 = "chau";
	FilterB.add(strin3);
	FilterB.add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.contains(strin4));

	FilterA.operator |=(FilterB);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin2));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin4));

}

void BloomFilterTest::testOperatorXorAndAssign() {
	/* test signature (const BloomFilter& filter) */

	bloom::BloomFilter FilterA(8196);
	bloom::BloomFilter FilterB(8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.add(strin1);
	FilterA.add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin2));

	strin3 = "hola";
	strin4 = "byebye";
	FilterB.add(strin3);
	FilterB.add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.contains(strin4));

	FilterA.operator ^=(FilterB);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin1));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.contains(strin2));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.contains(strin3));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.contains(strin4));

}

/*=== END   tests for class 'BloomFilter' ===*/

void BloomFilterTest::setUp() {
}

void BloomFilterTest::tearDown() {
}

