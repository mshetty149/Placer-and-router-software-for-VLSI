#include <iostream>
#include <vector>
#include <queue>
#include <math.h>

#ifndef ROUTER_FUNCTIONS
#define ROUTER_FUNCTIONS
enum pred{
	N,S,W,E,up,down
};
//make as small as possible 
class cell{
	public:
		enum pred pred_cell;
		short cost;
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
			this->pred_cell=b.pred_cell;
			this->lay_no=b.lay_no;
			this->x=b.x;
			this->y=b.y;
			this->pathcost=b.pathcost;
      return *this;
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
#endif
