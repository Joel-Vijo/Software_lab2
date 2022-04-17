#include <stdio.h>
#include <stdlib.h>
enum status{Free, Allocated};
enum bit{unset,set};
typedef struct Node{
    int i;
    int reference_count;
    enum bit mark;
    int size;
    enum status tag;
    struct Node *next;
    struct Node *prev;
    struct Node *next_free;//used for maintaining free list
    struct Node *prev_free;
}Node;

typedef struct Node2{
    Node *p;
    int i;
    struct Node2 *next[3];
}Node2;

void insert(Node2 **pointers,Node *p,int i){
    if(*pointers==NULL){
        *pointers=(Node2 *)malloc(sizeof(Node2));
        (*pointers)->p=p;
        (*pointers)->i=i;
        (*pointers)->next[0]=NULL;
        (*pointers)->next[1]=NULL;
        (*pointers)->next[2]=NULL;
    }
    else{
        Node2 *q=*pointers;
        while(q->next[0]!=NULL){
            q=q->next[0];
        }
        Node2 *t=(Node2 *)malloc(sizeof(Node2));
        t->p=p;
        q->next[0]=t;
        t->i=i;    
        t->next[0]=NULL;
        t->next[1]=NULL;
        t->next[2]=NULL;
    }
}

void remove_node(Node2 **pointers,Node *p){
    Node2 *q=*pointers;
    Node2 *r=NULL;
    while(q!=NULL && (q->p)!=p){
        r=q;
        q=q->next[0];
    }
    if(r!=NULL){
        r->next[0]=q->next[0];
    }
    else{
        *pointers=q->next[0];
    }
    free(q);
}

void initialize(Node *Heap){
    int s;
    printf("Enter size of heap in bits: ");
    scanf("%d",&s);
    Heap->size=s;
    Heap->next=NULL;
    Heap->prev=NULL;
    Heap->next_free=NULL;
    Heap->prev_free=NULL;
    Heap->tag=Free;
}

int closest_value(int a){
    int s=8;
    while(s<a){
        s=s+8;
    }
    return s;
}

Node *First_fit_Allocate(Node *Heap,int size,int i){
    Node *p=Heap;
    Node *q=p;
    while(q!=NULL && q->tag==1){
        q=q->next;
    }
    if(q==NULL){
        printf("Heap is completely utilized, Cannot allocate memory\n");
        return NULL;
    }
    while(q!=NULL && q->size<size){
        q=q->next_free;
    }
    if(q==NULL){
        printf("Cannot allocate memory of required size\n");
        return NULL;
    }
    else{
        int s=closest_value(size);
        Node *t=(Node *)malloc(sizeof(Node));
        if(s<q->size){
            t->size=s;
            t->tag=Allocated;
            t->next=q->next;
            if(q->next!=NULL){
                (q->next)->prev=t;
            }
            t->prev_free=NULL;
            t->next_free=NULL;
            t->prev=q;
            q->next=t;
            q->size=(q->size)-s;
            t->reference_count=1;
            t->i=i;
            return t;
        }
        else{
            if(q->prev_free!=NULL){
                (q->prev_free)->next_free=q->next_free;
            }
            if(q->next_free!=NULL){
                (q->next_free)->prev_free=q->prev_free;
            }
            q->tag=Allocated;
            q->prev_free=NULL;
            q->next_free=NULL;
            q->reference_count=1;
            q->i=i;
            free(t);
            return q;
        }
    }
}

Node *Best_fit_Allocate(Node *Heap, int size,int i){
    Node *p=Heap;
    Node *q=p;
    while(q!=NULL && q->tag==1){
        q=q->next;
    }
    if(q==NULL){
        printf("Heap is completely utilized, Cannot allocate memory\n");
        return NULL;
    }
    Node *best;
    while(q!=NULL && q->size<size){
        q=q->next_free;
    }
    if(q==NULL){
        printf("Cannot allocate memory of required size\n");
        return NULL;
    }
    else{
        best=q;
        while(q!=NULL){
            if(q->size>=size && q->size<best->size){
                best=q;
            }
            q=q->next_free;
        }
        q=best;
        int s=closest_value(size);
        Node *t=(Node *)malloc(sizeof(Node));
        if(s<q->size){
            t->size=s;
            t->tag=Allocated;
            t->next=q->next;
            if(q->next!=NULL){
                (q->next)->prev=t;
            }
            t->prev_free=NULL;
            t->next_free=NULL;
            t->prev=q;
            q->next=t;
            t->reference_count=1;
            q->size=(q->size)-s;
            t->i=i;
            return t;
        }
        else{
            if(q->prev_free!=NULL){
                (q->prev_free)->next_free=q->next_free;
            }
            if(q->next_free!=NULL){
                (q->next_free)->prev_free=q->prev_free;
            }
            q->tag=Allocated;
            q->prev_free=NULL;
            q->next_free=NULL;
            free(t);
            q->reference_count=1;
            q->i=i;
            return q;
        }
    }
}

