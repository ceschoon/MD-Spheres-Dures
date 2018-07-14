#include "placement.hpp"
#include <vector>
#include <iostream>
#include <fstream>

using std::vector;
using std::cout;
using std::endl;

void printMatrix(vector<vector<double>> r)
{
	int N = r.size();
	int M = r[0].size();
	
	for (int i=0; i<N; i++)
	{	
		for (int j=0; j<M; j++)
		{
			cout << r[i][j] << "	";
		}
		cout << endl;
	}
}

int main()
{	
	int N = 108;
	vector<vector<double>> r = placementR({3,3,3},1.1);
	vector<vector<double>> v = placementV(N,100);
	printMatrix(v);
	
	// Quantité de mouvement
	vector<double> qm(3,0);
	
	for (int m=0; m<3; m++)
	{
		for (int n=0; n<N; n++)
		{
			qm[m] = qm[m] + v[n][m];
		}
	}
	
	cout << "qm[0] = " << qm[0] << endl; 
	cout << "qm[1] = " << qm[1] << endl; 
	cout << "qm[2] = " << qm[2] << endl; 
	return 0;
}

// g++ -g -o testPlacement placement.cpp mainTestPlacement.cpp