#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

typedef struct DictionaryNode
{
    char *title;
    char *name;
    char *surname;
    struct DictionaryNode *next;
} DictionaryNode;

DictionaryNode* head, *findnode;

DictionaryNode * Addtodictionary(char titlepara, char*namepara, char *surnamepara){
    DictionaryNode *newNode= (DictionaryNode *)malloc(sizeof(DictionaryNode));
    newNode->title=(char *)malloc(sizeof(char)*4);
    if(titlepara=='f')
        newNode->title="Ms.";
    else{
        newNode->title="Mr.";
    }
    newNode->name= (char*)malloc(sizeof(char)*(strlen(namepara)+1));
    newNode->next=NULL;
    newNode->surname=(char*)malloc(sizeof(char)*(strlen(surnamepara)+1));
    strcpy(newNode->name,namepara);
    strcpy(newNode->surname,surnamepara);
    if(head==NULL)
    {   
        head=newNode;
        return head;
    }
    else{
        DictionaryNode* ptr=head;
        while(ptr->next!=NULL){
            ptr=ptr->next;
        }
        ptr->next=newNode;
        return head;
    }

}



void ReadDatabase(){
    char inputstring[50], name[25],surname[30],sex;
    bool isnameread=false,istitleread=false;
    FILE * ptr = fopen("database.txt","r+");
    while (fscanf(ptr,"%s",inputstring) != EOF){
        if(!istitleread){
            sex=inputstring[0];
            istitleread=true;
        }
        else if(!isnameread){
            strcpy(name,inputstring);
            isnameread=true;
        }
        else{
            isnameread=false;
            istitleread=false;
            strcpy(surname,inputstring);
            head=Addtodictionary(sex,name,surname);
        }
    }
    fclose(ptr);
}

bool Dictionaryfind(char * namepara){
    DictionaryNode * ptr=head;
    while (ptr!=NULL && strcmp(ptr->name,namepara)!=0){
        ptr=ptr->next;
    }
    if(ptr!=NULL){
        findnode=ptr;
        return true;
    }
    findnode=NULL;
    return false;
}

void correctfile(char* dir, char*dname){
    char word[256];
    char txtname[256];
    strcpy(txtname,dir);
    strcat(txtname,"/");
    strcat(txtname,dname);
    FILE * ptr = fopen(txtname, "r+");
    while (fscanf(ptr, "%s", word)!= EOF)
    {
        if(Dictionaryfind(word)){
            fseek(ptr,(-strlen(word)-4),SEEK_CUR);//beginning of title
            fputs(findnode->title,ptr);
            fseek(ptr,strlen(word)+2,SEEK_CUR);//beginning of surname
            fputs(findnode->surname,ptr);
        }
    }
    fclose(ptr);
}

void Destructor(){
    DictionaryNode * ptr=head;
    DictionaryNode* temp;
    while(ptr!=NULL){
        temp=ptr->next;
        //free(ptr->title);
        free(ptr->name);
        free(ptr->surname);
        free(ptr);
        ptr=temp;
    }
}

void Searcher(char *dir){
    struct dirent* directent;
    DIR *directory = opendir(dir);
    if (dir!=NULL){
        directent= readdir(directory);
        while(directent!=NULL){
            int follen= strlen(directent->d_name);
            char extension[5];
            memcpy(extension,&directent->d_name[follen-4],4);
            extension[4]='\0';
            if(directent->d_type == DT_REG){
                if(!(strcmp(directent->d_name, "database.txt") == 0 && strcmp(dir, ".") == 0)&&strcmp(extension,".txt")==0){
                    char dname[256];
                    strcpy(dname,directent->d_name);
                    correctfile(dir,dname);
                }
            }
            else if(directent->d_type== DT_DIR && strcmp(directent->d_name,".") !=0 && strcmp(directent->d_name,"..")!=0){//This line was inspired from CodeVault youtube channel
                char recursion[512];
                sprintf(recursion,"%s/%s",dir,directent->d_name);
                Searcher(recursion);
            }
            directent= readdir(directory);
        }
    }
    closedir(directory);
}

int main(int argc, char *argv[])
{
    head=NULL;
    ReadDatabase();
    Searcher(".");
    Destructor();
    return 0;
}
