/*
 * ProgressTest.cpp
 *
 *  Created on: 29.12.2012
 *      Author: till
 */

#include "ProgressTest.h"

namespace setsync {

namespace sync {

void ProgressTest::testConstructor() {
	Progress p1;
	CPPUNIT_ASSERT(p1.getMinimum() == 0);
	CPPUNIT_ASSERT(p1.getMaximum() == 100);
	CPPUNIT_ASSERT(p1.getValue() == p1.getMinimum());
	CPPUNIT_ASSERT(p1.isDeterministic() == true);
	CPPUNIT_ASSERT(p1.getDescription() == "");
	Progress p2(1, 101);
	CPPUNIT_ASSERT(p2.getMinimum() == 1);
	CPPUNIT_ASSERT(p2.getMaximum() == 101);
	CPPUNIT_ASSERT(p2.getValue() == p2.getMinimum());
	CPPUNIT_ASSERT(p2.isDeterministic() == true);
	CPPUNIT_ASSERT(p2.getDescription() == "");
	Progress p3(1, 101, false);
	CPPUNIT_ASSERT(p3.getMinimum() == 1);
	CPPUNIT_ASSERT(p3.getMaximum() == 101);
	CPPUNIT_ASSERT(p3.getValue() == p3.getMinimum());
	CPPUNIT_ASSERT(p3.isDeterministic() == false);
	CPPUNIT_ASSERT(p3.getDescription() == "");
	Progress p4(10, 5);
	CPPUNIT_ASSERT(p4.getMinimum() == 5);
	CPPUNIT_ASSERT(p4.getMaximum() == 10);
	CPPUNIT_ASSERT(p4.getValue() == p4.getMinimum());
	CPPUNIT_ASSERT(p4.isDeterministic() == true);
	CPPUNIT_ASSERT(p4.getDescription() == "");
}
void ProgressTest::testMinimum() {
	Progress p1;
	CPPUNIT_ASSERT(p1.getMinimum() == 0);
	CPPUNIT_ASSERT(p1.getValue() == 0);
	p1.setMinimum(0);
	CPPUNIT_ASSERT(p1.getMinimum() == 0);
	CPPUNIT_ASSERT(p1.getValue() == 0);
	p1.setMinimum(1);
	CPPUNIT_ASSERT(p1.getMinimum() == 1);
	CPPUNIT_ASSERT(p1.getValue() == 1);
	try {
		p1.setMinimum(p1.getMaximum());
		CPPUNIT_ASSERT(false);
	} catch (...) {

	}
}

void ProgressTest::testMaximum() {
	Progress p1;
	CPPUNIT_ASSERT(p1.getMaximum() == 100);
	CPPUNIT_ASSERT(p1.getValue() == 0);
	p1.setMaximum(10);
	CPPUNIT_ASSERT(p1.getMinimum() == 0);
	CPPUNIT_ASSERT(p1.getValue() == 0);
	CPPUNIT_ASSERT(p1.getMaximum() == 10);
	p1.setValue(10);
	p1.setMaximum(5);
	CPPUNIT_ASSERT(p1.getMinimum() == 0);
	CPPUNIT_ASSERT(p1.getValue() == 5);
	CPPUNIT_ASSERT(p1.getMaximum() == 5);
	try {
		p1.setMinimum(p1.getMaximum());
		CPPUNIT_ASSERT(false);
	} catch (...) {

	}

}
void ProgressTest::testValue() {
	Progress p1;
	CPPUNIT_ASSERT(p1.getValue() == p1.getMinimum());
	CPPUNIT_ASSERT(!p1.isFinished());
	p1.setValue(1);
	CPPUNIT_ASSERT(p1.getValue() == 1);
	CPPUNIT_ASSERT(!p1.isFinished());
	p1.setValue(p1.getMinimum());
	CPPUNIT_ASSERT(p1.getValue() == p1.getMinimum());
	CPPUNIT_ASSERT(!p1.isFinished());
	p1.setValue(-10);
	CPPUNIT_ASSERT(p1.getValue() == p1.getMinimum());
	CPPUNIT_ASSERT(!p1.isFinished());
	p1.setValue(1);
	CPPUNIT_ASSERT(p1.getValue() == 1);
	CPPUNIT_ASSERT(!p1.isFinished());
	p1.setValue(-1);
	CPPUNIT_ASSERT(p1.getValue() == p1.getMinimum());
	CPPUNIT_ASSERT(!p1.isFinished());
	p1.setValue(p1.getMaximum());
	CPPUNIT_ASSERT(p1.getValue() == p1.getMaximum());
	CPPUNIT_ASSERT(p1.isFinished());
	p1.setValue(p1.getMaximum() + 10);
	CPPUNIT_ASSERT(p1.getValue() == p1.getMaximum());
	CPPUNIT_ASSERT(p1.isFinished());
}
void ProgressTest::testDescription() {
	Progress p1;
	std::string desc = "bla";
	CPPUNIT_ASSERT(p1.getDescription() == "");
	p1.setDescription(desc);
	CPPUNIT_ASSERT(p1.getDescription() == desc);
	p1.setDescription("bla");
	CPPUNIT_ASSERT(p1.getDescription() == desc);
}

void ProgressTest::testPercentage() {
	Progress p1;
	CPPUNIT_ASSERT(p1.getPercentComplete() == 0.0);
	p1.setValue(50);
	CPPUNIT_ASSERT(p1.getPercentComplete() == 0.5);
	p1.setValue(p1.getMaximum());
	CPPUNIT_ASSERT(p1.getPercentComplete() == 1.0);
}

}

}
