#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <vector> 
using namespace std;
#define MAXP 777
#define MAXE 250010

vector<int> ants[MAXP];
int antnow[MAXP],degree[MAXP];
bool map[MAXP][MAXP];
double essence[MAXP];
int nodecnt,edgecnt,antcnt;
int large_ans = 0;
int AGES = 0;
vector<int> current_best, global_best;
int timestamp = 0;

void antbirth(){
	//ants[0].clear();
	//ants[0].push_back(1);
	//antnow[0] = 1;
	for(int i = 0;i<antcnt;++i){
		ants[i].clear();
		int r = (rand()%nodecnt)+1;
		ants[i].push_back(r);
		antnow[i] = r;
	}
}

void get_a_clique(int who){
	auto *ap = &(ants[who]);
	bool flag = true;
	bool visited[MAXP] = {0};
	while(flag){
		flag = false;
		int now = antnow[who];
		vector<int> nextstep;
		for(int a:*ap)
			visited[a] = true;
		double psum = 0.0;
		for(int st = 1;st <= nodecnt;++st)
			if(!visited[st] && map[now][st]){
				bool ok = true;
				for(int a:*ap)
					if(!map[a][st]){
						ok = false;
						break;
					}
				if(ok){
					flag = true;
					nextstep.push_back(st);
					psum += degree[st] * essence[st];
				}
			}
		if(!flag) break;
		sort(nextstep.begin(),nextstep.end(),[&](int a,int b)->bool{return degree[a]*essence[a]>degree[b]*essence[b];});
		double rd = rand()/double(RAND_MAX);
		if(rd<=0.7-(timestamp/2000)*0.2){
			ap->push_back(nextstep[0]);
			antnow[who] = nextstep[0];
			continue;
		}
		double total_p = 0.0;
		for(int a:nextstep){
			rd = rand()/double(psum);
			total_p += degree[a]*essence[a];
			if(rd<=total_p){
				ap->push_back(a);
				antnow[who] = a;
				break;
			}
		}
	}
}

void update_essence(){
	bool visited[MAXP] = {0};
	int cs = current_best.size();
	for(int a:current_best)
		visited[a] = true;
	for(int i = 1;i<=nodecnt;++i){
		if(visited[i])
			essence[i] = 0.9*essence[i] + double(cs)/nodecnt;
		else
			essence[i] = 0.9*essence[i];
	}
}
 
int main(){
	while(scanf("%d%d",&nodecnt,&edgecnt)!=EOF){
		memset(map,0,sizeof(map));
		memset(essence,0,sizeof(essence));
		global_best.clear();
		large_ans = 0;
		for(int i = 1;i<=nodecnt;++i){
			ants[i].clear();
			essence[i] = 1.0;
		}	
		antcnt = sqrt(nodecnt)*3/2+1;
		AGES = 6000;
		int x,y;
		for(int i = 0;i<edgecnt;++i){
			scanf("%d%d",&x,&y);
			map[x][y] = map[y][x] = true;
			++degree[x];
			++degree[y];
		}
		srand((int)time(0));
		for(timestamp = 0;timestamp<AGES;++timestamp){
			antbirth();
			current_best.clear();
			for(int j = 0;j<antcnt;++j){
				get_a_clique(j);
				if(ants[j].size() > current_best.size())
					current_best = ants[j];
			}
			update_essence();
			if(current_best.size() > global_best.size()){
				global_best = current_best;
				large_ans = current_best.size();
			}
		}
		printf("%d\n",large_ans);
		sort(global_best.begin(),global_best.end());
		for(int a:global_best) printf("%d ",a);
		printf("\n");
	} 
}
