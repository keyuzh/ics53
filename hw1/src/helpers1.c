// Declare all helper functions for hw1 in this file
#include "helpers1.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char getMode(char *argv[])
{
    return argv[1][1];
}

char* modeWFormat(char* msg)
{
    // printf("%ld\n", strlen(msg));
    char* result = malloc(strlen(msg) + 1);
    char* org = msg;
    char* res = result;

    while (*org != 0)
    {
        *res = tolower(*org);
        if (*org == ' ')
        {
            char* nxt = org;
            while (*nxt == ' ')
            {
                ++nxt;
            }
            org = --nxt;
        }
        ++res;
        ++org;
    }
    *res = '\0';
    
    // printf("MESSAGE:%s\n", result);
    return result;
}

char* modeLFormat(char* msg, int s)
{
    // printf("%ld\n", strlen(msg)+1);
    int msglen = strlen(msg) + 1;
    char* result = malloc(msglen);

    char* org = msg;
    char* res = result;

    while (*org != 0)
    {
        if (isalpha(*org) || (s != 0 && (*org == ' ')))
        {
            *res = tolower(*org);
            ++res;
        }
        ++org;
    }
    *res = '\0';
    
    // printf("MESSAGE:%s\n", result);
    return result;
}

void skipToNextLine()
{
    char t;
    while (1)
    {
        t = tolower(getchar());
        if (t == EOF) { return; }
        printf("%c", t);
        if (t == '\n') { return; }
    }
}

void w_skipToNextLine(Wstats* s)
{
    char t;
    while (1)
    {
        t = getchar();
        if (t == EOF) { return; }
        if (t == '\n') 
        {
            incLine(s);
            return;
        }
    }
}

void skipToNextWord(Wstats* s)
{
    char t;
    while (1)
    {
        t = getchar();
        switch (t)
        {
        case EOF:
            return;
        case '\n':
            incLine(s);
            return;
        case ' ':
            incWord(s);
            return;
        }
    }
}

Wstats modeLprocess(char* msg, Options op)
{
    unsigned int msgLen = strlen(msg);
    char temp;
    unsigned int count = 0;
    Wstats r;
    while (1)
    {
        temp = tolower(getchar());
        if (temp == EOF)
        {
            break;
        }
        if ((*msg != '\0' && *msg != ' ' && temp == *msg)
           || (*msg == ' ' && strchr(",!?.", temp) != NULL && op.s != 0))
        {
            temp = toupper(temp);
            printf("%c", temp);
            if (*msg == ' ')
            {
                printf("%*c", op.s, ' ');
            }
            if (op.o)
            {
                skipToNextLine();
            }
            ++msg;
            ++count;
        }
        else
        {
            printf("%c", temp);
        }
    }
    r.match = count;
    r.exitcode = (count != msgLen);
    return r;
}

void incWord(Wstats* s)
{
    ++s->word;
}

void incLine(Wstats* s)
{
    ++s->line;
    s->word = 1;
}

void readCompare(char* word, Wstats* s, Options op)
{
    char* nextWord = word;
    char* target = nextWord;
    char temp;

    // while (1)
    // {
    //     target = nextWord;
        while (1)
        {
            temp = tolower(getchar());
            // printf("%c", temp);
            if (temp == EOF)
            {
                s->exitcode = 1; // input end before message
                return;
            }
            if ((*target == ' ' || *target == '\0') 
                && (temp == ' ' || temp == '\n'))
            {
                // match whole word
                printf("%d:%d\n", s->line, s->word);
                ++s->match;
                if (*target == '\0') { return; /* entire message is found */ }
                nextWord = ++target;
                if (op.o)
                {
                    w_skipToNextLine(s);
                    continue;
                }
                // if (temp == ' ') { incWord(s); }
                // if (temp == '\n') { incLine(s); }
                temp == ' ' ? incWord(s) : incLine(s);
                continue;
            }
            if (temp == *target)
            {
                ++target;
                continue;
            }
            skipToNextWord(s);
            target = nextWord;
        }
    // }
}

void readNextWordInMessage(char** msg, char* buf)
{
    // skip til non space
    while (**msg == ' ')
    {
        ++*msg;
    }
    // copy til space
    while (**msg != ' ' && **msg != '\0')
    {
        *buf = **msg;
        ++*msg;
        ++buf;
    }
    *buf = '\0';
}

void removeLeadingPunc(char* buf, unsigned int bufSz)
{
    unsigned int from = 0;
    unsigned int to = 0;
    while ((from < bufSz) && ispunct(buf[from]))
    {
        ++from;
    }
    if (from == to) { return; }
    while ((from < bufSz) && (buf[from] != '\0'))
    {
        buf[to] = buf[from];
        ++to;
        ++from;
    }
    buf[to] = '\0';
}

