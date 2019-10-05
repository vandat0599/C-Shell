#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include<sys/wait.h> 
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE 80 /* The maximum length command */

char **getInput(char *input) {
	char* inputCpy = malloc(strlen(input)+1);
	strcpy(inputCpy,input);
    char **command = malloc(MAX_LINE * sizeof(char *));
    char *separator = " ";
    char *parsed;
    int index = 0;
    parsed = strtok(inputCpy, separator);
    while (parsed != NULL) {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

int getIndex(char** arr, char* s){
	for(int i=0;arr[i]!=NULL;i++){
		if(strcmp(arr[i],s)==0){
			return i;
		}
	}
	return -1;
}

int getDup2FileDiscriptor(char* fileName, char operator){
	int fileDes = open(fileName,O_RDWR|O_CREAT);
	if(fileDes!=-1){
		//remove all content if file existed
		truncate(fileName,0);
	}else{
		printf("Can't open file '%s'",fileName);
	}
	return operator=='>'?dup2(fileDes,STDOUT_FILENO):dup2(fileDes,STDIN_FILENO);
}

int main() {
	int stat_loc;
	char* prevCommand;
	int hasPrev = 0;
	while (1) {
		char* cmdInput = malloc(MAX_LINE*sizeof(char*));
		char** arrCmd;
		printf("\n--dddunix> ");
		cmdInput = gets(cmdInput);
		arrCmd = getInput(cmdInput);
		if(strcmp(arrCmd[0],"exit")==0){
			return 0;
		}

		//check history
		if(strcmp(cmdInput,"!!")==0){
			if(hasPrev){
				arrCmd = getInput(prevCommand);
			}else{
				printf("---No commands in history.\n");
			}
		}else{
			hasPrev = 1;
			int i = 0;
			prevCommand = malloc(strlen(cmdInput)+1);
			strcpy(prevCommand,cmdInput);
		}

		// check Redirecting Input and Output
		int hasLargerOpe = getIndex(arrCmd,">")!=-1;
		int hasSmallerOpe = getIndex(arrCmd,"<") != -1;
		char operatorRedirect = (hasLargerOpe||hasSmallerOpe)?(hasLargerOpe?'>':'<'):' ';
		int indexOperatorRedirect = hasLargerOpe?getIndex(arrCmd,">"):getIndex(arrCmd,"<");
		arrCmd[indexOperatorRedirect] = NULL;
		int canRedirect = operatorRedirect!=' ' && arrCmd[indexOperatorRedirect + 1] != NULL;
		printf(">: %d, <: %d, operator: %c, index: %d",hasLargerOpe, hasSmallerOpe, operatorRedirect, indexOperatorRedirect);

		//check & in cmd for running cmd parallel
		int indexOfAndOperator = getIndex(arrCmd,"&");
		int canRunParallel = indexOfAndOperator!=-1?1:0;
		if(canRunParallel){
			arrCmd[getIndex(arrCmd,"&")] = NULL;
		}

		//create a child process
		int child_pid = fork();
        if (child_pid == 0) {
			//run cmd

			//Redirecting Input and Output
			if(canRedirect){
				if (getDup2FileDiscriptor(arrCmd[indexOperatorRedirect+1],operatorRedirect)==-1){
					printf("dup2 err\n");
				}
			}

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
	} 
	free (prevCommand);
	return 0;
} 
