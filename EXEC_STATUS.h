#include "dicttree.h"

/**
 * Shared constants among the three threads
 **/
#define NUMOFFILES 2
#define DICTSRCFILEINDEX 0
#define TESTFILEINDEX 1

/**
 * Default and minimum number of progress marks for representing 100% progress
 **/
#define DEFAULT_NUMOF_MARKS 50
#define MIN_NUMOF_MARKS 10

/**
 * Default interval for hash mark placement in the progress bar
 **/
#define DEFAULT_HASHMARKINTERVAL 10

/**
 * Default minimum number of dictionary words that start from a prefix for printing
 * and writing to the output
 **/
#define DEFAULT_MINNUM_OFWORDS_WITHAPREFIX 1

/**
 * Data structure used for common data sharing between threads 
 **/
class EXEC_STATUS {
    public:
        dictNode *dictRootNode;
        long totalNumOfCharsInFile[NUMOFFILES];
        long numOfCharsProcessedFromFile[NUMOFFILES];
        long wordCountInFile[NUMOFFILES];
        bool taskCompleted[NUMOFFILES];
        EXEC_STATUS(dictNode *rootOfTree, int numProgMarks, int hashmarkInterv, int minCountOfWordsWithAPrefixForPrinting, char* srcFilePath, char* testFilePath);

    int numOfProgressMarks;
    int hashmarkInterval;
    int minNumOfWordsWithAPrefixForPrinting;
    const char *filePath[NUMOFFILES];
};

