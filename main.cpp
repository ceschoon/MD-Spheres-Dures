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
  *		-	//TODO: comment définir l'unité temporelle/température ?
  */

#include "placement.hpp"
#include "move.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <assert.h>

using std::cout;
using std::endl;
using std::vector;

/******************************************************************************/

/* Implémentation de la fonction "velocityQuadMean" */

double velocityQuadMean(vector<vector<double>> v)
{
	int N = v.size();
	double sumV2 = 0;
	
	for (int i=0; i<N; i++) 
		{sumV2 += v[i][0]*v[i][0]+v[i][1]*v[i][1]+v[i][2]*v[i][2];}
	
	return sumV2/N;
}

/******************************************************************************/

/* Implémentation de la fonction "compPairs" */

/*
 * Cette fonction est utilisée pour comparer deux vecteurs d'indices 
 * représentant une paire de particules du système. Elle renvoie "true" que
 * lorsque i<=k et j<=l pour les paires {i,j} {k,l} dans leur forme
 * canonique sauf si les paires sont équivalentes (i==k et j==l dans la forme
 * canonique). La "forme canonique" d'une paire est définie comme {i,j} tel
 * que i>=j.
 */

bool compPairs(vector<int> a, vector<int> b)
{
	assert(a.size()==2);
	assert(b.size()==2);
	
	if (a[0]<a[1]) {std::swap(a[0], a[1]);} // forme canonique
	if (b[0]<b[1]) {std::swap(b[0], b[1]);}
	
	bool aIsBeforeb = true;
	if (a[0]>b[0]) {aIsBeforeb = false;}
	else if (a[0]==b[0] && a[1]>=b[1]) {aIsBeforeb = false;}
	
	return aIsBeforeb; // comparaison
}

/******************************************************************************/

/* Implémentation de la fonction "main" */

int main(int argc, char *argv[]) // /!\ entree d'arguments pas idiot-proof
{
	/* Déclaration des paramètres par défaut */
	
	// paramètres physiques
	int mx = 3; 
	int my = 3; 
	int mz = 3;
	double n = 0.00022; 
	double T = 1; 
	
	// paramètres de simulation
	double tSim = 100;  
	int seed = 1;
	
	/* Récupération des paramètres donnés en argument */
	
	if (argc>7)
	{
		mx = atoi(argv[1]);
		my = atoi(argv[2]);
		mz = atoi(argv[3]);
		n = atof(argv[4]);
		T = atof(argv[5]);
		tSim = atof(argv[6]);
		seed = atoi(argv[7]);
	}
	
	/* Initialisation des fichiers utilisés pour la simulation */
	
	// fichier "data/infoSimulation.dat" utilisé plus loin
	
	std::ofstream fileCollisionData("data/collisionData.csv");
	if (fileCollisionData)
	{
		fileCollisionData << "t,vDotr" << endl;
	}
	fileCollisionData.close();
	
	/* Initialisation de la simulation */
	 
	if (seed==0) 
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
		fileInfoSimulation << "mx,my,mz,n,T,tSim,seed,N,a" 
						   << endl;
		fileInfoSimulation << mx << ", "
						   << my << ", "
						   << mz << ", "
						   << n << ", "
						   << T << ", "
						   << tSim << ", "
						   << seed << ", "
						   << N << ", "
						   << a << endl;
	}
	fileInfoSimulation.close();
	
	/* Simulation */
	
	map<int,double> exitTimes; // temps sortie boîte pour les particules
	map<int,int> exitPlanes; // plans par lesquels les particules sortent
	
	bool(*fn_pt)(vector<int>,vector<int>) = compPairs;
	map<vector<int>,double,bool(*)(vector<int>,vector<int>)>
		collisionTimes(fn_pt); // temps de collisions pour les paires de part.
	map<vector<int>,vector<int>,bool(*)(vector<int>,vector<int>)>
		collisionTarget(fn_pt); // indices {cible, copie de la cible} par paire
	
	vector<int> indices(N); // indices des part. dont les collis. ou
		// sorties sont inconnues (tous au départ)
	for (int i=0; i<N; i++) {indices[i] = i;}
	
	while (t<=tSim)
	{	
		/* Met les temps de collisions/sorties de la boîte à jour */
		
		updateCollisions(collisionTimes,collisionTarget,r,v,indices,
			boxDimensions,t);
		updateExits(exitTimes,exitPlanes,r,v,indices,boxDimensions,t);
		
		/* Trouve la prochaine collision */
		
		double tColl; // instant de la prochaine collision
		int i1; // indice de la particule #1 de la prochaine collision
		int i2;
		int iCopy2; // indice de la copie de #2 qui collisionne
		
		nextCollision(collisionTimes,collisionTarget,N,tColl,i1,i2,iCopy2,t);
		
		/* Trouve la prochaine sortie de particule */
		
		double tExit; // instant de la prochaine sortie
		int iPart; // indice de la première particule qui sort
		int exitPlane; // indice du plan par lequel la particule sort
		
		nextExit(exitTimes, exitPlanes, N, tExit, exitPlane, iPart, t);
		
		/* Réalise prochaine sortie/collision */
		
		if (tColl<tExit && tColl>t)
		{
			move(r, v, tColl-t);
			t = tColl;
			collide(r[i1], v[i1], r[i2], v[i2], boxDimensions, iCopy2, t);
			
			indices = {i1,i2}; // indices dont les coll sont à mettre à jour.
		}
		else if (tExit>t)
		{
			move(r, v, tExit-t);
			t = tExit;
			crossBorder(r[iPart], boxDimensions, exitPlane);
			
			indices = {iPart}; // indices dont sorties sont à mettre à jour.
		}
		
		// TODO: calculer la fonction de densité de paires
	} 
	
	return 0;
}