#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include<sys/wait.h> 
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#define MAX_LINE 80 /* The maximum length command */

char **getInput(char *input) {
    char **command = malloc(MAX_LINE * sizeof(char *));
    char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL) {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

int main() { 
	int stat_loc;
	char** prevCommand;
	int hasPrev = 0;
	while (1) {
		char* cmdInput = malloc(MAX_LINE*sizeof(char*));
		char** arrCmd;
		printf("\n--dddunix> ");
		cmdInput = gets(cmdInput);
		arrCmd = getInput(cmdInput);
		int canRunParallel = 0;
		if(strcmp(arrCmd[0],"exit")==0){
			return 0;
		}
		if(strcmp(cmdInput,"!!")==0){
			if(hasPrev){
				int i = 0;
				for(;(*(prevCommand+i)!=NULL);i++){
					printf("run '%s'\n",(*(prevCommand+i)));
					*(arrCmd+i) = *(prevCommand+i);
				}
				*(arrCmd+i) = NULL;
			}else{
				printf("---No commands in history.\n");
			}
		}else{
			hasPrev = 1;
			int i = 0;
			for(;(*(arrCmd+i)!=NULL);i++){
				printf("set '%s'\n",(*(arrCmd+i)));
				*(prevCommand+i) = *(arrCmd+i);
			}
			*(prevCommand+i) = NULL;

		}
		int index = 0;
		while(arrCmd[index]){
			if(strchr(arrCmd[index],'&')){
				canRunParallel = 1;
				arrCmd[index] = NULL;
				break;
			}
			++index;
		}
		int child_pid = fork();
        if (child_pid == 0) {
            execvp(arrCmd[0], arrCmd);
			if(strcmp(cmdInput,"!!")!=0){
            	printf("---dddunix bash '%s': command not found\n",cmdInput);
			}
        } else {
			if(canRunParallel){
			}else{
				waitpid(child_pid, &stat_loc, WUNTRACED);
			}
        }

		free (cmdInput);
		free (arrCmd);
		
		/*
		* After reading user input, the steps are: 
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) parent will invoke wait() unless command included & 
		*/
	} 
	free (prevCommand);
	return 0;
} 
