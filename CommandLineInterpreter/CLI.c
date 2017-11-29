#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "tokenizer.h"

#define MAXLINE 1024
#define DELIMETER " "
#define CMDINDEX 0
#define FINDNEXTWORD 1
FILE *in;
FILE *out;
bool redirectOflag = false;
bool redirectIflag = false;

/*
  parameter: none
  return: void
  this gets the current working directory using getcwd and then prints it
*/
void handlePWD(){
  char buf[MAXLINE];
  char *pwd = getcwd(buf, MAXLINE);
  if(pwd != NULL){
    printf("%s ", pwd);
  }
}
/*
  parameter: arg - char *, this is the directory that cd is trying to change to
  return: void
  this attempts to change the current directory to the given argument
   if it is successful it prints the new working directory,
   if it is unsuccessful says there is no such directory
*/
void handleCD(char *arg){
  char *directory = arg;
  if(chdir(directory) == 0){
    fprintf(stdout, "cwd changed to ");
    handlePWD();
    printf("\n");
  }else {
    printf("No such directory: %s\n", directory);
  }
}
/*
  parameter: none
  returns: int, 1 on success, 0 on error
  handle calc calls an exec function to run the program calc.c
*/
int handleCALC(){
  pid_t status;
  int childexit;
  // create child process
  status = fork();
  // check that fork didn't fail
  if (status < 0) {
      printf("fork failed\n");
      return 0;
  }
  // child process
  if (status == 0) {
    //calls exec to run the program calc
      if(execl("/home/arnesob/352/Assignment2/calc", "./calc", NULL) == (-1)){
        fprintf(stderr, "exec failed\n");
        return 0;
      }
      return 1;
  }
  // parent process
  else {
    //waits for calc to be terminated
      wait(&childexit);
      return 1;
  }
  return 1;
}
/*
  parameter: cmdargs - char **, this is the array of other commands given with listf
  return: int, 1 on success, 0 on error
  sets the first arg in cmdargs as ./listf so it will run the program and then
  passes the cmdargs array to the exec function
*/
int handleLISTF(char **cmdargs){
  pid_t status;
  int childexit;
  // create child process
  status = fork();
  // check that fork didn't fail
  if (status < 0) {
      printf("fork failed\n");
      return 0;
  }
  // child process
  if (status == 0) {
    //calls exec to execute the command listf
      cmdargs[CMDINDEX] = "./listf";
      if(execv("/home/arnesob/352/Assignment2/listf", cmdargs) == -1){
        fprintf(stdout, "exec failed\n");
        return 0;
      }
      return 1;
  }
  // parent process
  else {
    //waits for listf to finish
      wait(&childexit);
     return 1;
  }
  return 1;
}
/*
  parameter: cmdargs - char **, the commands that were given to the cmd line
  returns: int, 1 when successful, 0 when it errors
  this is for every command given to the CLI that isn't one implemented in
  this program. It finds the name of the command which is the first element in
  the array then calls exec for the command the arguments that were given with it
*/
int handleOTHER(char **cmdargs){
  pid_t status;
  int childexit;
  // create child process
  status = fork();
  // chech that fork didn't fail
  if (status < 0) {
      printf("fork failed\n");
      return 0;
  }
  // child process
  if (status == 0) {
      //calls exec to execute the command
      char *cmd = cmdargs[CMDINDEX];
      if(execvp(cmd, cmdargs) == -1){
        fprintf(stdout, "exec failed\n");
        return 0;
      }
      return 1;
  }
  // parent process
  else {
    //waits for the child process to finish
      wait(&childexit);
      return 1;
  }
  return 1;
}
/*
  parameter: cmds - char **, this is all the commands given to CLI
  returns: int, returns the values of functions that could fail, if the do it exits
  the first element in cmds is the cmd given to the CLI, it is compared against
  the implemented values and if it is one calls the function that handles that
  or sends cmd to handleOTHER to be passed to exec function
*/
int handleARG(char **cmds){
  //command is exit
  if(strcmp(cmds[CMDINDEX], "exit") == 0){
    printf("exit\n");
    return 0;
  }
  else if(strcmp(cmds[CMDINDEX], "pwd") == 0){
    handlePWD();
    return 1;
  }
  else if(strcmp(cmds[CMDINDEX], "cd") == 0){
    handleCD(cmds[1]);
    return 1;
  }
  else if(strcmp(cmds[CMDINDEX], "calc") == 0){
    return handleCALC();
  }
  else if(strcmp(cmds[CMDINDEX], "listf") == 0){
    return handleLISTF(cmds);
  }else {
    return handleOTHER(cmds);
  }
  return 1;
}

/*
  parameter: none
  returns: char **, returns a char ** of the remaining arguments once the io
          redirection has been taken out, to be used to run the commands
  finds if there is redirection and sets stdin or stdout to the file, also finds
  the remaining args since the io redirect can be in any part of the command line
*/

char **findArgs(){
  char **remainingArgs = (char **)malloc(sizeof(char *) * MAXLINE);
  redirectOflag = false;
  redirectIflag = false;
  char *input;
  char *output;
  char *next;
  int argindex = 0;
  //loops through the command line arguments to see if there is any redirection
  while((next = nextWord()) != NULL){
    //checks for output redirection
    if(strncmp(next, ">", 1) == 0){
      redirectOflag = true;
      //finds file name if there are spaces or no spaces
      if(next[FINDNEXTWORD] == '\0'){
        output = nextWord();
      }else {
        output = &next[FINDNEXTWORD];
      }
    }
    //checks for input redirection
    else if(strncmp(next, "<", 1) == 0){
      redirectIflag = true;
      //finds file name if there are spaces or no spaces
      if(next[FINDNEXTWORD] == '\0'){
        input = nextWord();
      }else {
        input = &next[FINDNEXTWORD];
      }
    //assigns remaining values to a char** that will be returned
    }else {
      remainingArgs[argindex] = next;
      argindex++;
    }
  }
  //assigns new output redirection if redirect was found
  if(redirectOflag == true){
    freopen(output, "w", stdout);
  }
  //assigns new input redirection if redirect was found
  if(redirectIflag == true){
    freopen(input, "r", stdin);
  }
  return remainingArgs;
}
/*
  parameter: none
  return: void
  this resets stdout to the console and stdin to the keyboard if there
  was a redirect
*/
void stdIO(){
  if(redirectOflag){
    freopen("/dev/tty", "w", stdout);
  }
  if(redirectIflag){
    freopen("/dev/tty", "r", stdin);
  }
}
//main, contains CLI loop
int main(int argc, char *argv[]){
  char CLarg[MAXLINE];
  bzero(CLarg, MAXLINE);
  char *n;
  fprintf(stdout, "$> ");
  while((n = fgets(CLarg, MAXLINE, stdin)) != NULL){
    //tokenizes the command line arguments by spaces
    tokenize(CLarg, DELIMETER);
    char **arguments = findArgs();
    //if handle arg fails or exit is called returns 0 and exits
    if(!handleARG(arguments)){
      return EXIT_SUCCESS;
    }
    //resets io redirect to stdin and stdout
    stdIO();
    fprintf(stdout, "$> ");
    bzero(CLarg, MAXLINE);
    bzero(arguments, MAXLINE);
  }
}
