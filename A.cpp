#define FILE_EXTENSION ".txt"
#include<fstream>
#include<string>
#include<cstring>
#include<algorithm>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<set>
#include<iostream>
#include<chrono>

using namespace std;

//////////////////////////////////////////////////////////////////////// 
// Data info
////////////////////////////////////////////////////////////////////////

int longest = 0;
// infos
unordered_map<int, string> TITLE;
unordered_set<string> words;
// matches
unordered_map<string, unordered_set<int>> exact;

//////////////////////////////////////////////////////////////////////// 
// Definition of Trie 
////////////////////////////////////////////////////////////////////////

string processing_word = "";
unordered_set<int> ans;

// trie node
struct TrieNode{
	unordered_map<char, TrieNode*> next;
	unordered_set<int> S = {};
	string prefix = "";
	bool isWord = false;
	int longest_len = 0;

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
			char c = word[i];
			if(cur->next.find(c) == cur->next.end()){
				cur->next[c] = new TrieNode();
				cur->next[c]->prefix = cur->prefix + c;
			}
			cur = cur->next[c];
			if(n > cur->longest_len) cur->longest_len = n;
			cur->S.insert(id);
			cur->isWord = (i == n-1)? true : false;
		}
	}

	// search operation
	unordered_set<int> search(string& prefix){
		TrieNode* cur = root;
		int n = prefix.size();
		for(int i=0 ; i<n ; i++){
			char c = prefix[i];
			if(cur->next.find(c) == cur->next.end()) return {};
			cur = cur->next[c];
		}
		return cur->S;
	}

	// match operation
	void match(string& patterm, TrieNode* cur, int i, int reg_len, int n){
		if(i + reg_len > cur->longest_len && cur != root){
			//cout << "STOP!" << endl;
			return;
		}
		if(i == n){
			ans.merge(exact[cur->prefix]);
			return;
		}
		if(patterm[i] == '*'){
			if(cur->next.find(patterm[i+1]) != cur->next.end()){
				match(patterm, cur, i+1, reg_len, n);
			}
			for(auto nxt : cur->next){
				match(patterm, nxt.second, i, reg_len, n);
			}
		}
		else{
			if(i == n-2 && patterm[n-1] == '*'){
				if(cur->next.find(patterm[i]) != cur->next.end())
                    ans.merge(cur->next[patterm[i]]->S);
			}
			else if(cur->next.find(patterm[i]) != cur->next.end()){
				match(patterm, cur->next[patterm[i]], i+1, reg_len-1, n);
			}
		}
	}

private:
	TrieNode* root = new TrieNode();
};

struct MatchNode{
	TrieNode* node = nullptr;
	int i = -1, reg_len = -1;

	MatchNode(TrieNode* cur, int i, int reg_len) : node(cur), i(i), reg_len(reg_len) {}
};

//////////////////////////////////////////////////////////////////////// 
// Exact search
////////////////////////////////////////////////////////////////////////

unordered_set<int> exact_search(string& target){

	// check history
	if(exact.find(target) != exact.end()) return exact[target];
	return {};
}

// assign trie structures
Trie prefix_trie, suffix_trie;

//////////////////////////////////////////////////////////////////////// 
// Prefix search
////////////////////////////////////////////////////////////////////////

unordered_set<int> prefix_search(string& target, Trie& t){

	// check history
	unordered_set<int> temp = t.search(target);
	return temp;
}

//////////////////////////////////////////////////////////////////////// 
// Suffix search
////////////////////////////////////////////////////////////////////////

unordered_set<int> suffix_search(string& target, Trie& t){

	// check history
	unordered_set<int> temp = t.search(target);
	return temp;
}

//////////////////////////////////////////////////////////////////////// 
// Wildcard search
////////////////////////////////////////////////////////////////////////

unordered_set<int> wildcard_search(string& target, Trie& t){

	// check history
	ans.clear();
	int n = target.size(), reg_len = 0;
	for(int i=0 ; i<n ; i++){
		if(target[i] != '*') reg_len++;
	}
	t.match(target, t.getRoot(), 0, reg_len, n);
	return ans;
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
// set operations & request
////////////////////////////////////////////////////////////////////////

unordered_set<int> solve(string& s){
	int n = s.size();
	string res = s.substr(1, n-2);
	if(s[0] == '\"') return exact_search(res);
	else if(s[0] == '*') {\
		reverse(res.begin(), res.end());
		return suffix_search(res, suffix_trie);
	}
	else if(s[0] == '<'){
		if(res[0] != '*') return wildcard_search(res, prefix_trie);
		if(res.back() != '*'){
			reverse(res.begin(), res.end());
			return wildcard_search(res, suffix_trie);
		}
		else return wildcard_search(res, prefix_trie); 
	}
	else return prefix_search(s, prefix_trie);
}

void opAND(unordered_set<int>& A, unordered_set<int>& B){
	for(auto i : A){
		if(B.find(i) == B.end()) A.erase(i);
	}
	return;
}

void opOR(unordered_set<int>& A, unordered_set<int>& B){
	for(auto i : B){
		A.insert(i);
	}
	return;
}

void opEXCLUDE(unordered_set<int>& A, unordered_set<int>& B){
	for(auto i : B){
		A.erase(i);
	}
	return;
}

//////////////////////////////////////////////////////////////////////// 
// main function
////////////////////////////////////////////////////////////////////////

int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	// TIMER
	auto begin = std::chrono::high_resolution_clock::now();

    // INPUT :
	// 1. data directory in data folder
	// 2. number of txt files
	// 3. output route

    string data_dir = "data-more" + string("/");
	string query = "test.txt";
	string output = "Q1.txt";

	// Read File & Parser Example

	string file, title_name, tmp;
	fstream fi, ofi;
	vector<string> tmp_string;

	// collect data
	int data_id = 0;

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

	// query for test
	int idx = 0;
	string path = query, request;
	fi.open(path, ios::in);
	ofi.open(output, ios::out);
	while(getline(fi, request)){
		vector<string> req = split(request, " ");
		unordered_set<int> A = solve(req[0]);
		// 165
		if(idx == 165){
			cout << req[0] << endl;
			cout << "A: ";
			for(auto i : A) cout << i << " ";
			cout << endl;
		}
		A = solve(req[0]);
		for(int i=1 ; i<req.size() ; i+=2){
			auto B = solve(req[i+1]);
			if(idx == 165){
				cout << "B: ";
				for(auto i : B) cout << i << " ";
				cout << endl;
			}
			if(req[i] == "+") opAND(A, B);
			else if(req[i] == "/") opOR(A, B);
			else if(req[i] == "-") opEXCLUDE(A, B);
			if(idx == 165){
				cout << "A: ";
				for(auto i : A) cout << i << " ";
				cout << endl;
			}
		}
		set<int> res(A.begin(), A.end());
		if(res.empty()) ofi << "Not Found!" << endl;
		for(auto &id : res){
			ofi << TITLE[id] << endl;
		}
		idx++;
	}
	fi.close();
	ofi.close();

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
// g++ -std=c++17 -O2 -o tA ./A.cpp
// ./tA data-more query_more.txt Q4.txt
// ./tA data-more test.txt Q3.txt
// or
// g++ -std=c++17 -O2 -o essay-search ./*.cpp
// ./essay-search.exe data-more test.txt Q1

