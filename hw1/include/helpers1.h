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
} Wstats;

// typedef struct
// {
//     unsigned int numHidden;
//     int exitcode;
// } Result;


char getMode(char *argv[]);

char* modeWFormat(char* msg);
char* modeLFormat(char* msg, int s);

Wstats modeLprocess(char* msg, Options op);

int isModeS(int argc, char* argv[]);

int isModeOther(int argc, char* argv[], const char mode);

void incWord(Wstats* s);
void incLine(Wstats* s);

Wstats modeWprocess(char* msg, Options op);

void checkOptions(Options* opt, int argc, char* argv[]);

void removeLeadingPunc(char* buf, unsigned int bufSz);

# endif