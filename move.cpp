#include "move.hpp"
#include <math.h>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <assert.h>

using std::vector;
using std::cout;
using std::endl;


/******************************************************************************/

/* Implémentation de la fonction "getCopies" */

vector<vector<int>> getCopies()
{
	vector<vector<int>> copies;
	copies.reserve(27);

	copies.push_back({ 0, 0, 0}); //1

	copies.push_back({ 1, 0, 0}); // copie du système décalée de 1 selon x
	copies.push_back({ 0, 1, 0});
	copies.push_back({ 0, 0, 1});
	copies.push_back({-1, 0, 0});
	copies.push_back({ 0,-1, 0});
	copies.push_back({ 0, 0,-1}); //6

	copies.push_back({ 0, 1, 1});
	copies.push_back({ 1, 0, 1});
	copies.push_back({ 1, 1, 0});
	copies.push_back({ 0, 1,-1});
	copies.push_back({-1, 0, 1});
	copies.push_back({ 1,-1, 0});
	copies.push_back({ 0,-1, 1});
	copies.push_back({ 1, 0,-1});
	copies.push_back({-1, 1, 0});
	copies.push_back({ 0,-1,-1});
	copies.push_back({-1, 0,-1});
	copies.push_back({-1,-1, 0}); //12

	copies.push_back({ 1, 1, 1});
	copies.push_back({-1, 1, 1});
	copies.push_back({ 1,-1, 1});
	copies.push_back({ 1, 1,-1});
	copies.push_back({ 1,-1,-1});
	copies.push_back({-1, 1,-1});
	copies.push_back({-1,-1, 1});
	copies.push_back({-1,-1,-1}); //8

	//------------------------------27 = 1+6+12+8 = 3^3 ok
	
	return copies;
}

/******************************************************************************/

/* Implémentation de la routine "move" */

void move(vector<vector<double>> &r,
		  vector<vector<double>> v,
		  double t)
{
	int N = r.size();
	
	for (int i=0; i<N; i++) 
	{
		for (int j=0; j<3; j++) {r[i][j] = r[i][j] + v[i][j]*t;}
 	}
}

/******************************************************************************/

/* Implémentation de la fonction "collisionTime" */

// implémentation nécessaire du produit scalaire
double dot(vector<double> x, vector<double> y)
{
	return x[0]*y[0]+x[1]*y[1]+x[2]*y[2];
}

double collisionTime(vector<double> r1,
					 vector<double> v1,
					 vector<double> r2,
					 vector<double> v2)
{	
	vector<double> v12(3); // v2-v1
	for (int i=0; i<3; i++) {v12[i] = v2[i]-v1[i];}
	
	vector<double> r12(3); // r2-r1 en t=0
	for (int i=0; i<3; i++) {r12[i] = r2[i]-r1[i];}
	
	/*
	 * Le temps de collision t est défini par le plus petit t tel que
	 * 		
	 *		|r12+t*v12| = d
	 *
	 * avec d=2 le diamètre d'une sphère. Cela se résume à une équation du
	 * second degré a*t^2+b*t+c = 0 dont les coefficients sont:
	 *
	 * 		a = v12.v12
	 *		b = 2 * r12.v12
	 *		c = r12.r12 - d^2
	 */
	
	double a = dot(v12,v12);
	double b = 2*dot(r12,v12);
	double c = dot(r12,r12)-4;
	
	double delta = b*b-4*a*c;
	if (delta<0) {return -1;} // veut dire qu'il n'y pas de collision
	double sqrtDelta = sqrt(delta);
	
	double t1 = (-b + sqrtDelta)/(2*a);
	double t2 = (-b - sqrtDelta)/(2*a);
	
	return (t1<t2)?t1:t2;
}

/******************************************************************************/

/* Implémentation de la routine "updateCollisions" */

