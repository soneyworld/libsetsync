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
using namespace std;

void printUsage() {
	cout << "Usage: ssync [OPTION]..." << endl;
	cout << "Builds 2 sets and synchronizes them and prints out a report"
			<< endl;
	cout << endl;
	cout
			<< "\t-s, --storage <levelDB|berkeleyDB> chooses a key value storage type,"
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
	evaluation::SyncType type = evaluation::BOTH;
	for (iter = args.begin(); iter != args.end(); ++iter) {
		if (*iter == "--storage" || *iter == "-s") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			if (*iter == "levelDB") {
				cout << "Storage=levelDB"<< endl;
				config.storage = LEVELDB;
			} else if (*iter == "berkeleyDB") {
				cout << "Storage=berkeleyDB"<< endl;
				config.storage = BERKELEY_DB;
			} else {
				printUsage();
			}
		} else if (*iter == "--type" || *iter == "-t") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			if (*iter == "BLOOM") {
				cout << "Type=BLOOM"<< endl;
				type = evaluation::LOOSE;
			} else if (*iter == "TRIE") {
				cout << "Type=TRIE"<< endl;
				type = evaluation::STRICT;
			} else if (*iter == "BOTH") {
				cout << "Type=BOTH"<< endl;
				type = evaluation::BOTH;
			} else {
				printUsage();
			}
		} else if (*iter == "--help" || *iter == "-?") {
			printUsage();
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
	{
		config.bf_max_elements = maxelements;
		evaluation::SetSync test(config, a, b, same, type, salt, buffersize);
		test.run();
	}
}
