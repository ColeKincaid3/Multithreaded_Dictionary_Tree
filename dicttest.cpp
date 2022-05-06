#include <pthread.h>
#include "EXEC_STATUS.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

using namespace std;

/**
 * numCharsInFile Function
 *  
 * Uses the stat() function to determine how many bytes/chars a file is (1 char = 1 byte)
 **/
int numCharsInFile(char* fileName){
    struct stat currFileStats;
    stat(fileName, &currFileStats);
    return currFileStats.st_size;
}

/**
 * Populate Tree Function
 * 
 * creates a dictionary tree structure within the sharedData and populates it with the words from file in command line arguments (typically dictionarysource.txt)
 **/
void* populateDictionaryTree( void *ptr ){
    EXEC_STATUS *sharedData;
    sharedData = (EXEC_STATUS *) ptr;

    // opening dictionary source file to begin inserting words into the tree
    ifstream dictStream(sharedData->filePath[DICTSRCFILEINDEX]);
    dictNode tmpRoot;
    string currDictLine;
    
    // delimiters used to seperate words that are read in from the 
    const char *delimeters = "\n\r !\"#$%&()*+,-./0123456789:;<=>?@[\\]^_`{|}~";

    // inserting each word from dictionary source file line by line
    while(getline(dictStream, currDictLine)){
        char* addrCurrDictLine = &currDictLine[0];
        char* word = strtok(addrCurrDictLine,delimeters);
        tmpRoot.add(word, word);
        sharedData->numOfCharsProcessedFromFile[DICTSRCFILEINDEX] += currDictLine.length() + 1;
        sharedData->wordCountInFile[DICTSRCFILEINDEX] = sharedData->wordCountInFile[DICTSRCFILEINDEX] + 1;
        word = strtok(NULL,delimeters);
    }

    dictStream.close();
    *sharedData->dictRootNode = tmpRoot;
    sharedData->taskCompleted[DICTSRCFILEINDEX] = true; //populate tree thread is complete
    pthread_exit(0); // terminates thread
}

/**
 * Count Words Function
 * 
 * looks at the dictionary tree within the sharedData and for each word from the test file in the command line arguments count the number of words within the
 * dictionary tree that have the given word as a prefix and prints it to a file called countwords_output.txt
 * 
 * it only prints words from the test file and their count if their count is greater than or equal to the minNumOfWordsWithPrefixForPrinting variable that is 
 * in the sharedData
 **/
void* countWordsFunction( void *ptr ){

    EXEC_STATUS *sharedData;
    sharedData = (EXEC_STATUS *) ptr;

    const char *delimeters = "\n\r !\"#$%&()*+,-./0123456789:;<=>?@[\\]^_`{|}~";

    ifstream textFile(sharedData->filePath[TESTFILEINDEX]);
    string currTFLine;

    ofstream outputFile;
    outputFile.open("countwords_output.txt");

    while(sharedData->taskCompleted[DICTSRCFILEINDEX] == false); // this blocks the thread from continueing until the dict tree has been populated

    dictNode tmpRoot = *sharedData->dictRootNode;

    while(getline(textFile, currTFLine)){
        char* addrCurrTFLine = &currTFLine[0]; 
        char* word = strtok(addrCurrTFLine, delimeters); 
        
        while(word != nullptr){ 
            int tempCounter = 0;
            dictNode* temp = tmpRoot.findEndingNodeOfAStr(word,word); // finds the node in the dictionary that corresponds to the current words end
            temp->countWordsStartingFromANode(tempCounter); // takes node that is returned above and counts the amount of words within the dictionary that start with the current word
            if (tempCounter >= sharedData->minNumOfWordsWithAPrefixForPrinting) // checks to see if it is great or equal to the min num of words with a prefix variable
                outputFile << word << " " << tempCounter << endl;
            sharedData->wordCountInFile[TESTFILEINDEX] = sharedData->wordCountInFile[TESTFILEINDEX] + 1; // increases the word count variable within the sharedData for the test file
            word = strtok(NULL, delimeters);
        }
        sharedData->numOfCharsProcessedFromFile[TESTFILEINDEX] += currTFLine.length() + 1; // increases the # of chars processed in the test file in the sharedData
    }

    sharedData->taskCompleted[TESTFILEINDEX] = true; // countwords thread is complete
    textFile.close();
    outputFile.close();
    pthread_exit(0); // terminates the thread
}

