/*
 * LevelDbTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "LevelDbTest.h"
#include <setsync/utils/FileSystem.h>
#include <setsync/storage/LevelDbStorage.h>
#include <leveldb/cache.h>
#include "StopWatch.h"

using namespace std;

LevelDbTest::LevelDbTest(const setsync::utils::CryptoHash& hash) :
	hash(hash) {
	leveldb::Options options;
	options.create_if_missing = true;
	options.error_if_exists = true;
	leveldb::Status status = leveldb::DB::Open(options, "testdb", &db);
	assert(status.ok());
}

LevelDbTest::~LevelDbTest() {
	delete db;
	setsync::utils::FileSystem::rmDirRecursive("testdb");
}

void LevelDbTest::run() {
	// insert ITERATION
	runInsertTest();
	// ITERATIONS inserted
	runRequestTest();
	// delete all items
	runDeletionTest();
}

void LevelDbTest::runInsertTest() {
	cout << "running Level DB insertion test (" << hash.getName() << ")"
			<< endl;
	cout << "noinserts,CPUintervalDuration,realtimeIntervalDuration,duration"
			<< endl;
	unsigned int count = 0;
	StopWatch stopwatch;
	double duration = 0;
	unsigned char keybuffer[hash.getHashSize()];
	leveldb::WriteOptions woptions;
	//	woptions.sync = true;
	leveldb::Slice value(NULL, 0);
	leveldb::Status status;
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		hash(keybuffer, (unsigned char *) &i, sizeof(uint64_t));
		leveldb::Slice key((char*) keybuffer, hash.getHashSize());
		stopwatch.start();
		status = db->Put(woptions, key, value);
		stopwatch.stop();
		if (count == ITEMS_PER_LOOPS) {
			duration += stopwatch.getDuration();
			cout << i << "," << stopwatch.getCPUDuration() << ","
					<< stopwatch.getDuration() << "," << duration << endl;
			count = 0;
			stopwatch.reset();
		}
		count++;
		if (!status.ok()) {
			cerr << "Fail on putting data[" << i << "] to level db" << endl;
			return;
		}
	}
	duration += stopwatch.getDuration();
	cout << ITERATIONS << "," << stopwatch.getCPUDuration() << ","
			<< stopwatch.getDuration() << "," << duration << endl;
}
void LevelDbTest::runRequestTest() {
	cout << "running Level DB request test (" << hash.getName() << ")" << endl;
	cout << "norequests,CPUintervalDuration,realtimeIntervalDuration,duration"
			<< endl;
	unsigned int count = 0;
	StopWatch stopwatch;
	double duration = 0;
	unsigned char keybuffer[hash.getHashSize()];
	leveldb::ReadOptions roptions;
	std::string value;
	leveldb::Status status;
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		hash(keybuffer, (unsigned char *) &i, sizeof(uint64_t));
		leveldb::Slice key((char*) keybuffer, hash.getHashSize());
		stopwatch.start();
		status = db->Get(roptions, key, &value);
		stopwatch.stop();
		if (count == ITEMS_PER_LOOPS) {
			duration += stopwatch.getDuration();
			cout << i << "," << stopwatch.getCPUDuration() << ","
					<< stopwatch.getDuration() << "," << duration << endl;
			count = 0;
			stopwatch.reset();
		}
		count++;
		if (!status.ok()) {
			cerr << "Reading failed on data[" << i << "] in level db" << endl;
			return;
		}
	}
	duration += stopwatch.getDuration();
	cout << ITERATIONS << "," << stopwatch.getCPUDuration() << ","
			<< stopwatch.getDuration() << "," << duration << endl;
}
void LevelDbTest::runDeletionTest() {
	leveldb::WriteOptions wo;
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		db->Delete(wo, it->key());
	}
	assert(it->status().ok()); // Check for any errors found during the scan
	delete it;
}
void LevelDbTest::runInsertDeletionTest() {

}

void LevelDbTest::runDbSizeTestInSteps(const size_t keysize,
		const size_t valuesize) {
	unsigned char md[hash.getHashSize()];
	size_t keysize_;
	if (keysize < sizeof(uint64_t))
		keysize_ = sizeof(uint64_t);
	else
		keysize_ = keysize;
	cout << "running Level DB Size test: " << endl;
	std::string path = "temp_level_db_tests";
	leveldb::Options leveldboptions;
	leveldboptions.create_if_missing = true;
	leveldboptions.error_if_exists = true;
	leveldb::WriteOptions woptions;
	woptions.sync = true;
	setsync::storage::LevelDbStorage * storage;
	storage = new setsync::storage::LevelDbStorage(path, leveldboptions,
			woptions);
	cout << "noentries,keysize,valuesize,filesize,sizePerEntry,sizePerByte"
			<< endl;
	unsigned char keybuf[keysize_];
	unsigned char valuebuf[valuesize];
	memset(valuebuf, 0, valuesize);
	uint64_t i = 0;
	for (uint64_t iter = 0; iter < LOOP_ITERATIONS; iter++) {
		for (unsigned int j = 0; j < ITEMS_PER_LOOPS; j++) {
			hash(md, (unsigned char *) &i, sizeof(uint64_t));
			memcpy(keybuf, md, std::min(hash.getHashSize(), keysize_));
			storage->put(keybuf, keysize_, valuebuf, valuesize);
			i++;
		}
		cout << i << "," << keysize_ << "," << valuesize << ",";
		uint64_t dirsize = setsync::utils::FileSystem::dirSize(path);
		double sizePerEntry = ((double) dirsize) / (double) (i);
		double sizePerByte = ((double) dirsize) / (double) ((i) * (keysize_
				+ valuesize));
		cout << dirsize << "," << sizePerEntry << "," << sizePerByte << endl;
	}
	delete storage;
	setsync::utils::FileSystem::rmDirRecursive(path);
}

void LevelDbTest::runDbSizeTestInSteps() {
	runDbSizeTestInSteps(8, 0);
	runDbSizeTestInSteps(16, 0);
	runDbSizeTestInSteps(20, 0);
	runDbSizeTestInSteps(20, 100);
	runDbSizeTestInSteps(20, 200);
}
