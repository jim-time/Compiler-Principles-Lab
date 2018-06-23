#include "my_list.h"

int List_Create(ListPtr list){
    list->size = 0;
    list->trailer = (ListNodePtr)malloc(sizeof(ListNode));
    list->header = (ListNodePtr)malloc(sizeof(ListNode));
    list->trailer->prev = list->header;
    list->trailer->succ = NULL;
    
    list->header->prev = NULL;
    list->header->succ = list->trailer;

    return 1;
}

int List_Destroy(ListPtr list){
    ListNodePtr pNode = list->header->succ;
    for(;pNode != list->trailer;){
        ListNodePtr old = pNode;
        pNode = pNode->succ;
        free(old);
    }
    return 1;
}

int List_Ins_XY(ListPtr list, ListNodePtr nodex,ListNodePtr nodey){
    nodey->succ = nodex->succ;
    nodey->prev = nodex;
    nodex->succ->prev = nodey;
    nodex->succ = nodey;

    return 1;
}
int List_Ins_YX(ListPtr list, ListNodePtr nodex,ListNodePtr nodey){
    nodey->succ = nodex;
    nodey->prev = nodex->prev;
    nodex->prev->succ = nodey;
    nodex->prev = nodey;
    return 1;
}
int List_Del(ListPtr list, ListNodePtr node){
    node->prev->succ = node->succ;
    node->succ->prev = node->prev;
    free(node);

    return 1;
}

int List_PushBack(ListPtr list, ListNodePtr node){
    node->prev = list->trailer->prev;
    node->succ = list->trailer;
    list->trailer->prev->succ = node;
    list->trailer->prev = node;
    return 1;
}

ListNodePtr List_Pop(ListPtr list){
    ListNodePtr ptail;
    if(list->trailer->prev->prev){
        ptail = list->trailer->prev;
        list->trailer->prev->prev->succ = list->trailer;
        list->trailer->prev = list->trailer->prev->prev;
        //free(ptail);
    }
    return ptail;
}
