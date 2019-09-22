#include <iostream>
#include <valarray>
#include "solver.h"
#include  <vector>
#define fname "../benchmarks/3QP/toy2"
#define oq1name "./netq1.txt"
#define oq2name "./netq2.txt"
#define oq3name "./netq3.txt"
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
int main(void){
  ifstream fin(fname);
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
  for (int i=0;i<nnz;i++){
//  	cout<< i+1 <<" ";//net id
//	print_vector(v[i]);// gates connected to net
 for (size_t j = 0; j < v[i].size(); ++j) {
 	for (size_t p = 0; p < v[i].size(); ++p) {
 	if(j!=p)
 	d[v[i][j]-1][v[i][p]-1]--;
 }
  }
  }	
  
  for (int i=0;i<n;i++){
  	
    d[i][i]-=vector_sum(d[i]);
  
  }
  for(int j=0;j<pad_total;j++){
  		for(int m=0;m<pad_gate[j].size();m++){
  	d[pad_gate[j][m]-1][pad_gate[j][m]-1]++;
	}
}

  print_matrix(d);
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

   //initialize as [1, 1, 1]
  valarray<double> t(1.0, A.n);
  valarray<double> b(A.n);
  A.matvec(t, b); // b = At

  cout << "b =" << endl;
  print_valarray(b);

  // solve for t
  cout << "t = " << endl;
  A.solve(b, t);
  print_valarray(t);
  //Create b_x, b_y
    valarray<double> b_x(A.n),b_y(A.n);
    for(int i=0;i<pad_total;i++){
    	for (int j=0;j<pad_gate[i].size();j++){
		b_x[pad_gate[i][j]-1]=pad_x[i];
    	b_y[pad_gate[i][j]-1]=pad_y[i];
        }
			}
	print_valarray(b_x);
	valarray <double> x_q1(A.n), y_q1(A.n);
	A.solve(b_x,x_q1);
	A.solve(b_y,y_q1);
    ofstream fout(oq1name);
    for(int i=0;i<A.n;i++)
    fout<< i+1 <<" "<< x_q1[i] <<" "<<y_q1[i] <<endl ;
	
	
	fout.close();
	} else cout<< "Unable to open input file"; 
		
	return 0;
}
