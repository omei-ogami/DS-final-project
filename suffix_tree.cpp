#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct TreeNode{
    vector<TreeNode*> next = vector<TreeNode*>(26, nullptr);
    TreeNode* suffixlink;
    string suffix = "";
    int start = -1, end = -1;
    TreeNode(TreeNode* link, string suffix, int start, int end) : suffixlink(link), suffix(suffix), start(start), end(end) {}
};

class SuffixTree{
public:

    void insert(string& word){
        int n = word.size();
        rootEnd = -1;
        root = new TreeNode(root, "#", -1);
        activeNode = root;
        inserting_word = word;
        for(int i=0 ; i<n ; i++){
            processing += word[i];
            extend(i);
        }
    }

    void extend(int pos){
        leafEND = pos;
        remain++;
        lastNode = nullptr;
        while(remain > 0){
            if(activeLength == 0) activeEdge = inserting_word[pos] - 'a';
            if(activeNode->next[activeEdge] == nullptr){
                string s = "" + inserting_word[pos];
                activeNode->next[activeEdge] = new TreeNode(root, s, pos);
                if(lastNode != nullptr){
                    lastNode->suffixlink = activeNode;
                    lastNode = nullptr;
                }
            }
            else{
                TreeNode* nxt = activeNode->next[activeEdge];
                if(walkDown(nxt)) continue;
                if(inserting_word[nxt->start + activeLength] == inserting_word[pos]){
                    if(lastNode && activeNode != root){
                        lastNode->suffixlink = activeNode;
                        lastNode = nullptr;
                    }
                    activeLength++;
                    break;
                }
                splitEnd = nxt->start + activeLength - 1;
                TreeNode* split = new TreeNode(root, )
            }
        }
    }

    bool walkDown(TreeNode* cur){
        int len = cur->suffix.size();
        if(activeLength >= len){
            activeEdge = inserting_word[activeEdge + len] - 'a';
            activeLength -= len;
            activeNode = cur;
            return true;
        }
        return false;
    }

private:

    TreeNode* root = nullptr;
    TreeNode* lastNode = nullptr;
    TreeNode* activeNode = nullptr;
    string processing = "";
    string inserting_word = "";
    int activeEdge = -1;
    int activeLength = 0;
    int remain = 0;
    int leafEND = -1;
    int rootEnd;
    int splitEnd;
};