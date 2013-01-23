/*
 * SetSyncMain.cpp
 *
 *      Author: Till Lorentzen
 */
#include "config.h"
#include "StopWatch.h"
#include "Test.h"
#include "SetSync.h"
#include <string>
#include <list>
#include <setsync/Set.hpp>
#include <setsync/config/Configuration.h>
#include <sstream>
#include <cstdlib>

using namespace std;

void printUsage() {
	cout << "Usage: ssync [OPTION]..." << endl;
	cout << "Builds 2 sets and synchronizes them and prints out a report"
			<< endl;
	cout << endl;
	cout
			<< "\t-s, --storage <levelDB|berkeleyDB|memDB> chooses a key value storage type,"
			<< endl;
	cout
			<< "\t                                   default is the compiled default"
			<< endl;
	cout
			<< "\tA <size>                           initial elements of set A [default=100]"
			<< endl;
	cout
			<< "\tB <size>                           initial elements of set B [default=100]"
			<< endl;
	cout << "\t-e,--equal <size>                  equal entries [default=95]"
			<< endl;
	cout << "\t-m,--max <size>                    maximal entries [default=95]"
			<< endl;
	cout
			<< "\t-t,--type <BF|TRIE|BOTH>           type of synchronization [default=BOTH]"
			<< endl;
	cout
			<< "\t--salt <string>                    if multiple runs should be executed,"
			<< endl
			<< "\t                                   a salt can be defined [default=\"salt \"]"
			<< endl;
	cout
			<< "\t--buffer <size>                    sets the sending and receiving buffer size[default=160]"
			<< endl;
	cout << "\t--md5                              uses MD5 as hash function"
			<< endl;
	cout << "\t--sha1                             uses SHA1 as hash function"
			<< endl;
	cout
			<< "\t--bf <falsePositiveRate>           bloom filter false positive rate[default=0.001]"
			<< endl
			<< "\t                                   Allowed values= ]1,0["
			<< endl;
	cout
			<< "\t--dot                              If sync was not successful, a dot graph will be produced"
			<< endl
			<< "\t                                   from set A and B wit the error marked red[default=off]"
			<< endl;
	cout
			<< "\t--oneway                           If only A\\B is calculated[default=off]"
			<< endl;
	cout
			<< "\t--absolute-synchron                If this is given, the sync process does not work asynchron"
			<< endl;

	cout << "\t-?, --help                         prints out this message"
			<< endl;
	exit(-1);
}

int main(int ac, char **av) {
	std::list<std::string> args;
	std::list<std::string>::iterator iter;
	for (int i = 1; i < ac; i++) {
		std::string arg(av[i]);
		args.push_back(arg);
	}
	SET_CONFIG config = set_create_config();
	size_t a = 100;
	size_t b = 100;
	size_t same = 95;
	string salt = "salt ";
	size_t buffersize = 160;
	size_t maxelements = a + b - same;
	bool maxIsGiven = false;
	bool dot = false;
	bool oneway = false;
	bool synchron = false;
	evaluation::SyncType type = evaluation::BOTH;
	for (iter = args.begin(); iter != args.end(); ++iter) {
		if (*iter == "--storage" || *iter == "-s") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			if (*iter == "levelDB") {
				config.storage = LEVELDB;
			} else if (*iter == "berkeleyDB") {
				config.storage = BERKELEY_DB;
			} else if (*iter == "memDB") {
				config.storage = IN_MEMORY_DB;
			} else {
				printUsage();
			}
		} else if (*iter == "--type" || *iter == "-t") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			if (*iter == "BF") {
				type = evaluation::LOOSE;
			} else if (*iter == "TRIE") {
				type = evaluation::STRICT;
			} else if (*iter == "BOTH") {
				type = evaluation::BOTH;
			} else {
				printUsage();
			}
		} else if (*iter == "--help" || *iter == "-?") {
			printUsage();
		} else if (*iter == "--md5") {
			config.function = MD_5;
		} else if (*iter == "--sha1") {
			config.function = SHA_1;
		} else if (*iter == "--dot") {
			dot = true;
		} else if (*iter == "--oneway") {
			oneway = true;
		} else if (*iter == "A") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> a;
		} else if (*iter == "B") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> b;
		} else if (*iter == "--equal" || *iter == "-e") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> same;
		} else if (*iter == "--max" || *iter == "-m") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> maxelements;
			maxIsGiven = true;
		} else if (*iter == "--salt") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			salt = *iter;
		} else if (*iter == "--buffer") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> buffersize;
		} else if (*iter == "--bf") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> config.false_positive_rate;
		}else if (*iter == "--absolute-synchron") {
			synchron = true;
		}

	}
	if (!maxIsGiven) {
		maxelements = a + b - same;
	}
	cout << "A=" << a << endl;
	cout << "B=" << b << endl;
	cout << "Equal=" << same << endl;
	cout << "Salt=" << salt << endl;
	cout << "MaximalElementes=" << maxelements << endl;
	cout << "BufferSize=" << buffersize << endl;
	cout << "NumberOfElementsAfterSync=" << a + b - same << endl;
	if (config.storage == IN_MEMORY_DB) {
		uint64_t gigabyte = 1024 * 1024 * 1024;
		uint64_t size = (a + b - same) * 50 + 1024 * 1024;
		std::size_t gbsize = 0;
		if (size > gigabyte) {
			gbsize = size / gigabyte;
			size = size % gigabyte;
		}
		config.storage_cache_bytes = size;
		config.storage_cache_gbytes = gbsize;
		cout << "InMemoryDBsize=" << gbsize << "GB " << size << "B" << endl;
	}
	{
		config.bf_max_elements = maxelements;
		evaluation::SetSync test(config, a, b, same, type, salt, buffersize,
				dot, synchron);
		test.run();
	}
}
