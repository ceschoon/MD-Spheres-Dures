#include "placement.hpp"
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <limits>


using std::vector;
using std::cout;
using std::endl;

double boxMuller(double mu, double sigma) // Wikipédia
{
	static const double epsilon = std::numeric_limits<double>::min();
	static const double two_pi = 2.0*3.14159265358979323846;

	thread_local double z1;
	thread_local bool generate;
	generate = !generate;

	if (!generate)
	   return z1 * sigma + mu;

	double u1, u2;
	do
	 {
	   u1 = rand() * (1.0 / RAND_MAX);
	   u2 = rand() * (1.0 / RAND_MAX);
	 }
	while ( u1 <= epsilon );

	double z0;
	z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
	return z0 * sigma + mu;
}

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

vector<vector<double>> placementV(int N, double T) // <v^2> = 3kT/m
{
	vector<vector<double>> v(N,{0,0,0});
	double mu = 0;
	double sigma = sqrt(T);
	
	for (int n=0; n<N; n++)
	{
		v[n][0] = boxMuller(mu,sigma);
		v[n][1] = boxMuller(mu,sigma);
		v[n][2] = boxMuller(mu,sigma);		
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
