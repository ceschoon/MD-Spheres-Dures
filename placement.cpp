#include "placement.hpp"
#include <math.h>
#include <iostream>


using std::vector;
using std::cout;
using std::endl;


vector<vector<double>> placementR(vector<int> sizes, double a) 
{ 
	vector<vector<double>> base = {{0,0,0},{a/2,a/2,0},{a/2.0,0,a/2.0},
									{0,a/2,a/2.0}};
	int mx = sizes[0];
	int my = sizes[1];
	int mz = sizes[2];
 	vector<vector<double>> network(4*mx*my*mz,{0,0,0});
 	vector<vector<int>> temp(mx*my*mz,{0,0,0}); // variable temporaire
 	 
 	int p = 0;
	for (int i=0; i<mx; i++)
	{	
		for (int j=0; j<my; j++)
		{
			for (int k=0; k<mz; k++)
			{
				temp[p] = {i,j,k};
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
				network[s][m] = a* (base[q][m] + temp[l][m]);	
			}
			s = s+1;
		}
	}
	
	return network;
}

vector<vector<double>> placementV(int N, double T, 
	std::default_random_engine &eng) // <v^2> = 3kT/m
{
	vector<vector<double>> v(N,{0,0,0});
	double mu = 0;
	double sigma = sqrt(T);
	
	std::normal_distribution<double> normalDist(mu, sigma);
	
	for (int n=0; n<N; n++)
	{
		v[n][0] = normalDist(eng);
		v[n][1] = normalDist(eng);
		v[n][2] = normalDist(eng);		
	}
	
	vector<double> qm(3,0); // qté de mouvt totale avec masse = 1
	for (int m=0; m<3; m++)
	{
		for (int n=0; n<N; n++)
		{
			qm[m] = qm[m] + v[n][m];
		}
		
		for (int n=0; n<N; n++)
		{
			v[n][m] = v[n][m] - qm[m]/N;
		}
	}
			
	return v;
}
