/*
 * BloomFilterDiff.cpp
 *
 *  Created on: 20.01.2013
 *      Author: till
 */
#include "config.h"
#include "BloomFilterDiff.h"
#include <setsync/DiffHandler.h>
#include <sstream>
#include <cstdlib>
#include <string>
#include <list>
using namespace std;
namespace evaluation {

BloomFilterDiff::BloomFilterDiff(const setsync::crypto::CryptoHash& hash,
		const size_t A, const float p, const size_t salt) :
	storageA_(1, 0), storageB_(1, 0), A_(hash, storageA_, "", A, false, p),
			B_(hash, storageB_, "", A, false, p), diffsize_(A), p_(p), salt_(salt) {
	uint64_t i;
	unsigned char buffer[hash.getHashSize()];

	for (i = salt; i < A+salt; i++) {
		hash(buffer, (unsigned char *) &i, sizeof(uint64_t));
		A_.add(buffer);
	}
	for (; i < 2 * A+salt; i++) {
		hash(buffer, (unsigned char *) &i, sizeof(uint64_t));
		B_.add(buffer);
	}
}

BloomFilterDiff::~BloomFilterDiff() {
}

void BloomFilterDiff::run() {
	setsync::ListDiffHandler handler;
	unsigned char buffer[1024];
	size_t length;
	size_t offset = 0;
	while (true) {
		length = A_.getChunk(buffer, 1024, offset);
		if (length == 0) {
			break;
		} else {
			B_.diff(buffer, length, offset, handler);
		}
		offset += length;
	}
	cout << "StartDiffsize,detectedDiffsize,p,allowedDiffSize,salt" << endl;
	float allowed = diffsize_ - (diffsize_ * p_);
	cout << diffsize_ << "," << handler.size() << "," << p_ << "," << allowed
			<< "," << salt_<< endl;
}

}

void printUsage() {
	cout << "" << endl;
	exit(-1);
}

int main(int ac, char **av) {
	std::list<std::string> args;
	std::list<std::string>::iterator iter;
	for (int i = 1; i < ac; i++) {
		std::string arg(av[i]);
		args.push_back(arg);
	}
	string ch = "sha1";
	size_t A = 100;
	float p = 0.1;
	size_t salt = 0;
	for (iter = args.begin(); iter != args.end(); ++iter) {
		if (*iter == "--md5") {
			ch = "md5";
		} else if (*iter == "-?" || *iter == "--help") {
			printUsage();
		} else if (*iter == "p") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> p;
		} else if (*iter == "A") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> A;
		} else if (*iter == "salt") {
			iter++;
			if (iter == args.end()) {
				printUsage();
			}
			istringstream(*iter) >> salt;
		}
	}
	setsync::crypto::CryptoHash hash(ch);
	evaluation::BloomFilterDiff bfd(hash, A, p, salt);
	bfd.run();
}
