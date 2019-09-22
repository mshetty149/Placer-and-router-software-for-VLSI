#include <iostream>
#include <valarray>
#include <vector>
#include "solver.h"
#define fname "../benchmarks/3QP/fract"
#define f2name "./ind2.1.txt"
#define f3name "./ind2.2.txt"
#define f4name "./leftupnetlist.txt"
#define f5name "./leftdownnetlist.txt"
#define f6name "./rightupnetlist.txt"
#define f7name "./rightdownnetlist.txt"
#define oq1name "./netiq1.txt"
#define oq2name "./netiq2.txt"
#define oq3name "./netiq3.txt"
#define oq4name  "./output_upleft_netlist.txt"
#define oq5name  "./output_downleft_netlist.txt"
#define oq6name  "./output_upright_netlist.txt"
#define oq7name  "./output_downright_netlist.txt"
#define oq8name   "./net_oplu.txt"
#define oq9name   "./net_opld.txt"
#define oq10name   "./net_opru.txt"
#define oq11name   "./net_oprd.txt"
#define ot1name   "./recursil.txt"
#define ot2name   "./recursol.txt"
#define ot3name   "./recursir.txt"
#define ot4name   "./recursor.txt"
#define opname "./finalimp_8x8ind1.txt"
#define loname "./lo.txt"
#define roname "./ro.txt"
#define lo1name "./lo1.txt"
#define ro1name "./ro1.txt"
#define lo2name "./lo2.txt"
#define ro2name "./ro2.txt"
#define lo3name "./lo3.txt"
#define ro3name "./ro3.txt"
using namespace std;

void print_vector(vector<int>& v) {
  for (size_t i = 0; i < v.size(); ++i) {
    cout << v[i] << " ";
  }
  cout <<"\n";
  }
