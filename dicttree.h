
/**
 * Valid characters are: a-z and ' 
 **/
#define NUMOFCHARS 27

/**
 * Data structure for the dictionary tree
 * 
 * Each node has an array called next that has an element for each of the corresponding valid characters
 * and also has a flag that is set if the current node corresponds to a character sequence that is a word.
 **/
class dictNode {
    public:
        bool isWord; // isWord flag that keeps track if the node is the last character of a word
        dictNode *next[NUMOFCHARS]; // array of all 
        dictNode();
        dictNode(bool isWordVal);

    bool add(const char *remainingChars, const char *wordBeingInserted); // adds a word into the dictionary tree

    dictNode* findEndingNodeOfAStr(const char *remainingStr, const char *strBeingSearched); // finds the node that contains the last character of the string being searched

    void countWordsStartingFromANode(int &count); // counts the number of words within the tree that starts from a node
};
