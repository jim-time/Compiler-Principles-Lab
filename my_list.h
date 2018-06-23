#ifndef __MY_LIST_H_
#define __MY_LIST_H_

//#include "main.h"
#include "IntermediateCode.h"

typedef struct List_t List;
typedef List* ListPtr;
typedef InterCodeListNode ListNode;
typedef InterCodeListNodePtr ListNodePtr;

struct List_t{
    ListNodePtr header;
    ListNodePtr trailer;
    int size;
    int (*create)(ListPtr list);
    int (*destroy)(ListPtr list);
    int (*ins_xy)(ListPtr list, ListNodePtr nodex, ListNodePtr nodey);
    int (*ins_yx)(ListPtr list, ListNodePtr nodex,ListNodePtr nodey);
    int (*del)(ListPtr list, ListNodePtr node);
    int (*push_back)(ListPtr list, ListNodePtr node);
    ListNodePtr (*pop)(ListPtr list);
};

int List_Create(ListPtr list);
int List_Destroy(ListPtr list);
int List_Ins_XY(ListPtr list, ListNodePtr nodex,ListNodePtr nodey);
int List_Ins_YX(ListPtr list, ListNodePtr nodex,ListNodePtr nodey);
int List_Del(ListPtr list, ListNodePtr node);
int List_PushBack(ListPtr list, ListNodePtr node);
ListNodePtr List_Pop(ListPtr list);


#endif
