#include <iostream>
#include <valarray>
#include <vector>
#include "solver.h"

#ifndef PLACER_FUNCTIONS
#define PLACER_FUNCTIONS

class Gate{
	public :
		double x,y;
		int gate_id;
		Gate(int id){
			gate_id=id;
		}
		vector<int> net_ids;
};

int is_connected(Gate* a,Gate *b){
	for(int i=0;i<a->net_ids.size();i++){
		for(int j=0;j<b->net_ids.size();j++)
			if(a->net_ids[i]==b->net_ids[j])
				return a->net_ids[i];
	}
	return -1;
}

bool sortx(const Gate* a,const Gate* b){
	return (100000*a->x+a->y)<(100000*b->x+b->y);
}

bool sorty(const Gate* a,const Gate* b){
	return (100000*a->y+a->x)<(100000*b->y+b->x);
}

bool sortid(const Gate* a,const Gate* b){
	return a->gate_id<b->gate_id;
}


void print_gates(vector<Gate*> gates){
	for(auto g:gates)
		cout<<g->gate_id <<" "<<g->x<<" "<<g->y<<endl;
}

void quad_placement(vector<Gate*>& gates,double weigh[],vector<vector<int> > pad_net,vector<double> pad_x,vector<double> pad_y ){
	int n=gates.size();
	if(n==1)return;
	vector<int> vr;
	vector <int> vc;
	vector <double> vw;
	double div_arr[n]={0};
	int pad_total=pad_net.size();

	for(int i=0;i<n-1;i++){
		for(int j=i+1;j<n;j++){
			int ny=is_connected(gates[i],gates[j]);
			if(ny!=-1){
				double div=weigh[ny];
				vr.push_back(i);
				vc.push_back(j);
				vw.push_back(-div);
				vr.push_back(j);
				vc.push_back(i);
				vw.push_back(-div);
				div_arr[i]+=div;
				div_arr[j]+=div;
			}
		}
	}

    valarray<double> b_x(n),b_y(n);
	
    for(int j=0;j<n;j++){
   			
		for(auto nt: gates[j]->net_ids){ 	
	    	for(int i=0;i<pad_total;i++){
	    		
	    		for(int o=0;o<pad_net[i].size();o++){
		    		double div=weigh[pad_net[i][o]];
		    		if(nt==pad_net[i][o]){
			    		b_x[j]+=pad_x[i]*div;
			    		b_y[j]+=pad_y[i]*div;
			    		div_arr[j]+=div;
	//			    		break;
			    	}
				}
			}
		}

	}
	for(int i=0;i<n;i++){
		vr.push_back(i);
		vc.push_back(i);
		vw.push_back(div_arr[i]);
	}

	int ro[vr.size()];
	int co[vc.size()];
	double dao[vw.size()];
	copy(vr.begin(),vr.end(),ro);
	copy(vc.begin(), vc.end(), co);
	copy(vw.begin(), vw.end(), dao);
	coo_matrix A;
	A.n= n;
	A.nnz= vr.size();
	A.row.resize(A.nnz);
	A.col.resize(A.nnz);
	A.dat.resize(A.nnz);
	
	A.row = valarray<int>(ro, A.nnz);
	A.col = valarray<int>(co, A.nnz);
	A.dat = valarray<double>(dao, A.nnz);



	valarray <double> x_q1(A.n), y_q1(A.n);
	A.solve(b_x,x_q1);
	A.solve(b_y,y_q1);
//	print_valarray(A.dat);
	for(int i=0;i<n;i++){
		if(x_q1[i]<0)x_q1[i]=0;
		if(y_q1[i]<0)y_q1[i]=0;
    	gates[i]->x=x_q1[i];
    	gates[i]->y=y_q1[i];
    }
}

void contain_x(vector<Gate*> gates,vector <vector <int> > pad_net, vector<double> pad_x,vector<double> pad_y,double x_min,double x_max){
	

	for(int i=0;i<gates.size();i++){
		pad_net.push_back(gates[i]->net_ids);
		pad_x.push_back(gates[i]->x);
		pad_y.push_back(gates[i]->y);
	}
	
	int n=pad_net.size();
	for(int i=0;i<n;i++){
		if(pad_x[i]>x_max)
			pad_x[i]=x_max;
		if(pad_x[i]<x_min)
			pad_x[i]=x_min;
	}
}

void contain_y(vector<Gate*> gates,vector <vector <int> > pad_net, vector<double> pad_x,vector<double> pad_y,double y_min,double y_max){
	

	for(int i=0;i<gates.size();i++){
		pad_net.push_back(gates[i]->net_ids);
		pad_x.push_back(gates[i]->x);
		pad_y.push_back(gates[i]->y);
	}
	
	int n=pad_net.size();
	for(int i=0;i<n;i++){
		if(pad_y[i]>y_max)
			pad_y[i]=y_max;
		if(pad_y[i]<y_min)
			pad_y[i]=y_min;
	}
}

