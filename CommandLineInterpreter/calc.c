#include <string.h>
#include <stdlib.h>
#include 	<stdio.h>
#include	<unistd.h>
#include  "tokenizer.h"

#define MAXLINE 1024
#define NUMDELIMITERS " 123456789()"
#define OPDELIMITERS " +-*/^%#$!@()"


void calculate(int first, int second, char *operation){
  int result;
  if (strcmp(operation,"+") == 0){
    result = first + second;
  }else if (strcmp(operation,"-") == 0){
    result = first - second;
  }else if (strcmp(operation,"*") == 0){
    result = first * second;
  }else if(strcmp(operation,"/") == 0){
    result = first / second;
  }else {
    printf("That's not an allowed operation\n");
    return;
  }
  printf("%d %s %d = %d\n", first, operation, second, result);
}

int main(int argc, char *argv[]) {
  char input[MAXLINE];
  bzero(input, MAXLINE);
  int first;
  int second;

  while(read(0, input, MAXLINE) != 0){
    tokenize(input, NUMDELIMITERS);
    char *operation = nextWord();

    tokenize(input, OPDELIMITERS);
    char *firstNum = nextWord();
    char *secondNum = nextWord();
    if(operation != NULL && firstNum != NULL && secondNum != NULL){
      first = atoi(firstNum);
      second = atoi(secondNum);
      calculate(first, second, operation);
    }else {
      printf("Not a valid expression\n");
    }

    bzero(input, MAXLINE);
  }
}
