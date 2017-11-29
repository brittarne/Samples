/*
  Brittany Arneson
  CSCI 352 Spring 2016
  Assignment 2: Command Line Interpreter
    implements own version of pwd, cd, ls(listf), calc, exit and io redirection
    passes other commands to exec function
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define MAXLINE 1024
#define MAXOPS 4
#define MAXDIRS 1024
#define TOTWODIGITS 100
#define PERMISSIONS(X,Y,P) (((X) & (Y)) ? printf("%s", P) : printf("-"));

char *directories[MAXDIRS];
char options[MAXOPS]; //array of all the options the user has chosen, l, a, c, m
bool lflag; //used to check if the -l option was chosen

/*
  parameter: mode - is the st_mode from a stat structure associated with a file
  return: void
  prints out the specified character to indicate what kind of file type the file is,
  regular, directory, link or other
*/
void printFileType(mode_t mode){
  //file is regular
  if(S_ISREG(mode)){
    printf("-");
  }
  //file is a directory
  else if(S_ISDIR(mode)){
    printf("d");
  }
  //file is a link
  else if(S_ISLNK(mode)){
    printf("l");
  }
  //file is something else
  else{
    printf("o");
  }
}
/*
  paratemter: mode - is st_mode from a stat structure associated with a file
  return: void
  prints out the permissions of owner, group, and other
  calls a macro PERMISSIONS to determine if the permission is there.
*/
void printPermissions(mode_t mode){
  PERMISSIONS(mode, S_IRUSR, "r");  //checks owner read permissions
  PERMISSIONS(mode, S_IWUSR, "w");  //checks owner write permissions
  PERMISSIONS(mode, S_IXUSR, "x");  //checks owner execute permissions
  PERMISSIONS(mode, S_IRGRP, "r");  //checks group read permissions
  PERMISSIONS(mode, S_IWGRP, "w");  //checks group write permissions
  PERMISSIONS(mode, S_IXGRP, "x");  //checks group execute permissions
  PERMISSIONS(mode, S_IROTH, "r");  //checks other read permissions
  PERMISSIONS(mode, S_IWOTH, "w");  //checks other write permissions
  PERMISSIONS(mode, S_IXOTH, "x");  //checks other execute permissions
}
/*
  parameter: uid - the userid of the owner of a file
  returns: void
  prints the owners name and owners group by using getpwuid to
  find the name of the owner and using getgrgid to find the name of the group
  the owner belongs to
*/
void printOwnerInfo(uid_t uid){
  struct passwd *owner;
  struct group *ownerGroup;
  owner = getpwuid(uid);
  ownerGroup = getgrgid(owner->pw_gid);
  printf("%s %s ", owner->pw_name, ownerGroup->gr_name);
}
/*
  parameter: filestats[] - a struct stat of a file
  returns: int 1 - used to flag if this function has been called
  prints the last access time of a file
  */
int accessed(struct stat *filestats){
  struct tm *atime = localtime(&filestats->st_atime);
  printf("A%02d/%02d/%02d-%02d:%02d ", atime->tm_mon, atime->tm_mday, atime->tm_year%TOTWODIGITS, atime->tm_hour, atime->tm_min);
  return 1;
}
/*
  parameter: filestats[] - a struct stat of a file
  returns: int 1 - used to flag if this function has been called
  prints the creation time of a file
  */
int created(struct stat *filestats){
  struct tm *crtime = localtime(&filestats->st_ctime);
  printf("C%02d/%02d/%02d-%02d:%02d ", crtime->tm_mon, crtime->tm_mday, crtime->tm_year%TOTWODIGITS, crtime->tm_hour, crtime->tm_min);
  return 1;
}
/*
  parameter: filestats[] - a struct stat of a file
  returns: int 1 - used to flag if this function has been called
  prints the last modification time of a file
  */
