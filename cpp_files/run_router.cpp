#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <math.h>
#include "router_functions.h"
using namespace std;

#define NETLISTFILE "../benchmarks/bench1.nl"
#define GRIDFILE "../benchmarks/bench1.grid"
#define OUTPUTFILE "./outputs/router_outputs/routeroutput.txt"

int main(){
	int x_size,y_size,bendpenalty,viapenalty;

// GRID INPUT (.grid) //
	ifstream fin(GRIDFILE);
	if(!fin.is_open()){
		cout<<"Unable to open file"<<endl;
		return -1;
	}
	fin>>x_size>>y_size;
	fin>>bendpenalty>>viapenalty;

	cell grid[2][x_size][y_size];  //  GRID Data Structure//
		
	bool via[x_size][y_size];      // Presence of VIA //
	for(int l_no=0;l_no<2;l_no++){
		for(int j=0;j<y_size;j++){
			for(int i=0;i<x_size;i++){
				fin>>grid[l_no][i][j].cost;
				via[i][j]=false;
			}
    	}
	}	
	fin.close();
//  NETLIST INPUT (.nl)  //

	int nnz,lno;
	ifstream fnin(NETLISTFILE);
	
	if(!fnin.is_open()){
		cout<<"Unable to open file"<<endl;
		return -1;
	}

	fnin>>nnz;
		
	net in_nets[nnz]; // To store info about source and target nets
	
	
	for(int i=0;i<nnz;i++){
		int i_lk;
		fnin>>i_lk;

		for(int i_l=0;i_l<2;i_l++){
			
			fnin>>lno;
			
			in_nets[i].lay_no[i_l]=(lno==1)?false:true;
			fnin>>in_nets[i].x[i_l]>>in_nets[i].y[i_l];

			grid[in_nets[i].lay_no[i_l]][in_nets[i].x[i_l]][in_nets[i].y[i_l]].is_routed=true; // So earlier nets don't route over source and targets of upcoming nets
		}
		
	}
	fnin.close();
		
	vector <wave_obj> netout[nnz];  // OUTPUT NET //  
	
//  GO through each 2 point Net and start a wavefront(min heap) with source as first element and push eligible neighbours into it

	for(int i=0;i<nnz;i++){
		
		cout<<i+1<<endl;
		wave_obj w_src,w_tar;
//  Declare source and target of net
		w_src.lay_no=in_nets[i].lay_no[0];
		w_src.x=in_nets[i].x[0];
		w_src.y=in_nets[i].y[0];
		w_src.pathcost=1;
	
		grid[w_src.lay_no][w_src.x][w_src.y].is_reached=true;
		if(grid[w_src.lay_no][w_src.x][w_src.y].cost<0) grid[w_src.lay_no][w_src.x][w_src.y].cost=1;  //Remove block added over the source

		w_tar.lay_no=in_nets[i].lay_no[1];
		w_tar.x=in_nets[i].x[1];
		w_tar.y=in_nets[i].y[1];
		if(grid[w_tar.lay_no][w_tar.x][w_tar.y].cost<0) grid[w_tar.lay_no][w_tar.x][w_tar.y].cost=1;//Remove block added over the target
		grid[w_tar.lay_no][w_tar.x][w_tar.y].is_routed=false;										//Remove block added over the target
		
		
		cout<<"Source "<<w_src.x<<" "<<w_src.y<<" "<< w_src.lay_no<<endl;
		cout<<"Target "<<w_tar.x<<" "<<w_tar.y<<" "<< w_tar.lay_no<<endl;
		
// EXPANSION using MIN HEAP
		priority_queue <wave_obj> wavefront;

		wavefront.push(w_src);

		bool target_found=false;
		bool notpossible=false;
		while(!target_found){
			if(wavefront.empty()){
				cout<<"No path possible for net number " << i+1 <<endl;
				notpossible=true;
				break;
		    }

		    enum pred pred_temp; 
			vector<enum pred> neigh;
		    wave_obj w= wavefront.top();
	// FIND ELIGIBLE NEIGHBOURS of the min heap top (w)
		    int x_ng[]={-1,1,0,0};
		    int y_ng[]={0,0,-1,1};
		    enum pred p_ng[]={E,W,N,S};
		    
	        for(int ip=0;ip<4;ip++){

				int x_tmp=w.x+x_ng[ip];
				int y_tmp=w.y+y_ng[ip];
				if(x_tmp<x_size&&x_tmp>=0&&y_tmp<y_size&&y_tmp>=0)
				    if(!grid[w.lay_no][x_tmp][y_tmp].is_routed&&!grid[w.lay_no][x_tmp][y_tmp].is_reached&&!grid[w.lay_no][x_tmp][y_tmp].is_blocked())
				   		neigh.push_back(p_ng[ip]);//Not alreaady routed by earlier net or reached by wavefront and not blocked by the input
			
			}
			if(!grid[!w.lay_no][w.x][w.y].is_routed&&!grid[!w.lay_no][w.x][w.y].is_reached&&!grid[!w.lay_no][w.x][w.y].is_blocked())
				if(w.lay_no)
					neigh.push_back(up);
				else
					neigh.push_back(down);
//				for(int neif=0;neif<neigh.size();neif++)
//				cout<<neigh[neif]<<endl;
//				cout<<neigh.size()<<endl;

	// ITERATE over the NEIGHBOURS
			for(int n_i=0;n_i<neigh.size();n_i++){
			// CALCULATE COST to go to each neighbour (w_temp)
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
						w_temp.pathcost+=viapenalty;  //ADD VIA PENALTY for up down pred
						break;
				}
//					cout<<w_temp.lay_no<<" "<<w_temp.x<<" "<<w_temp.y<<endl;

				w_temp.pathcost+=grid[w_temp.lay_no][w_temp.x][w_temp.y].cost;	//ADD THE GRID COST
				
				if(isbendpresent(w_temp,w,w_src))
					w_temp.pathcost+=bendpenalty; //ADD BEND PENALTY IF BEND EXISTS
					
				
				wavefront.push(w_temp);//PUSH THE NEIGHBOUR INTO MIN HEAP 

			//SAVE the info in the GRID itself
				grid[w_temp.lay_no][w_temp.x][w_temp.y].pred_cell=w_temp.pred_cell;
				grid[w_temp.lay_no][w_temp.x][w_temp.y].is_reached=true;

			//BREAK IF the NEIGHBOUR IS TARGET;
				if(istarget_reached(w_tar,w_temp)){
					w_tar.pred_cell=w_temp.pred_cell;
					w_tar.pathcost=w_temp.pathcost;
					target_found=true;
					break;
				}
				
			}

			wavefront.pop();
				
		}

		//// BACKTRACE  TO GET THE OUTPUT NET (netout) if a net was possible
		if(!notpossible){
			cout<< "PATHCOST ="<<w_tar.pathcost<<endl;
			wave_obj w_btr=w_tar;
			grid[w_btr.lay_no][w_btr.x][w_btr.y].is_routed=true;
			netout[i].push_back(w_btr);
		//REACHING the source using target with BACKTRACE
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
		//CLEANUP THE REACHED CELLS BY MIN HEAP
		for(int l_c=0;l_c<2;l_c++){
			for(int i_c=0;i_c<x_size;i_c++){
				for(int j_c=0;j_c<y_size;j_c++){
					grid[l_c][i_c][j_c].is_reached=false;
				}
			}
		}
		
		cout<<"Route of net " <<i+1<<" completed"<<endl;
		//EMPTY THE MIN HEAP
		while(!wavefront.empty())
			wavefront.pop();		
	}
	
// MAKE THE OUTPUT FILE with format (Netnumber\n with all the cells the net goes through[lay x y] \n 0)
// 3 is VIA
	ofstream fout(OUTPUTFILE);
	
	fout<<nnz<<endl;

	for(int i=0;i<nnz;i++){
		
		fout<<i+1<<endl; 
	
		for(int j=netout[i].size()-1;j>=0;j--){
			fout<<netout[i][j].lay_no+1<<" "<<netout[i][j].x<<" "<<netout[i][j].y<<endl;
			
			if(via[netout[i][j].x][netout[i][j].y]){
				fout<< "3 "<<netout[i][j].x<<" "<<netout[i][j].y<<endl;
			}
			
		}
		
		fout<<"0"<<endl;
	}
	fout.close();
	return 0;	
}
