/****************************************************************************
 *																			*
 *	mainTestMove: Programme destiné à tester la fonction "move" du fichier	*
 *		move.cpp															*
 * 																			*
 * 	Usage: ./testMove <tMax> <dt>											*
 *																			*
 *	Auteur: Cédric Schoonen													*
 *																			*
 ****************************************************************************/

#include "move.hpp"
#include "observables.hpp"
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;

void printParticles(vector<vector<double>> r,
					vector<vector<double>> v)
{
	int N = r.size();
	
	for (int i=0; i<N ; i++)
	{
		cout << "particle " << i << ":" << endl;
		cout << r[i][0] << "	" << r[i][1] << "	" << r[i][2] << endl;
		cout << v[i][0] << "	" << v[i][1] << "	" << v[i][2] << endl;
	}
}

int main(int argc, char *argv[]) // /!\ entree d'arguments pas idiot-proof
{	
	/* Déclaration des paramètres par défaut */
	
	double tMax = 1.6;
	double dt = 0.05;
	double dMax = 3.5;
	double dr = 0.1;
	
	/* Récupération des paramètres donnés en argument */
	
	if (argc>1){tMax = atof(argv[1]);} 	// attention mauvais inputs
	if (argc>2){dt = atof(argv[2]);} 	// attention mauvais inputs
	
	/* Test du code */
	
	int nt = int(tMax/dt) + 1;
	double t = 0;

	vector<vector<double>> r;
	vector<vector<double>> v;
	vector<double> boxDimensions = {7,7,7};
	
	r.push_back({0,0,0}); v.push_back({0,1,0});
	r.push_back({3,3,0}); v.push_back({-1,0,0});
	
	for (int i=0; i<nt; i++)
	{
		cout << "t = " << t+dt << endl;
		pairsIndDist pairs = pairList(r,boxDimensions,dMax);
		move(r,v,boxDimensions,pairs,dt,t);
		printParticles(r,v);
		t = t+dt;
	}
	
	return 0;
}