int modified(struct stat *filestats){
  struct tm *mtime = localtime(&filestats->st_mtime);
  printf("M%02d/%02d/%02d-%02d:%02d ", mtime->tm_mon, mtime->tm_mday, mtime->tm_year%TOTWODIGITS, mtime->tm_hour, mtime->tm_min);
  return 1;
}
/*
  parameter: filestats - a struct stat of a file
  returns: void
  calls modified, accessed and or created, determined by the options that were
  given to listf, if none of these are called it calls modified because that's
  the default
*/
void printTimes(struct stat *filestats){
  int m = 0;
  int a = 0;
  int c = 0;
  int i = 0;
  while(options[i] != '\0'){
    if(options[i] == 'm'){
      m = modified(filestats);
    }
    if(options[i] == 'a'){
      a = accessed(filestats);
    }
    if(options[i] == 'c'){
      c = created(filestats);
    }
    i++;
  }
  if(a == 0 && c == 0 && m == 0){
    modified(filestats);
  }
}
/*
  parameters: dirEntry - struct dirent, it is an entry in the directoryFiles,
                          purpose is so the name of the file is printed
              filestat - a struct stat associated with the file that corresponds
                         with the dirEntry, purpose is to access different stats
                         about the file
              digits - the highest number of digits that a filesize of a file
                       has in this directory, purpose is for the correct formatting
                       of the printed strings of the directory
  returns: void
  prints out the file information when the -l option is specified,
  prints the file type, file permissions, file size, owner name, group name,
  the specified times of the file, the file name.
*/
void printFileInfo(struct dirent *dirEntry, struct stat *filestat, int digits){
  //file type, - for regular, d for directory, l for links, o for other
  printFileType(filestat->st_mode);
  //read write and execute permissions for owner, group and other
  printPermissions(filestat->st_mode);
  //number of links to the file
  printf(" %lu ", (unsigned long)filestat->st_nlink);
  //name of file owner & file owners group
  printOwnerInfo(filestat->st_uid);
  //file size in bytes
  printf("%*d ", digits, (int)filestat->st_size);
  //access, create and modify times according to given options
  printTimes(filestat);
  //name of file
  printf("%s\n", dirEntry->d_name);
}
/*
  parameter: fileInfo - struct stat of a file, used to access the size of the file
              max - int, it is the max number of digits found so far in the directory
  returns: int, the max number of digits, either the previous max or the number of
          digits just found
  finds the max number of digits between a given max and a new size for a file
*/
int maxDigits(struct stat *fileInfo, int max){
  int tempMax = 0;
  int filesize = fileInfo->st_size;
  while(filesize != 0){
    filesize = filesize / 10;
    tempMax++;
  }
  if(tempMax > max){
    return tempMax;
  }
  return max;
}
/*
  parameter: path - the path to a directory that was given, or to the cwd
                    purpose is to have the beginning of a files path
             dirEntry - an entry in the file, purpose is to append the name to
                        the directory's path.
  returns: struct stat* - a new stat structure of a file, that is found using the path
                          put together using the parameters.
  makes a struct stat with information about an entry in a given directory
  and returns it to an array of struct stats for files in a directory
*/
struct stat *getFileStat(char *path, struct dirent *dirEntry){
  char filepath[1024];
  //makes the path to the file
  strncpy(filepath, path, MAXLINE);
  strcat(filepath, "/");
  strcat(filepath, dirEntry->d_name);
  //mallocs space for the filestat so that it can be returned
  struct stat *filestat = (struct stat *)malloc(sizeof(struct stat *) * MAXLINE);
  if(stat(filepath, filestat) == 0){
    return filestat;
  }
  return 0;
}
/*
  parameter: dirFiles - an array of struct dirent, it is all the entries in a
                        directory, purpose is to create a file struct and print
                        information about each entry
             n - int, the number of entries in the the directory, used to index
                 through the array
             path - char *, the path to the directory that is given, passed to
                    getFileStat to make a struct stat for each entry
  returns: void
  creates a struct stat for each file by calling getFileStat(), finds max digits,
  prints a directory if it is just a simple listf, or call printFileInfo for
  each file
*/
void printDirectory(struct dirent *dirFiles[], int n, char *path){
  struct stat *filesInfo[MAXLINE];
  int max = 0;
  //creates struct stats for the files, and finds max digits
  for(int i = 0; i < n; i++){
    filesInfo[i] = getFileStat(path, dirFiles[i]);
    max = maxDigits(filesInfo[i], max);
  }
  //prints the directory info
  for(int i = 0; i < n; i++){
    if(lflag == true){
      printFileInfo(dirFiles[i], filesInfo[i], max);
    }else{
      printf("%s ", dirFiles[i]->d_name);
    }
  }
  printf("\n");
}
/*
  parameter: files - array of struct dirent, all the files in a directory, used
                     to access the names of the files to place them in the right order
             n - int, the total number of entries in a directory, used to index
                 through the array
  returns: void
  sorts the entries in the files array so that they are in alphabetical order
*/
void sortDirectory(struct dirent *files[], int n){
  bool swapped = true;
  while(swapped){
    swapped = false;
    for(int i = 1; i < n; i++){
      if(strcasecmp(files[i - 1]->d_name, files[i]->d_name) > 0){
        struct dirent *temphi = files[i - 1];
        struct dirent *templo = files[i];
        files[i - 1] = templo;
        files[i] = temphi;
        swapped = true;
      }
    }
  }
}
/*
  parameters: directory - char *, the name of the directory, used to append to
                          the cwd to find the path of the directory
  returns: void
  opens a directory and reads all the files in the directory into an array of dirents
*/
void directoryFiles(char *directory){
  struct dirent *files[MAXLINE];
  bzero(files, MAXLINE);
  //makes the path to the a given directory
  char buf[MAXLINE];
  getcwd(buf, MAXLINE);
  strcat(buf, "/");
  strcat(buf, directory);
  DIR *dir = opendir(buf);
  if(dir != NULL){
    struct dirent *dirEntry;
    int n = 0;
    while((dirEntry = readdir(dir)) != NULL){
      if(strncmp(dirEntry->d_name, ".", 1) != 0 && strncmp(dirEntry->d_name, "..", 2) != 0){
        files[n] = dirEntry;
        n++;
      }
    }
    sortDirectory(files, n);
    printDirectory(files, n, buf);
  }else {
    printf("Not a valid directory\n");
  }
}
/*
  parameter: numArgs, int, the number of arguments given to the program,
            used to index through the array of arguments, to find directorys
            or options
  return: void
  handles the arguments given to the program, figures out if they are directorys
  or added options, also separates out strings of options that are connected
  (ie -lmac)
*/
void handleArgs(int numArgs, char *args[]){
  int numOps = 0;
  int numDirs = 0;
  lflag = false;

  for(int i = 1; i < numArgs; i++){
    //finds directorys
    if(strncmp(args[i], "-", 1) != 0){
      directories[numDirs] = args[i];
      numDirs++;
    }else {
      int n = 0;
      //finds each option from a list of them not given seperately
      while (strcmp(&args[i][n],"\0") != 0){
        char c;
        strncpy(&c, &args[i][n], 1);
        if(c != '-'){
          strncpy(&options[numOps], &c, 1);
          numOps++;
          //sees if the -l option is chosen, if it's not no other options have an effect
          if(c == 'l'){
            lflag = true;
          }
        }
        n++;
      }
    }
  }
  if (numDirs > 0){
    for(int i = 0; i < numDirs; i++){
      printf("%s:\n", directories[i]);
      directoryFiles(directories[i]);
    }
  }else {
    //if listf isn't given a directory it uses the cwd, so "" will be appended
    directoryFiles("");
  }
}
//main, just calls handle arguments
int main(int argc, char *argv[]){
  handleArgs(argc, argv);
}
