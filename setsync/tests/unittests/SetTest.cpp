/*
 * SetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetTest.h"
namespace setsync {
void SetTest::testInsert() {
	setsync::Set set(config);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(set.insert("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 1);
}
void SetTest::testErase() {
	setsync::Set set(config);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(!set.erase("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(set.insert("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 1);
	CPPUNIT_ASSERT(set.erase("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(!set.erase("hallo"));
}
void SetTest::testClear() {
	setsync::Set set(config);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(set.insert("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 1);
	set.clear();
	CPPUNIT_ASSERT(set.getSize() == 0);
}

void SetTest::testFind() {
	setsync::Set set(config);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(!set.find("hallo"));
	CPPUNIT_ASSERT(set.insert("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 1);
	CPPUNIT_ASSERT(set.find("hallo"));
	CPPUNIT_ASSERT(set.erase("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(!set.find("hallo"));
}

void SetTest::testMaximum() {
	config::Configuration::BloomFilterConfig bfconfig;
	bfconfig.setHardMaximum(true);
	bfconfig.setMaxElements(5);
	config::Configuration maxConfig(bfconfig);
	maxConfig.setPath(dir->getPath());
	setsync::Set set(maxConfig);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(set.insert("1"));
	CPPUNIT_ASSERT(set.getSize() == 1);
	CPPUNIT_ASSERT(set.insert("2"));
	CPPUNIT_ASSERT(set.getSize() == 2);
	CPPUNIT_ASSERT(set.insert("3"));
	CPPUNIT_ASSERT(set.getSize() == 3);
	CPPUNIT_ASSERT(set.insert("4"));
	CPPUNIT_ASSERT(set.getSize() == 4);
	CPPUNIT_ASSERT(set.insert("5"));
	CPPUNIT_ASSERT(set.getSize() == 5);
	CPPUNIT_ASSERT(!set.insert("6"));
	CPPUNIT_ASSERT(set.getSize() == 5);
	CPPUNIT_ASSERT(set.erase("5"));
	CPPUNIT_ASSERT(set.getSize() == 4);
	CPPUNIT_ASSERT(set.insert("6"));
	CPPUNIT_ASSERT(set.getSize() == 5);
}

void SetTest::testSync() {
	setsync::Set localset(config);
	setsync::Set remoteset(config);
	CPPUNIT_ASSERT(localset.insert("hallo"));
	CPPUNIT_ASSERT(remoteset.insert("hallo"));

	CPPUNIT_ASSERT(localset.insert("hallo1"));
	setsync::sync::AbstractSyncProcessPart * localprocess =
			localset.createSyncProcess();
	setsync::ListDiffHandler localDiffHandler;
	setsync::sync::AbstractSyncProcessPart * remoteprocess =
			remoteset.createSyncProcess();
	setsync::ListDiffHandler remoteDiffHandler;
	std::size_t buffersize = 20;
	unsigned char buffer[buffersize];
	std::size_t sending;
	while (localprocess->pendingOutput()) {
		sending = localprocess->writeOutput(buffer, buffersize);
		remoteprocess->processInput(buffer, sending, remoteDiffHandler);
		if (remoteprocess->pendingOutput()) {
			sending = remoteprocess->writeOutput(buffer, buffersize);
			localprocess->processInput(buffer, sending, localDiffHandler);
		}
	}
	while (remoteprocess->pendingOutput()) {
		sending = remoteprocess->writeOutput(buffer, buffersize);
		localprocess->processInput(buffer, sending, localDiffHandler);
	}
	delete localprocess;
	delete remoteprocess;
	CPPUNIT_ASSERT(localset == remoteset);
}

void SetTest::testStartSync() {
	setsync::Set localset(config);
	setsync::Set remoteset(config);
	CPPUNIT_ASSERT(localset.insert("hallo"));
	CPPUNIT_ASSERT(remoteset.insert("hallo"));
	CPPUNIT_ASSERT(localset == remoteset);
	setsync::sync::AbstractSyncProcessPart * localprocess =
			localset.createSyncProcess();
	setsync::ListDiffHandler localDiffHandler;
	setsync::sync::AbstractSyncProcessPart * remoteprocess =
			remoteset.createSyncProcess();
	setsync::ListDiffHandler remoteDiffHandler;
	std::size_t buffersize = localset.hash_.getHashSize();
	unsigned char buffer[buffersize];
	std::size_t sending;
	localprocess->writeOutput(buffer, buffersize);
	remoteprocess->processInput(buffer, buffersize, remoteDiffHandler);
	remoteprocess->writeOutput(buffer, buffersize);
	localprocess->processInput(buffer, buffersize, localDiffHandler);
	CPPUNIT_ASSERT(localDiffHandler.size()==0);
	CPPUNIT_ASSERT(remoteDiffHandler.size()==0);
	delete localprocess;
	delete remoteprocess;
	CPPUNIT_ASSERT(localset == remoteset);
}

void SetTest::testLooseSync() {
	throw "";
}

void SetTest::testStrictSync() {
	throw "";
}

void SetTest::setUp() {
	this->dir = new utils::FileSystem::TemporaryDirectory("temp_");
	this->config.setPath(dir->getPath());
}
void SetTest::tearDown() {
	delete this->dir;
}
}
