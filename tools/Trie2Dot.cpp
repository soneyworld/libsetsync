/*
 * Trie2Dot.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Trie2Dot.h"
#include <setsync/utils/CryptoHash.h>
#include <setsync/trie/Trie.h>
#include <setsync/trie/DBTrie.h>
#include <setsync/utils/OutputFunctions.h>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace trie;

Trie2Dot::Trie2Dot() {
	this->db_ = new Db(NULL, 0);
	db_->set_flags(trie::DBTrie::getTableFlags());
	db_->open(NULL, NULL, trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	trie_ = new DBTrie(hash_, db_);
}

void Trie2Dot::out() {
	cout << "digraph trie {" << endl;
	int leafs = 20;
	for (int i = 0; i < leafs; ++i) {
		stringstream ss;
		ss << "test";
		ss << i;
		trie_->Trie::add(ss.str());
	}
	cout << trie_->toDotString();

	int max_cut_size = 10;
	unsigned char buffer[max_cut_size * hash_.getHashSize()];
	unsigned char root[hash_.getHashSize()];
	trie_->getRoot(root);
	for (int i = max_cut_size; i <= max_cut_size; i++) {
		// unsigned short color = i * (200 / max_cut_size);
		//		cout << "subgraph cluster_" << i << " {" << endl;
		//		cout << "label=\"cutsize: " << i << "\"" << endl;
		//		cout << "style=filled;" << endl;
		size_t cutsize = trie_->getSubTrie(root, buffer,
				hash_.getHashSize() * i);
		for (size_t j = 0; j < cutsize / hash_.getHashSize(); j++) {
			string color = "yellow";
			if (trie_->contains(buffer + j * hash_.getHashSize()) == LEAF_NODE) {
				color = "green";
			}
			cout << "N" << utils::OutputFunctions::CryptoHashtoString(
					buffer + j * hash_.getHashSize())
					<< " [style=filled color=\"" << color << "\"];" << endl;
		}
		//		cout << "}" << endl;
	}

	cout << "}" << endl;
}

Trie2Dot::~Trie2Dot() {
	delete trie_;
	db_->close(0);
	delete db_;
}

int main(int argc, const char* argv[]) {
	Trie2Dot trie;
	trie.out();
	return 0;
}
