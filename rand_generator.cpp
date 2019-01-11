#include<bits/stdc++.h>
#include<unistd.h>
using namespace std;
int main(){
    int nodeN, edgeN, edgecnt;;
    srand(time(NULL));
    scanf("%d", &nodeN);
    edgeN = nodeN * (nodeN - 1) / 2;
    edgecnt = rand() % edgeN + 1;
    printf("%d %d\n", nodeN, edgecnt);
    for(int i = 0; i < edgecnt; ++i){
        printf("%d %d\n", rand() % nodeN + 1, rand() % nodeN + 1);
    }
    return 0;
}