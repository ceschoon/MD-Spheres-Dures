/***************************************************************************
 *																		   *
 * 	Ce programme est une simulation d'un système de sphères dures.		   *
 *																		   *
 *	Utilisation: ./main mx my mz n T tEq tSim dt dMax dr				   *
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
//TODO: trouver comment fixer tEq
//TODO: ajuster le temps de simulation avec le temps de corrélation

#include "placement.hpp"
#include "move.hpp"
#include "observables.hpp"
#include "utilities.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;


int main(int argc, char *argv[]) // /!\ entree d'arguments pas idiot-proof
{
	/* Déclaration des paramètres par défaut */
	
	// paramètres physiques
	int mx = 3; // mx*my*mz = nombre de mailles cubiques FCC à simuler
	int my = 3; // 4*mx*my*mz = nombre de particules
	int mz = 3;
	double n = 0.01; // densité max = 1/32/sqrt(2) approx 0.022
	double T = 1; // température
	
	// paramètres de simulation
	double tEq = 10; // temps pour atteindre l'équilibre
	double tSim = 1; // temps de simulation une fois à l'équilibre
	double dt = 0.01; // pas de temps
	double dMax = 0; // dist max pour lister les paires -- assigné plus loin
	double dr = 0.1; // largeur de bin pour g(r)
	
	/* Récupération des paramètres donnés en argument */
	
	double isdMaxFixedByUser = false;
	
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
		dMax = atof(argv[9]);
		dr = atof(argv[10]);
		
		isdMaxFixedByUser = true;
	}
	
	/* Nettoyage des fichiers utilisés pour la simulation */
	
	std::ofstream fileCollisionData("data/collisionData.dat");
	fileCollisionData.close();
	std::ofstream filePairDensity("data/pairDensity.dat");
	filePairDensity.close();
	
	/* Initialisation de la simulation */
	
	int N = 4*mx*my*mz; // nombre de particules
	double a = pow(4/n, 1/3.0);
	
	vector<vector<double>> r = placementR({mx,my,mz}, a);
	vector<vector<double>> v = placementV(N, T);
	
	vector<double> boxDimensions = {a*mx, a*my, a*mz};
	
	double t=0;
	double corrLength = 
		*std::min_element(boxDimensions.begin(),boxDimensions.end());
	
	if (!isdMaxFixedByUser) {dMax = a * 0.9;}
	
	/*
	 * Remarque:
	 * a * 0.9 suffisant pour trouver les collisions à traiter
	 * corrLength * 0.9 si on veut g(r), mais prend plus de temps
	 * Le temps du calcul des paires grandi approx selon dMax^5
	 */
	
	/* Simulation jusqu'à l'équilibre */
	
	/*
	cout << "a = " << a << endl;
	cout << "r en t=0: " << endl;
	printMatrix(r);
	cout << "v en t=0: " << endl;
	printMatrix(v);
	*/
	
	int nt = int(tEq/dt);


	for (int i=0; i<nt; i++)
	{
		/*
		cout << "-------------------------------------------------" << endl;
		cout << "t = " << t+dt << endl;
		*/
		
		pairsIndDist pairs = pairList(r,boxDimensions,dMax);
		move(r,v,boxDimensions,pairs,dt,t);
		pairDensity(pairs,dMax,dr);
		
		/*
		cout << "r: " << endl;
		printMatrix(r);
		cout << "v: " << endl;
		printMatrix(v);
		*/
		
		t = t+dt;
	}
	
	/* Simulation à l'équilibre */
	
	return 0;
}