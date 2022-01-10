// Declare all helper functions for hw1 in this file
#include "helpers1.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char getMode(char *argv[])
{
    return argv[1][1];
}

char* modeWFormat(char* msg)
{
    char* result = malloc(strlen(msg) + 1);
    char* org = msg;
    char* res = result;
    while (*org != '\0')
    {
        *res = tolower(*org);
        if (*org == ' ')
        {
            char* nxt = org;
            while (*nxt == ' ')
            {
                // skip multiple spaces
                ++nxt;
            }
            org = --nxt;
        }
        ++res;
        ++org;
    }
    *res = '\0';
    return result;
}

char* modeLFormat(char* msg, int s)
{
    char* result = malloc(strlen(msg) + 1);
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

void w_skipToNextLine(Stats* s)
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

void skipToNextWord(Stats* s)
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

Stats modeLprocess(char* msg, Options op)
{
    unsigned int msgLen = strlen(msg);
    unsigned int count = 0;
    char temp;
    Stats r;
    while (1)
    {
        temp = tolower(getchar());
        if (temp == EOF) { break; }
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
        else { printf("%c", temp); }
    }
    r.match = count;
    r.exitcode = (count != msgLen);
    return r;
}

void incWord(Stats* s)
{
    ++s->word;
}

void incLine(Stats* s)
{
    ++s->line;
    s->word = 1;
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

char readNextWordFromInput(char* buf, Stats* s, unsigned int bufSz, Options op)
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

void readCompare(char* msg, Stats* s, Options op)
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
        if (*msgBuffer == '\0') { break; }
        int notMatch = 1;
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
            switch (lastChar)
            {
            case ' ':
                incWord(s);
                break;
            case '\n':
                incLine(s);
                break;
            case EOF:
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

Stats modeWprocess(char* msg, Options op)
{
    Stats stats;
    stats.match = 0;
    stats.line = 1;
    stats.word = 1;
    stats.exitcode = 0;
    
    char* nextWord = msg;
    readCompare(msg, &stats, op);
    return stats;
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
