#include<stdio.h>
#include<string.h>
#include "parse.h"


tcmdLine parseLine(char *line)
{
  tcmdLine strCm={"",{'\0'},1,"","","",0};
  char delims[] = " "; 
  char *result = NULL; 
  result = strtok(line, delims); 
  int n=0,pnum=0;
 // int errCode=0;
  while(result != NULL ) 
    { 
      if(result[0]=='<' && n>0)
	{
	  result=& result[1];
	  strcpy(strCm.inFile,result);
	}
      else if(result[0]=='>' && n>0)
	{
	  result=& result[1];
	  strcpy(strCm.outFile,result);
	}
      else if(result[0]=='2' && result[1]=='>' && n>0)
	{
	  result=& result[2];
	  strcpy(strCm.outFile,result);
	}
      else if(result[0]=='&')
	{
	  strCm.background=1;
	  break;
	}
      else if(strCm.inFile[0]=='\0' && strCm.outFile[0]=='\0' && strCm.errFile[0]=='\0'
              && result[0]!='<' && result[0]!='>' && result[0]!='2' && result[1]!='>')
	{
	  if(n==0)
	    strcpy(strCm.command,result);
	  strcpy(strCm.param[pnum++],result);
	}
      else
	{
	  fprintf(stderr,"Sintassi non valida\n");
	  strCm.command[0]='\0';
	  break;
	}
      result = strtok(NULL, delims ); 
      n++;
    }
  strCm.numpar=pnum;
  return strCm;
} 


