/*
./filter --help
Usage: filter [options] [file1 file2 ...] 

Options:
 -d  --delete=X            deletes all occurrences of char X.
 -r  --replace=XY          replace all char X with char Y.
 -u  --toupper             convert all characters to upper case.
 -l  --tolower             convert all characters to lower case.
 -b  --remove-blank-lines  removes empty lines.
 -x  --expand-tabs[=X]     convert tabs to X spaces (default is 8).
 -n  --line-numbers        prepend line numbers to each line.
 -h  --help                display this information.
 */


#include <unistd.h> /* ADD COMMENTS */
#include <getopt.h> /* ADD COMMENTS */
#include <stdio.h>  /* ADD COMMENTS */
#include "stdlib.h"

#define TRUE 1
#define FALSE 0
#define TAB_DEFAULT 8
#define UNREFRENCED_PARAMETER(p) ((void)(p))

/* STATIC VARIABLES AND COMMENTS */
extern char *optarg;
extern int optind, opterr, optopt;

 enum flags
{
  deleteChar,
  replace,
  upper,
  lower,
  blanksRemove,
  expandTabs,
  numLines,

  //end of flag list
  flagCount
};

static int flagOptions[flagCount] = {};
static char** fileNames;
char *optArgs[flagCount];
int option_index = 0;
static FILE* file;
static struct option long_options[] = 
{
  {"delete",             required_argument, NULL, 'd'},
  {"replace",            required_argument, NULL, 'r'},
  {"toupper",            no_argument,       NULL, 'u'},
  {"tolower",            no_argument,       NULL, 'l'},
  {"remove-blank-lines", no_argument,       NULL, 'b'},
  {"expand-tabs",        optional_argument, NULL, 'x'},
  {"line-numbers",       no_argument,       NULL, 'n'},
  {"help",               no_argument,       NULL, 'h'},
  {NULL,                 0,                 NULL,  0 }
};

void Replace(const char* replaceMe)
{
  int delim = 0;
  if(replaceMe[0] == '=')
    delim++;

  const char replaceWith = replaceMe[1 + delim];
  char thisChar; //read in the next character

  while(!feof(file))
  {
    //read in the next character
    thisChar = fgetc(file);

    //do nothing if deleteMe is detected
    if(thisChar == replaceMe[0 + delim])
      fputc(replaceWith, stdout);
    
    //otherwise just print that character
    else
    {
      if(thisChar != -1)
        fputc(thisChar, stdout);
    }
  }
}

void Delete(const char* deleteMe)
{
  char lowerChar = deleteMe[0];
  char upperChar = deleteMe[0];
  int delim = 0;

  if(deleteMe[0] == '=' && deleteMe[1] != '\0')
    delim++;

  //is a lower char
  if(deleteMe[0] >= 'a' && deleteMe[0] <= 'z')
    lowerChar = deleteMe[delim];

  //is an upper char
  if(deleteMe[0] >= 'A' && deleteMe[0] <= 'Z')
    upperChar = deleteMe[delim];

  char thisChar; //read in the next character

  while(!feof(file))
  {
    //read in the next character
    thisChar = fgetc(file);

    //do nothing if deleteMe is detected, otherwise print
    if(thisChar != lowerChar && thisChar != upperChar)
    {
      if(thisChar != -1)
        fputc(thisChar, stdout);
    }    
  }
}

void ToUpper(const char* unused)
{
  UNREFRENCED_PARAMETER(unused);

  char thisChar; //read in the next character

  while(!feof(file))
  {
    //read in the next character
    thisChar = fgetc(file);

    //or shift to upper
    if(thisChar >= 'a' && thisChar <= 'z')
    {
      fputc(thisChar - ' ', stdout);
    }
    
    //otherwise just print that character
    else
    {
      if(thisChar != -1)
      {
        fputc(thisChar, stdout);
      }
    }
  }
}

void ToLower(const char* unused)
{
  UNREFRENCED_PARAMETER(unused);

  char thisChar; //read in the next character

  while(!feof(file))
  {
    //read in the next character
    thisChar = fgetc(file);

    //shift to lower
    if(thisChar >= 'A' && thisChar <= 'Z')
    {
      fputc(thisChar + ' ', stdout);
    }
    
    //otherwise just print that character
    else
    {
      if(thisChar != -1)
      {
        fputc(thisChar, stdout);
      }
    }
  }
}

void BlankLines(const char* unused)
{  
  UNREFRENCED_PARAMETER(unused);
  char previousChar; //the previously read character
  char thisChar = 0; //read in the next character

  while(!feof(file))
  {
    //read in the next character while preserving the last character
    previousChar = thisChar; 
    thisChar = fgetc(file);

    //do nothing if there are two new lines
    if(previousChar == '\n' && thisChar == '\n')
      continue;
    
    //otherwise just print that character
    else
    {
      if(thisChar != -1)        
        fputc(thisChar, stdout);
    }
  }
}

