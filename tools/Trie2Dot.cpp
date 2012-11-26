/*
 * Trie2Dot.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Trie2Dot.h"
#include <setsync/utils/CryptoHash.h>
#include <setsync/trie/Trie.h>
#include <setsync/trie/DBTrie.h>
using namespace std;
using namespace trie;

Trie2Dot::Trie2Dot() {
}

void Trie2Dot::out(){
	Db triedb(NULL, 0);
	triedb.set_flags(trie::DBTrie::getTableFlags());
	utils::CryptoHash hash;
	DBTrie trie(hash, &triedb);
	cout << endl;
	triedb.close(0);
}

Trie2Dot::~Trie2Dot() {
}

int main( int argc, const char* argv[] )
{
	Trie2Dot trie;
	trie.out();
	return 0;
}
