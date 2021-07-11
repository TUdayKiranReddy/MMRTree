#include <fstream>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

class Node{
public:
	int _level;
	long _hash;
	Node() {
        _level = -1;
        _hash = 0;
    }
    Node(int level, const long& hash) {
        _level = level;
        _hash = hash;
    }
    Node& operator=(const Node& node) {
        if(this == &node) {
            return *this;
        }

        _level = node._level;
        _hash = node._hash;
        return *this;
    }
};

class MMRTree{
public:
	
	MMRTree();

	~MMRTree();

	void constructTree(const std::vector<long>& hashes, std::vector<long>& peaks);

	void updateTree(const long hash, std::vector<long>& peaks);

	void prove(const int index, std::vector<std::pair<long, bool>>& proof);

	bool verify(const int index, const std::vector<std::pair<long, bool>>& proof);

	void printTree();

	int height;
	int _leafs;

private:
	std::vector<Node> tree;
	std::vector<int> leafs_positions;
	std::vector<Node> _peaks;
	std::vector<int> peaks_positions;

	long concat(long lhash, long rhash);

	int getSiblingOffset(const int index);

	int isleftSibling_hasParent(const int index);

	void addNode(int& idx, int level, long hash);

	void addParent(int& idx);
};