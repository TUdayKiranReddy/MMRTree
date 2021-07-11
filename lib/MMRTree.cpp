#include <fstream>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>


class MMRTree{
public:
	
	MMRTree() {
	}

	~MMRTree() {
		tree.clear();
	}

	void constructTree(const std::vector<long>& hashes, std::vector<long>& peaks){
		int hashes_size = hashes.size();
		leafs_positions.resize(hashes_size);
		if(hashes_size==0)
			return;
		int temp = hashes_size;
		int tree_size=hashes_size;
		
		while(temp!=0){
			temp = temp >> 1;
			tree_size += temp;
		}

		tree.resize(tree_size);
		_leafs = hashes_size;
		std::cout << "Tree size:- " << tree_size << std::endl;

		int idx=0;
		std::cout << "\n/---------Adding Hash " << 0 << "(" << hashes[0] << ")"<< "----------/" << std::endl;
		std::cout << idx << " -> ";
		addNode(idx, 0, hashes[0]);
		leafs_positions[0] = 0;
		std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;
		for(int i=1;(i<hashes_size);i++){
			if(tree[idx-1]._level==0){
				std::cout << "\n/---------Adding Hash " << i  << "(" << hashes[i] << ")"<< "----------/" << std::endl;
				std::cout << idx << " -> ";
				addNode(idx, 0, hashes[i]);
				leafs_positions[i] = idx-1;
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
				leafs_positions[i] = idx-1;
				std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;				
			}
		}

		// Calculate Peaks
		
		std::vector<int> bin;
		int temp_n = _leafs;
		while(temp_n>0){
			bin.push_back(temp_n%2);
			temp_n >>= 1;
		}

		int sum_prev_peaks = (1<<(bin.size()))-2;
		_peaks.push_back(tree[sum_prev_peaks]);
		peaks_positions.push_back(sum_prev_peaks);
		peaks.push_back(tree[sum_prev_peaks]._hash);
		for(int j=(bin.size()-2);j>=0;j--){
			if(bin[j]==1){
				sum_prev_peaks += (1<<(j+1)) - 1;
				_peaks.push_back(tree[sum_prev_peaks]);
				peaks_positions.push_back(sum_prev_peaks);
				peaks.push_back(tree[sum_prev_peaks]._hash);
			}
		}

	}

	void updateTree(const long hash, std::vector<long>& peaks){
		// std::cout << "Not yet done!\n"; 
		int idx = tree.size();
		int temp=_leafs+1;
		int tree_size=temp;
		
		while(temp!=0){
			temp = temp >> 1;
			tree_size += temp;
		}
		leafs_positions.resize(_leafs+1);
		tree.resize(tree_size);
		std::cout << tree[idx-1]._level << std::endl;
		if(tree[idx-1]._level>0){
			std::cout << "\n/---------Adding Hash (" << hash << ")"<< "----------/" << std::endl;
			std::cout << idx << " -> ";
			addNode(idx, 0, hash);
			leafs_positions[_leafs] = idx-1;
			std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;		
		}
		//else if(tree[idx-1]._level==0){
		else{
			std::cout << "\n/---------Adding Hash (" << hash << ")"<< "----------/" << std::endl;
			std::cout << idx << " -> ";
			addNode(idx, 0, hash);
			leafs_positions[_leafs] = idx-1;
			std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;
			while(isleftSibling_hasParent(idx)==0){
				std::cout << "Adding Parent..\n";
				std::cout << idx << " -> ";
				addParent(idx);
				std::cout << idx << " added at level " << tree[idx-1]._level << std::endl;
			}
		}
		
		std::cout << tree.size() << std::endl;
		_leafs++;
		//Update MMR Peaks
		peaks.clear();
		_peaks.clear();
		peaks_positions.clear();
		std::vector<int> bin;
		int temp_n = _leafs;
		while(temp_n>0){
			bin.push_back(temp_n%2);
			temp_n >>= 1;
		}

		int sum_prev_peaks = (1<<(bin.size()))-2;
		_peaks.push_back(tree[sum_prev_peaks]);
		peaks_positions.push_back(sum_prev_peaks);
		peaks.push_back(tree[sum_prev_peaks]._hash);
		for(int j=(bin.size()-2);j>=0;j--){
			if(bin[j]==1){
				sum_prev_peaks += (1<<(j+1)) - 1;
				_peaks.push_back(tree[sum_prev_peaks]);
				peaks_positions.push_back(sum_prev_peaks);
				peaks.push_back(tree[sum_prev_peaks]._hash);
			}
		}
	}

	void prove(const int index, std::vector<std::pair<long, bool>>& proof){ // {long, bool} => {Hash, left(false) or right(true)}
		int idx = leafs_positions[index];
		int siblingOffset = getSiblingOffset(idx);
		while(siblingOffset != 0){
			//std::cout << "(" << idx << ", " << siblingOffset << ") -> "; 
			if(siblingOffset>0){
				proof.push_back({tree[idx+siblingOffset]._hash, true});
				idx += 1+siblingOffset;
			}
			else{
				proof.push_back({tree[idx+siblingOffset]._hash, false});
				idx++;
			}
			siblingOffset = getSiblingOffset(idx);
			//std::cout << "(" << idx << ", " << siblingOffset << ")\n"; 
		}
		//proof.push_back(tree[idx]._hash);
		std::cout << "\n/----------------Proof for Leaf at "<< index << " ----------------/\n";
		for(int i=0;i<proof.size();i++){
			std::cout << "{" << proof[i].first << ", ";
			if(proof[i].second)
				std::cout << "Right} ";
			else
				std::cout << "Left} ";
		}
		std::cout << std::endl;
	}

	bool verify(const int index, const std::vector<std::pair<long, bool>>& proof){
		int leaf_idx = leafs_positions[index];
		long hash = tree[leaf_idx]._hash;
		for(int i=0;i<proof.size();i++){
			if(proof[i].second)
				hash = concat(hash, proof[i].first);
			else
				hash = concat(proof[i].first, hash);
		}
		int i=0;
		for(;i<peaks_positions.size();i++){
			if(peaks_positions[i]>leaf_idx)
				break;
		} 
		return (hash == _peaks[i]._hash) ? true:false;
	}

	void printTree(){
		int tree_size = tree.size();
		int max_lvl = std::log2(_leafs);
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
			std::cout << "{" << _peaks[i]._hash << ", " << peaks_positions[i] << "} ";
		std::cout << "\n/----------------Leafs Positions in the Tree----------------/\n";
		for(int i=0;i<leafs_positions.size();i++)
			std::cout << leafs_positions[i] << " ";
		std::cout << std::endl;
	}

	int height;
	int _leafs;

private:
	std::vector<Node> tree;
	std::vector<int> leafs_positions;
	std::vector<Node> _peaks;
	std::vector<int> peaks_positions;

	long concat(long lhash, long rhash){
		return lhash + rhash;
	}

	int getSiblingOffset(const int index){
		int lS_off =  -1*((1<<(tree[index]._level + 1))-1);
		int rS_off = (1<<(tree[index]._level + 1))-1;
		int offset;
		if(((index+rS_off) < tree.size()) && (tree[index+rS_off]._level == tree[index]._level))
			offset = rS_off;
		else if(((lS_off+index) >= 0 ) && (tree[index+lS_off]._level == tree[index]._level))
			offset = lS_off;
		else
			offset = 0;
		return offset;
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