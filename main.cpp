/********************************************************************
 * This is the project of finding maximum clique of a given graph, 
 * assigned in the course Data Structure and Algorithm Practice.
 * 
 * The members are:
 *      Chen Zhibin     1700012764
 *      Wang Yuanfei    17000.....
 *      Lei Yuntong     1700012893
 *      Cao Haowei      1700013032
 *      Pan Shengyuan   17000.....
 * 
 * ###The style could be modified###
 * This algorithm is based on Ewls inspired by Shaowei Cai, PKU,
 * with reference: 
 *      Shaowei Cai, Kaile Su, Qingliang Chen, 
 *      Ewls: A New Local Search for Minimum Vertex Cover
 *      AAAI-10
 *******************************************************************/
#include<bits/stdc++.h>
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<set>
#include<cstring>
#include<algorithm>
using namespace std;

#define DEBUG
#ifdef DEBUG
    #define dbg_printf(...) printf(__VA_ARGS__)
#else
    #define dbg_printf(...)
#endif

// first, let's try minimum vertex cover
//use adjacent list to save the graph
const int maxn = 400000;
const int maxn_node = 800;
struct Edge{
    int to, weight, next, uncover_time;
}origin_edge[maxn << 1], edge[maxn << 1];
struct Less{
    Less(){}
    bool operator()(int x, int y){
        return edge[x].uncover_time < edge[y].uncover_time;
    }
};
int nodeN, edgeN;
int origin_head[maxn_node], head[maxn_node], cnt, origin_cnt, 
    min_coversize, max_steps, delta;
int coverable_degree[maxn_node], origin_degree[maxn_node];
multiset<int, Less> uncovered;  
    //must ensure unqual timestamps for different edges, thus use multiset
long timestamp;
bool vis[maxn], check[maxn], cover[maxn_node], 
    min_cover[maxn_node], tabu[maxn_node];
int cover_size, viscnt, check_cnt;

/* Basic operations */
void init();
void output();
void add_edge(int x, int y, Edge* _edge, int* _head, int* degree, int* cnt);
void Add_edge(int x, int y, Edge* _edge, int* _head, int* degree, int* cnt);
void find_complement();
Edge* find_edge(int x, int y, Edge* _edge, int* _head);
/* Components of the Algorithm */
int score(int x, int y);
int dscore(int x, int y);
/* Main part of the Algorithm */
void greedy();  // initialize cover set by using greedy policy
void add_to_cover(int node);
void delete_from_cover(int node);
int choose_exchange_pair();
void Ewls(int max_steps);

/**
 * main - init and run Ewls
 */
int main(){;
    while(~scanf("%d%d", &nodeN, &edgeN)){        
        init();
        // TODO: modify delta and max_steps
        delta = 2;
        max_steps = nodeN * 3;
        for(int i = 0; i < edgeN; ++i){
            int x, y;
            scanf("%d%d", &x, &y);
            Add_edge(x, y, origin_edge, origin_head, 
                origin_degree, &origin_cnt);
        }
        find_complement();  // here edgeN is redefined
        srand(time(NULL));
        Ewls(max_steps);
        output();
    }
    return 0;
}

/**
 * init - initialization for every input data set
 */
void init(){
    memset(cover, 0, sizeof(cover));
    memset(min_cover, 0, sizeof(min_cover));
    memset(edge, 0, sizeof(edge));
    memset(origin_edge, 0, sizeof(origin_edge));
    memset(head, -1, sizeof(head));
    memset(origin_head, -1, sizeof(origin_head));
    memset(vis, 0, sizeof(vis));
    memset(check, 0, sizeof(check));
    memset(coverable_degree, 0, sizeof(coverable_degree));
    memset(origin_degree, 0, sizeof(origin_degree));
    memset(tabu, 0, sizeof(tabu));
    uncovered.clear();
    max_steps = 0;
    cnt = 0; origin_cnt = 0;
    viscnt = 0; check_cnt = 0;
    timestamp = 0;
    cover_size = 0; min_coversize = 0;
}

/**
 * output - output the cover
 */
void output(){
    printf("%d\n", nodeN - min_coversize);
    for(int i = 1; i <= nodeN; ++i)
        if(!min_cover[i])
            printf("%d ", i);
    printf("\n");
}

/**
 * add_edge - add a directed edge into the edge list,
 *      maintain head[] as edge list head, index is node number
 * 
 *      cnt counts from 0
 */
void add_edge(int x, int y, Edge* _edge, int* _head, int* degree, int* cnt){
    _edge[*cnt].to = y;
    _edge[*cnt].weight = 1;
    _edge[*cnt].next = _head[x];
    _head[x] = (*cnt)++;
    degree[x]++;
}

/**
 * Add_edge - wrapper of add() function, add two directions of an edge
 *      at the same time to make it easy to search edges. 
 *      Also, their id are the same.
 */