void Delete(Node **Heap, Node *p){
    if(p->prev!=NULL && (p->prev)->tag==0){
        if(p->next!=NULL && (p->next)->tag==0){
            (p->prev)->size+=p->size+(p->next)->size;
            (p->prev)->next=(p->next)->next;
            if((p->next)->next!=NULL){
                ((p->next)->next)->prev=p->prev;
            }
            (p->prev)->next_free=(p->next)->next_free;
            if((p->next)->next_free!=NULL){
                ((p->next)->next_free)->prev_free=p->prev;                
            }
            free(p);
            free(p->next);
            p=NULL;
        }
        else{
            (p->prev)->size+=p->size;
            (p->prev)->next=p->next;
            if(p->next!=NULL){
                (p->next)->prev=p->prev;
            }
            free(p);
            p=NULL;
        }
    }
    else if(p->next!=NULL && (p->next)->tag==0){
        (p->next)->size+=p->size;
        if(p->prev!=NULL){
            (p->prev)->next=p->next;
        }
        else{
            *Heap=p->next;
        }
        (p->next)->prev=p->prev;
        free(p);
        p=NULL;
    }
    else{
        p->tag=Free;
        Node *q=*Heap;
        Node *r=NULL;
        while(q!=p){
            if(q->tag==0){
                r=q;
            }
            q=q->next;
        }
        p->prev_free=r;
        if(r!=NULL){
            p->next_free=r->next_free;
            r->next_free=p;
        }
        if(p->next_free!=NULL){
            (p->next_free)->prev_free=p;
        }
        p->i=0;
    }

}

int calculate_free_space(Node *Heap){
    Node *p=Heap;
    while(p!=NULL && p->tag==1){
        p=p->next;
    }
    int count=0;
    while(p!=NULL){
        count=count+p->size;
        p=p->next_free;
    }
    return count;
}

void print_free_list(Node *Heap){
    Node *p=Heap;
    while(p!=NULL && p->tag==1){
        p=p->next;
    }
    int i=0;
    while(p!=NULL){
        printf("Free Block %d: %d\n",i+1,p->size);
        p=p->next_free;
        i++;
    }
    printf("\n");
}

void print_heap(Node *Heap){
    Node *p=Heap;
    while(p!=NULL){
        printf("Block %d: %d",p->i,p->size);
        if(p->tag==0){
            printf(" Free\n");
        }
        else{
            printf(" Allocated\n");
        }
        p=p->next;
    }
    printf("\n");
}

void print_pointers(Node2 *pointers){
   if(pointers==NULL){
       return ;
   }
   printf("%d: %d ",pointers->i,pointers->p->reference_count);
   printf("[");
   print_pointers(pointers->next[0]);
   print_pointers(pointers->next[1]);
   print_pointers(pointers->next[2]);
   printf("]");
}

void delete_heap(Node *Heap){
    Node *p=Heap;
    Node *q=p->next;
    while(p!=NULL){
        free(p);
        p=q;
        if(q!=NULL){
            q=q->next;
        }
    }
}

