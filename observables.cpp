#include "observables.hpp"
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

void pairDensity(pairsIndDist pairs,
				 double dMax,
				 double dr)
{
	/* Comptage des paires dans [r,r+dr[ */
	
	int nBins = int(dMax/dr)+1;
	vector<int> n(nBins);
	
	vector<double> distances = pairs.distances;
	
	for (double d: distances)
	{
		if (d<dMax)
		{
			int bin = int(d/dr);
			n[bin] += 1;
		}
	}
	
	/* Enregistrement dans un fichier */
	
	std::ofstream file("data/pairDensity.csv", std::ios_base::app);
	if (file)
	{
		for (int i=0; i<nBins-1; i++) {file << n[i] << " " ;}
		file << n[nBins-1] << endl;
	}
	else { cout << "Unable to open file \"data/pairDensity.csv\"" << endl;}
	
	file.close();
}

double velocityQuadMean(vector<vector<double>> v)
{
	int N = v.size();
	double sumV2 = 0;
	
	for (int i=0; i<N; i++) 
		{sumV2 += v[i][0]*v[i][0]+v[i][1]*v[i][1]+v[i][2]*v[i][2];}
	
	return sumV2/N;
}