#include "Bitmap.h"

uint32_t bit_mask[32] = {
    0x80000000,0x40000000,0x20000000,0x10000000,
    0x08000000,0x04000000,0x02000000,0x01000000,
    0x00800000,0x00400000,0x00200000,0x00100000,
    0x00080000,0x00040000,0x00020000,0x00010000,
    0x00008000,0x00004000,0x00002000,0x00001000,
    0x00000800,0x00000400,0x00000200,0x00000100,
    0x00000080,0x00000040,0x00000020,0x00000010,
    0x00000008,0x00000004,0x00000002,0x00000001
};

uint32_t range_mask[32] = {
    0xffffffff,0x7fffffff,0x3fffffff,0x1fffffff,
    0x0fffffff,0x07ffffff,0x03ffffff,0x01ffffff,
    0x00ffffff,0x007fffff,0x003fffff,0x001fffff,
    0x000fffff,0x0007ffff,0x0003ffff,0x0001ffff,
    0x0000ffff,0x00007fff,0x00003fff,0x00001fff,
    0x00000fff,0x000007ff,0x000003ff,0x000001ff,
    0x000000ff,0x0000007f,0x0000003f,0x0000001f,
    0x0000000f,0x00000007,0x00000003,0x00000001
};

int Bitmap_Create(BitmapPtr bitmap,int size){
    bitmap->vectorSize = (size+31)>>5;
    bitmap->setSize = size;
    bitmap->bitVector = (uint32_t*)malloc(sizeof(uint32_t)*bitmap->vectorSize);
    if(!bitmap->bitVector){
        printf("Create bitmap failed!\n");
        return 0;
    }
    int iter = 0;
    for(;iter < bitmap->vectorSize;iter++){
        bitmap->bitVector[iter] = 0;
    }
    return 1;
}

int Bitmap_MakeEmpty(BitmapPtr bitmap){
    int iter = 0;
    for(;iter < bitmap->vectorSize; iter++){
        bitmap->bitVector[iter] = 0;
    }
    return 1;
}

int Bitmap_getMember(BitmapPtr bitmap,int x){
    int nr_vector = x / 32, nr_id = x % 32;
    uint32_t elem = bitmap->bitVector[nr_vector];
    return (elem>>(31-x))%2;
}

int Bitmap_putMember(BitmapPtr bitmap,int x, int v){
    int nr_vector = x / 32, nr_id = x % 32;
    if(v == 1)
        bitmap->bitVector[nr_vector] |= bit_mask[nr_id];
    else if(v == 0)
        bitmap->bitVector[nr_vector] &= ~bit_mask[nr_id];
    return 1;
}

int Bitmap_addMember(BitmapPtr bitmap, int x){
    if(x >=0 && x <= bitmap->setSize){
        if(Bitmap_getMember(bitmap,x) == 0){
            Bitmap_putMember(bitmap,x,1);
            return 1;
        }
    }
    return 0;
}

int Bitmap_delMember(BitmapPtr bitmap,int x){
    if(x>=0 && x <= bitmap->setSize){
        if(Bitmap_getMember(bitmap,x) == 1){
            Bitmap_putMember(bitmap,x,0);
            return 1;
        }
    }
    return 0;
}

BitmapPtr Bitmap_unionWith(BitmapPtr A,BitmapPtr B){
    BitmapPtr setA,setB,setC;
    if(A->setSize >= B->setSize){
        setA = A;
        setB = B;
    }else{
        setA = B;
        setB = A;
    }
    setC = (BitmapPtr)malloc(sizeof(Bitmap));
    Bitmap_Create(setC,setA->setSize);

    // union operation
    int iter = 0;
    for(;iter < setB->vectorSize; iter++){
        setC->bitVector[iter] = setA->bitVector[iter] | setB->bitVector[iter];
    }
    for(;iter < setA->vectorSize; iter++){
        setC->bitVector[iter] = setA->bitVector[iter];
    }
    return setC;
}

BitmapPtr Bitmap_intersectWith(BitmapPtr A, BitmapPtr B){
    BitmapPtr setA,setB,setC;
    if(A->setSize >= B->setSize){
        setA = A;
        setB = B;
    }else{
        setA = B;
        setB = A;
    }
    setC = (BitmapPtr)malloc(sizeof(Bitmap));
    Bitmap_Create(setC,setB->setSize);

    // intersect operation
    int iter = 0;
    for(;iter < setB->vectorSize; iter++){
        setC->bitVector[iter] = setA->bitVector[iter] & setB->bitVector[iter];
    }
    
    return setC;
}

BitmapPtr Bitmap_differenceFrom(BitmapPtr A,BitmapPtr B){
    BitmapPtr setA,setB,setC;
    if(A->setSize >= B->setSize){
        setA = A;
        setB = B;
    }else{
        setA = B;
        setB = A;
    }
    setC = (BitmapPtr)malloc(sizeof(Bitmap));
    Bitmap_Create(setC,A->setSize);

    // difference operation
    int iter = 0;
    for(;iter < setB->vectorSize; iter++){
        setC->bitVector[iter] = A->bitVector[iter] & (~B->bitVector[iter]);
    }
    return setC;
}