void delete_pointers_list(Node2 *pointers){
    Node2 *p=pointers;
    Node2 *q=pointers->next[0];
    while(p!=NULL){
        free(p);
        p=q;
        if(q!=NULL){
            q=q->next[0];
        }
    }
}
void assign(Node2 *root2,Node2 *root1,int i){
    if(root2->next[i]!=NULL){
        Node2*p=root2->next[i];
        p->p->reference_count-=1;
    }
    root2->next[i]=root1;
    root1->p->reference_count+=1;
    
}
void deassign(Node2 *root){
    if(root->next[0]!=NULL){
        root->next[0]->p->reference_count-=1;
        root->next[0]=0;
    }
    if(root->next[1]!=NULL){
        root->next[1]->p->reference_count-=1;
        root->next[1]=0;
    }
    if(root->next[2]!=NULL){
        root->next[2]->p->reference_count-=1;
        root->next[2]=0;
    }
}
void garbage_collection_rc(Node **heap){
    Node *p=*heap;
    while(p!=NULL){
        if(p->tag==1 && p->reference_count==0){
            Delete(heap,p);
        }
        p=p->next;
    }
}
void marking(Node2 *root1){
    Node2 *prev=NULL;
    Node2 *p=NULL;
    Node2 *curr=root1;
    do{ 
        curr->p->mark=set;
        if(curr->next[0]!=NULL && curr->next[0]->p->mark!=1){
            p=prev;
            prev=curr;
            curr=curr->next[0];
            prev->next[0]=p;
        }    
        else if(curr->next[1]!=NULL && curr->next[1]->p->mark!=1){
            p=prev;
            prev=curr;
            curr=curr->next[1];
            prev->next[1]=p;
        }
        else if(curr->next[2]!=NULL && curr->next[2]->p->mark!=1){            
            p=prev;
            prev=curr;
            curr=curr->next[2];
            prev->next[2]=p;
        }
        else{
            if(prev!=NULL){
                if(prev->next[0]==p){
                    prev->next[0]=curr;
                }
                else if(prev->next[1]==p){
                    prev->next[1]=curr;
                }
                else{
                    prev->next[2]=curr;
                }
                curr=prev;
                prev=p;
                if(p!=NULL){
                    if(p->next[0]==NULL || p->next[0]->p->mark==1){
                        p=p->next[0];
                    }
                    else if(p->next[1]==NULL || p->next[1]->p->mark==1){
                        p=p->next[1];
                    }
                    else{
                        p=p->next[2];
                    }
                }
            }
        }    
    }
    while(curr!=root1);
}
void sweeping(Node **Heap){
    Node *p=*Heap;
    while(p!=NULL){
        if(p->tag==1 && p->mark!=1){
            Delete(Heap,p);
        }
        p=p->next;
    }
}
void garbage_collection_ms(Node2 *root1,Node2 *root2,Node **Heap){
    marking(root1);
    marking(root2);
    sweeping(Heap);
}
int main(){
    Node *Heap=(Node *)malloc(sizeof(Node));
    initialize(Heap);
    printf("------Heap Management------\n");
    printf("\n");
    Node2 *root1=NULL;
    Node2 *root2=NULL;
    int run=1;
    int i=0;
    while(run){
        int x;
        printf("Options: 1)First fit allocation  2)Best fit allocation  3)Free  4)Print heap  5)Print Free_list  6)Print pointers  7)Print free space  8)exit\n");
        printf("Enter your choice: ");
        printf("\n");
        scanf("%d",&x);
        Node *p;
        switch(x){
            case 1:;
                i++;
                int size;
                printf("Enter size to allocate: ");
                scanf("%d",&size);
                p=First_fit_Allocate(Heap,size,i);
                if(p!=NULL){
                    printf("%x\n",p);
                    insert(&root1,p,i);
                }
                break;
            case 2:;
                i++;
                int s;
                printf("Enter size to allocate: ");
                scanf("%d",&s);
                p=Best_fit_Allocate(Heap,s,i);
                if(p!=NULL){
                    printf("%x\n",p);
                    insert(&root2,p,i);
                }
                break;
            case 3:;
                printf("Enter address of Node to be freed: ");
                scanf("%p",&p);
                remove_node(&root1,p);
                Delete(&Heap,p);
                break;
            case 4:
                print_heap(Heap);
                break;
            case 5:
                print_free_list(Heap);
                break;
            case 6:
                print_pointers(root1);
                printf("\n");
                print_pointers(root2);
                printf("\n");
                break;
            case 7:
                printf("%d\n",calculate_free_space(Heap));
                break;
            case 8:
                run=0;
                break;
        }   
    }
    printf("root1: ");
    print_pointers(root1);
    printf("\n");
    printf("root2: ");
    print_pointers(root2);
    printf("\n");  
    printf("Creating Garbage\n");  
    Node2 *p=root2;
    Node2 *q=p->next[0]->next[0];
    root2=root1->next[0];
    root2->p->reference_count+=1;
    p->p->reference_count-=1;
    assign(root2,p,1);
    assign(root2,p->next[0],2);
    deassign(root2->next[1]);
    deassign(root2->next[2]);
    assign(q,q->next[0],1);
    assign(q,root2,0);
    struct Node2 *r=q->next[1]->next[0];
    assign(q->next[1],root2->next[1],0);
    assign(r,q->next[1],0);
    assign(r,root2->next[2],1);
    printf("root1: ");
    print_pointers(root1);
    printf("\n");
    printf("root2: ");
    print_pointers(root2);
    printf("\n");
    print_pointers(q);
    printf("\n");
    print_pointers(r);
    printf("\n");
    printf("Before garbage collection:\n");
    print_heap(Heap);
    garbage_collection_rc(&Heap);
    printf("After reference counting: \n");
    print_heap(Heap);
    garbage_collection_ms(root1,root2,&Heap);
    printf("After marksweep: \n");
    print_heap(Heap);
    delete_heap(Heap);
    delete_pointers_list(root1);
    delete_pointers_list(root2);
    delete_pointers_list(q);
    delete_pointers_list(r);
    return 0;
} 