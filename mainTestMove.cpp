/****************************************************************************
 *																			*
 *	mainTestMove: Programme destiné à tester la fonction "move" du fichier	*
 *		move.cpp															*
 * 																			*
 * 	Usage: ./testMove <tMax> <dt>											*
 *																			*
 * 	Compilation flags pour debugging: 										*
 * 		-DDEBUG1: Localise une seg fault									*
 *	Auteur: Cédric Schoonen													*
 *																			*
 ****************************************************************************/

#include "move.hpp"
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
	
	double tMax = 1;
	double dt = 0.05;
	
	/* Récupération des paramètres donnés en argument */
	
	if (argc>1){tMax = atof(argv[1]);} 	// attention mauvais inputs
	if (argc>2){dt = atof(argv[2]);} 	// attention mauvais inputs
	
	#ifdef DEBUG2
	cout << "tMax = " << tMax << "	dt = " << dt << endl;
	#endif // DEBUG2
	
	/* Test du code */
	
	int nt = int(tMax/dt) + 1;

	vector<vector<double>> r; r.clear();
	vector<vector<double>> v; v.clear();
	vector<double> boxDimensions = {6,6,6};
	
	r.push_back({0,0,0}); v.push_back({1,0,0});
	r.push_back({3,0,0}); v.push_back({-1,0,0});
	
	for (int i=0; i<nt; i++)
	{
		double t = t+dt;
		cout << "t = " << t << endl;
		move(r,v,boxDimensions,dt);
		printParticles(r,v);
	}
}