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
	
	MMRTree() {
	}

	~MMRTree() {
		tree.clear();
	}

	void constructTree(const std::vector<long>& hashes, std::vector<long>& peaks){
		int hashes_size = hashes.size();
		if(hashes_size==0)
			return;
		int temp = hashes_size;
		int tree_size=hashes_size;
		
		while(temp!=0){
			temp = temp >> 1;
			tree_size += temp;
		}

		tree.resize(tree_size);
		leafs = hashes_size;
		std::cout << "Tree size:- " << tree_size << std::endl;

		int idx=0;
		std::cout << "\n/---------Adding Hash " << 0 << "(" << hashes[0] << ")"<< "----------/" << std::endl;
		std::cout << idx << " -> ";
		addNode(idx, 0, hashes[0]);
		std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;
		for(int i=1;(i<hashes_size);i++){
			if(tree[idx-1]._level==0){
				std::cout << "\n/---------Adding Hash " << i  << "(" << hashes[i] << ")"<< "----------/" << std::endl;
				std::cout << idx << " -> ";
				addNode(idx, 0, hashes[i]);
				std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;

				std::cout << "Adding Parent..\n";
				std::cout << idx << " -> ";		
				addParent(idx);
				std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;
				while(isleftSibling_hasParent((idx-1))==0){
					std::cout << "Adding Parent..\n";
					std::cout << idx << " -> ";
					addParent(idx);
					std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;
				}
			}
			else{
				std::cout << "\n/---------Adding Hash " << i  << "(" << hashes[i] << ")"<< "----------/" << std::endl;
				std::cout << idx << " -> ";
				addNode(idx, 0, hashes[i]);
				std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;				
			}
		}

		// Calculate Peaks
		
		std::vector<int> bin;
		int temp_n = leafs;
		while(temp_n>0){
			bin.push_back(temp_n%2);
			temp_n >>= 1;
		}

		int sum_prev_peaks = (1<<(bin.size()))-2;
		_peaks.push_back(tree[sum_prev_peaks]);
		peaks.push_back(tree[sum_prev_peaks]._hash);
		for(int j=(bin.size()-2);j>=0;j--){
			if(bin[j]==1){
				sum_prev_peaks += (1<<(j+1)) - 1;
				_peaks.push_back(tree[sum_prev_peaks]);
				peaks.push_back(tree[sum_prev_peaks]._hash);
			}
		}

	}

	void updateTree(const long hash, std::vector<long>& peaks){
		// std::cout << "Not yet done!\n"; 
		int idx = tree.size();
		int temp=leafs+1;
		int tree_size=temp;
		
		while(temp!=0){
			temp = temp >> 1;
			tree_size += temp;
		}

		tree.resize(tree_size);
		std::cout << tree[idx-1]._level << std::endl;
		if(tree[idx-1]._level>0){
			std::cout << "\n/---------Adding Hash (" << hash << ")"<< "----------/" << std::endl;
			std::cout << idx << " -> ";
			addNode(idx, 0, hash);
			std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;		
		}
		//else if(tree[idx-1]._level==0){
		else{
			std::cout << "\n/---------Adding Hash (" << hash << ")"<< "----------/" << std::endl;
			std::cout << idx << " -> ";
			addNode(idx, 0, hash);
			std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;
			while(isleftSibling_hasParent(idx)==0){
				std::cout << "Adding Parent..\n";
				std::cout << idx << " -> ";
				addParent(idx);
				std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;
			}
		}
		
		std::cout << tree.size() << std::endl;
		leafs++;
		//Update MMR Peaks
		peaks.clear();
		_peaks.clear();
		std::vector<int> bin;
		int temp_n = leafs;
		while(temp_n>0){
			bin.push_back(temp_n%2);
			temp_n >>= 1;
		}

		int sum_prev_peaks = (1<<(bin.size()))-2;
		_peaks.push_back(tree[sum_prev_peaks]);
		peaks.push_back(tree[sum_prev_peaks]._hash);
		for(int j=(bin.size()-2);j>=0;j--){
			if(bin[j]==1){
				sum_prev_peaks += (1<<(j+1)) - 1;
				_peaks.push_back(tree[sum_prev_peaks]);
				peaks.push_back(tree[sum_prev_peaks]._hash);
			}
		}
	}

	void printTree(){
		int tree_size = tree.size();
		int max_lvl = std::log2(leafs);
		int curr_lvl = max_lvl;
		std::cout << "Tree size: " << tree_size << std::endl;
		while(curr_lvl>=0){
			std::cout << "\n/----------------Level "<< curr_lvl << "----------------/\n";
			for(int i=0;i<tree_size;i++){
				if(tree[i]._level == curr_lvl)
					std::cout << tree[i]._hash << " ";
			}
			curr_lvl--;
		}
		std::cout << "\n/----------------Peaks----------------/\n";
		for(int i=0;i<_peaks.size();i++)
			std::cout << _peaks[i]._hash << " ";
		std::cout << std::endl;
	}

//private:
	int height;
	int leafs;
	std::vector<Node> tree;
	std::vector<Node> _peaks;
private:
	long concat(long lhash, long rhash){
		return lhash + rhash;
	}

	int isleftSibling_hasParent(const int index){	//-1 means MMR Node does'nt, 0 means parent does not exist, 1 means parent exist
		if(index==0)
			return 0;

		int lSib_index = index - ((1 << (tree[index]._level + 1))-1);
		std::cout << "Recent filled index " << index << ", Calculated Left Sibling " << lSib_index;
		if(lSib_index<0){
			std::cout << " not adding next Parent!\n";
			return -1;
		}
		std::cout  << " and its level is " << tree[lSib_index]._level << std::endl;
		int flag;
		if(tree[lSib_index]._level == tree[index]._level)
			flag = 0;
		else
			flag = 1;
		return flag;
	}

	void addNode(int& idx, int level, long hash){
		Node node(level, hash);
		tree[idx] = node;
		std::cout << "Node Info{level:" << level << ",hash:" << hash << "} -> "; 
		idx++;
	}

	void addParent(int& idx){
		Node rChild = tree[idx-1];
		Node lChild = tree[idx - (1 << (tree[idx-1]._level+1))];
		std::cout << "[" << idx - (1 << (tree[idx-1]._level+1)) << ", " << idx-1 << "] -> ";
		long hash = concat(lChild._hash, rChild._hash);
		addNode(idx, (rChild._level) + 1, hash);
		//idx++;
	}
};

int main(){
	std::vector<long> hashes = {1, 2, 3, 4};
	MMRTree mmr;
	std::vector<long> peaks;
	mmr.constructTree(hashes, peaks);
	mmr.printTree();
	mmr.updateTree(5, peaks);
	mmr.printTree();
	mmr.updateTree(6, peaks);
	mmr.printTree();
	mmr.updateTree(7, peaks);
	mmr.printTree();
	mmr.updateTree(8, peaks);
	mmr.printTree();
	mmr.updateTree(9, peaks);
	mmr.printTree();


}