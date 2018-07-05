#include "placement.hpp"
#include <math.h>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;



vector<vector<double>> placementR(vector<int> sizes, double a) 
{ 
	vector<vector<double>> base = {{0,0,0},{1.0/2,1.0/2,0},{1/2.0,0,1/2.0},
									{0,1.0/2,1/2.0}};
	int mx = sizes[0];
	int my = sizes[1];
	int mz = sizes[2];
 	vector<vector<double>> network(4*mx*my*mz,{0,0,0});
 	vector<vector<double>> temp(mx*my*mz,{0,0,0}); // variable temporaire
 	 
 	int p = 0;
	for (int i=0; i<mx; i++)
	{	
		for (int j=0; j<my; j++)
		{
			for (int k=0; k<mz; k++)
			{
				temp[p] = {i*a,j*a,k*a};
				p = p+1;
			}
	 	}
	 }			
 	
 	int s = 0;
	for (int l=0; l<temp.size(); l++)
	{	
		for (int q=0; q<base.size(); q++) // transla. de chaque élém. de la base
		{
			for (int m=0; m<network[0].size(); m++) // addition de vecteurs
			{
				network[s][m] = base[q][m] + temp[l][m];	
			}
			s = s+1;
		}
	}
	
	return network;
}

































