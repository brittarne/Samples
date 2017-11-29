/*
 * string tokenizer
 * 
 * David Bover, March, 2016
 */

#include <string.h>
#include <stdlib.h>

static char *source;    // a local copy of the string to be tokenized
static char *skip;      // a local copy of the delimiters
static int length;      // length of the original string
static int pos;         // current position in the source string

// check that the current character is a delimiter or a control character
static int isdelimiter(char c) {
    if (strchr(skip, source[pos])) 
        return 1;
    
    if (source[pos] < ' ')
        return 1;
 
    return 0;        
}

// initialize the tokenizer
void tokenize (char *line, char *delimiters) {
    
    // make a local copy of the string to be tokenized
    int len = strlen(line);
    source = (char *) malloc(len +1);
    strncpy(source, line, len);
    source[len] = '\0';
    
    // save the length of the original string and reset pos
    length = len;
    pos = 0;
    
    // make a local copy of the delimiters
    len = strlen(delimiters);
    skip = (char *) malloc(len+1);
    strncpy(skip, delimiters, len);
    skip[len] = '\0';
}

// get the next token from the source string
// return NULL if no more tokens
char *nextWord() {
    
    // past the end of the source string
    if (pos >= length) return NULL;
    
    // skip over delimiters
    while (isdelimiter(source[pos]) && (pos < length))
        pos++;
    
    // check for end of line - no more tokens
    if (pos >= length)
        return NULL;
    
    // token starts at index start
    int start = pos;
    
    // find the next delimiter
    while (!isdelimiter(source[pos]) && (pos < length))
        pos++;
    
    // check for empty token
    if (pos == start) 
        return NULL;
    
    // insert a NULL at the end of the token
    source[pos] = '\0';
    
    // start point for next call to getword()
    pos++;
    
    // return the token
    return &(source[start]);
}
    
