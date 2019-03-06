#ifndef COMPRESS
#define COMPRESS

#include <iostream>
#include <vector>
#include <boost/dynamic_bitset.hpp>

#include <map>
#include <bitset>
#include <queue>
#include <string>
#include <unordered_set>
#include <stack>

/*
This code was written by Samuel Hapgood
*/


class heapNode{
	public:
		heapNode(char tempChar, unsigned long tempFrequency){
			left = right = NULL;
			this->character = tempChar;
			this->occurances = tempFrequency;
		}

		char getChar(){
			return this->character;
		}
	
		unsigned long getFrequency(){
			return this->occurances;
		}

		char character;
		unsigned long occurances;
		int internalNode;
		heapNode *left, *right;
};

struct findLesser{
	bool operator()(heapNode* l, heapNode* r){
		return (l->getFrequency() > r->getFrequency());
	}
};

void generateHuffmanCode(struct heapNode *root, std::string encoding, std::map<char, std::string> &hmap){
	//if the internal node is 1 then in is some internal node that holds a frequency not a character
	if(root == NULL){
		return;
	}

	if(root->internalNode != 1){
		//map our character to an encoded string
		hmap[root->character] = encoding;
	}
	
	generateHuffmanCode(root->left, encoding + "0", hmap);
	generateHuffmanCode(root->right, encoding + "1",hmap);
}

int compress(boost::dynamic_bitset<unsigned char>& input, boost::dynamic_bitset<unsigned char>& output){
// Take the input, do compression and place the output in the output bitset

	std::priority_queue<heapNode*, std::vector<heapNode*>, findLesser> minHeap;
	std::map<char, unsigned long> frequencyMap;
	std::vector<char> data;
	std::map<char, std::string> huffmanEncoding;

	for(boost::dynamic_bitset<>::size_type i = 0; i < input.size(); i++){
		//read in bits 1 byte at a time, convert the byte to a char and
		//add it to the data vector. 

		if(i <= input.size()){
			std::bitset<8> mySet;
			for(boost::dynamic_bitset<>::size_type j = 0; j < 8; j++){
				if(i < input.size())
					mySet[j] = input[i];
				if(j != 7)
					i++;
			}
			unsigned long toInt = mySet.to_ulong();
			char character = static_cast<char>( toInt );
			data.push_back(character);	
			frequencyMap[character]++;
		}}//endfor
	
	struct heapNode *top, *left, *right;
	
	for(auto it = frequencyMap.begin(); it != frequencyMap.end(); ++it){
		//count the frequencies of the characters
		minHeap.push(new heapNode(it->first, it->second));
	}
	
	while(minHeap.size() != 1){
		//populates a minimum heap to create our huffman code;
		left = minHeap.top();
		//create one node have it point to the left which will subsequently represent a 0
		minHeap.pop();
		right = minHeap.top();
		//create another node have it point to the right which will subsequently represent a 1
		minHeap.pop();
		
		top = new heapNode(' ', left->occurances + right->occurances);
		//create a new node that is the sum of the frequencies of left and right
		top->internalNode = 1;
		//we now note that this node is not a leaf character
		top->left = left;
		top->right = right;
		//have this node point to the left and right nodes
		minHeap.push(top);
		//put this node back on the heap
	}

	generateHuffmanCode(minHeap.top(), "", huffmanEncoding);

	struct heapNode *head = minHeap.top();
	struct heapNode *current = head;
	std::unordered_set<heapNode*> charMap;
	std::vector<char> myChars;
	
	while(current && charMap.find(current) == charMap.end()){
		//get the huffman tree in post order traversal to compress it
		if(current->left && charMap.find(current->left) == charMap.end())
			current = current->left;
		else if(current->right && charMap.find(current->right) == charMap.end())
			current = current->right;
		else{
			if(current->internalNode != 1){
				//A 1 signifies that it is not an internal node
				myChars.push_back('1');
				myChars.push_back(current->character);
			}else{
				//a character signifies it is an internal node
				myChars.push_back('0');
			}
			charMap.insert(current);
			current = head; 
		}		
	}
	
	unsigned long treeLayoutSize = myChars.size();
	
	std::bitset<32> treeSize;
	for(int i = 0; i < 32; i++){
		treeSize[i] = (treeLayoutSize >> i) & 1;
	}

	//push back the size needed to encode the huffman tree into the output set so we know how much to read in
	//in the decompress method so we can regenerate the tree
	for(int i = 0; i < 32; i++){
		output.push_back(treeSize[i]);
	}

	//push the nodes of the huffman tree in post-order traversal into the output set so we know how to 
	//recreate the huffman tree
	for(unsigned long i = 0; i < myChars.size(); i++){
		char c = myChars[i];

		for(int j = 0; j < 8; j++){
			output.push_back((c >> j) & 1);
		}
	}	

	for(unsigned long i = 0; i < data.size(); i++){
		//go back through the original input as a char string and assign the encoded character to the output
		std::string str = huffmanEncoding[data[i]];
		for(unsigned int j = 0; j < str.length(); j++){
			if(str.at(j) == '0')
				output.push_back(0);
			else if(str.at(j) == '1')
				output.push_back(1);
		}
	}//endfor
	
	return true;
}

