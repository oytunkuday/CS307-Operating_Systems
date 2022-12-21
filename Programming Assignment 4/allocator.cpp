#include <string>
#include <algorithm>
#include <iostream>	
#include <list>
#include <pthread.h>
#include <unistd.h>

using namespace std;



struct HeapNode {
	int id;
    int size;
    int index;
    HeapNode* next=NULL;
    HeapNode(){}
    HeapNode(int idpara, int sizepara, int indexpara,HeapNode* nextpara=NULL):id(idpara),size(sizepara),index(indexpara),next(nextpara){}
};

class HeapManager {
	private:
        HeapNode* head=NULL;
        pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	public:
        HeapManager(){
            head= new HeapNode(-1,0,0,NULL);
            head->next=NULL;
        }
        ~HeapManager(){
            HeapNode * ptr=head;
            HeapNode * temp;
            while(ptr!=NULL)
            {
                temp=ptr->next;
                delete ptr;
                ptr=NULL;
                ptr=temp;
            }
        }
        int initHeap(int sizepara){
            head->size=sizepara;
            print();
            return 1;
        }
        int myMalloc(int ID, int size){
            pthread_mutex_lock(&lock);
            //cout<<ID<<"    "<<size<<" ALLOC"<<endl;
            HeapNode * ptr=head;
            HeapNode * prev=NULL;
            int index;
            while(ptr->id!=-1 || ptr->size<size){
                prev=ptr;
                ptr=ptr->next;
                if(ptr==NULL)
                    break;
            }     
            if(ptr==NULL){
                cout<<"Can not allocate, requested size "<< size <<" for thread "<< ID <<" is bigger than remaining size"<< endl;
                print();
                pthread_mutex_unlock(&lock);
                return -1;
            }
            else{
                if(ptr->size==size){
                    ptr->id=ID;
                    index=ptr->index;
                }
                else{
                    HeapNode* newNode=new HeapNode(ID,size,ptr->index,ptr);
                    if(prev!=NULL){
                        prev->next=newNode;
                    }
                    ptr->index=ptr->index+size;
                    ptr->size=ptr->size-size;
                    index=newNode->index;
                    if(newNode->index==0){
                        head=newNode;
                    }
                }
                cout <<"Allocated for thread "<<ID<<endl;
                print();             
                pthread_mutex_unlock(&lock);
                return index;
            }
            
        }
        int myFree(int ID, int index){
            pthread_mutex_lock(&lock);
            //cout<<ID<<"          "<<index<<" FREE"<<endl;
            HeapNode * ptr=head;
            HeapNode * prev=NULL;
            while(ptr->id!=ID || ptr->index!=index){
                prev=ptr;
                ptr=ptr->next;
                if(ptr==NULL)
                    break;
            }
            if(ptr==NULL){
                cout<<"Can not free, requested index "<< index <<" for thread "<< ID <<" can not be found"<< endl;
                print();
                pthread_mutex_unlock(&lock);
                return -1;
            }
            else{
                ptr->id=-1;
                if(prev!=NULL&&prev->id==-1){
                    prev->size+=ptr->size;
                    prev->next=ptr->next;
                    delete ptr;
                    ptr=prev;
                }
                if(ptr->next!=NULL&&ptr->next->id==-1){
                    ptr->size+=ptr->next->size;
                    HeapNode * temp=ptr->next;
                    ptr->next=ptr->next->next;
                    delete temp;
                }
                if(ptr->index==0){
                    head=ptr;
                }
                cout<<"Freed for thread "<<ID<<endl;
                print();
                pthread_mutex_unlock(&lock);
                return 1;
            }

        }
        void print(){
            HeapNode * ptr=head;
            while(ptr!=NULL){
                cout<<"["<<ptr->id<<"]"<<"["<<ptr->size<<"]"<<"["<<ptr->index<<"]";
                ptr=ptr->next;
                if(ptr!=NULL)
                    cout<<"---";
            }
            cout<<endl;
        }
};