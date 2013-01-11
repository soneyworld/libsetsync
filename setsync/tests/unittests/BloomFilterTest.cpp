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
#include <stdlib.h>

using namespace std;
namespace setsync {
namespace bloom {

/*=== BEGIN tests for class 'BloomFilter' ===*/
void BloomFilterTest::testLoad() {
	bloom::BloomFilter Filter1(hash, 10, false, 0.01);
	bloom::BloomFilter Filter2(hash, 10, false, 0.01);
	string s1 = "hello";
	string s2 = "byebye";
	Filter1.AbstractBloomFilter::add(s1);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains(s1));
	CPPUNIT_ASSERT_EQUAL(false, Filter2.AbstractBloomFilter::contains(s1));
	std::stringstream buf;
	Filter1.save(buf);
	Filter2.load(buf, Filter1.numberOfElements());
	CPPUNIT_ASSERT_EQUAL(true, Filter2.AbstractBloomFilter::contains(s1));
	CPPUNIT_ASSERT_EQUAL(Filter1.numberOfElements(), Filter2.numberOfElements());

}

void BloomFilterTest::testInsert() {
	bloom::BloomFilter Filter1(hash, 10, false, 0.01);

	//	 test signature (const unsigned char* key)

	unsigned char cad1[hash.getHashSize()];
	hash(cad1, "ejemplo");
	Filter1.add(cad1);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.contains(cad1));

	//	 test signature (const std::string& key)

	string strin;
	strin = "hello";
	Filter1.AbstractBloomFilter::add(strin);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains(strin));
	//	std::cout << Filter1.toString() << std::endl;

}

void BloomFilterTest::testContains() {
	/* test signature (const std::string& key) const */
	/* test signature (const char* data, const std::size_t& length) const */

	bloom::BloomFilter Filter1(hash, 8196);
	char word[8];
	for (int j = 0; j <= 127; j++) {
		for (int i = 0; i <= 7; i++) {
			word[i] = 33 + rand() % (126 - 23);
		}
		Filter1.AbstractBloomFilter::add(word);
		CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains(word));
	}
	Filter1.clear();
	CPPUNIT_ASSERT_EQUAL(false, Filter1.AbstractBloomFilter::contains(word));

}

void BloomFilterTest::testContainsAll() {
	bloom::BloomFilter Filter2(hash, 8196);
	unsigned char hashes[100 * hash.getHashSize()];
	for (int j = 0; j < 100; j++) {
		unsigned char word[8];
		for (int i = 0; i < 8; i++) {
			word[i] = 33 + rand() % (126 - 23);
		}
		hash(hashes + hash.getHashSize() * j, word, 8);
	}
	for (int i = 0; i < 100; i++) {
		Filter2.add(hashes + i * hash.getHashSize());
	}
	Filter2.containsAll(hashes, 100);
	CPPUNIT_ASSERT(Filter2.containsAll(hashes, 100));
}

void BloomFilterTest::testOperatorAndAndAssign() {
	/* test signature (const BloomFilter& filter) */

	bloom::BloomFilter FilterA(hash, 8196);
	bloom::BloomFilter FilterB(hash, 8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.AbstractBloomFilter::add(strin1);
	FilterA.AbstractBloomFilter::add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));

	strin3 = "hola";
	strin4 = "byebye";
	FilterB.AbstractBloomFilter::add(strin3);
	FilterB.AbstractBloomFilter::add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin4));

	FilterA.operator &=(FilterB);
	CPPUNIT_ASSERT_EQUAL(false, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin4));

}

void BloomFilterTest::testOperatorInclusiveOrAndAssign() {
	/* test signature (const BloomFilter& filter) */
	bloom::BloomFilter FilterA(hash, 8196);
	bloom::BloomFilter FilterB(hash, 8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.AbstractBloomFilter::add(strin1);
	FilterA.AbstractBloomFilter::add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));

	strin3 = "hola";
	strin4 = "chau";
	FilterB.AbstractBloomFilter::add(strin3);
	FilterB.AbstractBloomFilter::add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin4));

	FilterA.operator |=(FilterB);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin4));

}

void BloomFilterTest::testOperatorXorAndAssign() {
	/* test signature (const BloomFilter& filter) */

	bloom::BloomFilter FilterA(hash, 8196);
	bloom::BloomFilter FilterB(hash, 8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.AbstractBloomFilter::add(strin1);
	FilterA.AbstractBloomFilter::add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));

	strin3 = "hola";
	strin4 = "byebye";
	FilterB.AbstractBloomFilter::add(strin3);
	FilterB.AbstractBloomFilter::add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin4));

	FilterA.operator ^=(FilterB);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.AbstractBloomFilter::contains(strin2));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.AbstractBloomFilter::contains(strin4));

}

/*=== END   tests for class 'BloomFilter' ===*/

void BloomFilterTest::setUp() {
}

void BloomFilterTest::tearDown() {
}
}
}