int decompress(boost::dynamic_bitset<unsigned char>& input, boost::dynamic_bitset<unsigned char>& output){
// Take the input, do decompression and place the output in the output bitset

	std::vector<char> treeSetup;

	//read in the first 32 bits that told us how big our tree was going to be
	unsigned long huffSize = 0;
	std::bitset<32> getSize;
	for(unsigned long i = 0; i < 32; i++){
		getSize[i] = input[i];
	}
	huffSize = getSize.to_ulong();
	huffSize = (huffSize * 8) + 32;

	//read in the huffman tree nodes we defined by in our size input bits which are input[32, huffSize + 32 - 1]
	//nodes storing the characters in a character vector
	for(unsigned long i = 32; i < huffSize; i++){
		std::bitset<8> temp;
		for(boost::dynamic_bitset<>::size_type j = 0; j < 8; j++){
			temp[j] = input[i];
			if(j != 7)
				i++;
		}
		unsigned long toInt = temp.to_ulong();
		char character = static_cast<char>( toInt );
		treeSetup.push_back(character);
	}

	std::stack<heapNode*> myStack;
	struct heapNode *top;

	//use a stack to go through out tree setup vector, remember that its in post-order and then
	//rebuild the tree. After the tree is build we can use the tree to decode the rest of the data 
	for(unsigned int i = 0; i < treeSetup.size(); i++){
		if(treeSetup[i] == '1'){
			struct heapNode *node = new heapNode(treeSetup[i+1], 0);
			myStack.push(node);
			i+=1;
		}else if((treeSetup[i] == '0') && (myStack.size() != 1)){
			top = new heapNode(' ', 0);
			top->internalNode = 1;
			top->right = myStack.top();
			myStack.pop();
			top->left = myStack.top();
			myStack.pop();
			myStack.push(top);	
		}
	}

	//the top node is the root of the huffman tree. Go down from left to right to decode the data
	//return to root node after we found a character in the tree and then repeat	
	unsigned long i = huffSize;
	struct heapNode *currentNode = top;
	for( ; i < input.size(); i++){
		//traverse through the tree until we get to a leaf
		//a leaf in the tree is a char 
		if(input[i] == 0){
			//if input is 0 we traverse down the the tree left
			currentNode = currentNode->left;
		}else{
			//if input is 1 we traverse down the tree right
			currentNode = currentNode->right;
		}
		
		if((currentNode->left == NULL) && (currentNode->right == NULL)){
			//get the character at the leaf value and push back its value into output bitset
			char c = currentNode->character;
			for(int j = 0; j < 8; j++){
				output.push_back((c >> j) & 1);
			}
			currentNode = top;	
		}
	}

	return true;
}

#endif