void print_matrix(vector< vector<double> > & p ){
	for(int i=0 ; i< p.size();i++){
		for (int j=0;j<p[i].size() ;j++){
			cout<< p[i][j] <<" ";
		}
		cout<< "\n";
	}
}  
double vector_sum (const vector<double> & v){
	double sum=0;
	for (int i=0;i<v.size();i++){
		sum+=v[i];
	}
	return sum;
}
void qplacer(const char *fnam,const char *oq1nam){
  ifstream fin(fnam);
  if(fin.is_open()){

  int n,nnz,gate_no,net_no,k;
  int pad_id,pad_total;
  fin >> n >>nnz;
  vector < vector<int> > v(nnz);// list of nets
  vector< vector<double> > d(n,vector<double> (n,0)); //declaring matrix and initialising it to zero
  vector <int> r,c;
  vector <double> da;
    //Input format Gate number #nets Net numbers
  for (int j=0;j<n;j++){
  // Number of gates , Number of nets
  fin >> gate_no >>net_no;
  //Gate number, #nets, Net numbers
  for (int i=0;i<net_no ;i++){
  fin >> k;//net number k
   v[k-1].push_back(gate_no);
 }
  }
  fin>> pad_total ;
  vector < vector<int> > pad_gate(pad_total);
  int pad_net[pad_total];
  double pad_x[pad_total],pad_y[pad_total];
  for (int pad_i=0 ; pad_i<pad_total; pad_i++){
  	fin >> pad_id;
  	fin >> pad_net[pad_id-1] >> pad_x[pad_id-1] >> pad_y[pad_id-1];
    pad_gate[pad_id-1]=v[pad_net[pad_id-1]-1];
  }
  fin.close();
  // Creating d matrix for A
   vector < vector<int> > net_padsgates(nnz);// list of nets
   net_padsgates=v;
   	for(int  pi=0;pi<pad_total;pi++){
   	 net_padsgates[pad_net[pi]-1].push_back(n+pi+1);
     }
  for (int i=0;i<nnz;i++){
//  	cout<< i+1 <<" ";//net id
//	print_vector(net_padsgates[i]);// gates connected to net
 for (size_t j = 0; j < v[i].size(); ++j) {
 	for (size_t p = 0; p < v[i].size(); ++p) {
 		double div=net_padsgates[i].size()-1.0;
 	if(j!=p)
 	d[v[i][j]-1][v[i][p]-1]-=1.0/div;
 }
  }
  }	
  
  for (int i=0;i<n;i++){
  	
    d[i][i]-=vector_sum(d[i]);
  
  }
  for(int j=0;j<pad_total;j++){
  		for(int m=0;m<pad_gate[j].size();m++){
  			double divp=net_padsgates[pad_net[j]-1].size()-1.0;
  	d[pad_gate[j][m]-1][pad_gate[j][m]-1]+=1.0/divp;
	}
}

//  print_matrix(d);  //To print matrix A
// convert matrix in 2d vector into coo_matrix form
for (int i =0 ;i<n;i++){
	for (int j=0; j< n ;j++){
	if(d[i][j] !=0){
	r.push_back(i);
	c.push_back(j);
	da.push_back(d[i][j]);	
	}
	}
}
// convert vector into array
int ro[r.size()];
int co[r.size()];
double dao[r.size()]; 
copy(r.begin(), r.end(), ro);
copy(c.begin(), c.end(), co);
copy(da.begin(), da.end(), dao);
//Create matrix A
	coo_matrix A;
	A.n= n;
	A.nnz= r.size();
	
   A.row.resize(A.nnz);
  A.col.resize(A.nnz);
  A.dat.resize(A.nnz);
  
  A.row = valarray<int>(ro, A.nnz);
  A.col = valarray<int>(co, A.nnz);
  A.dat = valarray<double>(dao, A.nnz);


    valarray<double> b_x(A.n),b_y(A.n);
    for(int i=0;i<pad_total;i++){
    	for (int j=0;j<pad_gate[i].size();j++){
    		double divb=1.0/(net_padsgates[pad_net[i]-1].size()-1);
		b_x[pad_gate[i][j]-1]+=pad_x[i]*divb;
    	b_y[pad_gate[i][j]-1]+=pad_y[i]*divb;
        }
			}
	valarray <double> x_q1(A.n), y_q1(A.n);
	A.solve(b_x,x_q1);
	A.solve(b_y,y_q1);
    ofstream fout(oq1nam);
    fout<< n <<endl;
    for(int i=0;i<n;i++)
    fout<< i+1 <<" "<< x_q1[i] <<" "<<y_q1[i] <<endl ;
    fout << nnz <<endl ;
	for(int i=0;i<nnz;i++){
		fout << i+1 <<" " <<v[i].size()<<" ";
		for(int j=0;j<v[i].size();j++)
		fout << v[i][j] <<" ";
		fout<<endl;
	}
	fout<<pad_total<<endl;
	for(int i=0;i <pad_total;i++){
		fout<< i+1 <<" " << pad_net[i]<<" "<< pad_x[i]<<" "<<pad_y[i]<<endl;
	}
	fout.close();
	} else cout<< "q1p:Unable to open input file"; 
		
}
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
  	double sort_nox() 
	  {
	  return 100000*x+y;
      }
    double sort_noy() 
	  {
	  return 100000*y+x;
      }
    void equals(gate g){
    	x=g.x;
    	y=g.y;
    	id=g.id;
    	nets_connected=g.nets_connected;
    	pads_connected=g.pads_connected;
	}
  };
  void swap_gates(gate& g1, gate& g2){
  	gate g;
  	g.equals(g2);
  	g2.equals(g1);
  	g1.equals(g);
  }
  void sort_gatesx(gate* g,int n,gate* left_gates,int nl, gate* right_gates, int nr ){
  	for (int i=0;i<n-1;i++){
  		int k=i;
  		for(int j=i+1;j<n;j++){
  			if(g[k].sort_nox()>g[j].sort_nox()){
  				swap_gates(g[k],g[j]);
  				k=j;
  				i=-1;
			  }else break;
		  }
	  }
	  
	for(int i=0; i<n ;i++){
		if (i<nl)
		left_gates[i].equals(g[i]);
		else
		right_gates[i-nl].equals(g[i]);
	}
  }
  void sort_gatesy(gate* g,int n,gate* up_gates,int nu, gate* down_gates, int nd){
  
	  for (int i=0;i<n-1;i++){
  		int k=i;
  		
  		for(int j=i+1;j<n;j++){
  			if(g[k].sort_noy()>g[j].sort_noy()){
  				swap_gates(g[k],g[j]);
  				k=j;
  				i=-1;
  				
			  }else break;
		  }
	  }
	cout<<g[0].id<<" "<<n<<endl;
	for(int i=0; i<n ;i++){
		if (i<nu)
		up_gates[i].equals(g[i]);
		else
		down_gates[i-nu].equals(g[i]);
	}
	
  }
  vector<pads> contain_leftright(const gate* left_gates,const gate* right_gates,int  nr, int nl, double horiz_cutpos){
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
					pads_temp.pad_x=horiz_cutpos;
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
 vector<pads> contain_updown(const gate* up_gates,const gate* down_gates,int  nd, int nu,double vert_cutpos){
		vector<pads> up_pads;

	for(int i=0;i<nd;i++){
		for(int ui=0; ui<nu;ui++){
		pads pads_temp;
		for(int uj=0;uj<up_gates[ui].nets_connected.size();uj++)
		for(int j=0;j<down_gates[i].nets_connected.size();j++){
				if(down_gates[i].nets_connected[j]==up_gates[ui].nets_connected[uj]){
					int yu=0;
				  for(int padi=0;padi<up_pads.size();padi++){
				  	if (up_pads[padi].pad_id==down_gates[i].id){
				  		yu=1;
				  		int nu=0;
				  		for(int neti =0 ; neti<up_pads[padi].nets_connected.size();neti++){
				  			
				  		  if(up_pads[padi].nets_connected[neti]==down_gates[i].nets_connected[j]){
				  		  	nu=1;
				  		  	break;
							}
							}
						  if (nu==0)
						  up_pads[padi].nets_connected.push_back(down_gates[i].nets_connected[j]);
				  		
						up_pads[padi].gates_connected.push_back(down_gates[ui].id);
						break;
				    }
				}
				    if(yu==0){
				    pads_temp.pad_y=vert_cutpos;
					pads_temp.pad_id= down_gates[i].id;
					pads_temp.pad_x=down_gates[i].x;
					pads_temp.nets_connected.push_back(down_gates[i].nets_connected[j]);
					pads_temp.gates_connected.push_back(up_gates[ui].id);
					up_pads.push_back(pads_temp)	;
				    }
			    } 
				
		
			}		
		
     	}
   }
   return up_pads;
}
void q2placer(const char* f2nam ,const char* oq2nam){
  ifstream fin(f2nam);
  if(fin.is_open()){
  
  int n,nnz,gate_no,net_no,k;
  int pad_id,pad_total;
  fin >> n >>nnz;
  vector < vector<int> > v(nnz);// list of nets
  vector< vector<double> > d(n,vector<double> (n,0)); //declaring matrix and initialising it to zero
  vector <int> r,c;
  vector <double> da;
    //Input format Gate number #nets Net numbers
  for (int j=0;j<n;j++){
  // Number of gates , Number of nets
  fin >> gate_no >>net_no;
  //Gate number, #nets, Net numbers
  for (int i=0;i<net_no ;i++){
  fin >> k;//net number k
   v[k-1].push_back(gate_no);
 }
  }
  fin>> pad_total ;
  vector < vector<int> > pad_gate(pad_total);
  double pad_x[pad_total],pad_y[pad_total];
  int nets_pad,ri;
  vector< vector<int> > pad_nets(pad_total);
  for (int pad_i=0 ; pad_i<pad_total; pad_i++){
  	fin >> pad_id;
  	fin>>nets_pad;
  	for(int i=0;i<nets_pad;i++){
	  fin >> ri;
	  pad_nets[pad_id-1].push_back(ri);
    }
	fin >> pad_x[pad_id-1] >> pad_y[pad_id-1];
	pad_gate[pad_id-1]=v[pad_nets[pad_id-1][0]-1];
	for(int i=1;i<pad_nets[pad_id-1].size();i++){
    pad_gate[pad_id-1].insert(pad_gate[pad_id-1].end(),v[pad_nets[pad_id-1][i]-1].begin(),v[pad_nets[pad_id-1][i]-1].end());
    }
  }
    fin.close();
  // Creating d matrix for A
   vector < vector<int> > net_padsgates(nnz);// list of nets with pads and gates
   net_padsgates=v;
   	for(int  pi=0;pi<pad_total;pi++){
   		for(int pnni=0;pnni<pad_nets[pi].size();pnni++)
   	 net_padsgates[pad_nets[pi][pnni]-1].push_back(n+pi+1);
     }
  for (int i=0;i<nnz;i++){
//  	cout<< i+1 <<" ";//net id
//	print_vector(net_padsgates[i]);// gates connected to net
 for (size_t j = 0; j < v[i].size(); ++j) {
 	for (size_t p = 0; p < v[i].size(); ++p) {
 		double div=net_padsgates[i].size()-1.0;
 	if(j!=p)
 	d[v[i][j]-1][v[i][p]-1]-=1.0/div;
 }
  }
  }	
  
  for (int i=0;i<n;i++){
  	
    d[i][i]-=vector_sum(d[i]);
  
  }
  for(int j=0;j<pad_total;j++){ 		
  			for(int px=0;px<pad_nets[j].size();px++){
  				for(int m=0;m<v[pad_nets[j][px]-1].size();m++){
  			double divl =1.0/(net_padsgates[pad_nets[j][px]-1].size()-1);
			d[v[pad_nets[j][px]-1][m]-1][v[pad_nets[j][px]-1][m]-1]+=divl;
            }
	   }
}

  //print_matrix(d);
// convert matrix in 2d vector into coo_matrix form
for (int i =0 ;i<n;i++){
	for (int j=0; j< n ;j++){
	if(d[i][j] !=0){
	r.push_back(i);
	c.push_back(j);
	da.push_back(d[i][j]);	
	}
	}
}
// convert vector into array
int ro[r.size()];
int co[r.size()];
double dao[r.size()] ;
copy(r.begin(), r.end(), ro);
copy(c.begin(), c.end(), co);
copy(da.begin(), da.end(), dao);
//Create matrix A
	coo_matrix A;
	A.n= n;
	A.nnz= r.size();
	
   A.row.resize(A.nnz);
  A.col.resize(A.nnz);
  A.dat.resize(A.nnz);
  
  A.row = valarray<int>(ro, A.nnz);
  A.col = valarray<int>(co, A.nnz);
  A.dat = valarray<double>(dao, A.nnz);

    valarray<double> b_x(A.n),b_y(A.n);
    for(int i=0;i<pad_total;i++){
    	for (int j=0;j<pad_nets[i].size();j++){
    		for(int kl=0;kl<v[pad_nets[i][j]-1].size();kl++){
    		double divb=1.0/(net_padsgates[pad_nets[i][j]-1].size()-1);	
		b_x[v[pad_nets[i][j]-1][kl]-1]+=pad_x[i]*divb;
    	b_y[v[pad_nets[i][j]-1][kl]-1]+=pad_y[i]*divb;
        }
        }
			}
	//print_valarray(b_x);
	valarray <double> x_q1(A.n), y_q1(A.n);
	A.solve(b_x,x_q1);
	A.solve(b_y,y_q1);
    ofstream fout(oq2nam);
    for(int i=0;i<A.n;i++)
    fout<< i+1 <<" "<< x_q1[i] <<" "<<y_q1[i] <<endl ;
	
	
	fout.close();
	} else cout<< "q2p:Unable to open input file"; 
		
}
  void containment_updown(vector<vector<int> > ve,gate* left_ogates,int nlro,vector<pads>& left_pads,gate* left_gates,int nl,double vert_pos,int nnz,const char* fnam, const char* onam, const char* f1nam, const char * o1nam, const char* oqo1nam,const char*oqo2nam){
int nlu=nl/2;
 int nld=nl-nlu;
 
 gate left_upgates[nlu],left_downgates[nld];
  sort_gatesy(left_gates,nl,left_upgates,nlu,left_downgates,nld);
  
  vector<pads> left_uppads= contain_updown(left_upgates,left_downgates,nld,nlu,vert_pos);
  
  		// Pad distribution of left pads(up and down)
	int initial_lup=left_uppads.size();
	vector<pads> left_downpads;
	int luppad_total=left_pads.size() ;
	
	for(int i=0;i<luppad_total;i++){
	 pads pady;
	 pady.pad_id= left_pads[i].pad_id;
	 pady.pad_x = left_pads[i].pad_x;
	 pady.pad_y=left_pads[i].pad_y;
	
	pady.nets_connected=left_pads[i].nets_connected;
	pady.gates_connected=left_pads[i].gates_connected;
	if (pady.pad_y >=vert_pos)left_uppads.push_back(pady);
	else left_downpads.push_back(pady);
     }
    // Leftside down(r) pad propagation to up(l) pad
      int initial_ldp=left_downpads.size();
     int final_lup=left_uppads.size();
     for(int i=0;i<left_downpads.size();i++){
     	for(int ri=0; ri<left_downpads[i].nets_connected.size();ri++)
		 for(int j=0;j<nlu;j++){
     		for(int h=0;h<left_upgates[j].nets_connected.size();h++){
     			if(left_upgates[j].nets_connected[h]==left_downpads[i].nets_connected[ri]){
     			 pads pad_tu;
     			 	 int ltu=0;
				 for(int ci=final_lup;ci<left_uppads.size();ci++){
				 	if (left_uppads[ci].pad_id==left_downpads[i].pad_id){
				 		ltu=1;
						left_uppads[ci].gates_connected.push_back(left_upgates[j].id);
						break;
					 }
				 	
				 }
				 if(ltu==0){
				 pad_tu.nets_connected.push_back(left_downpads[i].nets_connected[ri]);
				 pad_tu.pad_id= left_downpads[i].pad_id;
				 pad_tu.gates_connected=ve[left_downpads[i].nets_connected[ri]-1];
				 pad_tu.pad_x= left_downpads[i].pad_x;
				 pad_tu.pad_y=vert_pos;
				 left_uppads.push_back(pad_tu);
				 }
			     }
			 }
		 }
	 }
  

 //Left upside netlist
ofstream f4out(fnam);
f4out<<nlu<<" "<<nnz <<endl;
for(int u=0; u<nlu;u++){
f4out<< u+1 <<" "<<left_upgates[u].nets_connected.size()<<" ";
for(int f=0;f<left_upgates[u].nets_connected.size();f++)
f4out<< left_upgates[u].nets_connected[f]<<" ";
f4out <<endl;
}
f4out<<left_uppads.size()<<endl;
for(int pi=0; pi<left_uppads.size();pi++){
f4out<<pi+1 <<" ";
f4out<<left_uppads[pi].nets_connected.size()<<" ";
for(int pni=0;pni<left_uppads[pi].nets_connected.size();pni++)
f4out<<left_uppads[pi].nets_connected[pni]<<" ";
f4out<<left_uppads[pi].pad_x<<" "<<left_uppads[pi].pad_y;
f4out <<endl;
}
f4out.close();
  
//Placement of left upside
q2placer(fnam,onam);
  
   
  //Containment of left downside 
// up pads propagation as down pads(pad propagation)
     for(int i=initial_lup;i<final_lup;i++){
     	for(int ri=0; ri<left_uppads[i].nets_connected.size();ri++){
		 for(int j=0;j<nld;j++){
     		for(int h=0;h<left_downgates[j].nets_connected.size();h++){
     			if(left_downgates[j].nets_connected[h]==left_uppads[i].nets_connected[ri]){
				 pads pad_rd;
				 int rtd=0;
				 for(int ci=initial_ldp;ci<left_downpads.size();ci++){
				 	if (left_downpads[ci].pad_id==left_uppads[i].pad_id){
				 		rtd=1;
						 left_downpads[ci].gates_connected.push_back(left_downgates[j].id);
						 break;
					 }
				 	
				 }
				 if(rtd==0){
				 pad_rd.nets_connected.push_back(left_uppads[i].nets_connected[ri]);
			     pad_rd.pad_id= left_uppads[i].pad_id;
				 pad_rd.gates_connected.push_back(left_downgates[j].id);
				 pad_rd.pad_x= left_uppads[i].pad_x;
				 pad_rd.pad_y=vert_pos;
				 left_downpads.push_back(pad_rd);
			     }
				 }
			 }
		 }
		 }
	 }

 //READ NEW left up positions
 ifstream f4in(onam);
 int nix;
 for(int i=0; i<nlu;i++){
  f4in>>nix;
  f4in>>left_upgates[i].x ;
  f4in>>left_upgates[i].y;
  if (left_gates[i].id==left_upgates[i].id){
  left_gates[i].x=left_upgates[i].x ;
  left_gates[i].y=left_upgates[i].y;	
 }
}
 f4in.close();
vector<pads> left_downpad_temp= contain_updown(left_downgates,left_upgates,nlu,nld,vert_pos);
 left_downpads.insert(left_downpads.end(),left_downpad_temp.begin(),left_downpad_temp.end());
  
   //Left downside netlist
  
  ofstream f5out(f1nam);
f5out<<nld<<" "<<nnz <<endl;
for(int u=0; u<nld;u++){
f5out<< u+1 <<" "<<left_downgates[u].nets_connected.size()<<" ";
for(int f=0;f<left_downgates[u].nets_connected.size();f++)
f5out<< left_downgates[u].nets_connected[f]<<" ";
f5out <<endl;
}
f5out<<left_downpads.size()<<endl;
for(int pi=0; pi<left_downpads.size();pi++){
f5out<<pi+1 <<" ";
f5out<<left_downpads[pi].nets_connected.size()<<" ";
for(int pni=0;pni<left_downpads[pi].nets_connected.size();pni++)
f5out<<left_downpads[pi].nets_connected[pni]<<" ";
f5out<<left_downpads[pi].pad_x<<" "<<left_downpads[pi].pad_y;
f5out <<endl;
}
f5out.close();
q2placer(f1nam,o1nam);
  
  //READ NEW left down positions
 ifstream f5in(o1nam);
 for(int i=0; i<nld;i++){
  f5in>>nix;
  f5in>>left_downgates[i].x ;
  f5in>>left_downgates[i].y;
  if (left_gates[i+nlu].id==left_downgates[i].id){
  left_gates[i+nlu].x=left_downgates[i].x ;
  left_gates[i+nlu].y=left_downgates[i].y;	
 }
} cout<<"YOOOO BROS";
    ofstream fout(oqo1nam);
    fout<< nlu <<endl;
    for(int i=0;i<nlu;i++)
    fout<< left_upgates[i].id <<" "<< left_upgates[i].x <<" "<<left_upgates[i].y <<endl ;
    fout << nnz <<endl ;
	for(int i=0;i<nnz;i++){
		fout << i+1 <<" " <<ve[i].size()<<" ";
		for(int j=0;j<ve[i].size();j++)
		fout << ve[i][j] <<" ";
		fout<<endl;
	}
fout<<left_uppads.size()<<endl;
for(int pi=0; pi<left_uppads.size();pi++){
fout<<pi+1 <<" ";
fout<<left_uppads[pi].nets_connected.size()<<" ";
for(int pni=0;pni<left_uppads[pi].nets_connected.size();pni++)
fout<<left_uppads[pi].nets_connected[pni]<<" ";
fout<<left_uppads[pi].pad_x<<" "<<left_uppads[pi].pad_y;
fout <<endl;
}
	fout.close();
	   ofstream f2out(oqo2nam);
    f2out<< nlu <<endl;
    for(int i=0;i<nlu;i++)
    f2out<< left_downgates[i].id <<" "<< left_downgates[i].x <<" "<<left_downgates[i].y <<endl ;
    f2out << nnz <<endl ;
	for(int i=0;i<nnz;i++){
		f2out << i+1 <<" " <<ve[i].size()<<" ";
		for(int j=0;j<ve[i].size();j++)
		f2out << ve[i][j] <<" ";
		f2out<<endl;
	}
f2out<<left_downpads.size()<<endl;
for(int pi=0; pi<left_downpads.size();pi++){
f2out<<pi+1 <<" ";
f2out<<left_downpads[pi].nets_connected.size()<<" ";
for(int pni=0;pni<left_downpads[pi].nets_connected.size();pni++)
f2out<<left_downpads[pi].nets_connected[pni]<<" ";
f2out<<left_downpads[pi].pad_x<<" "<<left_downpads[pi].pad_y;
f2out <<endl;
}
	f2out.close();
	 for(int i=0 ; i< nlro;i++){
 	for(int j=0;j<nld;j++){
 		if(j<nlu){
		 if(left_upgates[j].id==left_ogates[i].id){
 			left_ogates[i].x=left_upgates[j].x;
 			left_ogates[i].y=left_upgates[j].y;
		 }
	     }
		if(left_downgates[j].id==left_ogates[i].id){
 			left_ogates[i].x=left_downgates[j].x;
 			left_ogates[i].y=left_downgates[j].y;
		 }
	 }
 }
  
}
void containment_leftright(gate* left_ogates,int nll,vector<pads>& left_recpads,vector<pads>& right_recpads,const char* oqo1nam,const char* qpilnam,const char* qpolnam,const char* qpirnam,const char* qpornam,const char* lonam,const char* ronam,double horiz_pos){
		
		int n,nnz;
	int gates_no,net_id,l;
	ifstream fin(oqo1nam);// output of qplacer1
	fin>>n;
	gate gatess[n];
	if(fin.is_open()){
		for(int i=0;i<n;i++){
		fin >> gatess[i].id;
		fin >>gatess[i].x;
		fin >> gatess[i].y;
		}
		//Assignment of gates to left and right
		
	int nl=n/2;
	int nr=n-nl;
	
	gate left_gates[nl],right_gates[nr];
		// one vertical cut
    sort_gatesx(gatess,n,left_gates,nl,right_gates,nr);
		
	

//	cout << "Left gates"<<endl;
//		for(int i=0;i<nl;i++)
//	cout  << left_gates[i].x <<endl;
//	cout << "Right gates"<<endl;
//		for(int i=0;i<nr;i++)
//	cout  << right_gates[i].x <<endl;
//		cout << "Left gates ID"<<endl;
//		for(int i=0;i<nl;i++)
//	cout  << left_gates[i].id <<endl;
//	cout << "Right gates ID"<<endl;
//		for(int i=0;i<nr;i++)
//	cout  << right_gates[i].id <<endl;	
//    
		fin>>nnz;
		vector<vector<int> > ve (nnz);
		// Distribute nets to left and right according to gates.
	for (int i=0;i<nnz;i++){
		fin>> net_id;
		fin >> gates_no;
		for (int j=0;j<gates_no;j++){
		fin >>l;
		ve[i].push_back(l);
		for (int q=0;q<nr;q++){
	    
		if (l==right_gates[q].id)
			right_gates[q].nets_connected.push_back(net_id);
	    if (q<nl){
		    if (l==left_gates[q].id)
		    left_gates[q].nets_connected.push_back(net_id);
		}
	    }
	    }  
	}

	vector<pads> left_pads=contain_leftright(left_gates,right_gates,nr,nl,horiz_pos);//right gates as left pads
		// Pad distribution of real pads
	int initial_lp=left_pads.size();
	vector<pads> right_pads;
	int pad_total;
	fin>> pad_total;
	
	for(int i=0;i<pad_total;i++){
		pads padx;
		int po,pol;
	fin>> padx.pad_id >> po;
	for(int i=0; i< po ;i++){
		fin>> pol;
		padx.nets_connected.push_back(po);
	}
	fin>>padx.pad_x>>padx.pad_y;
	padx.gates_connected=ve[po-1];
	if (padx.pad_x <=horiz_pos)left_pads.push_back(padx);
	else right_pads.push_back(padx);
     }
     // right pad propagation to left
     int initial_rp=right_pads.size();
     int final_lp=left_pads.size();
     for(int i=0;i<right_pads.size();i++){
     	for(int ri=0; ri<right_pads[i].nets_connected.size();ri++)
		 for(int j=0;j<nl;j++){
     		for(int h=0;h<left_gates[j].nets_connected.size();h++){
     			if(left_gates[j].nets_connected[h]==right_pads[i].nets_connected[ri]){
     			 pads pad_t;
     			 	 int lt=0;
				 for(int ci=final_lp;ci<left_pads.size();ci++){
				 	if (left_pads[ci].pad_id==right_pads[i].pad_id){
				 		lt=1;
						left_pads[ci].gates_connected.push_back(left_gates[j].id);
						break;
					 }
				 	
				 }
				 if(lt==0){
				 pad_t.nets_connected.push_back(right_pads[i].nets_connected[ri]);
				 pad_t.pad_id= right_pads[i].pad_id;
				 pad_t.gates_connected=ve[right_pads[i].nets_connected[ri]-1];
				 pad_t.pad_x= horiz_pos;
				 pad_t.pad_y=right_pads[i].pad_y;
				 left_pads.push_back(pad_t);
				 }
			     }
			 }
		 }
	 }
//	cout <<" NETS "<<endl;
//for (int i=0;i<left_pads.size();i++){	
//	for(int j=0;j<left_pads[i].nets_connected.size();j++)
//	cout << left_pads[i].nets_connected[j]<<" ";
//	cout<<endl;
//    }
//		cout<<"LEFT GATES"<< endl;
//    for (int i=0;i<left_pads.size();i++){
//
//	for(int h=0;h<left_pads[i].gates_connected.size();h++)
//	cout << left_pads[i].gates_connected[h] <<" ";
//   cout<<endl;
//    }
//     cout<<"PAD "<<endl;
//    for (int i=0;i<left_pads.size();i++){
//	cout << left_pads[i].pad_id;
//     cout<<endl;
//	 }


//Leftside netlist
ofstream f2out(qpilnam);
f2out<<nl<<" "<<nnz <<endl;
for(int u=0; u<nl;u++){
f2out<< u+1 <<" "<<left_gates[u].nets_connected.size()<<" ";
for(int f=0;f<left_gates[u].nets_connected.size();f++)
f2out<< left_gates[u].nets_connected[f]<<" ";
f2out <<endl;
}
f2out<<left_pads.size()<<endl;
for(int pi=0; pi<left_pads.size();pi++){
f2out<<pi+1 <<" ";
f2out<<left_pads[pi].nets_connected.size()<<" ";
for(int pni=0;pni<left_pads[pi].nets_connected.size();pni++)
f2out<<left_pads[pi].nets_connected[pni]<<" ";
f2out<<left_pads[pi].pad_x<<" "<<left_pads[pi].pad_y;
f2out <<endl;
}
f2out.close();
//Placement of left side
q2placer(qpilnam,qpolnam);


//Containment of right side 
// left pads propagation as right pads(pad propagation)
     for(int i=initial_lp;i<final_lp;i++){
     	for(int ri=0; ri<left_pads[i].nets_connected.size();ri++){
		 for(int j=0;j<nr;j++){
     		for(int h=0;h<right_gates[j].nets_connected.size();h++){
     			if(right_gates[j].nets_connected[h]==left_pads[i].nets_connected[ri]){
				 pads pad_r;
				 int rt=0;
				 for(int ci=initial_rp;ci<right_pads.size();ci++){
				 	if (right_pads[ci].pad_id==left_pads[i].pad_id){
				 		rt=1;
						 right_pads[ci].gates_connected.push_back(right_gates[j].id);
						 break;
					 }
				 	
				 }
				 if(rt==0){
				 pad_r.nets_connected.push_back(left_pads[i].nets_connected[ri]);
			     pad_r.pad_id= left_pads[i].pad_id;
				 pad_r.gates_connected.push_back(right_gates[j].id);
				 pad_r.pad_x= horiz_pos;
				 pad_r.pad_y=left_pads[i].pad_y;
				 right_pads.push_back(pad_r);
			     }
				 }
			 }
		 }
		 }
	 }


//Contain right side by using new left_gate positions;
 
 //READ NEW left positions
 ifstream f2in(qpolnam);
 int ni;
 for(int i=0; i<nl;i++){
  f2in>>ni;
  f2in>>left_gates[i].x ;
  f2in>>left_gates[i].y;	
 }
 f2in.close();
 //(gate  propagation)
 vector<pads> rightpad_temp= contain_leftright(right_gates,left_gates,nl,nr,horiz_pos);
 right_pads.insert(right_pads.end(),rightpad_temp.begin(),rightpad_temp.end());
// 	cout <<" NETS "<<endl;
//for (int i=0;i<right_pads.size();i++){	
//	for(int j=0;j<right_pads[i].nets_connected.size();j++)
//	cout << right_pads[i].nets_connected[j]<<" ";
//	cout<<endl;
//    }
//		cout<<"right GATES"<< endl;
//    for (int i=0;i<right_pads.size();i++){
//
//	for(int h=0;h<right_pads[i].gates_connected.size();h++)
//	cout << right_pads[i].gates_connected[h] <<" ";
//   cout<<endl;
//    }
//     cout<<"PAD "<<endl;
//    for (int i=0;i<right_pads.size();i++){
//	cout << right_pads[i].pad_id;
//     cout<<endl;
//	 }

//RIGTHSIDE NETLIST
ofstream f3out(qpirnam);
f3out<<nr<<" "<<nnz <<endl;
for(int u=0; u<nr;u++){
f3out<< u+1 <<" "<<right_gates[u].nets_connected.size()<<" ";
for(int f=0;f<right_gates[u].nets_connected.size();f++)
f3out<< right_gates[u].nets_connected[f]<<" ";
f3out <<endl;
}
f3out<<right_pads.size()<<endl;
for(int pi=0; pi<right_pads.size();pi++){
f3out<<pi+1 <<" ";
f3out<<right_pads[pi].nets_connected.size()<<" ";
for(int pni=0;pni<right_pads[pi].nets_connected.size();pni++)
f3out<<right_pads[pi].nets_connected[pni]<<" ";
f3out<<right_pads[pi].pad_x<<" "<<right_pads[pi].pad_y;
f3out <<endl;
}
f3out.close();
q2placer(qpirnam,qpornam);
//READ NEW right positions
 ifstream f3in(qpornam);
 int nk;
 for(int i=0; i<nr;i++){
  f3in>>nk;
  f3in>>right_gates[i].x ;
  f3in>>right_gates[i].y;	
 }
 f3in.close();
 
 
 for(int i=0 ; i< nll;i++){
 	for(int j=0;j<nr;j++){
 		if(j<nl){
		 if(left_gates[j].id==left_ogates[i].id){
 			left_ogates[i].x=left_gates[j].x;
 			left_ogates[i].y=left_gates[j].y;
		 }
	     }
		if(right_gates[j].id==left_ogates[i].id){
 			left_ogates[i].x=right_gates[j].x;
 			left_ogates[i].y=right_gates[j].y;
		 }
	 }
 }
 
// gate left_refgates[nl],right_refgates[nr];
// for(int i=0;i<nr;i++){
// 	if (i<nl)
//left_refgates[i].equals(left_gates[i]);
//right_refgates[i].equals(right_gates[i]);
//}
//left_recgates=left_refgates;
//right_recgates=right_refgates;
//nlrec=nl;
//nrrec=nr;

ofstream f_lout(lonam);
f_lout<<nl<<endl;
for(int i=0;i<nl;i++){

f_lout<<left_gates[i].id<<" "<<left_gates[i].x<<" "<<left_gates[i].y<<endl;
f_lout<<left_gates[i].nets_connected.size()<<endl;
for(int nec=0;nec<left_gates[i].nets_connected.size();nec++){
	f_lout<<left_gates[i].nets_connected[nec]<<" ";
}
f_lout<<left_gates[i].pads_connected.size()<<endl;
for(int pec=0;pec<left_gates[i].pads_connected.size();pec++){
	f_lout<<left_gates[i].pads_connected[pec]<<" ";
}

}
ofstream f_rout(ronam);
f_rout<<nr<<endl;
for(int i=0;i<nr;i++){

f_rout<<right_gates[i].id<<" "<<right_gates[i].x<<" "<<right_gates[i].y<<endl;
f_rout<<right_gates[i].nets_connected.size()<<endl;
for(int nec=0;nec<right_gates[i].nets_connected.size();nec++){
	f_rout<<right_gates[i].nets_connected[nec]<<" ";
}
f_rout<<right_gates[i].pads_connected.size()<<endl;
for(int pec=0;pec<right_gates[i].pads_connected.size();pec++){
	f_rout<<right_gates[i].pads_connected[pec]<<" ";
}

}
left_recpads=left_pads;
right_recpads=right_pads;
}else cout<<"Unable to open vertical cut";
}
void updownrecursion(vector<vector<int> > ve,gate* left_gates,int nl,int nnz,vector<pads> left_1recpads,vector<pads> right_1recpads,double horiz_rec,const char* lonam, const char* ronam,const char* oq8nam,const char* oq9nam,const char* oq10nam,const char* oq11nam){
int nlrec,nrrec;
	ifstream f_lin(lonam);
	 int ns,ps;
	 int nsk,psk;
  f_lin>>nlrec;
  gate lgate[nlrec];
  for(int i=0;i<nlrec;i++){
  f_lin>>lgate[i].id>>lgate[i].x>>lgate[i].y;
  f_lin>>ns;
  for(int j=0;j<ns;j++){
  f_lin>>nsk;
  lgate[i].nets_connected.push_back(nsk);
  }
  f_lin>>ps;
  for(int j=0;j<ps;j++){
  f_lin>>psk;
  lgate[i].pads_connected.push_back(psk);
  }
  }

  containment_updown(ve,left_gates,nl,left_1recpads,lgate,nlrec,horiz_rec,nnz,f4name,oq4name,f5name,oq5name,oq8nam,oq9nam); 
    	 ifstream f_rin(ronam);
  f_rin>>nrrec;
  gate rgate[nrrec];
  for(int i=0;i<nrrec;i++){
  f_rin>>rgate[i].id>>rgate[i].x>>rgate[i].y;
  f_rin>>ns;
  for(int j=0;j<ns;j++){
  f_rin>>nsk;
  rgate[i].nets_connected.push_back(nsk);
  }
  f_rin>>ps;
  for(int j=0;j<ps;j++){
  f_rin>>psk;
  rgate[i].pads_connected.push_back(psk);
  }
  }

  containment_updown(ve,left_gates,nl,right_1recpads,rgate,nrrec,horiz_rec,nnz,f6name,oq6name,f7name,oq7name,oq10nam,oq11nam); 
}
int main(){
	qplacer(fname,oq1name);//first qp 
	int n,nnz;
	int gates_no,net_id,l;
	ifstream fin(oq1name);// output of qplacer1
	fin>>n;
	gate gatess[n];
	if(fin.is_open()){
		for(int i=0;i<n;i++){
		fin >> gatess[i].id;
		fin >>gatess[i].x;
		fin >> gatess[i].y;
		}
		//Assignment of gates to left and right
		
	int nl=n/2;
	int nr=n-nl;
	gate left_gates[nl],right_gates[nr];
		// one vertical cut
    sort_gatesx(gatess,n,left_gates,nl,right_gates,nr);
		
	

//	cout << "Left gates"<<endl;
//		for(int i=0;i<nl;i++)
//	cout  << left_gates[i].x <<endl;
//	cout << "Right gates"<<endl;
//		for(int i=0;i<nr;i++)
//	cout  << right_gates[i].x <<endl;
//		cout << "Left gates ID"<<endl;
//		for(int i=0;i<nl;i++)
//	cout  << left_gates[i].id <<endl;
//	cout << "Right gates ID"<<endl;
//		for(int i=0;i<nr;i++)
//	cout  << right_gates[i].id <<endl;	
//    
		fin>>nnz;
		vector<vector<int> > ve (nnz);
		// Distribute nets to left and right according to gates.
	for (int i=0;i<nnz;i++){
		fin>> net_id;
		fin >> gates_no;
		for (int j=0;j<gates_no;j++){
		fin >>l;
		ve[i].push_back(l);
		for (int q=0;q<nr;q++){
	    
		if (l==right_gates[q].id)
			right_gates[q].nets_connected.push_back(net_id);
	    if (q<nl){
		    if (l==left_gates[q].id)
		    left_gates[q].nets_connected.push_back(net_id);
		}
	    }
	    }  
	}

	vector<pads> left_pads=contain_leftright(left_gates,right_gates,nr,nl,50.0);//right gates as left pads
		// Pad distribution of real pads
	int initial_lp=left_pads.size();
	vector<pads> right_pads;
	int pad_total;
	fin>> pad_total;
	
	for(int i=0;i<pad_total;i++){
		pads padx;
		int po;
	fin>> padx.pad_id >> po>>padx.pad_x>>padx.pad_y;
	padx.nets_connected.push_back(po);
	padx.gates_connected=ve[po-1];
	if (padx.pad_x <=50)left_pads.push_back(padx);
	else right_pads.push_back(padx);
     }
     // right pad propagation to left
     int initial_rp=right_pads.size();
     int final_lp=left_pads.size();
     for(int i=0;i<right_pads.size();i++){
     	for(int ri=0; ri<right_pads[i].nets_connected.size();ri++)
		 for(int j=0;j<nl;j++){
     		for(int h=0;h<left_gates[j].nets_connected.size();h++){
     			if(left_gates[j].nets_connected[h]==right_pads[i].nets_connected[ri]){
     			 pads pad_t;
     			 	 int lt=0;
				 for(int ci=final_lp;ci<left_pads.size();ci++){
				 	if (left_pads[ci].pad_id==right_pads[i].pad_id){
				 		lt=1;
						left_pads[ci].gates_connected.push_back(left_gates[j].id);
						break;
					 }
				 	
				 }
				 if(lt==0){
				 pad_t.nets_connected.push_back(right_pads[i].nets_connected[ri]);
				 pad_t.pad_id= right_pads[i].pad_id;
				 pad_t.gates_connected=ve[right_pads[i].nets_connected[ri]-1];
				 pad_t.pad_x= 50;
				 pad_t.pad_y=right_pads[i].pad_y;
				 left_pads.push_back(pad_t);
				 }
			     }
			 }
		 }
	 }
//	cout <<" NETS "<<endl;
//for (int i=0;i<left_pads.size();i++){	
//	for(int j=0;j<left_pads[i].nets_connected.size();j++)
//	cout << left_pads[i].nets_connected[j]<<" ";
//	cout<<endl;
//    }
//		cout<<"LEFT GATES"<< endl;
//    for (int i=0;i<left_pads.size();i++){
//
//	for(int h=0;h<left_pads[i].gates_connected.size();h++)
//	cout << left_pads[i].gates_connected[h] <<" ";
//   cout<<endl;
//    }
//     cout<<"PAD "<<endl;
//    for (int i=0;i<left_pads.size();i++){
//	cout << left_pads[i].pad_id;
//     cout<<endl;
//	 }


//Leftside netlist
ofstream f2out(f2name);
f2out<<nl<<" "<<nnz <<endl;
for(int u=0; u<nl;u++){
f2out<< u+1 <<" "<<left_gates[u].nets_connected.size()<<" ";
for(int f=0;f<left_gates[u].nets_connected.size();f++)
f2out<< left_gates[u].nets_connected[f]<<" ";
f2out <<endl;
}
f2out<<left_pads.size()<<endl;
for(int pi=0; pi<left_pads.size();pi++){
f2out<<pi+1 <<" ";
f2out<<left_pads[pi].nets_connected.size()<<" ";
for(int pni=0;pni<left_pads[pi].nets_connected.size();pni++)
f2out<<left_pads[pi].nets_connected[pni]<<" ";
f2out<<left_pads[pi].pad_x<<" "<<left_pads[pi].pad_y;
f2out <<endl;
}
f2out.close();
//Placement of left side
q2placer(f2name,oq2name);


//Containment of right side 
// left pads propagation as right pads(pad propagation)
     for(int i=initial_lp;i<final_lp;i++){
     	for(int ri=0; ri<left_pads[i].nets_connected.size();ri++){
		 for(int j=0;j<nr;j++){
     		for(int h=0;h<right_gates[j].nets_connected.size();h++){
     			if(right_gates[j].nets_connected[h]==left_pads[i].nets_connected[ri]){
				 pads pad_r;
				 int rt=0;
				 for(int ci=initial_rp;ci<right_pads.size();ci++){
				 	if (right_pads[ci].pad_id==left_pads[i].pad_id){
				 		rt=1;
						 right_pads[ci].gates_connected.push_back(right_gates[j].id);
						 break;
					 }
				 	
				 }
				 if(rt==0){
				 pad_r.nets_connected.push_back(left_pads[i].nets_connected[ri]);
			     pad_r.pad_id= left_pads[i].pad_id;
				 pad_r.gates_connected.push_back(right_gates[j].id);
				 pad_r.pad_x= 50;
				 pad_r.pad_y=left_pads[i].pad_y;
				 right_pads.push_back(pad_r);
			     }
				 }
			 }
		 }
		 }
	 }


//Contain right side by using new left_gate positions;
 
 //READ NEW left positions
 ifstream f2in(oq2name);
 int ni;
 for(int i=0; i<nl;i++){
  f2in>>ni;
  f2in>>left_gates[i].x ;
  f2in>>left_gates[i].y;	
 }
 f2in.close();
 //(gate  propagation)
 vector<pads> rightpad_temp= contain_leftright(right_gates,left_gates,nl,nr,50.0);
 right_pads.insert(right_pads.end(),rightpad_temp.begin(),rightpad_temp.end());
// 	cout <<" NETS "<<endl;
//for (int i=0;i<right_pads.size();i++){	
//	for(int j=0;j<right_pads[i].nets_connected.size();j++)
//	cout << right_pads[i].nets_connected[j]<<" ";
//	cout<<endl;
//    }
//		cout<<"right GATES"<< endl;
//    for (int i=0;i<right_pads.size();i++){
//
//	for(int h=0;h<right_pads[i].gates_connected.size();h++)
//	cout << right_pads[i].gates_connected[h] <<" ";
//   cout<<endl;
//    }
//     cout<<"PAD "<<endl;
//    for (int i=0;i<right_pads.size();i++){
//	cout << right_pads[i].pad_id;
//     cout<<endl;
//	 }

//RIGTHSIDE NETLIST
ofstream f3out(f3name);
f3out<<nr<<" "<<nnz <<endl;
for(int u=0; u<nr;u++){
f3out<< u+1 <<" "<<right_gates[u].nets_connected.size()<<" ";
for(int f=0;f<right_gates[u].nets_connected.size();f++)
f3out<< right_gates[u].nets_connected[f]<<" ";
f3out <<endl;
}
f3out<<right_pads.size()<<endl;
for(int pi=0; pi<right_pads.size();pi++){
f3out<<pi+1 <<" ";
f3out<<right_pads[pi].nets_connected.size()<<" ";
for(int pni=0;pni<right_pads[pi].nets_connected.size();pni++)
f3out<<right_pads[pi].nets_connected[pni]<<" ";
f3out<<right_pads[pi].pad_x<<" "<<right_pads[pi].pad_y;
f3out <<endl;
}
f3out.close();
q2placer(f3name,oq3name);
//READ NEW right positions
 ifstream f3in(oq3name);
 int nk;
 for(int i=0; i<nr;i++){
  f3in>>nk;
  f3in>>right_gates[i].x ;
  f3in>>right_gates[i].y;	
 }
 f3in.close();
 

//Horizontal cuts;
 double vert_pos=50.0;
 
 containment_updown(ve,left_gates,nl,left_pads,left_gates,nl,vert_pos,nnz,f4name,oq4name,f5name,oq5name,oq8name,oq9name); 
  containment_updown(ve,right_gates,nr,right_pads,right_gates,nr,vert_pos,nnz,f6name,oq6name,f7name,oq7name,oq10name,oq11name); 
  double horiz_rec=25.0;
  vector<pads> left_1recpads, left_2recpads,left_3recpads,left_4recpads,right_1recpads,right_2recpads,right_3recpads,right_4recpads;
	
  containment_leftright(left_gates,nl,left_1recpads,right_1recpads,oq8name,ot1name,ot2name,ot3name,ot4name,loname,roname,horiz_rec);
  containment_leftright(left_gates,nl,left_2recpads,right_2recpads,oq9name,ot1name,ot2name,ot3name,ot4name,lo1name,ro1name,horiz_rec);
  containment_leftright(right_gates,nr,left_3recpads,right_3recpads,oq10name,ot1name,ot2name,ot3name,ot4name,lo2name,ro2name,100-horiz_rec);
  containment_leftright(right_gates,nr,left_4recpads,right_4recpads,oq11name,ot1name,ot2name,ot3name,ot4name,lo3name,ro3name,100-horiz_rec);
  updownrecursion(ve,left_gates,nl,nnz,left_1recpads,right_1recpads,100-horiz_rec,loname,roname,oq8name,oq9name,oq10name,oq11name);
  updownrecursion(ve,left_gates,nl,nnz,left_2recpads,right_2recpads,horiz_rec,lo1name,ro1name,oq8name,oq9name,oq10name,oq11name);
  updownrecursion(ve,right_gates,nr,nnz,left_3recpads,right_3recpads,100-horiz_rec,lo2name,ro2name,oq8name,oq9name,oq10name,oq11name);
  updownrecursion(ve,right_gates,nr,nnz,left_4recpads,right_4recpads,horiz_rec,lo3name,ro3name,oq8name,oq9name,oq10name,oq11name);
//    containment_leftright(left_gates,nl,left_recpads,left_recgates,nlrec,right_recpads,right_recgates,nrrec,oq9name,ot1name,ot2name,ot3name,ot4name,horiz_rec);
//    
//      containment_leftright(right_gates,nr,left_recpads,left_recgates,nlrec,right_recpads,right_recgates,nrrec,oq10name,ot1name,ot2name,ot3name,ot4name,100-horiz_rec);
//        containment_leftright(right_gates,nr,left_recpads,left_recgates,nlrec,right_recpads,right_recgates,nrrec,oq11name,ot1name,ot2name,ot3name,ot4name,100-horiz_rec);
//  	

 //Final output after placement
 gate final_gates[n];
 for(int li=0;li<nl;li++){
 final_gates[left_gates[li].id-1].id= left_gates[li].id;
 final_gates[left_gates[li].id-1].x=left_gates[li].x;
 final_gates[left_gates[li].id-1].y=left_gates[li].y;
 }
  for(int ri=0;ri<nr;ri++){
 final_gates[right_gates[ri].id-1].id= right_gates[ri].id;
 final_gates[right_gates[ri].id-1].x=right_gates[ri].x;
 final_gates[right_gates[ri].id-1].y=right_gates[ri].y;
 }
 ofstream flout(opname);
 for(int i=0;i<n;i++){
		flout<< final_gates[i].id <<" "<<final_gates[i].x<<" "<<final_gates[i].y<<endl;
		}
	}else cout <<"Unable to open the file";
	return 0;
}
