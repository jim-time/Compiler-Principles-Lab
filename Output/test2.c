// ./parser ./Output/test2.c > ./Output/test2.output
// int main(float b);
// int main(){
//     int i,j;
//     float k,m;
//     //if(k>m){
//         {
//         i = j+m;
//         j = i*j;
//     }
// }

//Sample 1
// int main()
// {
//     int i = 0;
//     j = i + 1;
// }

//Sample 2
// int main()
// {
//     int i = 0;
//     inc(i);
// }

//Sample 3
// int main()
// {
//     int i, j;
//     int i;
// }

//Sample 4
// int func(int i)
// {
//     return i;
// }

// int func()
// {
//     return 0;
// }

// int main()
// {
// }

//Sample 5
// int main()
// {
//     int i;
//     i = 3.7;
// }

//Sample 6
// int main()
// {
//     int i;
//     10 = i;
// }

//Sample 7
// int main()
// {
//     float j;
//     10 + j;
// }

//Sample 8
// int main()
// {
//     float j = 1.7;
//     return j;
// }

//Sample 9
// int func(int i)
// {
//     return i;
// }

// int main()
// {
//     func(1, 2);
// }

//Sample10
// int main()
// {
//     int i;
//     i[0];
// }

//Sample 11
// int main()
// {
//     int i;
//     i(10);
// }

//Sample 12
// int main()
// {
//     int i[10];
//     i[1.5] = 10;
// }

// Sample 13
// struct Position
// {
//     float x, y;
// };

// int main()
// {
//     int i;
//     i.x;
// }

//Sample 14
// struct Position
// {
//     float x, y;
// };

// int main()
// {
//     struct Position p;
//     if (p.n == 3.7)
//     return 0;
// }

// Sample 15
// struct Position
// {
//     float x, y;
//     int x;
// };

// int main()
// {
// }

// Sample 16
// struct Position
// {
//     float x;
// };

// struct Position
// {
//     int y;
// };

// int main()
// {
// }

//Sample 17
// int main()
// {
//     struct Position pos;
// }

// Option Sample 1
// int func(int a);
// int func(int a)
// {
//     return 1;
// }

// int main()
// {
// }

// Option Sample 2
struct Position
{
    float x,y;
};
int func(int a);

int func(struct Position p);

int main()
{
}

// Option Sample 3
// int func()
// {
//     int i = 10;
//     return i;
// }

// int main()
// {
//     int i;
//     i = func();
// }

// Option Sample 4
// int func()
// {
//     int i = 10;
//     return i;
// }

// int main()
// {
//     int i;
//     int i, j;
//     i = func();
// }

//Option Sample5
// struct Temp1
// {
//     int i;
//     float j;
// };

// struct Temp2
// {
//     int x;
//     float y;
// };

// int main()
// {
//     struct Temp1 t1;
//     struct Temp2 t2;
//     t1 = t2;
// }

//Option Sample6
// struct Temp1
// {
//     int i;
//     float j;
// };

// struct Temp2
// {
//     int x;
// };

// int main()
// {
//     struct Temp1 t1;
//     struct Temp2 t2;
//     t1 = t2;
// }

//Option Sample7
// int f()
// {
//     int a, b, c;
//     a = a + b;
//     if (b > 0)
//     {
//         int a = c * 2;
//         b = b - a;
//     }
// }