void Add_edge(int x, int y, Edge* _edge, int* _head, int* degree, int* cnt){
    add_edge(x, y, _edge, _head, degree, cnt);
    add_edge(y, x, _edge, _head, degree, cnt);
}

/**
 * find_complement - find the complement graph of the given graph, 
 *      the MVC in the complement is the MC in the original graph
 */
void find_complement(){
    edgeN = 0;  //here redefine edgeN
    for(int i = 1; i <= nodeN - 1; ++i){
        for(int j = i + 1; j <= nodeN; ++j){
            if(i != j && find_edge(i, j, origin_edge, origin_head) == NULL){
                Add_edge(i, j, edge, head, coverable_degree, &cnt);
                //add the edges into uncovered set
                uncovered.insert(cnt - 1);
                uncovered.insert(cnt - 2);
                edgeN++;
            }
        }
    }
}

/**
 * find_edge - to find if an edge between x and y exists
 *      when get edge[i], edge[i ^ 1] refer to the same undirected edge
 */
Edge* find_edge(int x, int y, Edge* _edge, int* _head){
    for(int i = _head[x]; i != -1; i = _edge[i].next){
        if(_edge[i].to == y)
            return &_edge[i];
    }
    return NULL;
}

/**
 * dscore - calculate dscore of a node
 *      time complexity: O(nodeN)
 * 
 *      TODO: test it, maybe we should maintain an array of Dscore[]?
 */
int dscore(int x){
    int res = 0;
    for(int i = head[x]; i != -1; i = edge[i].next){
        if(cover[edge[i].to]) continue;
        if(cover[x])
            res -= edge[i].weight;
        else
            res += edge[i].weight;
    }
    return res;
}

/**
 * score - calculate score of a node pair
 *      time complexity: O(nodeN), 
 *          but if Dscore is maintained, then O(1), 
 *          so Score[] is not necessary
 * 
 *      TODO: test it, maybe we should maintain an array of Score[]?
 */
int score(int x, int y){
    Edge *p = find_edge(x, y, edge, head);
    return dscore(x) + dscore(y) + (p ? p->weight : 0);
}

/**
 * greedy - find an initial cover set
 *      complexity seems to be O(n^2)...
 * 
 *      TODO: test it
 */
void greedy(){
    while(viscnt < edgeN){
        int max_degree = 0;
        int node = 0;
        for(int i = 1; i <= nodeN; ++i){
            if(max_degree < coverable_degree[i]){
                max_degree = coverable_degree[i];
                node = i;
            }
        }
        add_to_cover(node);
    }
}

/**
 * add_to_cover - add a node to cover set, and set visited to the edges
 *      associated with the node
 * 
 *      TODO: test it
 */
void add_to_cover(int node){
    if(cover[node]) return;
    int weight = 0;
    dbg_printf("added a node %d\nadd edges: ", node);
    for(int i = head[node]; i != -1; i = edge[i].next){
        if(!vis[i] && !vis[i ^ 1] && !cover[edge[i].to]){
            dbg_printf("%d ", i);
            vis[i] = true;
            vis[i ^ 1] = true;
            viscnt++;
            coverable_degree[node]--;
            coverable_degree[edge[i].to]--;
            uncovered.erase(i);
            uncovered.erase(i ^ 1);
            weight += edge[i].weight;
        }
    }
    dbg_printf("\n");
    cover[node] = true;
    cover_size++;
    dbg_printf("covered weight = %d\n", weight);
    dbg_printf("viscnt changed! viscnt = %d\n", viscnt);
    dbg_printf("cover_size changed!, cover_size = %d\n", cover_size);
}

/**
 * delete_from_cover - remove a node from cover set
 * 
 *      TODO: test it
 */
void delete_from_cover(int node){
    if(!cover[node]) return;
    int weight = 0;
    dbg_printf("delete a node %d\nuncover edges: ", node);
    for(int i = head[node]; i != -1; i = edge[i].next){
        if(vis[i] && vis[i ^ 1] && !cover[edge[i].to]){
            dbg_printf("%d ", i);
            vis[i] = false;
            vis[i ^ 1] = false;
            viscnt--;
            coverable_degree[node]++;
            coverable_degree[edge[i].to]++;
            uncovered.erase(i);
            uncovered.erase(i ^ 1);
            edge[i].uncover_time = timestamp;
            edge[i ^ 1].uncover_time = timestamp++;
            uncovered.insert(i);
            uncovered.insert(i ^ 1);
            weight += edge[i].weight;
        }
    }
    dbg_printf("\n");
    cover[node] = false;
    cover_size--;
    dbg_printf("uncovered weight = %d\n", weight);
    dbg_printf("viscnt changed! viscnt = %d\n", viscnt);
    dbg_printf("cover_size changed!, cover_size = %d\n", cover_size);
}

/**
 * choose_exchange_pair - choose an edge with a node in cover 
 *      and the other not
 *      return: the index of the edge in edge[]
 * 
 *      TODO: check if the random policy is correct and 
 *      corresponds with the paper
 */
