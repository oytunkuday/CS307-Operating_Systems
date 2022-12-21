#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

char winner;

char realwinner='n';

char **matrix;

bool gameison;

int size;

void delete_matrix(){
    int i;
    for(i = 0; i < size; i++){
        free(matrix[i]);
    }
    free(matrix);
}
void print_matrix(){
    int i;
    for (i=0;i<size;i++){//printing
        int j;
        for (j=0;j<size;j++){
            printf("[%c] ",matrix[i][j]);
        }
        printf("\n");
    }
}

bool isFull(){
    int i;
    for (i=0;i<size;i++){//checking if full
        int j;
        for (j=0;j<size;j++){
            if (matrix[i][j]==' '){
                return false;
            }
        }
    }
    return true;
}

bool check_win(){
    bool winnerwinnerchickendinner=false;
    int rcount,ccount;
    int a;
    for (a=0;a<size;a++){//check rows and cols
        rcount=0;
        ccount=0;
        int b;
        char tempchar=matrix[a][0];
        char tempchar2=matrix[0][a];
        if(tempchar!=' ' && tempchar2!=' '){
            for (b=0;b<size;b++){
                if(matrix[a][b]==tempchar){
                    rcount++;
                }
                if(matrix[b][a]==tempchar2){
                    ccount++;
                }
            }
        }
        if(rcount==size){
            realwinner=tempchar;
            winnerwinnerchickendinner=true;
        }
        else if(ccount==size){
            realwinner=tempchar2;
            winnerwinnerchickendinner=true;
        }
    }
    if(!winnerwinnerchickendinner){
        int diagcount=0;
        int reversediagcount=0;
        char diagtemp=matrix[0][0];
        char reversediagtemp=matrix[size-1][0];
        int c;
        if(diagtemp!=' '&& reversediagtemp!=' '){
            for (c=0;c<size;c++){
                if(matrix[c][c]==diagtemp){
                    diagcount++;
                }
                if(matrix[size-1-c][c]==reversediagtemp){
                    reversediagcount++;
                }
            }
            if(reversediagcount==size){
                realwinner=reversediagtemp;
                winnerwinnerchickendinner=true;
            }
            else if(diagcount==size){
                realwinner=diagtemp;
                winnerwinnerchickendinner=true;
            }
        }

    }
    return winnerwinnerchickendinner;
}

void* matrix_func(void * arg){
    char *playertemp= (char *)arg;
    char player= *playertemp;
    while (gameison && realwinner=='n'){
        pthread_mutex_lock(&lock);
        if(realwinner=='n'&& winner!=player && gameison && !isFull()){
            int a=rand()%size;
            int b=rand()%size;
            while (matrix[a][b]!=' '){
                a=rand()%size;
                b=rand()%size;
            }
            if(realwinner=='n'){
            	matrix[a][b]=player;
            	printf("Player %c played on: (%d,",player,a);
            	printf("%d)\n",b);
           	winner=player;
            }
            if(check_win() || isFull() || realwinner!='n'){
                realwinner=player;
                gameison=false;
                pthread_mutex_unlock(&lock);
                return NULL;
            }
        }
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}

int main(int argc, char *args[]){
    srand (time(NULL));
    gameison=true;
    size = atoi(args[1]);
    matrix = (char **)malloc(size * sizeof(char *));
    int i;
    for (i=0;i<size;i++){
        matrix[i]=(char *) malloc(size*sizeof(char));
    }
    for (i=0;i<size;i++){//initalization
        int j;
        for (j=0;j<size;j++){
            matrix[i][j]=' ';
        }
    }
    char x='x';
    char o='o';
    pthread_t playerX,playerO;
    printf("Board Size: ");
    printf("%d",size);
    printf("x");
    printf("%d",size);
    printf("\n");
    winner='o';
    pthread_create(&playerX, NULL, matrix_func,&x);
    pthread_create(&playerO, NULL, matrix_func,&o);
    pthread_join(playerX,NULL);
    pthread_join(playerO,NULL);
    printf("Game end\n");
    if(check_win()){
        printf("Winner is %c\n",toupper(realwinner));
    }
    else if(isFull()){
        printf("It is a tie\n");
    }
    print_matrix();
    delete_matrix();
    return 0;
}
