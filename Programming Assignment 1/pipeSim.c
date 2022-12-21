#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main(){
    int fd[2];
    printf("I’m SHELL process, with PID: %d", getpid());
    printf(" - My command is: man touch |  grep -e -c -A 1 > output.txt  \n");
    if (pipe(fd)<0){
        perror("pipe");
        exit(1);
    
    }
    int cpid1=fork();
    if (cpid1<0){
        perror("Fork");
        exit(1);

    }
    
    else if (cpid1==0){
        printf("I’m MAN process, with PID: %d", getpid());
        printf(" - My command is: man touch \n");
        char* args[]={strdup("man"),strdup("touch"),NULL};
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execvp(args[0],args);
    }
    else{
        //parent processs

    }
    int cpid2=fork();
    if (cpid2<0){
        perror("Fork");
        exit(1);
    }
    else if (cpid2==0){
        printf("I’m GREP process, with PID: %d", getpid());
        printf(" - My command is: grep -c \n");
        close(fd[1]);
        int new_fd = open("output.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (dup2(new_fd,STDOUT_FILENO)< 0){
            perror("dup2");
            exit(1);
        }
        //char msg[500]={0};
        //read(fd[0],msg,sizeof(msg));

        dup2(fd[0],STDIN_FILENO);
        char* args2[]={"grep","-e","-c","-A","1",NULL};
        execvp(args2[0],args2);
    }
    else{
        //parent processs
        //wait 
        waitpid(cpid1,NULL,0);
        printf("I’m SHELL process, with PID: %d", getpid());
        printf(" - execution is completed, you can find the results in output.txt \n");
        
    }
    return 0;

}
