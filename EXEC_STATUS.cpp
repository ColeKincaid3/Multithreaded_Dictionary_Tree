#include "EXEC_STATUS.h"

/**
 * EXEC_STATUS Constructor
 * 
 * takes in a pointer to a dictNode (root), numOfProgressMarks, hashmarkInterval, minNumOfOWordsWithAPrefixForPrinting,
 * and the file paths to both the dictionary source and the test file
 **/
EXEC_STATUS::EXEC_STATUS(dictNode *rootOfTree, int numProgMarks, int hashmarkInterv, int minCountOfWordsWithAPrefixForPrinting, char* srcFilePath, char* testFilePath){
    dictRootNode = rootOfTree;
    numOfProgressMarks = numProgMarks;
    hashmarkInterval = hashmarkInterv;
    minNumOfWordsWithAPrefixForPrinting = minCountOfWordsWithAPrefixForPrinting;
    filePath[DICTSRCFILEINDEX] = srcFilePath;
    filePath[TESTFILEINDEX] = testFilePath;
}