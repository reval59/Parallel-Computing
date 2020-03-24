#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <cassert>

#include "smith_waterman.h"

using namespace std;

// #define DEBUG;

#ifdef DEBUG
void print(int *&array)
{
    // cout<<array[0]<<"\t";
    for (int i = 1; i <= array[0]; i++)
    {
        char buffer[15];
        sprintf (buffer, "%3d", array[i]);
        cout<<buffer<<" ";
        // cout << array[i] << " ";
    }
    cout << endl;
}
#endif
void list(int *&array, int n)
{
    array = new int[n + 1];
    array[0] = n;
    for (int i = 1; i < n + 1; i++)
    {
        array[i] = 0;
    }
}

int smith_waterman(char *a, char *b, int a_len, int b_len)
{
    int max_score = 0;

    int *first_atdia = NULL;
    int *second_atdia = NULL;
    int *current_atdia = NULL;
#ifdef DEBUG
    cout<<a_len<<" "<<b_len<<endl;
#endif
    list(first_atdia, 1);
    list(second_atdia, 2);


    for (int k=1; k<=a_len+b_len-1;k++){
      
        int i = min(k, a_len);
        int j = max(0, k-a_len) + 1;
        int cnt = min(k, min(b_len-j+1, i));

        int s;
       
        if(j==1){
                s=0;
            } else if(j==2){
                s=1;
            } else {
                first_atdia++;
                s=1;
        }
        
        list(current_atdia, cnt+2);
        for (int l=1; l<=cnt; l++){
            current_atdia[l+1] = max(0,
                                    max(first_atdia[l+s] + sub_mat(a[i-1], b[j-1]),
                                    max(second_atdia[l+s] - GAP,
                                    second_atdia[l+s+1] - GAP)));
            
            i--;j++;
            max_score = max(max_score, current_atdia[l + 1]);

            
        }
#ifdef DEBUG
        print(current_atdia);
#endif
        

        first_atdia = second_atdia; 
        second_atdia = current_atdia; 
    }

    return max_score;
}