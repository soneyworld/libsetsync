/*
 * CryptoBenchmark.cpp
 *
 *  Created on: 08.01.2013
 *      Author: till
 */

#include "CryptoBenchmark.h"
#include "config.h"
namespace evaluation {
using namespace std;
using namespace setsync;

CryptoBenchmark::CryptoBenchmark() {
}

CryptoBenchmark::~CryptoBenchmark() {
}

void CryptoBenchmark::run() {
#ifdef HAVE_OPENSSL
	std::cout << "OPENSSL BENCHMARK" << std::endl;
#else
	std::cout << "POLARSSL BENCHMARK" << std::endl;
#endif
	crypto::CryptoHash md5("md5");
	crypto::CryptoHash sha1("sha1");
	std::string s = "HASH ME, I AM A STRING";
	StopWatch w;
	unsigned char md5buffer[md5.getHashSize()];
	unsigned char md5trie[md5.getHashSize() * 2];
	unsigned char b1024[1024];
	std::cout << "MD5 (" << ITERATIONS << "):" << std::endl;
	cout << "stringCPU,stringReal,trieCPU,trieReal,1024CPU,1024Real" << endl;
	w.start();
	for (int i = 0; i < ITERATIONS; i++) {
		md5(md5buffer, s);
	}
	w.stop();
	std::cout << w.getCPUDuration() << "," << w.getDuration() << ",";
	w.start();
	for (int i = 0; i < ITERATIONS; i++) {
		md5(md5buffer, md5trie, md5.getHashSize() * 2);
	}
	w.stop();
	std::cout << w.getCPUDuration() << "," << w.getDuration() << ",";
	w.start();
	for (int i = 0; i < ITERATIONS; i++) {
		md5(md5buffer, b1024, 1024);
	}
	w.stop();
	std::cout << w.getCPUDuration() << "," << w.getDuration() << endl;
	w.reset();
	unsigned char sha1buffer[sha1.getHashSize()];
	unsigned char sha1trie[sha1.getHashSize() * 2];
	std::cout << "SHA1 (" << ITERATIONS << "):" << std::endl;
	cout << "stringCPU,stringReal,trieCPU,trieReal,1024CPU,1024Real" << endl;
	w.start();
	for (int i = 0; i < ITERATIONS; i++) {
		sha1(sha1buffer, s);
	}
	w.stop();
	std::cout << w.getCPUDuration() << "," << w.getDuration() << ",";
	w.start();
	for (int i = 0; i < ITERATIONS; i++) {
		sha1(sha1buffer, sha1trie, sha1.getHashSize() * 2);
	}
	w.stop();
	std::cout << w.getCPUDuration() << "," << w.getDuration() << ",";
	w.start();
	for (int i = 0; i < ITERATIONS; i++) {
		sha1(sha1buffer, b1024, 1024);
	}
	w.stop();
	std::cout << w.getCPUDuration() << "," << w.getDuration() << endl;
}
}

int main() {
	evaluation::CryptoBenchmark benchmark;
	benchmark.run();
	return 0;
}
