#include "placement.hpp"
#include <vector>
#include <iostream>

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
	vector<vector<double>> r = placementR({3,3,3},1);
	printMatrix(r);
	return 0;
}

// g++ -g -o testPlacement placement.cpp mainTestPlacement.cpp