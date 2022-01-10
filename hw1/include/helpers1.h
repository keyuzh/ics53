// Declare all helper functions for hw1 in this file
# ifndef HW1_HELPERS
# define HW1_HELPERS

#define USAGE_HELPER "usage: 53secret -L | -W MESSAGE [-S NUM] [-O] [-P]"

typedef struct
{
    int s, o, p;
    char mode;
} Options;

typedef struct 
{
    unsigned int match, line, word;
    int exitcode;
} Stats;

char getMode(char *argv[]);

char* modeWFormat(char* msg);
char* modeLFormat(char* msg, int s);

void skipToNextLine();
void w_skipToNextLine(Stats* s);
void skipToNextWord(Stats* s);

Stats modeLprocess(char* msg, Options op);

void incWord(Stats* s);
void incLine(Stats* s);

void readNextWordInMessage(char** msg, char* buf);

void removeLeadingPunc(char* buf, unsigned int bufSz);
void removeTrailingPunc(char* buf, unsigned int bufSz);

char readNextWordFromInput(char* buf, Stats* s, unsigned int bufSz, Options op);

void readCompare(char* msg, Stats* s, Options op);

Stats modeWprocess(char* msg, Options op);

void invalid();
void checkS(int index, int argc, char* argv[]);
void checkOptions(Options* opt, int argc, char* argv[]);

# endif