void removeTrailingPunc(char* buf, unsigned int bufSz)
{
    int i = 0;
    // go to the end of string
    while (buf[i] != '\0')
    {
        ++i;
    }
    while ((--i >= 0) && ispunct(buf[i]))
    {
        buf[i] = '\0';
    }
}

char readNextWordFromInput(char* buf, Wstats* s, unsigned int bufSz, Options op)
{
    char nextChar;
    unsigned int index = 0;
    do // skip til non space
    {
        nextChar = tolower(getchar());
        if (nextChar == '\n') { incLine(s); }
    } while (isspace(nextChar) && nextChar != EOF);
    if (op.p)
    {
        // skip leading punctuation
        while (ispunct(nextChar))
        {
            nextChar = tolower(getchar());
            if (nextChar == '\n') { incLine(s); }
        }
    }
    // copy til space
    do
    {
        buf[index] = nextChar;
        nextChar = tolower(getchar());
        if (++index >= bufSz)
        {
            // word longer than buffer, impossible to be a match
            index = op.p ? bufSz-1 : 0;
            // skip til space
            while (!isspace(nextChar) && nextChar != EOF)
            {
                nextChar = getchar();
                // printf("%c", nextChar);
            }
            break;
        }
    } while (!isspace(nextChar) && nextChar != EOF);
    buf[index] = '\0';
    return nextChar;
}

void readCompare2(char* msg, Wstats* s, Options op)
{
    unsigned int inputBufferSize = strlen(msg) + 1;
    char* msgBuffer = malloc(inputBufferSize);
    char* inputBuffer = malloc(inputBufferSize);

    while (1)
    {
        readNextWordInMessage(&msg, msgBuffer);
        if (op.p)
        {
            removeLeadingPunc(msgBuffer, inputBufferSize);
            removeTrailingPunc(msgBuffer, inputBufferSize);
        }
        if (*msgBuffer == '\0')
        {
            break;
        }
        int notMatch = 1;
        // Wstats sCopy = *s;
        while (notMatch)
        {
            char lastChar = readNextWordFromInput(inputBuffer, s, inputBufferSize, op);
            if (op.p) { removeTrailingPunc(inputBuffer, inputBufferSize); }
            if (!strcmp(msgBuffer, inputBuffer))
            {
                printf("%d:%d\n", s->line, s->word);
                ++s->match;
                notMatch = 0;
                if (op.o && lastChar != '\n')
                {
                    w_skipToNextLine(s);
                    break;
                }
            }
            if (lastChar == ' ')
            {
                incWord(s);
            }
            if (lastChar == '\n')
            {
                incLine(s);
            }
            if (lastChar == EOF)
            {
                s->exitcode = 1;
                free(msgBuffer);
                free(inputBuffer);
                return;
            }
        }
        
    }
    free(msgBuffer);
    free(inputBuffer);
}

Wstats modeWprocess(char* msg, Options op)
{
    // char* buf = malloc(sizeof(*msg));
    Wstats stats;
    stats.match = 0;
    stats.line = 1;
    stats.word = 1;
    stats.exitcode = 0;
    
    char* nextWord = msg;
    // readCompare(msg, &stats, op);
    readCompare2(msg, &stats, op);
    return stats;
}

int isModeS(int argc, char* argv[])
{
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-S") == 0)
        {
            return atoi(argv[i+1]);
        }
    }
    return 0;
}

int isModeOther(int argc, char* argv[], const char mode)
{
    char target[2] = "-";
    // strcat(target, mode);
    target[1] = mode;
    // printf("%s\n", target);
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], target) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void invalid()
{
    fprintf(stderr, "%s\n", USAGE_HELPER);
    exit(-1);
}

void checkS(int index, int argc, char* argv[])
{
    if (index >= argc)
    {
        invalid();
    }
    for (int i = 0; i < strlen(argv[index]); i++)
    {
        if (isdigit(argv[index][i]) == 0)
        {
            invalid();
        }
    }
}


void checkOptions(Options* opt, int argc, char* argv[])
{
    if ((argc < 3) || (strcmp(argv[1], "-L") && strcmp(argv[1], "-W")))
    { 
        invalid();
    }

    opt->mode = getMode(argv);
    opt->s = opt->o = opt->p = 0;

    for (int i = 3; i < argc; i++)
    {
        if (!strcmp(argv[i], "-S"))
        {
            checkS(i+1, argc, argv);
            opt->s = 1;
            opt->s = atoi(argv[i+1]);
            ++i;
        }
        else if (!strcmp(argv[i], "-O")) { opt->o = 1; }
        else if (!strcmp(argv[i], "-P")) { opt->p = 1; }
        else { invalid(); }
    }
    
    if ((opt->mode != 'L' && opt->mode != 'W')
        || (opt->s && opt->mode != 'L')
        || (opt->p && opt->mode != 'W'))
    {
        invalid();
    }
}