int choose_exchange_pair(){
    memset(check, 0, sizeof(check));
    int res[maxn_node << 1] = {0};
    int edge_cnt = 0;
    multiset<int, Less>::iterator p = uncovered.begin();
    int sel = *p;   
    // compute available node pair set
    if(sel == -1) return -1;
    int v[2] = {edge[sel].to, edge[sel ^ 1].to};
    for(int k = 0; k < 2; ++k)
        for(int i = head[v[k]]; i != -1; i = edge[i].next){
            if(!tabu[edge[i].to] && 
                cover[edge[i].to] && score(edge[i].to, v[k]) > 0)
            {
                res[edge_cnt++] = i;
            }
        }
    // select a pair randomly and return the edge between the nodes
    if(edge_cnt){
        sel = rand() % edge_cnt;
        return res[sel];
    }
    else{
        for(++p; p != uncovered.end(); ++p){
            sel = *p;
            if(check[sel]) continue;    //if already checked, skip it
            check[sel] = true;
            check[sel ^ 1] = true;
            v[0] = edge[sel].to; v[1] = edge[sel ^ 1].to;
            for(int k = 0; k < 2; ++k)
                for(int i = head[v[k]]; i != -1; i = edge[i].next){
                    check[i] = true;    //already checked the edge
                    if(!tabu[edge[i].to] && 
                        cover[edge[i].to] && score(edge[i].to, v[k]) > 0)
                    {
                        res[edge_cnt++] = i;
                    }
                }
            if(edge_cnt){
                sel = rand() % edge_cnt;
                return res[sel];
            }
        }
    }
    return -1;
}

/**
 * Ewls - main algorithm
 *      set min_cover[] during running
 */
void Ewls(int max_steps){
    greedy();
    int upper_bound = cover_size;
    dbg_printf("ub = %d\n", upper_bound);
    dbg_printf("cover_size = %d\n", cover_size);
    memcpy(min_cover, cover, sizeof(cover));
    min_coversize = cover_size;
    dbg_printf("min_coversize = %d\n", min_coversize);
    //remove some nodes to make coversize be upper_bound - delta
    int sel_node;
    for(int i = 0; i < delta; ++i){
        sel_node = rand() % nodeN + 1;
        while(!cover[sel_node]){
            sel_node = rand() % nodeN + 1;
        }
        delete_from_cover(sel_node);
    }
    for(int step = 0; step < max_steps; ++step){
        int idx = choose_exchange_pair();
        if(idx != -1){
            dbg_printf("idx = %d\n", idx);
            int u = edge[idx].to, v = edge[idx ^ 1].to;
            dbg_printf("*************\n");
            delete_from_cover(u);
            add_to_cover(v);
            dbg_printf("*****************\n");
            tabu[u] = true;
            tabu[v] = false;
        }
        else{
            // update edge weight
            multiset<int, Less>::iterator p;
            for(p = uncovered.begin(); p != uncovered.end(); ++p)   // IMPORTANT!!!
                edge[*p].weight++;
            continue;
        }
        if(cover_size + (edgeN - viscnt) < upper_bound){
            upper_bound = cover_size + (edgeN - viscnt);
            dbg_printf("upper bound lowered!, now ub = %d\n", upper_bound);
            if(viscnt == edgeN){
                memcpy(min_cover, cover, sizeof(cover));
                min_coversize = cover_size;
                dbg_printf("min_coversize = %d\n", min_coversize);
            }
            else{
                dbg_printf("use greedy to cover the remained edges\n");
                // copy the origin status before greedy
                bool tmp_cover[maxn_node] = {0}, tmp_vis[maxn] = {0};
                int tmp_cover_size = cover_size, tmp_viscnt = viscnt;
                multiset<int, Less> tmp_uncover = uncovered;    
                int tmp_degree[maxn_node] = {0};
                memcpy(tmp_cover, cover, sizeof(cover));
                memcpy(tmp_vis, vis, sizeof(vis));
                memcpy(tmp_degree, coverable_degree, sizeof(coverable_degree));

                greedy();   // TODO: check it, this is still doubtful
                memcpy(min_cover, cover, sizeof(cover));
                min_coversize = cover_size;
                dbg_printf("min_coversize = %d\n", min_coversize);

                // recover the origin status before greedy
                cover_size = tmp_cover_size; viscnt = tmp_viscnt;
                uncovered = tmp_uncover;
                memcpy(cover, tmp_cover, sizeof(tmp_cover));
                memcpy(vis, tmp_vis, sizeof(tmp_vis));
                memcpy(coverable_degree, tmp_degree, sizeof(tmp_degree));
            }
            // remove some nodes from cover
            for(int i = 0; i < delta; ++i){
                sel_node = rand() % nodeN + 1;
                while(!cover[sel_node]){
                    sel_node = rand() % nodeN + 1;
                }
                delete_from_cover(sel_node);
            }
        }
    }
}