#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <math.h>
using namespace std;
#define netlistfile "../benchmarks/bench5.nl"
#define gridfile "../benchmarks/bench5.grid"
#define outputfile "./routeroutput.txt"
void showpq(priority_queue <int> q){
	while(!q.empty()){
		cout<< q.top()<<" ";
		q.pop();
	}
}
enum pred{
	N,S,W,E,up,down
};
//make as small as possible 
class cell{
	public:
		short cost;
		enum pred pred_cell;
		bool is_blocked(){
			if(cost<0) return true;
			return false;
		}
		bool is_routed;
		bool is_reached;
		
};
class net{
	public:
		bool lay_no[2];
		int x[2],y[2];
	
};
class wave_obj{
	public:
		enum pred pred_cell;
		bool lay_no;
		unsigned long pathcost;
		int x,y;
};
	bool operator<(const wave_obj& b,const wave_obj & a){
			return(b.pathcost>a.pathcost);
		}
bool istarget_reached(const wave_obj tar,const wave_obj  tem){
	if((tar.x==tem.x)&&(tar.y==tem.y)&&(tar.lay_no==tem.lay_no))
	return true;
	return false;
}
bool isbendpresent(const wave_obj temp,const wave_obj pre,const wave_obj src){
	if(istarget_reached(pre,src))return false;
	int pas_x,pas_y;
	pas_x=pre.x;
	pas_y=pre.y;
	switch (pre.pred_cell){
		case N:
			pas_y+=1;
			break;
		case S:
			pas_y-=1;
			break;
		case E:
			pas_x+=1;
			break;
		case W:
			pas_x-=1;
			break;
		default: 
			return false;
			break;		
	}
	if((pas_x!=temp.x)&&(pas_y!=temp.y)) return true;
	return false;
	
}
int bbox(int x1,int y1,int x2,int y2){
	return (abs(x2-x1)+abs(y2-y1)); 
}
int main(){
	int x_size,y_size,bendpenalty,viapenalty;
	//Input Files (Grid)
	
	ifstream fin(gridfile);
	if(fin.is_open()){
		fin>>x_size>>y_size;
		fin>>bendpenalty>>viapenalty;
		cell grid1[x_size][y_size],grid2[x_size][y_size];
		bool via[x_size][y_size];
		for(int j=0;j<y_size;j++){
			for(int i=0;i<x_size;i++){
				fin>>grid1[i][j].cost;
				via[i][j]=false;
			}
    	}
    	for(int j=0;j<y_size;j++){
			for(int i=0;i<x_size;i++)
				fin>>grid2[i][j].cost;
    	}
    	fin.close();
    //Netlist
    	int nnz,lno;
    	ifstream fnin(netlistfile);
    	fnin>>nnz;
    	net in_nets[nnz];
    	int i_lk;
    	for(int i=0;i<nnz;i++){
    		fnin>>i_lk;
    		for(int i_l=0;i_l<2;i_l++){
    			
    			fnin>>lno;
				in_nets[i].lay_no[i_l]=(lno==1)?false:true;
    			fnin>>in_nets[i].x[i_l]>>in_nets[i].y[i_l];
    			
			}
		}
		fnin.close();	
		//One layer unit cost router
		vector <wave_obj> netout[nnz];  
		
		for(int i=0;i<nnz;i++){
			cout<<i+1<<endl;
			wave_obj w_src,w_tar;
			//declare source and target of net
			w_src.lay_no=in_nets[i].lay_no[0];
			w_src.x=in_nets[i].x[0];
			w_src.y=in_nets[i].y[0];
			w_src.pathcost=1;
		
			if(!w_src.lay_no){
			grid1[w_src.x][w_src.y].is_reached='1';
			if(grid1[w_src.x][w_src.y].cost<0) grid1[w_src.x][w_src.y].cost=1;
			}
			else {
			grid2[w_src.x][w_src.y].is_reached='1';
			if(grid2[w_src.x][w_src.y].cost<0) grid2[w_src.x][w_src.y].cost=1;
			}
			w_tar.lay_no=in_nets[i].lay_no[1];
			w_tar.x=in_nets[i].x[1];
			w_tar.y=in_nets[i].y[1];
			cout<<"Source "<<w_src.x<<" "<<w_src.y<<" "<< w_src.lay_no<<endl;
			cout<<"Target "<<w_tar.x<<" "<<w_tar.y<<" "<< w_tar.lay_no<<endl;
			//expand
		priority_queue <wave_obj> wavefront;
		int mi=0;
		wavefront.push(w_src);
		bool target_found=false;
		bool notpossible=false;
			while(!target_found){
//				cout<<"f"<<endl;
				if(wavefront.empty()){
				cout<<"No path possible for net number " << i+1 <<endl;
				notpossible=true;
				break;
			    }
			    //Expansion
			    enum pred pred_temp; 
			    vector<enum pred> neigh;
			   wave_obj w= wavefront.top();
			   //neighbours
			   if(!w.lay_no){
			   	if(w.x!=0){
			   if(!grid1[w.x-1][w.y].is_reached&&!grid1[w.x-1][w.y].is_blocked())neigh.push_back(E);
				}
				if(w.x!=x_size-1){
				if(!grid1[w.x+1][w.y].is_reached&&!grid1[w.x+1][w.y].is_blocked())neigh.push_back(W);
				}
			   if(w.y!=0){
			   if(!grid1[w.x][w.y-1].is_reached&&!grid1[w.x][w.y-1].is_blocked())neigh.push_back(N);
				}
				if(w.y!=y_size-1){
			   if(!grid1[w.x][w.y+1].is_reached&&!grid1[w.x][w.y+1].is_blocked())neigh.push_back(S);
			   	}
					   if(!grid2[w.x][w.y].is_reached&&!grid2[w.x][w.y].is_blocked())neigh.push_back(down);
				}else{
					if(w.x!=0){
					if(!grid2[w.x-1][w.y].is_reached&&!grid2[w.x-1][w.y].is_blocked())neigh.push_back(E);
			   		}
			   		if(w.x!=x_size-1){
					if(!grid2[w.x+1][w.y].is_reached&&!grid2[w.x+1][w.y].is_blocked())neigh.push_back(W);
					}
			   if(w.y!=0){
			   if(!grid2[w.x][w.y-1].is_reached&&!grid2[w.x][w.y-1].is_blocked())neigh.push_back(N);
				}
				if(w.y!=y_size-1){
			   if(!grid2[w.x][w.y+1].is_reached&&!grid2[w.x][w.y+1].is_blocked())neigh.push_back(S);
				}
			   if(!grid1[w.x][w.y].is_reached&&!grid1[w.x][w.y].is_blocked())neigh.push_back(up);
				}
//				for(int neif=0;neif<neigh.size();neif++)
//				cout<<neigh[neif]<<endl;
//				cout<<neigh.size()<<endl;
				for(int n_i=0;n_i<neigh.size();n_i++){
					
					wave_obj w_temp;
					if(neigh[n_i]==N){
						w_temp.x=w.x;
						w_temp.y=w.y-1;
						w_temp.lay_no=w.lay_no;
						w_temp.pred_cell=N;
						w_temp.pathcost=0;
					}	
					if(neigh[n_i]==S){
						w_temp.x=w.x;
						w_temp.y=w.y+1;
						w_temp.lay_no=w.lay_no;
						w_temp.pred_cell=S;
						w_temp.pathcost=0;
					}
					if(neigh[n_i]==E){
						w_temp.x=w.x-1;
						w_temp.y=w.y;
						w_temp.lay_no=w.lay_no;
						w_temp.pred_cell=E;
						w_temp.pathcost=0;
					}
					if(neigh[n_i]==W){
						w_temp.x=w.x+1;
						w_temp.y=w.y;
						w_temp.lay_no=w.lay_no;
						w_temp.pred_cell=W;
						w_temp.pathcost=0;
					}
					if(neigh[n_i]==up){
						w_temp.x=w.x;
						w_temp.y=w.y;
						w_temp.lay_no=!w.lay_no;
						w_temp.pred_cell=up;
						w_temp.pathcost=viapenalty;
					}
					if(neigh[n_i]==down){
						w_temp.x=w.x;
						w_temp.y=w.y;
						w_temp.lay_no=!w.lay_no;
						w_temp.pred_cell=down;
						w_temp.pathcost=viapenalty;
					}
				//	int bias=bbox(w_temp.x,w_temp.y,w_tar.x,w_tar.y);
				//	bool givebias=false;	
					if(isbendpresent(w_temp,w,w_src))
					w_temp.pathcost+=(!w_temp.lay_no)?(w.pathcost+grid1[w_temp.x][w_temp.y].cost+bendpenalty):(w.pathcost+grid2[w_temp.x][w_temp.y].cost+bendpenalty);
					else w_temp.pathcost+=(!w_temp.lay_no)?(w.pathcost+grid1[w_temp.x][w_temp.y].cost):(w.pathcost+grid2[w_temp.x][w_temp.y].cost);
					wavefront.push(w_temp);
				//	if(givebias)w_temp.pathcost+=bias;
					if(!w_temp.lay_no) grid1[w_temp.x][w_temp.y].pred_cell=w_temp.pred_cell;
					else grid2[w_temp.x][w_temp.y].pred_cell=w_temp.pred_cell;
					
					if(!w_temp.lay_no) grid1[w_temp.x][w_temp.y].is_reached=true;
					else grid2[w_temp.x][w_temp.y].is_reached=true;
//					cout<<w_temp.x<<" "<<w_temp.y<<" "<<w_temp.lay_no<<" "<<w_temp.pred_cell<<endl;
					if(istarget_reached(w_tar,w_temp)){w_tar.pred_cell=w_temp.pred_cell;w_tar.pathcost=w_temp.pathcost;target_found=true;break;}
					
				}
//			if(mi>30) break;
//					mi++;
				wavefront.pop();
				
			}
//			cout<<mi<<endl;
//			cout<<grid1[119][31].cost<<endl;
			cout<< " "<<w_tar.pathcost<<endl;
			//backtrace
			if(!notpossible){
			wave_obj w_btr=w_tar;
			if(!w_btr.lay_no)grid1[w_btr.x][w_btr.y].cost=-1;
			else grid2[w_btr.x][w_btr.y].cost=-1;
			netout[i].push_back(w_btr);
			int li=0;
		
			while(!istarget_reached(w_src,w_btr)){
				
			//cout<<w_btr.x<<" "<<w_btr.y<<" Layno "<<w_btr.lay_no<<" "<<w_btr.pred_cell<<endl;	
			if(w_btr.pred_cell==N){
				w_btr.y=w_btr.y+1;
				if(!w_btr.lay_no){
				w_btr.pred_cell=grid1[w_btr.x][w_btr.y].pred_cell;
				grid1[w_btr.x][w_btr.y].cost=-1;
				netout[i].push_back(w_btr);
				}
				else {w_btr.pred_cell=grid2[w_btr.x][w_btr.y].pred_cell;grid2[w_btr.x][w_btr.y].cost=-1;netout[i].push_back(w_btr);}
			}
			else if(w_btr.pred_cell==S){
				w_btr.y=w_btr.y-1;	
				if(!w_btr.lay_no){
				w_btr.pred_cell=grid1[w_btr.x][w_btr.y].pred_cell;
				grid1[w_btr.x][w_btr.y].cost=-1;
				netout[i].push_back(w_btr);
				}
				else {w_btr.pred_cell=grid2[w_btr.x][w_btr.y].pred_cell;grid2[w_btr.x][w_btr.y].cost=-1;netout[i].push_back(w_btr);}
			}
			else if(w_btr.pred_cell==W){
				w_btr.x=w_btr.x-1;
				if(!w_btr.lay_no){
				w_btr.pred_cell=grid1[w_btr.x][w_btr.y].pred_cell;
				grid1[w_btr.x][w_btr.y].cost=-1;
				netout[i].push_back(w_btr);
				}
				else {w_btr.pred_cell=grid2[w_btr.x][w_btr.y].pred_cell;grid2[w_btr.x][w_btr.y].cost=-1;netout[i].push_back(w_btr);}
			}
			else if(w_btr.pred_cell==E){
				w_btr.x=w_btr.x+1;
			if(!w_btr.lay_no){
				w_btr.pred_cell=grid1[w_btr.x][w_btr.y].pred_cell;
				grid1[w_btr.x][w_btr.y].cost=-1;
				netout[i].push_back(w_btr);
				}
				else {w_btr.pred_cell=grid2[w_btr.x][w_btr.y].pred_cell;grid2[w_btr.x][w_btr.y].cost=-1;netout[i].push_back(w_btr);}
			}
		
			else if(w_btr.pred_cell==up){
				w_btr.lay_no=!w_btr.lay_no;
				if(!w_btr.lay_no){
				w_btr.pred_cell=grid1[w_btr.x][w_btr.y].pred_cell;
				grid1[w_btr.x][w_btr.y].cost=-1;
				netout[i].push_back(w_btr);
				}
				else {w_btr.pred_cell=grid2[w_btr.x][w_btr.y].pred_cell;grid2[w_btr.x][w_btr.y].cost=-1;netout[i].push_back(w_btr);}
				via[w_btr.x][w_btr.y]=true;
			}
			else if(w_btr.pred_cell==down){
				w_btr.lay_no=!w_btr.lay_no;
				if(!w_btr.lay_no){
				w_btr.pred_cell=grid1[w_btr.x][w_btr.y].pred_cell;
				grid1[w_btr.x][w_btr.y].cost=-1;
				netout[i].push_back(w_btr);
				}
				else {w_btr.pred_cell=grid2[w_btr.x][w_btr.y].pred_cell;grid2[w_btr.x][w_btr.y].cost=-1;netout[i].push_back(w_btr);}
				via[w_btr.x][w_btr.y]=true;
			}
	
			}
		}
			//cleanup
		
			for(int i_c=0;i_c<x_size;i_c++){
				for(int j_c=0;j_c<y_size;j_c++){
					grid1[i_c][j_c].is_reached=false;
					grid2[i_c][j_c].is_reached=false;
				}
			}
			
			cout<<"Route of net " <<i+1<<" completed"<<endl;		
		}
		ofstream fout(outputfile);
		
		fout<<nnz<<endl;
	
		for(int i=0;i<nnz;i++){
		fout<<i+1<<endl; 
		if(netout[i].size()!=2){
		for(int j=netout[i].size()-1;j>=0;j--){
		fout<<netout[i][j].lay_no+1<<" "<<netout[i][j].x<<" "<<netout[i][j].y<<endl;
		if(via[netout[i][j].x][netout[i][j].y]){
			fout<< "3 "<<netout[i][j].x<<" "<<netout[i][j].y<<endl;
			via[netout[i][j].x][netout[i][j].y]=false;
		}
		}
		}
		fout<<"0"<<endl;
		}
		
	}else cout<<"Unable to open file"<<endl;

//	priority_queue <int> k;
//	k.push(5);
//	k.push(17);
//	k.push(3);
//	showpq(k);
//	int i=k.top();
	
}
