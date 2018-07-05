#ifndef __BITMAP_H_
#define __BITMAP_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct Bitmap_t Bitmap;
typedef struct Bitmap_t* BitmapPtr;

typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t; 

#define BITMAP_BASE 32
extern uint32_t bit_mask[32];
extern uint32_t range_mask[32];

struct Bitmap_t{
    uint32_t* bitVector;
    uint32_t setSize;
    uint32_t vectorSize;

    // method 
    int (*create)(BitmapPtr bitmap,int size);
    int (*makeEmpty)(BitmapPtr bitmap);
    int (*getMember)(BitmapPtr bitmap,int x);
    int (*putMember)(BitmapPtr bitmap,int x ,int v);
    int (*addMember)(BitmapPtr bitmap, int x);
    int (*delMember)(BitmapPtr bitmap,int x);
};

int Bitmap_Create(BitmapPtr bitmap,int size);
int Bitmap_MakeEmpty(BitmapPtr bitmap);
int Bitmap_getMember(BitmapPtr bitmap,int x);
int Bitmap_putMember(BitmapPtr bitmap,int x ,int v);
int Bitmap_addMember(BitmapPtr bitmap, int x);
int Bitmap_delMember(BitmapPtr bitmap,int x);

BitmapPtr Bitmap_unionWith(BitmapPtr A,BitmapPtr B);
BitmapPtr Bitmap_intersectWith(BitmapPtr A, BitmapPtr B);
BitmapPtr Bitmap_differenceFrom(BitmapPtr A,BitmapPtr B);

#endif
