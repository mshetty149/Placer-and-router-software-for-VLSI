#include <iostream>
#include <fstream>
#include <vector>
#include "placer_functions.h"
using namespace std;

#define INFNAME "../benchmarks/3QP/toy1"
#define OFNAME "./outputs/placer_outputs/toy1"

int main(){
	ifstream fin(INFNAME);
	if(!fin.is_open()){
		cout<<"Unable to open File"<<endl;
		return -1;
	}
		
	int n,nnz,gate_no,net_no,n_id;
	int pad_id,pad_total;
	// Number of gates , Number of nets
	fin >> n >>nnz;
	vector < vector<Gate*> > nets(nnz+1);// list of nets
	vector <Gate*> gates;

	//Input format Gate number #nets Net numbers
	for (int j=0;j<n;j++){
		//Gate number, TOTAL nets -> List of nets
		fin >> gate_no >>net_no;
		Gate* tmp =new Gate(gate_no);
		gates.push_back(tmp);
		for (int i=0;i<net_no ;i++){
			fin >> n_id;//net number k
			nets[n_id].push_back(tmp);
			tmp->net_ids.push_back(n_id);
		}
	}

	fin>> pad_total ;
	bool pad_present[nnz+1]={0};
	vector<vector <int> > pad_net(pad_total,vector<int>(1));
	vector<double> pad_x(pad_total),pad_y(pad_total);
	for (int pad_i=0 ; pad_i<pad_total; pad_i++){
		fin >> pad_id;
		fin >> pad_net[pad_id-1][0] >> pad_x[pad_id-1] >> pad_y[pad_id-1];
		pad_present[pad_net[pad_id-1][0]]=true;
	}
	fin.close();
	double weigh[nnz+1]; 
	for(int i=1;i<=nnz;i++){
		weigh[i]=(pad_present[i])?1.0/nets[i].size():1.0/(nets[i].size()-1);
	}
//  1 Qudratic Placement

	quad_placement(gates,weigh,pad_net,pad_x,pad_y);
	print_gates(gates);
	cout<<endl;

	quad_placement_nxn(gates,weigh,pad_net,pad_x,pad_y,0,100,0,100,2);

//  Print output to a file
	sort(gates.begin(),gates.end(),sortid);
	ofstream fout(OFNAME);
    fout<< n <<endl;
    for(int i=0;i<n;i++){
    	fout<< gates[i]->gate_id <<" "<< gates[i]->x <<" "<<gates[i]->y <<endl ;
    }
	
	
	return 0;
}
