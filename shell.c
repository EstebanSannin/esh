/*
 * Simple esteban shell: esh
 *
 * License: GPLv3
 */


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "parse.h"
#define LINELENGTH 256
#define PROMPT "> "
#define PERMS 0666
#define NUMCOMMANDS 10
#define CMDLENGTH 80

void executeCmd(tcmdLine cmd);
void execPipes(char *line);

pid_t childPid;

main()
{
  int finish=0;
  char line[LINELENGTH];
  char prompt[]=PROMPT;


  while(!finish)
    {
      printf("%s%s",get_current_dir_name(),prompt);
      fgets(line,LINELENGTH,stdin);
      line[strlen(line)-1]='\0';
      if(!strcmp(line,"exit")){
	finish=1;
	kill(0,SIGTERM);
	printf("bye :-)\n");
      }
      else if(!strcmp(line,"custom"))
	      printf("Custom mode..\n");
      else
	execPipes(line);
    }
}

void sendInt()
{
  signal(SIGINT,sendInt);
  kill(childPid,SIGINT);
}


void execPipes(char *line)
{
  char cmdLine[NUMCOMMANDS][CMDLENGTH];
  tcmdLine cmd[NUMCOMMANDS];
  int fd[NUMCOMMANDS][2];
  int numcomm=0;
  char *result=NULL;
  int x=0;

  signal(SIGINT,sendInt);

  result=strtok(line,"|");
  while(result)
    {
      strcpy(cmdLine[numcomm++],result);
      result=strtok(NULL,"|");
    }
  numcomm--;
  for(x=0;x<=numcomm;x++)
    cmd[x]=parseLine(cmdLine[x]);

  for(x=numcomm;x>=0;x--)
    {
      if(x!=numcomm)
	pipe(fd[x]);
      pid_t pid=fork();
      if(pid==-1)
	{
	  perror("Non posso eseguire il comando\n");
	  exit(2);
	}
      if(pid==0)
	{
	  //figlio 
	  if(x<numcomm)
	    {
	      close(1);
	      dup(fd[x][1]);
	      close(fd[x][0]);
	      close(fd[x][1]);
	    }
	  if(x==0)
	    executeCmd(cmd[x]);
	}
      else // Processo padre (shell)
	{
	  if(x==0)
	    childPid=pid;
	  if(x==numcomm){
	    int stat;
	    if(cmd[numcomm].background)
	      printf("[1] %d\n",pid);
	    else
	      wait(& stat);
	    break;
	  }
	  else
	    {
	      close(0);
	      dup(fd[x][0]);
	      close(fd[x][0]);
	      close(fd[x][1]);
	      executeCmd(cmd[x+1]);
	    }
	}
    }

}

void executeCmd(tcmdLine cmd)
{ 	  
  if(cmd.command[0]!='\0')
    {
      char ** parms=(char **) malloc((cmd.numpar+1)*sizeof(char **));
      int x=0;
      for(x=0;x<cmd.numpar;x++)
	parms[x]=cmd.param[x];
      parms[cmd.numpar]=NULL;
      if(cmd.inFile[0]!='\0')
	{
	  int fd=open(cmd.inFile,O_RDONLY);
	  close(0);
	  dup(fd);
	  close(fd);
	}
      if(cmd.outFile[0]!='\0')
	{
	  int fd=creat(cmd.outFile,PERMS);
	  close(1);
	  dup(fd);
	  close(fd);
	}
      if(cmd.errFile[0]!='\0')
	{
	  int fd=creat(cmd.errFile,PERMS);
	  close(2);
	  dup(fd);
	  close(fd);
	}
      if(!strcmp(cmd.command,"cd"))
        {
	      if(chdir(cmd.param[1]) != 0)
	      {
		      printf("%s: %s: Directory non esistente\n",cmd.command,cmd.param[1]);
	      }
	}
      if(execvp(cmd.command,parms)==-1 && strcmp(cmd.command,"cd"))
	fprintf(stderr,"Non posso eseguire %s\n",cmd.command);
      free(parms);
    }
  else
    fprintf(stderr,"Comando non valido\n"); 
}
