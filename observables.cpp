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
	
	int nBins = int(dMax/dr);
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
	
	std::ofstream file("data/pairDensity.dat", std::ios_base::app);
	if (file)
	{
		for (int i=0; i<nBins-1; i++) {file << n[i] << ", " ;}
		file << n[nBins-1] << endl;
	}
	else { cout << "Unable to open file \"data/pairDensity.dat\"" << endl;}
	
	file.close();
}