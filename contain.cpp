#include <iostream>
#include<valarray>
#include<vector>
#include "solver.h"
#define oq1name "./netq1.txt"
using namespace std;
class nets{
	public:
		vector<int>  nets_id;	
};
class pads{
	public:
		int pad_id;
		int gate_id;
		vector<int> nets_connected;
		vector<int> gates_connected;
		double pad_x,pad_y;
};
 class gate{
  	public:
  	int id;
  	double x;
  	double y;
  	vector <int> nets_connected;
  	vector <int> pads_connected;
  	double sort_no() 
	  {
	  return 100000*x+y;
      }
    void equals(gate g){
    	x=g.x;
    	y=g.y;
    	id=g.id;
	}
  };
int main(){
	ifstream fin(oq1name);
	int nnz,l,gates_no;
	int n=18;
	int nl=n/2;
	int nr=n-nl;
	int net_id;
	gate left_gates[nl],right_gates[nr];
	for(int i=0;i<nl;i++)
	cin>> left_gates[i].id;
	for(int j=0;j<nr;j++)
	cin>> right_gates[j].id;
	cin >>nnz ;
	for (int i=0;i<nnz;i++){
		cin>> net_id;
		cin >> gates_no;
		for (int j=0;j<gates_no;j++){
		cin >>l;
		for (int q=0;q<nr;q++){
	    
		if (l==right_gates[q].id)
			right_gates[q].nets_connected.push_back(net_id);
	    if (q<nl){
		    if (l==left_gates[q].id)
		    left_gates[q].nets_connected.push_back(net_id);
		}
	    }
	    }  
	}	for (int i=0;i<nl;i++){
	for(int j=0;j<left_gates[i].nets_connected.size();j++)
	cout <<" l= " <<left_gates[i].nets_connected[j]<<" ";
     cout<<endl;}
     for (int i=0;i<nr;i++){
	for(int j=0;j<right_gates[i].nets_connected.size();j++)
	cout <<" r= " << right_gates[i].nets_connected[j]<<" ";
     cout<<endl;}
	
	vector<pads> left_pads, right_pads;

	for(int i=0;i<nr;i++){
		for(int ui=0; ui<nl;ui++){
		pads pads_temp;
		for(int uj=0;uj<left_gates[ui].nets_connected.size();uj++)
		for(int j=0;j<right_gates[i].nets_connected.size();j++){
				if(right_gates[i].nets_connected[j]==left_gates[ui].nets_connected[uj]){
					int yu=0;
				  for(int padi=0;padi<left_pads.size();padi++){
				  	if (left_pads[padi].pad_id==right_gates[i].id){
				  		yu=1;
				  		int nu=0;
				  		for(int neti =0 ; neti<left_pads[padi].nets_connected.size();neti++){
				  			
				  		  if(left_pads[padi].nets_connected[neti]==right_gates[i].nets_connected[j]){
				  		  	nu=1;
				  		  	break;
							}
							}
						  if (nu==0)
						  left_pads[padi].nets_connected.push_back(right_gates[i].nets_connected[j]);
				  		
						left_pads[padi].gates_connected.push_back(left_gates[ui].id);
						break;
				    }
				}
				    if(yu==0){
				    pads_temp.pad_y=right_gates[i].y;
					pads_temp.pad_id= right_gates[i].id;
					pads_temp.pad_x=50;
					pads_temp.nets_connected.push_back(right_gates[i].nets_connected[j]);
					pads_temp.gates_connected.push_back(left_gates[ui].id);
					left_pads.push_back(pads_temp)	;
				    }
			    } 
				
		
			}		
		
     	}
   }
cout <<" NETS "<<endl;
for (int i=0;i<left_pads.size();i++){	
	for(int j=0;j<left_pads[i].nets_connected.size();j++)
	cout << left_pads[i].nets_connected[j]<<" ";
	cout<<endl;
    }
		cout<<"LEFT GATES"<< endl;
    for (int i=0;i<left_pads.size();i++){

	for(int h=0;h<left_pads[i].gates_connected.size();h++)
	cout << left_pads[i].gates_connected[h] <<" ";
   cout<<endl;
    }
     cout<<"PAD "<<endl;
    for (int i=0;i<left_pads.size();i++){
	cout << left_pads[i].pad_id;
     cout<<endl;}
     
    
	return 0;

}








vector<pads> contain_left(const gate* left_gates,const gate* right_gates,int  nr, int nl){
	vector<pads> left_pads;

	for(int i=0;i<nr;i++){
		for(int ui=0; ui<nl;ui++){
		pads pads_temp;
		for(int uj=0;uj<left_gates[ui].nets_connected.size();uj++)
		for(int j=0;j<right_gates[i].nets_connected.size();j++){
				if(right_gates[i].nets_connected[j]==left_gates[ui].nets_connected[uj]){
					int yu=0;
				  for(int padi=0;padi<left_pads.size();padi++){
				  	if (left_pads[padi].pad_id==right_gates[i].id){
				  		yu=1;
				  		int nu=0;
				  		for(int neti =0 ; neti<left_pads[padi].nets_connected.size();neti++){
				  			
				  		  if(left_pads[padi].nets_connected[neti]==right_gates[i].nets_connected[j]){
				  		  	nu=1;
				  		  	break;
							}
							}
						  if (nu==0)
						  left_pads[padi].nets_connected.push_back(right_gates[i].nets_connected[j]);
				  		
						left_pads[padi].gates_connected.push_back(left_gates[ui].id);
						break;
				    }
				}
				    if(yu==0){
				    pads_temp.pad_y=right_gates[i].y;
					pads_temp.pad_id= right_gates[i].id;
					pads_temp.pad_x=50;
					pads_temp.nets_connected.push_back(right_gates[i].nets_connected[j]);
					pads_temp.gates_connected.push_back(left_gates[ui].id);
					left_pads.push_back(pads_temp)	;
				    }
			    } 
				
		
			}		
		
     	}
   }
   return left_pads;
}

