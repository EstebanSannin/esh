#ifndef PARSE_H
#define PARSE_H

#define CLENGTH 80
#define PNUM 20

typedef struct cmdLine{
  char command[CLENGTH];
  char param[PNUM][CLENGTH];
  int numpar;
  char inFile[CLENGTH];
  char outFile[CLENGTH];
  char errFile[CLENGTH];
  int background;
} tcmdLine;

tcmdLine parseLine(char *line);

#endif