void ExpandTabs(const char* tabOverride)
{
  int tabSize = TAB_DEFAULT;
  if(tabOverride)
  {    
    if(tabOverride[0] == '=')
    {
      tabSize = atoi(&(tabOverride[1]));
    }
  }

  char thisChar = 0; //read in the next character

  while(!feof(file))
  {
    //read in the next character
    thisChar = fgetc(file);

    //kill that tab and print some spaces
    if(thisChar == '\t')
    {
      printf("%*c", tabSize, ' ');
    }
    
    //otherwise just print that character
    else
    {
      if(thisChar != -1)
        fputc(thisChar, stdout);
    }
  }
}

void LineNumbers(const char* unused)
{
  UNREFRENCED_PARAMETER(unused);

  int lineNum = 1;

  char thisChar; //read in the next character
  printf("%6d  ", lineNum);

  while(!feof(file))
  {
    //read in the next character
    thisChar = fgetc(file);

    if(thisChar == '\n')
    {
      lineNum++;
      printf("%c%6d  ",thisChar, lineNum);
    }
    
    //otherwise just print that character
    else
    {
      if(thisChar != -1)     
      { 
        fputc(thisChar, stdout);
      }
    }
  }
}

void Help()
{
  printf("Usage: filter [options] [file1 file2 ...]\n\n");
  printf("Options:\n");
  printf(" -b  --remove-blank-lines  removes empty lines.\n");
  printf(" -d  --delete=X            deletes all occurrences of char X.\n");
  printf(" -h  --help                display this information.\n");
  printf(" -l  --tolower             convert all characters to lower case.\n");
  printf(" -n  --line-numbers        prepend line numbers to each line.\n");
  printf(" -r  --replace=XY          replace all char X with char Y.\n");
  printf(" -u  --toupper             convert all characters to upper case.\n");
  printf(" -x  --expand-tabs[=X]     convert tabs to X spaces");
  printf(" (default is 8).\n\n");
}

int main(int argc, char *argv[])
{
  int opt = 0;
  int i, j = 0;
  int fileCount = 0;
  const char* optCommand = "-:d:r:u:l:b:x::nh";

  //find the file name
  while ((opt = getopt(argc, argv, optCommand)) != -1)
  { 
    switch (opt) 
    {
      case 'h':
        Help();
        return 0;

      //non option argument
      case 1:
        //fileName = optarg;
        fileCount++;
        break;
      
      case 'd':
      case 'r':
      case 'u':
      case 'l':
      case 'b':
      case 'x':
      case 'n':
      case ':':
      case '?':
      default:
        continue;
    }
  }

  fileNames = (char**)calloc(fileCount,sizeof(char*));
  optind = 1;

  while ((opt = getopt_long(argc, argv,optCommand, long_options, &option_index))
   != -1) 
  {
    switch (opt) 
    {
      case 'd':
        flagOptions[deleteChar] = TRUE;
        optArgs[deleteChar] = optarg;
        break;

      case 'r':
        flagOptions[replace] = TRUE;
        optArgs[replace] = optarg;
        break;

      case 'u':
        flagOptions[upper] = TRUE;
        optArgs[upper] = optarg;
        break; 

      case 'l':
        flagOptions[lower] = TRUE;
        optArgs[lower] = optarg;
        break;

      case 'b':
        flagOptions[blanksRemove] = TRUE;
        optArgs[blanksRemove] = optarg;
        break;

      case 'x':
        flagOptions[expandTabs] = TRUE;
        optArgs[expandTabs] = optarg;
        break;

      case 'n':
        flagOptions[numLines] = TRUE;
        optArgs[numLines] = optarg;
        break;

      case 'h':
      case 1:
        fileNames[i++] = optarg;
        break; 
        
      default:
        printf("you didn't handle this character %s\n",optarg);
        break;
     }
  }

  void (*filters[flagCount])(const char* data) = 
  {
    Delete,
    Replace,
    ToUpper,
    ToLower,
    BlankLines,
    ExpandTabs,
    LineNumbers,
  };

  //let stdin be your file
  if(!fileCount)
    fileCount++;

  for(i = 0; i < fileCount; i++)
  {
    file = fopen(fileNames[i],"rt");
    if(!file)
    {
      file = stdin;
    }

    for(j = 0; j < flagCount; j++)
    {
      if(filters[j] && flagOptions[j])
      {
        filters[j](optArgs[j]);
      }
    }
  }

  return 0;
}