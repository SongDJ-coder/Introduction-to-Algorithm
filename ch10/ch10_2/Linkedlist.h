
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
/*
초기화	빈 리스트를 만든다. 아직 상자는 하나도 없는 상태
탐색	주어진 값을 가진 상자를 앞에서부터 훑어서 찾는다. 찾으면 그 상자, 없으면 "없음"
삽입	새 상자를 만들어 맨 앞에 붙인다
삭제	지목된 상자 하나를 연결에서 빼내고 메모리를 돌려준다
전체 해제	남아 있는 상자를 전부 정리한다
*/


//Node 가 가지고 있어야 하는 값 : 데이터 , 앞노드 , 뒤노드 
typedef struct node {

    int data;
    struct node *next;
    struct node * prev;
    
}Node;

//연결 리스트 역할을 할 구조체를 선언
typedef struct{
    Node nil;
}List;

//초기화 부분: 처음 NULL 값을 가지고 있는 노드를 만들어서 다음하고 이전을 자기 자신으로 한다. 
//Null 노드를 생성하고 해당 노드의 주소 값을 반환 값으로 반환한다. 
void Setup_List(List * L);

//탐색	주어진 값을 가진 상자를 앞에서부터 훑어서 찾는다. 찾으면 그 상자, 없으면 "없음"
//리스트의 시작 지점에서 하나씩 순회 하면서 해당 값의 Node 주소를 반환하는 함수
Node * Search_Data(int data, List *L);

//삽입	새 상자를 만들어 맨 앞에 붙인다
//삽입할 데이터를 NULL 노드 다음으로 이어 붙이는 함수      성공 실패를 int 값을 통해서 알려준다. 
int Insert_Data(int data, List * L);

//삭제	지목된 상자 하나를 연결에서 빼내고 메모리를 돌려준다
//하나씩 순회 하면서 해당 값이 나오면 삭제하고 주소 값을 반환하는 함수 
int Delete_Data(int data, List * L);

//전체 해제	남아 있는 상자를 전부 정리한다
//성공 실패를 int 값을 이용해서 알려준다. 
int All_Delete(List* L);




#endif