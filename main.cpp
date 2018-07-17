/***************************************************************************
 *																		   *
 * 	Ce programme est une simulation d'un système de sphères dures.		   *
 *																		   *
 *	Utilisation: ./main mx my mz n T tSim dt dMax dr seed				   *
 *																		   *
 *		mx*my*mz 	nombre de mailles cubiques FCC à simuler			   *
 * 		n 			densité (densité max = 1/32/sqrt(2) approx 0.022)	   *
 *		T 	 		température											   *
 * 		tSim  		temps de simulation									   *
 *		dt 			pas de temps										   *
 *		dMax 		dist max pour lister les paires						   *
 *		dr 			largeur des bins pour g(r)							   *
 *		seed 		graine pour la génération de nombres aléatoires		   *
 *					seed = 0 est remplacé par un vrai nombre aléatoire	   *
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
  *		-	dt ~ 10^-2 le pas de temps //TODO: être plus précis, comment
  *				définir l'unité temporelle ?
  */

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
	int mx = 3; 
	int my = 3; 
	int mz = 3;
	double n = 0.01; 
	double T = 1; 
	
	// paramètres de simulation
	double tSim = 10; 
	double dt = 0.01; 
	double dMax = 0; // assigné plus loin
	double dr = 0.1; 
	int seed = 0;
	
	/* Récupération des paramètres donnés en argument */
	
	double isdMaxFixedByUser = false;
	
	if (argc>10)
	{
		mx = atoi(argv[1]);
		my = atoi(argv[2]);
		mz = atoi(argv[3]);
		n = atof(argv[4]);
		T = atof(argv[5]);
		tSim = atof(argv[6]);
		dt = atof(argv[7]);
		dMax = atof(argv[8]);
		dr = atof(argv[9]);
		seed = atoi(argv[10]);
		
		isdMaxFixedByUser = true;
	}
	
	/* Initialisation des fichiers utilisés pour la simulation */
	
	std::ofstream fileCollisionData("data/collisionData.csv");
	if (fileCollisionData)
	{
		fileCollisionData << "t,vDotr" << endl;
	}
	fileCollisionData.close();
	
	std::ofstream filePairDensity("data/pairDensity.csv");
	if (filePairDensity)
	{
		filePairDensity << "pairHistogram" << endl;
	}
	filePairDensity.close();
	
	// fichier "data/infoSimulation.dat" utilisé plus loin
	
	/* Initialisation de la simulation */
	
	if (seed == 0) 
	{
		std::random_device r;
		seed = r();
	}
	std::default_random_engine eng(seed);
	
	int N = 4*mx*my*mz; // nombre de particules
	double a = pow(4/n, 1/3.0);
	
	vector<vector<double>> r = placementR({mx,my,mz}, a);
	vector<vector<double>> v = placementV(N, T, eng);
	
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
	
	/* Réajustement des vitesse pour être à la bonne température */
	
	/*
	 * Les vitesses sont distribuées selon une gausienne à la bonne 
	 * mais le nombre fini de particules fait que la température réelle
	 * du système n'est pas celle de la distribution utilisée.
	 * Le système est censé vérifier la relation :
	 * 		
	 *		1/2*m*N*<v^2> = 3/2*(N-1)*k*T 	(il y a 3*(N-1) degrés de liberté)
	 *	
	 * Ou de manière équivalente dans notre système d'unités:
	 * 
	 *		<v^2> = 3*T*(N-1)/N
	 * 
	 * Il faut donc multiplier les vitesses par
	 * 
	 * 		lambda = sqrt( 3*T*(N-1)/(N*<v^2>) )
	 *
	 * pour s'assurer que le système se trouve à la bonne température.
	 */
	
	double avgV2 = velocityQuadMean(v);
	double lambda = sqrt( 3*T*(N-1)/(N*avgV2) );
	
	for (int i=0; i<N; i++)
	{
		v[i][0] = v[i][0] * lambda;
		v[i][1] = v[i][1] * lambda;
		v[i][2] = v[i][2] * lambda;
	}
	
	/* Enregistrement des caractéristiques du système */
	
	std::ofstream fileInfoSimulation("data/infoSimulation.csv");
	if (fileInfoSimulation)
	{
		fileInfoSimulation << "mx,my,mz,n,T,tSim,dt,dMax,dr,seed,N,a" 
						   << endl;
		fileInfoSimulation << mx << ", "
						   << my << ", "
						   << mz << ", "
						   << n << ", "
						   << T << ", "
						   << tSim << ", "
						   << dt << ", "
						   << dMax << ", "
						   << dr << ", "
						   << seed << ", "
						   << N << ", "
						   << a << endl;
	}
	fileInfoSimulation.close();
	
	/* Simulation */
	
	int nt = int(tSim/dt);

	for (int i=0; i<nt; i++)
	{	
		pairsIndDist pairs = pairList(r,boxDimensions,dMax);
		move(r,v,boxDimensions,pairs,dt,t);
		pairDensity(pairs,dMax,dr);
		t = t+dt;
	}
	
	return 0;
}