int main(int argc, char **argv){

    /**
     * Error checking for the correct number of arguments (< 3 and > 9) 
     **/
    {
        if (argc < 3){
            cout << "Not enough command line arguments." << endl;
            exit(1);
        }
        if (argc > 9){
            cout << "Too many command line arguments." << endl;
            exit(1);
        }
    }
    

    /**
     * Creation of the root node of the dictionary tree
     **/
    dictNode root;
    dictNode* addrRoot = &root;
    
    int option;

    /**
     * Temp variables for the optional flags
     **/
    int numMarks, hMarkInterv, minNumWords;

    numMarks = hMarkInterv = minNumWords = -1;

    while ((option = getopt(argc, argv, "p:h:n:")) != -1){ // uses getopt to process the optional command line arguments
        switch(option){
            case 'p': // if the argument is a -p then set the numMarks to the value following -p
                numMarks = stoi(optarg);
                break;
            case 'h': // if the argument is a -h then set the hMarkInterv to the value following -h
                hMarkInterv = stoi(optarg);
                break;
            case 'n': // if the argument is a -n then set the minNumWords to the value following -n
                minNumWords = stoi(optarg);
                break;
            case ':':
                break;
            case '?':
                break;
        }
    }

    /**
     * If no optional flags are used (temp variables are equal to -1) then use defined default values 
     **/
    {
        if (numMarks == -1)
            numMarks = DEFAULT_NUMOF_MARKS;
        if (hMarkInterv == -1)
            hMarkInterv = DEFAULT_HASHMARKINTERVAL;
        if (minNumWords == -1)
            minNumWords = DEFAULT_MINNUM_OFWORDS_WITHAPREFIX;
    }
    
    /**
     * Creation of the shared data structure for all of the threads
     **/
    EXEC_STATUS *sharedData = new EXEC_STATUS(addrRoot, numMarks, hMarkInterv, minNumWords, argv[optind], argv[optind+1]); // on edoras change argv[1] to argv[optind] and argv[2] to argv[optind+1]

    /**
     * Getting the number of characters in each file using stat() 
     **/
    sharedData->totalNumOfCharsInFile[DICTSRCFILEINDEX] = numCharsInFile((char *) sharedData->filePath[DICTSRCFILEINDEX]);
    sharedData->totalNumOfCharsInFile[TESTFILEINDEX] = numCharsInFile((char *) sharedData->filePath[TESTFILEINDEX]);

    /**
     * Error checking for the optional flags
     **/
    {
        if (numMarks < 10){
            cout<< "Number of progress marks must be a number and at least 10 then exit." << endl;
            exit(1);
        }
        if (hMarkInterv <= 0 || hMarkInterv > 10){
            cout << "Hash mark interval for progess must be a number, greater than 0, and less than or equal to 10." << endl;
            exit(1);
        }
        if (minNumWords < -1){
            cout << "Minimum number of words with a prefix has to be greater than or equal to zero." << endl;
            exit(1);
        }
    }
    

    /**
     * Checks to see if missing one of the mandatory arguments 
     **/
    if (argv[optind] == NULL || argv[optind+1] == NULL){
        cout << "Mandatory arguments are missing" << endl;
        exit(1);
    }

    /**
     * Checks to see if both mandatory argument files exist and can be opened 
     */
    {
        if (access(argv[optind], F_OK) != 0){ //
            cout << "Unable to open <<" << argv[1] << ">>" << endl;
            exit(1);
        }
        if (access(argv[optind+1], F_OK) != 0){ 
            cout << "Unable to open <<" << argv[2] << ">>" <<endl;
            exit(1);
        }
    }
    
    pthread_t thread1, thread2;

    /**
     * Creation of populatetree thread
     **/
    pthread_create(&thread1, NULL, &populateDictionaryTree, (void *) sharedData);

    /**
     * Creation of countwords thread
     **/
    pthread_create(&thread2, NULL, &countWordsFunction, (void *) sharedData);

    int numTicksPrinted;
    int bytesPerTick;

    /**
     * Progress bar for the populatetree worker thread
     **/
    {
        numTicksPrinted = 1;
        bytesPerTick = sharedData->totalNumOfCharsInFile[DICTSRCFILEINDEX] / sharedData->numOfProgressMarks; // num bytes per tick is the total num bytes divided by total num of progress marks  
        while (sharedData->taskCompleted[DICTSRCFILEINDEX] == false){ // runs while the populate tree thread is not complete
            if(sharedData->numOfCharsProcessedFromFile[DICTSRCFILEINDEX] > (long)(numTicksPrinted * bytesPerTick)){ // checks to see if the # of processed chars processed passes a threshold that causes another tick to be printed
                if (numTicksPrinted % sharedData->hashmarkInterval == 0) // does a modulous calculation to see if the current tickmark needs to be a # 
                    cout.flush() << "#"; // prints # in the progress bar
                else
                    cout.flush() << "-"; // prints - in the progress bar
                numTicksPrinted++; // increases the numTicksPrinted because it is used for calulations for the next tick
            }
        }
        cout << endl;
        cout << "There are " << sharedData->wordCountInFile[DICTSRCFILEINDEX] << " words in " << sharedData->filePath[DICTSRCFILEINDEX] <<  "." << endl; // prints the total number of words within the dictionary source file
    }
    
    /**
     * Progress bar for the countwords worker thread
     **/
    {
        numTicksPrinted = 1;
        bytesPerTick = sharedData->totalNumOfCharsInFile[TESTFILEINDEX] / sharedData->numOfProgressMarks; // num bytes per tick is the total num bytes divided by total num of progress marks  
        while(sharedData->taskCompleted[TESTFILEINDEX] == false){ // runs while the countwords thread is not complete
            if(sharedData->numOfCharsProcessedFromFile[TESTFILEINDEX] > (long)(numTicksPrinted * bytesPerTick)){ // checks to see if the # of processed chars processed passes a threshold that causes another tick to be printed
                if (numTicksPrinted % sharedData->hashmarkInterval == 0) // does a modulous calculation to see if the current tickmark needs to be a # 
                    cout.flush() << "#"; // prints # in the progress bar
                else
                    cout.flush() << "-"; // prints - in the progress bar
                numTicksPrinted++; // increases the numTicksPrinted because it is used for calulations for the next tick
            }
        }
        cout << endl;
        cout << "There are " << sharedData->wordCountInFile[TESTFILEINDEX] << " words in " << sharedData->filePath[TESTFILEINDEX] << "." << endl; // prints the total number of words within the Test file
    }
}
