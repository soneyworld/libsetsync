/*
 * Trie2Dot.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Trie2Dot.h"
#include <setsync/utils/CryptoHash.h>
#include <setsync/utils/OutputFunctions.h>
#include <setsync/storage/BdbStorage.h>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace trie;

Trie2Dot::Trie2Dot() {
	this->db1_ = new Db(NULL, 0);
	db1_->open(NULL, NULL, "trie",
			DB_HASH, DB_CREATE, 0);
	this->storage1 = new setsync::storage::BdbStorage(db1_);
	trie1_ = new KeyValueTrie(hash_, *storage1);
	this->db2_ = new Db(NULL, 0);
	db2_->open(NULL, NULL, "trie",
			DB_HASH, DB_CREATE, 0);
	this->storage2 = new setsync::storage::BdbStorage(db2_);
	trie2_ = new KeyValueTrie(hash_, *storage2);
}

void Trie2Dot::out() {
	cout << "digraph trie {" << endl;
	int leafs = 6;
	for (int i = 0; i < leafs; ++i) {
		stringstream ss;
		ss << "test";
		ss << i;
		trie1_->Trie::add(ss.str());
		if (i > 0)
			trie2_->Trie::add(ss.str());
	}
	cout << trie1_->Trie::toDotString();
	cout << trie2_->toDotString("_");
	int max_cut_size = 5;
	unsigned char buffer[max_cut_size * hash_.getHashSize()];
	unsigned char root[hash_.getHashSize()];
	trie1_->getRoot(root);
		// unsigned short color = i * (200 / max_cut_size);
		//		cout << "subgraph cluster_" << i << " {" << endl;
		//		cout << "label=\"cutsize: " << i << "\"" << endl;
		//		cout << "style=filled;" << endl;
		size_t cutsize = trie1_->getSubTrie(root, buffer,
				hash_.getHashSize() * max_cut_size);
		for (size_t j = 0; j < (cutsize / hash_.getHashSize()); j++) {
			string color = "grey";
			if (trie1_->contains(buffer + j * hash_.getHashSize()) == LEAF_NODE) {
				color = "green";
			}
			cout << "N" << utils::OutputFunctions::CryptoHashtoString(
					buffer + j * hash_.getHashSize())
					<< " [style=filled color=\"" << color << "\"];" << endl;
/*			cout << "N" << utils::OutputFunctions::CryptoHashtoString(
					buffer + j * hash_.getHashSize()) << " -> " << "_"
					<< utils::OutputFunctions::CryptoHashtoString(
							buffer + j * hash_.getHashSize())<<";"<< endl;*/
			cout << "_" << utils::OutputFunctions::CryptoHashtoString(
					buffer + j * hash_.getHashSize())
					<< " [style=filled color=\"" << color << "\"];" << endl;
		}
		//		cout << "}" << endl;

	cout << "}" << endl;
}

Trie2Dot::~Trie2Dot() {
	delete this->storage1;
	delete this->storage2;
	delete trie1_;
	db1_->close(0);
	delete db1_;
	delete trie2_;
	db2_->close(0);
	delete db2_;
}

int main(int argc, const char* argv[]) {
	Trie2Dot trie;
	trie.out();
	return 0;
}
