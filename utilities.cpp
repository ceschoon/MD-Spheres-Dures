#include "utilities.hpp"
#include <iostream>

using std::cout;
using std::endl;


void printVector(vector<int> r)
{
	int N = r.size();
	
	for (int i=0; i<N; i++)
	{	
		cout << r[i] << " 	";
	}
	cout << endl;
}

void printVector(vector<double> r)
{
	int N = r.size();
	
	for (int i=0; i<N; i++)
	{	
		cout << r[i] << " 	";
	}
	cout << endl;
}

void printMatrix(vector<vector<double>> r)
{
	int N = r.size();
	int M = r[0].size();
	
	for (int i=0; i<N; i++)
	{	
		for (int j=0; j<M; j++)
		{
			cout << r[i][j] << " 	";
		}
		cout << endl;
	}
}