void updateCollisions(map<vector<int>,double,bool(*)(vector<int>,vector<int>)>
						&collisionTimes,
					  map<vector<int>,vector<int>,bool(*)(vector<int>,
					  	vector<int>)> &collisionTarget,
					  vector<vector<double>> r, 
					  vector<vector<double>> v,
					  vector<int> indices,
					  vector<double> boxDimensions,
					  double tNow)
{
	int N = r.size();
	
	for (int i: indices) 
	{
		for (int j=0; j<N; j++) 
		{
			if (i!=j)
			{
				/* On calcule les temps de coll avec les copies de la part j */
				
				vector<double> tCollCopies(27,-1);
				vector<vector<int>> copies = getCopies();
				
				for (int k=0; k<27; k++)
				{
					vector<double> rCopy(3,0);
					for (int l=0; l<3; l++) 
						{rCopy[l] = r[j][l]+copies[k][l]*boxDimensions[l];}
					
					tCollCopies[k] = collisionTime(r[i],v[i],rCopy,v[j]);
				}
				
				/* On cherche le premier temps positif */
				
				double tColl = -1; // temps de collision le plus court
				int copyj = 0; // indice de la copie de la part j qui coll.
				int iMaxSearched = 0;
				
				for (int k=0; k<27; k++)
				{
					if (tCollCopies[k]>0) 
					{
						tColl = tCollCopies[k];
						copyj = k;
						iMaxSearched = k;
						break;
					}
				}
				
				/* On cherche s'il n'y a pas plus petit */
				
				for (int k=iMaxSearched+1; k<27; k++)
				{
					if (tCollCopies[k]<tColl && tCollCopies[k]>0) 
					{
						tColl = tCollCopies[k];
						copyj = k;
					}
				}
			
				/* 
				 * Met à jour ou ajoute le temps de coll. dans "collisionTimes"
				 * (-1 si pas de collision), ainsi que la cible de collision 
				 * dans "collisionTarget"
				 */
				
				collisionTimes[{i,j}] = tNow+tColl;
				collisionTarget[{i,j}] = {i,j,copyj};
			}
		}
	}
}

/******************************************************************************/

/* Implémentation de la routine "nextCollision */

void nextCollision(map<vector<int>,double,bool(*)(vector<int>,vector<int>)> 
						collisionTimes,
				   map<vector<int>,vector<int>,bool(*)(vector<int>,
					  	vector<int>)> collisionTarget,
				   int N,
				   double &tColl, 	
				   int &i1, 		
				   int &i2,
				   int &iCopy2,
				   double tNow)
{
	/* Valeurs par défaut, conservées si pas de collision */
	
	tColl = -1;
	i1 = 0;
	i2 = 0;
	iCopy2 = 0;
	
	/* Initialise les valeurs i1,i2,tColl avec la première collision possible */
	
	bool found = false;
	
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<i; j++)
		{
			double tCollij = collisionTimes[{i,j}];
			// tColl = -1 si pas de collision
			
			if (tCollij>tNow)
			{
				tColl = tCollij;
				i1 = collisionTarget[{i,j}][0];
				i2 = collisionTarget[{i,j}][1];
				iCopy2 = collisionTarget[{i,j}][2];
				
				found = true;
			}
		}
		if (found==true) {break;}
	}
	
	/* Parcoure les autres collisions pour en trouver une plus proche */
	
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<i; j++)
		{
			double tCollij = collisionTimes[{i,j}];
			
			if (tCollij<tColl && tCollij>tNow)
			{
				tColl = tCollij;
				i1 = collisionTarget[{i,j}][0];
				i2 = collisionTarget[{i,j}][1];
				iCopy2 = collisionTarget[{i,j}][2];
			}
		}
	}
}

/******************************************************************************/

/* Implémentation de la routine "collide" */

/*
 * La routine "collide" modifie les vitesses de deux particules étant entrées
 * en collision.
 *
 * On a V1 = v1 + proj et V2 = v2 + proj, avec proj la projection de v2-v1 
 * sur r2-r1, Vi désignant les nouvelles vitesses
 */

void collide(vector<double> r1, 
			 vector<double> &v1,
			 vector<double> r2,
			 vector<double> &v2,
			 vector<double> boxDimensions,
			 int iCopy2,
			 double tNow,
			 string suffix) // instant présent
{	
	/* Calcul de la position de la copie de la particule cible */
	
	vector<vector<int>> copies = getCopies();
	vector<int> copy = copies[iCopy2];
	
	vector<double> r2Copy(3,0);
	for (int i=0; i<3; i++) {r2Copy[i] = r2[i] + copy[i] * boxDimensions[i];}
	
	/* Calcul des nouvelles vitesses */
	
	vector<double> v12(3); // v2-v1
	for (int i=0; i<3; i++) {v12[i] = v2[i]-v1[i];}
	
	vector<double> r12(3); // r2Copy-r1
	for (int i=0; i<3; i++) {r12[i] = r2Copy[i]-r1[i];}
	
	double vDotr = dot(v12, r12);
	double lambda = vDotr/dot(r12,r12); 
	
	for (int i=0; i<3; i++) {v1[i] = v1[i] + lambda*r12[i];}
	for (int i=0; i<3; i++) {v2[i] = v2[i] - lambda*r12[i];}
	
	/* Enregistre abs(v12.r12) pour le calcul de la pression */
	
	std::ofstream file("data/collisionData"+suffix+".csv", std::ios_base::app);
	if (file)
	{
		file << tNow << ", " << abs(vDotr) << endl;
	}
	else { cout << "Unable to open file \"data/collisionData"+suffix+".csv\"" << endl;}
	
	file.close();
}

