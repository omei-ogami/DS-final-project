#define FILE_EXTENSION ".txt"
#include<fstream>
#include<string>
#include<cstring>
#include<algorithm>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<iostream>
#include<chrono>

using namespace std;

//////////////////////////////////////////////////////////////////////// 
// Definition of Trie
////////////////////////////////////////////////////////////////////////

string processing_word = "";

// trie node
struct TrieNode{
	vector<TrieNode*> next = std::vector<TrieNode*>(26, nullptr);
	unordered_set<int> S = {};
	bool isWord = false;

	TrieNode() {}
};

// class for trie
class Trie{
public:

	Trie() {}

	~Trie() {}

	// get root
	TrieNode* getRoot(){
		return root;
	}

	// insert operation
	void insert(string& word, int id){
		int n = word.size();
		TrieNode* cur = root;
		for(int i=0 ; i<n ; i++){
			int c = word[i] - 'a';
			if(!cur->next[c]) cur->next[c] = new TrieNode();
			cur = cur->next[c];
			cur->S.insert(id);
			cur->isWord = (i == n-1)? true : false;
		}
	}

	// search operation
	unordered_set<int> search(string& prefix){
		TrieNode* cur = root;
		int n = prefix.size();
		for(int i=0 ; i<n ; i++){
			int c = prefix[i] - 'a';
			if(!cur->next[c]) return {};
			cur = cur->next[c];
		}
		return cur->S;
	}

private:
	TrieNode* root = new TrieNode();
};

//////////////////////////////////////////////////////////////////////// 
// Data info
////////////////////////////////////////////////////////////////////////

int total;
unordered_map<int, string> TITLE;
unordered_set<string> words;
// matches
unordered_map<string, unordered_set<int>> exact;
unordered_map<string, unordered_set<int>> prefix;
unordered_map<string, unordered_set<int>> suffix;

//////////////////////////////////////////////////////////////////////// 
// Exact search
////////////////////////////////////////////////////////////////////////

unordered_set<int> exact_search(string& target){

	// check history
	if(exact.find(target) != exact.end()) return exact[target];
	return {};
}

//////////////////////////////////////////////////////////////////////// 
// Prefix search
////////////////////////////////////////////////////////////////////////

unordered_set<int> prefix_search(string& target, Trie& t){

	// check history
	if(prefix.find(target) != prefix.end()) return prefix[target];
	unordered_set<int> temp = t.search(target);
	prefix.emplace(target, temp);
	return temp;
}

//////////////////////////////////////////////////////////////////////// 
// Suffix search
////////////////////////////////////////////////////////////////////////

unordered_set<int> suffix_search(string& target, Trie& t){

	// check history
	if(suffix.find(target) != suffix.end()) return suffix[target];
	unordered_set<int> temp = t.search(target);
	suffix.emplace(target, temp);
	return temp;
}

//////////////////////////////////////////////////////////////////////// 
// Utility Func (Parsing)
////////////////////////////////////////////////////////////////////////

// string parser : output vector of strings (words) after parsing
vector<string> word_parse(vector<string> tmp_string){
	vector<string> parse_string;
	for(auto& word : tmp_string){
		string new_str;
    	for(auto &ch : word){
			if(isalpha(ch))
				new_str.push_back(tolower(ch));
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if("" == str) return res;

	char * strs = new char[str.length() + 1] ; 
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while(p) {
		string s = p; 
		res.push_back(s); 
		p = strtok(NULL, d);
	}

	return res;
}

//////////////////////////////////////////////////////////////////////// 
// main function
////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	// TIMER
	auto begin = std::chrono::high_resolution_clock::now();

    // INPUT :
	// 1. data directory in data folder
	// 2. number of txt files
	// 3. output route

    string data_dir = argv[1] + string("/");
	string query = string(argv[2]);
	string output = string(argv[3]);

	// Read File & Parser Example

	string file, title_name, tmp;
	fstream fi;
	vector<string> tmp_string;

	// collect data
	int data_id = 0;

	// assign a trie structure
	Trie prefix_trie, suffix_trie;

	while(1){
		// make string
		string path = data_dir + to_string(data_id) + FILE_EXTENSION;

		// from data_dir get file ....
		// eg : use 0.txt in data directory
		fi.open(path, ios::in);

		// check if file exists and assign words set
		if(!fi) break;
		unordered_set<string> temp;

		// GET TITLENAME
		getline(fi, title_name);
		TITLE[data_id] = title_name;

		// GET TITLENAME WORD ARRAY
		tmp_string = split(title_name, " ");

		vector<string> title = word_parse(tmp_string);

		for(auto &word : title){
			temp.insert(word);
		}

		// GET CONTENT LINE BY LINE
		while(getline(fi, tmp)){

			// GET CONTENT WORD VECTOR
			tmp_string = split(tmp, " ");

			// PARSE CONTENT
			vector<string> content = word_parse(tmp_string);

			for(auto &word : content){
				temp.insert(word);
			}
		}

		for(auto word : temp){
			exact[word].insert(data_id);
			prefix_trie.insert(word, data_id);
			reverse(word.begin(), word.end());
			suffix_trie.insert(word, data_id);
		}

		// CLOSE FILE
		fi.close();
		data_id++;
	}

	total = data_id;

	// query for test
	string path = query, request;
	fi.open(path, ios::in);
	while(getline(fi, request)){
		reverse(request.begin(), request.end());
		unordered_set<int> res = suffix_search(request, suffix_trie);
		for(auto &id : res){
			cout << TITLE[id] << endl;
		}
		cout << "======================" << endl;
	}
	fi.close();

	auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
}


// 1. UPPERCASE CHARACTER & LOWERCASE CHARACTER ARE SEEN AS SAME.
// 2. FOR SPECIAL CHARACTER OR DIGITS IN CONTENT OR TITLE -> PLEASE JUST IGNORE, YOU WONT NEED TO CONSIDER IT.
//    EG : "AB?AB" WILL BE SEEN AS "ABAB", "I AM SO SURPRISE!" WILL BE SEEN AS WORD ARRAY AS ["I", "AM", "SO", "SURPRISE"].
// 3. THE OPERATOR IN "QUERY.TXT" IS LEFT ASSOCIATIVE
//    EG : A + B / C == (A + B) / C

//

//////////////////////////////////////////////////////////

// To complile the file use the below command
// g++ -std=c++17 -O2 -o essay_search ./main.cpp
// ./essay_search data-more test.txt Q1
// or
// g++ -std=c++17 -O2 -o essay-search ./*.cpp
// ./essay-search.exe data-more test.txt Q1

