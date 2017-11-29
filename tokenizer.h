/*
 * header file for string tokenizer
 * 
 * David Bover, March, 2016
 */

#ifndef DB_TOKENIZER
#define DB_TOKENIZER

// initialize the tokenizer 
// parameters: 
//      line: the string to be tokenized
//      delimiters: a string of delimiter characters to be used 
//                  as token separators
void tokenize (char *line, char *delimiters);

// returns the next token from the tokenized string
// returns NULL if there are no more tokens in the string
char *nextWord();

#endif