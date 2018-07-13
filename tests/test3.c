//Sample 1
// int main()
// {
//     int n;
//     n = read();
//     if (n > 0)  write(1);
//     else if (n < 0) write (-1);
//     else write(0);
//     return 0;
// }

//Sample 2
// int fact(int n)
// {
//     if (n == 1)
//        return n;
//     else
//     return (n * fact(n - 1));
// }
// int main()
// {
//     int m, result;
//     m = read();
//     if (m > 1)
//         result = fact(m);
//     else
//         result = 1;
//     write(result);
//     return 0;
// }

// Sample 3
// struct Operands
// {
//     int o1;
//     int o2;
// };

// int add(struct Operands temp)
// {
//     return (temp.o1 + temp.o2);
// }
// int main()
// {
//     int n;
//     struct Operands op;
//     op.o1 = 1;
//     op.o2 = 2;
//     n = add(op);
//     write(n);
//     return 0;
// }

//Sample 4
// int add(int temp[2])
// {
//     return (temp[0] + temp[1]);
// }

// int main()
// {
//     int op[2];
//     int r[1][2];
//     int i = 0, j = 0;
//     while (i < 2)
//     {
//         while (j < 2)
//         {
//             op[j] = i + j;
//             j = j + 1;
//         }
//         r[0][i] = add(op);
//         write(r[0][i]);
//         i = i + 1;
//         j = 0;
//     }
//     return 0;
// }

// Sample 5
// struct a{
//     int i;
//     float j[4][9];
//     struct{ int j; float x[2]; }k[7];
// };
// int m(struct a b){
//     //b.i = b.k[0].j + c[0];
//     int mm = 8;
//     b.i = 1;
//     b.k[1].j = 2;
//     mm = mm + b.k[b.i].j;
//     return mm;
// }
// int add(int a,int b){
//     return a+b;
// }
// int main(){
//     int a = 0 ,b =1;
//     struct a k;    
//     a = add(a,b);
//     b = m(k);
//     write(a);
//     write(b);
//     if(a > k.i){
//         return a;
//     }
//     return b+k.k[2].j;
// }

// Sample 6
// int main()
// {
//     int a = 0, b = 1, i = 0, n;
//     n = read();
//     while (i < n)
//     {
//         int c = a + b;
//         write(b);
//         a = b;
//         b = c;
//         i = i + 1;
//     }
//     return 0;
// }

// Sample 7
// #include "stdio.h"
// int read(){
//     int x;
//     scanf("%d",&x);
//     return x;
// }
// int write(int x){
//     printf("%d ",x);
//     return 1;
// }

// int partition(int arr[10],int p, int q){
//     int pivot = arr[q-1];
//     int temp;
//     int i = p,j = p;
//     while(j < q-1){
//         if(arr[j] < pivot){
//             temp = arr[j];
//             arr[j] = arr[i];
//             arr[i] = temp;
//             i = i + 1;
//         }
//         j = j + 1;
//     }
//     temp = arr[q-1];
//     arr[q-1] = arr[i];
//     arr[i] = temp;
//     return i;
// }

// int quickSort(int arr[10],int p, int q){
//     int pivot;
//     if(p < q){
//         pivot = partition(arr,p,q);
//         quickSort(arr,p,pivot);
//         quickSort(arr,pivot+1,q);
//     }
//     return 1;
// }

// int main(){
//     int arr[10];
//     int i = 0,n = 0;
//     n = read();
//     while(i<n){
//         arr[i] = read();
//         i = i + 1;
//     }
//     // quick sort 
//     quickSort(arr,0,n);
//     i = 0;
//     while(i<n){
//         write(arr[i]);
//         i = i + 1;
//     }
//     return 1;
// }