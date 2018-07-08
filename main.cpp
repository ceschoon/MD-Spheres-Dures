/***************************************************************************
 *																		   *
 * 	Ce programme est une simulation d'un système de sphères dures.		   *
 *																		   *
 *	Utilisation: ./main TODO:arguments									   *
 * 																		   *
 *	Auteurs du programme: Cédric Schoonen et Maxime Jamotte				   *
 *	Auteur de ce fichier: Cédric Schoonen								   *
 *																		   *
 ***************************************************************************/
 
 /*
  * Remarques:
  *	-	Le système d'unités utilisé est le suivant:
  *		-	m = 1 la masse d'une particule
  *		-	R = 1 le rayon d'une particule
  *		-	k = 1 la constante de Boltzmann
  *		-	dt ~ 10^-4 le pas de temps //TODO: être plus précis
  */

//TODO: messages info sur le système
//TODO: print du temps de corrélation => séparation en blocs plus tard (python)

#include "placement.hpp"
#include "move.hpp"
#include "observables.hpp"
#include "utilities.hpp"
#include <vector>
#include <iostream>
#include <math.h>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;


int main(int argc, char *argv[]) // /!\ entree d'arguments pas idiot-proof
{
	/* Déclaration des paramètres par défaut */
	
	// paramètres physiques
	int mx = 2; // mx*my*mz = nombre de mailles cubiques FCC à simuler
	int my = 2; // 4*mx*my*mz = nombre de particules
	int mz = 2;
	double n = 0.01; // densité max = 1/32/sqrt(2) approx 0.022
	double T = 1; // température
	
	// paramètres de simulation
	double tEq = 5; // temps pour atteindre l'équilibre
	double tSim = 1; // temps de simulation une fois à l'équilibre
	double dt = 0.01; // pas de temps
	double dr = 0.1; // largeur de bin pour g(r)
	
	/* Récupération des paramètres donnés en argument */
	
	if (argc>10)
	{
		mx = atoi(argv[1]);
		my = atoi(argv[2]);
		mz = atoi(argv[3]);
		n = atof(argv[4]);
		T = atof(argv[5]);
		tEq = atof(argv[6]);
		tSim = atof(argv[7]);
		dt = atof(argv[8]);
		dr = atof(argv[10]);
	}
	
	/* Initialisation de la simulation */
	
	int N = 4*mx*my*mz; // nombre de particules
	double a = pow(4/n, 1/3.0);
	
	vector<vector<double>> r = placementR({mx,my,mz}, a);
	vector<vector<double>> v = placementV(N, T);
	
	vector<double> boxDimensions = {a*mx, a*my, a*mz};
	
	double t=0;
	double corrLength = 
		*std::min_element(boxDimensions.begin(),boxDimensions.end());
	
	double dMax = a * 0.9; // dist max pour lister les paires // corrLength*.9
	
	/* Simulation jusqu'à l'équilibre */
	
	int nt = int(tEq/dt);

/*
	for (int i=0; i<nt; i++)
	{
		cout << "t = " << t+dt << endl;
		
		pairsIndDist pairs = pairList(r,boxDimensions,dMax);
		move(r,v,boxDimensions,pairs,dt,t);
		pairDensity(pairs,dMax,dr);
		
		//TODO: pq v = 0 0 0
		//TODO: pq se rentre dedans ? <-> jigling: si repart pas avec la
		//		même vitesse, reste dans l'autre
		
		t = t+dt;
	}
*/
	
	pairsIndDist pairs = pairList(r,boxDimensions,dMax);
	pairDensity(pairs,dMax,dr);
	
/*
	vector<int> indices1 = pairs.indicesPart1;
	vector<int> indices2 = pairs.indicesPart1;
	vector<double> dists = pairs.distances;
	cout << "a = " << a << endl;
	cout << "indices1: " << endl;
	printVector(indices1);
	cout << "indices2: " << endl;
	printVector(indices2);
	cout << "dists: " << endl;
	printVector(dists);
*/
	
	/* Simulation à l'équilibre */
	
	return 0;
}