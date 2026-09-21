#include <stdlib.h>
#include "Linkedlist.h"



//setup 함수 nil Node 를 만들고 List 구조체 안에 넣고 Lsit 자체가 시작 지점이 된다. 

void Setup_List(List * L)
{
    L->nil.next = &(L->nil);
    L->nil.prev = &(L->nil);
}


//값을 삽입하고 성공하면 0을 실패하면 1을 반환한다. 산입할 때는 Lsit에서 nil 다음 위치로 할당이 된다. 
int Insert_Data(int data, List * L)
//사실 너가 포인터로 넘기라고 해서 그렇게 하고 있기는 한데 아직 그 이유를 잘 모르겠어. 
//대강은 알겠어 아마도 변수를 선언하는 순간 값이 복사되고 새로운 주소 값을 부여 받지. 그래서 다른 List 를 선언해서 대입한다는 가정하게 그게 원하게 흘러가지 않을 것이다는 점을 알고 있어.
//근데 내가 궁금한 거는 만약 지금 List 를 포인터로 선언하지 않았을 경우에 함수의 인자로 들어갈 때도 그 값이 복사되어 들어가는지 알고 싶은거야 
{
    Node* new_Node = malloc(sizeof(Node));

    if(new_Node == NULL) return 1;

    new_Node->data = data;
    L->nil.next->prev = new_Node;
    new_Node->next = L->nil.next;
    L->nil.next = new_Node;
    new_Node->prev = &(L->nil);

    return 0;
}

Node * Search_Data(int data, List *L)
{
    Node* search = L->nil.next;

    while(search != &(L->nil))
    {
        if(search->data == data) return search;
        else 
        {
            search = search->next;
        }
    }

    return NULL;
}

int Delete_Data(int data, List * L)
{
    Node* Del_Node;
    Del_Node = Search_Data(data, L);

    if(Del_Node == NULL) return 1;

    else 
    {
        Del_Node->prev->next = Del_Node->next;
        Del_Node->next->prev = Del_Node->prev;

        free(Del_Node);
        return 0;
    }
}

int All_Delete(List* L)
{
    Node * start = L->nil.next;
    Node * temp;

    while(start != &(L->nil))
    {
        temp = start;
        start = start->next;
        free(temp);
    }

    L->nil.next = &(L->nil);
    L->nil.prev = &(L->nil);


    return 0;
}