void quad_placement_nxn(vector<Gate*> gates,double weigh[],vector<vector<int> > pad_net,vector<double> pad_x,vector<double> pad_y,double x_min,double x_max,double y_min,double y_max,int n){


//  Horizontal Assignment
	if((n&n-1)){
		cout<<"n is not a power of 2";
		return;
	}
		
	if(n==1||gates.size()<=1)
		return;
	int m=gates.size();
	sort(gates.begin(),gates.end(),sortx);

	vector<Gate *> l_gates(gates.begin(),gates.begin()+1+(m-1)/2);
	vector<Gate *> r_gates(gates.begin()+(m-1)/2+1,gates.end());

//  Containment
	vector<vector<int> > l_pad_net(pad_net),r_pad_net(pad_net);
	vector<double> l_pad_x(pad_x),r_pad_x(pad_x);
	vector<double> l_pad_y(pad_y),r_pad_y(pad_x);
	
	contain_x(r_gates,l_pad_net,l_pad_x,l_pad_y,x_min,x_max);
	contain_x(l_gates,r_pad_net,r_pad_x,r_pad_y,x_min,x_max);
//  2 QP

	quad_placement(l_gates,weigh,l_pad_net,l_pad_x,l_pad_y);
	print_gates(l_gates);
	cout<<endl;
//  3 QP

	quad_placement(r_gates,weigh,r_pad_net,r_pad_x,r_pad_y);
	print_gates(r_gates);
	cout<<endl;
	
// Vertical Assigment	1

//  Left  Assignment
	m=l_gates.size();
	sort(l_gates.begin(),l_gates.end(),sorty);

	vector<Gate *> ld_gates(l_gates.begin(),l_gates.begin()+1+(m-1)/2);
	vector<Gate *> lu_gates(l_gates.begin()+(m-1)/2+1,l_gates.end());

//  L Up Down Containment
	vector<vector<int> > ld_pad_net(l_pad_net),lu_pad_net(l_pad_net);
	vector<double> ld_pad_x(l_pad_x),lu_pad_x(l_pad_x);
	vector<double> ld_pad_y(l_pad_y),lu_pad_y(l_pad_x);
	
	contain_y(lu_gates,ld_pad_net,ld_pad_x,ld_pad_y,y_min,y_max);
	contain_y(ld_gates,lu_pad_net,lu_pad_x,lu_pad_y,y_min,y_max);
//  2 QP

	quad_placement(ld_gates,weigh,ld_pad_net,ld_pad_x,ld_pad_y);
	print_gates(ld_gates);
	cout<<endl;
//  3 QP

	quad_placement(lu_gates,weigh,lu_pad_net,lu_pad_x,lu_pad_y);
	print_gates(lu_gates);
	cout<<endl;
	
// Vertical Assigment	2

//  Right Assignment
	m=r_gates.size();
	sort(r_gates.begin(),r_gates.end(),sorty);

	vector<Gate *> rd_gates(r_gates.begin(),r_gates.begin()+1+(m-1)/2);
	vector<Gate *> ru_gates(r_gates.begin()+(m-1)/2+1,r_gates.end());

//  R Up Down Containment
	vector<vector<int> > rd_pad_net(r_pad_net),ru_pad_net(r_pad_net);
	vector<double> rd_pad_x(r_pad_x),ru_pad_x(r_pad_x);
	vector<double> rd_pad_y(r_pad_y),ru_pad_y(r_pad_x);
	
	contain_y(ru_gates,rd_pad_net,rd_pad_x,rd_pad_y,y_min,y_max);
	contain_y(rd_gates,ru_pad_net,ru_pad_x,ru_pad_y,y_min,y_max);
//  2 QP

	quad_placement(rd_gates,weigh,rd_pad_net,rd_pad_x,rd_pad_y);
	print_gates(rd_gates);
	cout<<endl;
//  3 QP

	quad_placement(ru_gates,weigh,ru_pad_net,ru_pad_x,ru_pad_y);
	print_gates(ru_gates);
	cout<<endl;		
	
	double x_mid=(x_min+x_max)/2;
	double y_mid=(y_min+y_max)/2;
	
	quad_placement_nxn(lu_gates,weigh,lu_pad_net,lu_pad_x,lu_pad_y,x_min,x_mid,y_mid,y_max,n/2);
	quad_placement_nxn(ld_gates,weigh,ld_pad_net,ld_pad_x,ld_pad_y,x_min,x_mid,y_min,y_mid,n/2);
	quad_placement_nxn(ru_gates,weigh,ru_pad_net,ru_pad_x,ru_pad_y,x_mid,x_max,y_mid,y_max,n/2);
	quad_placement_nxn(rd_gates,weigh,rd_pad_net,rd_pad_x,rd_pad_y,x_mid,x_max,y_min,y_mid,n/2);
		
}

#endif