/******************************************************************************/

/* Implémentation de la routine "updateExits" */

void updateExits(map<int,double> &exitTimes,
				 map<int,int> &exitPlanes,
				 vector<vector<double>> r,
				 vector<vector<double>> v, 
				 vector<int> indices,
				 vector<double> boxDimensions,
				 double tNow)
{	
	double lx = boxDimensions[0];
	double ly = boxDimensions[1];
	double lz = boxDimensions[2];
	
	vector<double> tExits(6,0);
	
	for (int i: indices) 
	{
		/* Cherche quand intersecte les plans x=lx, x=0, y=ly, ... */
		
		tExits[0] = (lx-r[i][0])/v[i][0];
		tExits[1] = ( 0-r[i][0])/v[i][0];
		tExits[2] = (ly-r[i][1])/v[i][1];
		tExits[3] = ( 0-r[i][1])/v[i][1];
		tExits[4] = (lz-r[i][2])/v[i][2];
		tExits[5] = ( 0-r[i][2])/v[i][2];
		
		/* Cherche la première valeur non-négative */
		
		double tExit = -1;
		int exitPlane = 0;
		int iMaxSearched = 0;
		
		for (int j=0; j<6; j++)
		{	 
			if (tExits[j]>0)
			{
				tExit = tExits[j];
				exitPlane = j;
				iMaxSearched = j;
				break;
			}
		}
		
		/* Cherche s'il n'y a pas plus petit */
		
		for (int j=iMaxSearched+1; j<6; j++)
		{
			/*
			 * /!\ Risque de boucle infinie /!\
			 * Comme la particule est replacée sur un bord, elle pourrait par 
			 * la suite être détectée comme traversant avec celui-ci.
			 */
			 
			if (tExits[j]<tExit && tExits[j]>0)
			{
				tExit = tExits[j];
				exitPlane = j;
			}
		}
		
		/* Enregistre le temps minimum non-négatif */
		
		exitTimes[i] = tNow+tExit; // met à jour ou ajoute
		exitPlanes[i] = exitPlane; 
	}
}

/******************************************************************************/

/* Implémentation de la routine "nextExit" */

void nextExit(map<int,double> exitTimes,
			  map<int,int> exitPlanes,
			  int N,
			  double &tExit, 
			  int &exitPlane, 
			  int &iPart,
			  double tNow)
{
	/* Initialise les valeurs tExit,... avec la première sortie possible */
	
	int iMaxSearched = 0;
	
	for (int i=0; i<N; i++)
	{
		double tExiti = exitTimes[i];
		
		if (tExiti>tNow)
		{
			tExit = tExiti;
			exitPlane = exitPlanes[i];
			iPart = i;
			iMaxSearched = i;
			break;
		}
	}
	
	/* Parcoure les autres sorties pour en trouver une plus proche */
	
	for (int i=iMaxSearched+1; i<N; i++)
	{
		double tExiti = exitTimes[i];
		
		if (tExiti<tExit && tExiti>tNow) 
		{
			tExit = tExiti;
			exitPlane = exitPlanes[i];
			iPart = i;
		}
	}
}

/******************************************************************************/

/* Implémentation de la routine "crossBorders" */

void crossBorder(vector<double> &r, 
				 vector<double> boxDimensions,
				 int exitPlane) 
{
	double lx = boxDimensions[0];
	double ly = boxDimensions[1];
	double lz = boxDimensions[2];
	
	/* Déplace la particule suivant le cas */
	
	
	if      (exitPlane==0) {r[0] = 0;}
	else if (exitPlane==1) {r[0] = lx;}
	else if (exitPlane==2) {r[1] = 0;}
	else if (exitPlane==3) {r[1] = ly;}
	else if (exitPlane==4) {r[2] = 0;}
	else if (exitPlane==5) {r[2] = lz;}
}
