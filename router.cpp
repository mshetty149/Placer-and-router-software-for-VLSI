
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <math.h>
using namespace std;
#define netlistfile "../benchmarks/bench2.nl"
#define gridfile "../benchmarks/bench2.grid"
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
		bool is_reached=false;
		
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
		wave_obj operator=(const wave_obj &b){
			pred_cell=b.pred_cell;
			lay_no=b.lay_no;
			x=b.x;
			y=b.y;
			pathcost=b.pathcost;
		}
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
		cell grid[2][x_size][y_size];
		bool via[x_size][y_size];
		for(int l_no=0;l_no<2;l_no++){
			for(int j=0;j<y_size;j++){
				for(int i=0;i<x_size;i++){
					fin>>grid[l_no][i][j].cost;
					via[i][j]=false;
				}
	    	}
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
    			grid[lno][in_nets[i].x[i_l]][in_nets[i].y[i_l]].is_routed=true;
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
		
			grid[w_src.lay_no][w_src.x][w_src.y].is_reached=true;
//			if(grid[w_src.lay_no][w_src.x][w_src.y].cost<0) grid[w_src.lay_no][w_src.x][w_src.y].cost=1;
			
			w_tar.lay_no=in_nets[i].lay_no[1];
			w_tar.x=in_nets[i].x[1];
			w_tar.y=in_nets[i].y[1];

			grid[w_tar.lay_no][w_tar.x][w_tar.y].is_routed=false;
			cout<<"Source "<<w_src.x<<" "<<w_src.y<<" "<< w_src.lay_no<<endl;
			cout<<"Target "<<w_tar.x<<" "<<w_tar.y<<" "<< w_tar.lay_no<<endl;
			//expand
			priority_queue <wave_obj> wavefront;
			int mi=0;
			wavefront.push(w_src);

			bool target_found=false;
			bool notpossible=false;
			while(!target_found){
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
			    int x_ng[]={-1,1,0,0};
			    int y_ng[]={0,0,-1,1};
			    enum pred p_ng[]={E,W,N,S};
			    
		        for(int ip=0;ip<4;ip++){
//		        	cout<<grid[w.lay_no][w.x+x_ng[ip]][w.y+y_ng[ip]].is_routed<<endl;
					int x_tmp=w.x+x_ng[ip];
					int y_tmp=w.y+y_ng[ip];
					if(x_tmp<x_size&&x_tmp>=0&&y_tmp<y_size&&y_tmp>=0)
					    if(!grid[w.lay_no][x_tmp][y_tmp].is_routed&&!grid[w.lay_no][x_tmp][y_tmp].is_reached&&!grid[w.lay_no][x_tmp][y_tmp].is_blocked())
					   		neigh.push_back(p_ng[ip]);
				}
				if(!grid[!w.lay_no][w.x][w.y].is_routed&&!grid[!w.lay_no][w.x][w.y].is_reached&&!grid[!w.lay_no][w.x][w.y].is_blocked())
					if(w.lay_no)
						neigh.push_back(up);
					else
						neigh.push_back(down);
//				for(int neif=0;neif<neigh.size();neif++)
//				cout<<neigh[neif]<<endl;
//				cout<<neigh.size()<<endl;
				for(int n_i=0;n_i<neigh.size();n_i++){
					
					wave_obj w_temp;
					w_temp=w;
					w_temp.pred_cell=neigh[n_i];
					switch(neigh[n_i]){
						case N:
							w_temp.y--;
							break;
						case S:
							w_temp.y++;
							break;
						case E:
							w_temp.x--;
							break;
						case W:
							w_temp.x++;
							break;
						case up: case down:
							w_temp.lay_no=!w_temp.lay_no;
							w_temp.pathcost+=viapenalty;
							break;
					}
//					cout<<w_temp.lay_no<<" "<<w_temp.x<<" "<<w_temp.y<<endl;
				//	int bias=bbox(w_temp.x,w_temp.y,w_tar.x,w_tar.y);
				//	bool givebias=false;
					w_temp.pathcost+=grid[w_temp.lay_no][w_temp.x][w_temp.y].cost;	
					if(isbendpresent(w_temp,w,w_src))
						w_temp.pathcost+=bendpenalty;
					wavefront.push(w_temp);
				//	if(givebias)w_temp.pathcost+=bias;
				//Save the info in the grid itself
					grid[w_temp.lay_no][w_temp.x][w_temp.y].pred_cell=w_temp.pred_cell;
					grid[w_temp.lay_no][w_temp.x][w_temp.y].is_reached=true;
//					cout<<w_temp.x<<" "<<w_temp.y<<" "<<w_temp.lay_no<<" "<<w_temp.pred_cell<<endl;
					if(istarget_reached(w_tar,w_temp)){
						w_tar.pred_cell=w_temp.pred_cell;
						w_tar.pathcost=w_temp.pathcost;
						target_found=true;
						break;
					}
					
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
				grid[w_btr.lay_no][w_btr.x][w_btr.y].is_routed=true;
				netout[i].push_back(w_btr);
				int li=0;
			
				while(!istarget_reached(w_src,w_btr)){
						
					//cout<<w_btr.x<<" "<<w_btr.y<<" Layno "<<w_btr.lay_no<<" "<<w_btr.pred_cell<<endl;
					switch(w_btr.pred_cell){
						case N:
							w_btr.y++;
							break;
						case S:
							w_btr.y--;
							break;
						case E:
							w_btr.x++;
							break;
						case W:
							w_btr.x--;
							break;
						case up:case down:
							w_btr.lay_no=!w_btr.lay_no;
							via[w_btr.x][w_btr.y]=true;
							break;
					}	
					w_btr.pred_cell=grid[w_btr.lay_no][w_btr.x][w_btr.y].pred_cell;
					grid[w_btr.lay_no][w_btr.x][w_btr.y].is_routed=true;
					netout[i].push_back(w_btr);
				}
			}
			//cleanup
			for(int l_c=0;l_c<2;l_c++){
				for(int i_c=0;i_c<x_size;i_c++){
					for(int j_c=0;j_c<y_size;j_c++){
						grid[l_c][i_c][j_c].is_reached=false;
					}
				}
			}
			cout<<"Route of net " <<i+1<<" completed"<<endl;
			while(!wavefront.empty())
				wavefront.pop();		
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
		
	}
	else cout<<"Unable to open file"<<endl;
	
}
