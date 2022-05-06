#include "dicttree.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

/**
 * charMapped Function
 * 
 * Sets each valid character to its corresponding index of the "next" array of a dictNode
 * 
 * returns an integer value that corresponds to input char
 **/
int charMapped(char validChar){
    switch(validChar){
        case 'a':
        case 'A':
            return 0;
        case 'b':
        case 'B':
            return 1;
        case 'c': 
        case 'C':
            return 2;
        case 'd':
        case 'D':
            return 3;
        case 'e':
        case 'E':
            return 4;
        case 'f': 
        case 'F':
            return 5;
        case 'g':
        case 'G':
            return 6;
        case 'h':
        case 'H':
            return 7;
        case 'i': 
        case 'I':
            return 8;
        case 'j':
        case 'J':
            return 9;
        case 'k':
        case 'K':
            return 10;
        case 'l':
        case 'L':
            return 11;
        case 'm':
        case 'M':
            return 12;
        case 'n':
        case 'N':
            return 13;
        case 'o':
        case 'O':
            return 14;
        case 'p':
        case 'P':
            return 15;
        case 'q':
        case 'Q':
            return 16;
        case 'r':
        case 'R':
            return 17;
        case 's':
        case 'S':
            return 18;
        case 't':
        case 'T':
            return 19;
        case 'u':
        case 'U':
            return 20;
        case 'v':
        case 'V':
            return 21;
        case 'w':
        case 'W':
            return 22;
        case 'x':
        case 'X':
            return 23;
        case 'y':
        case 'Y':
            return 24;
        case 'z':
        case 'Z':
            return 25;
        case '\'':
            return 26;
        default:
            return -1;
    }
};

/**
 * dictNode Constructor
 * 
 * default constructor for dictNode class
 **/
dictNode::dictNode() {
    isWord = false;
    for(int i = 0; i < NUMOFCHARS; i++)
        next[i] = nullptr;
};

/**
 * Overloaded dictNode Constructor
 * 
 * construct a new dictNode with isWord set to isWordVal function parameter, allows for the isWord to be made true on creation
 **/
dictNode::dictNode(bool isWordVal){
    isWord = isWordVal;
    for(int i = 0; i < NUMOFCHARS; i++)
        next[i] = nullptr;
};

/**
 * Add Function
 * 
 * traverses through dictionary tree and inserts the word into the tree, if strings of characters are already in tree the additional
 * characters are added to the last node
 * 
 * returns true if successful, false otherwise
 **/
bool dictNode::add(const char *remainingChars, const char *wordBeingInserted = nullptr){
    dictNode* currentNode = this; // stores the address of the node that the function was called from as currentNode (the root of the tree)
    int wordLen = strlen(wordBeingInserted); //length of the word being inserted
    int letterValue;  // uses the charMapped function to transform valid characters into an integer that corresponds to its index in a nodes next array (of nodes)

    if (strlen(remainingChars) == strlen(wordBeingInserted)){ //if this is the first char of the array it checks to see if it is made up of valid characters
        for(int i = 0; i < wordLen; i++){ // this for loop checks the word to see it it contains any invalid characters, if so then it returns false and the word isn't added to the dictionary
            int validityOfChars = charMapped(wordBeingInserted[i]);
            if( validityOfChars == -1){
                std::cout << wordBeingInserted;
                std::cout << " failed" << std::endl;
                return false;
            }
        }
    }
    
    for(int i = 0; i < wordLen; i++){
        letterValue = charMapped(remainingChars[0]); //gets the index of the next array that corresponds to a valid character
        if (i != (wordLen - 1)){ // when you are not at the end of the array
            if(currentNode->next[letterValue] == nullptr){ // if the letter is not already part of the dictionary
                dictNode* tempNode = new dictNode(); // creates a new temporary node for the current character that is being inserted into the dictionary
                currentNode->next[letterValue] = tempNode;
                remainingChars++;
                currentNode = currentNode->next[letterValue];
            }
            else { // if the letter is already part of the dictionary
                remainingChars++;
                currentNode = currentNode->next[letterValue];
            }
        }
        else{ //when you are at the last character in the word, meaning isWord needs to be set to true
            if(currentNode->next[letterValue] == nullptr){ // if the letter is not already part of the dictionary
                dictNode* tempNode = new dictNode(true);
                currentNode->next[letterValue] = tempNode;
                currentNode = currentNode->next[letterValue];
            }
            else { // if the letter is already part of the dictionary
                currentNode = currentNode->next[letterValue];
                currentNode->isWord = true;
            }
        }
    };
    return true;
};  


/**
 * findEndingNodeOfAStr Function
 * 
 * used when counting the number of words that have the input string as a prefix,
 * finds the last node of the input string within the dictionary tree structure
 **/
dictNode* dictNode::findEndingNodeOfAStr(const char *remainingStr, const char *strBeingSearched){
    dictNode* currentNode = this; // getting the addr of the root
    int wordLen = strlen(strBeingSearched); // finds the length of the word that is being searched for
    int letterValue; // variable that will hold the vlaue that is returned when trying to find the array index that corresponds with its valid character

    for(int i = 0; i < wordLen; i++){ // for loop that makes loops as many times as there are characters in the word being searched for
        letterValue = charMapped(remainingStr[0]);
        if(currentNode->next[letterValue] != nullptr){ // node is found
            currentNode = currentNode->next[letterValue];
            remainingStr++;
        }
        else{ // node doesnt exist
            return NULL;
        }
    }
    return currentNode;
};

/**
 * countWordsStartingFromANode Function
 * 
 * starting at the node that calls the function, counts the number of words that are within the tree that start with current word
 * that is being looked at
 **/
void dictNode::countWordsStartingFromANode(int &count){
    dictNode* currentNode = this; // getting the address of the node you want to start counting from
    if(currentNode == nullptr) // if there wasnt a word in the dictionary that starts with the word trying to be counted
        return;
    if(currentNode->isWord == true) // count increases if isWord is true at the currentNode
        count++;
    for(int i = 0; i < NUMOFCHARS; i++){ // this iterates through the arrays within the node and covers all possible branches from the initial node that was being counted from
        if (currentNode->next[i] != nullptr)
            currentNode->next[i]->countWordsStartingFromANode(count); // recursively calls itself helping cover all possible branches
    }
};