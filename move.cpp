#include "move.hpp"
#include <math.h>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>

using std::vector;
using std::cout;
using std::endl;

/******************************************************************************/

/* 
 * Morceau de code servant à trier un vecteur tout en conservant les indices.
 * Trouvé à l'adresse https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
 */
 
template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

  // initialize original index locations
  vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  std::sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}

/******************************************************************************/

/* Implémentation de la fonction "pairList" */
 
pairsIndDist pairList(vector<vector<double>> r,
					  vector<double> boxDimensions,
					  double dMax)
{	
	int N = r.size();
	int lx = boxDimensions[0];
	int ly = boxDimensions[1];
	int lz = boxDimensions[2];
	
	vector<int> indicesPart1, indicesPart1Sorted;
	vector<int> indicesPart2, indicesPart2Sorted;
	vector<double> distances, distancesSorted;
	
	indicesPart1.reserve(N);
	indicesPart2.reserve(N);
	distances.reserve(N);
	indicesPart1Sorted.reserve(N);
	indicesPart2Sorted.reserve(N);
	distancesSorted.reserve(N);
	
	/* 
	 * On commence par créer une liste des particules pouvant collisionner 
	 * avec une particule donnée de notre système. Étant donné la périodicité,
	 * une particules peut entrer en collision avec n'importe quelle 
	 * particule se trouvant dans une des 27 copies voisines de notre système.
	 */	
	 
	vector<vector<double>> r2;
	vector<vector<double>> copies;
	
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
	
	for (int i=0; i<27; i++)
	{
		for (int j=0; j<N; j++)
		{
			r2.push_back({r[j][0]+copies[i][0]*lx,
						  r[j][1]+copies[i][1]*ly,
						  r[j][2]+copies[i][2]*lz});
		}
	}
	
	/* 
	 * On collecte maintenant les informations voulues pour chaque paire
	 * de séparation d <= dMax.
	 */
	
	for (int i=0; i<N; i++)
	{
		for (int j=i+1; j<27*N; j++)
		{
			double d = sqrt( pow(r[i][0]-r2[j][0],2)
							+pow(r[i][1]-r2[j][1],2)
							+pow(r[i][2]-r2[j][2],2));				
			
			if (d <= dMax) 
			{	
				indicesPart1.push_back(i);
				indicesPart2.push_back(j%N); // modulo pour copie (0,0,0)
				distances.push_back(d);
			}
		}
	}
	
	/* Tri des paires selon la distance */
	
	for (auto i: sort_indexes(distances)) 
	{
		indicesPart1Sorted.push_back(indicesPart1[i]);
		indicesPart2Sorted.push_back(indicesPart2[i]);
		distancesSorted.push_back(distances[i]);
	}
	
	/*pairsIndDist pairs;
	pairs.indiciesPart1 = indiciesPart1;
	pairs.indiciesPart2 = indiciesPart2;
	pairs.distances = distances;*/
	
	return {indicesPart1Sorted, indicesPart2Sorted, distancesSorted};
}

/******************************************************************************/

/* Implémentation de la routine "collide", nécessaire à la routine "move" */

double dot(vector<double> x, vector<double> y)
{
	return x[0]*y[0]+x[1]*y[1]+x[2]*y[2];
}

/*
 * La routine "collide" modifie les vitesses de deux particules étant entrées
 * en collision.
 *
 * On a V1 = v1 + proj et V2 = v2 + proj, avec proj la projection de v2-v1 
 * sur r2-r1, Vi désignant les nouvelles vitesses
 */

void collide(vector<double> &v1, vector<double> &v2,
			 vector<double> &r1, vector<double> &r2,
			 double t) // pour dater l'enregistrement de vij.rij
{	
	/* Calcul des nouvelles vitesses */
	
	vector<double> r12(3); // r2-r1
	for (int i=0; i<3; i++) {r12[i] = r2[i]-r1[i];}
	
	vector<double> v12(3); // v2-v1
	for (int i=0; i<3; i++) {v12[i] = v2[i]-v1[i];}
	
	double vDotr = dot(v12, r12);
	double lambda = vDotr/dot(r12,r12); 
	
	for (int i=0; i<3; i++) {v1[i] = v1[i] + lambda*r12[i];}
	for (int i=0; i<3; i++) {v2[i] = v2[i] - lambda*r12[i];}
	
	/* Enregistre vij.rij pour le calcul de la pression */
	
	std::ofstream file("data/collisionData.dat", std::ios_base::app);
	if (file)
	{
		file << t << ", " << vDotr << endl;
	}
	else { cout << "Unable to open file \"data/collisionData.dat\"" << endl;}
	
	file.close();
}

/******************************************************************************/

/* Implémentation de la routine "move" */

/*
 * La routine "move" recalcule les vitesses pour les particules subissant une
 * collision. Ensuite, elle déplace toutes les particules selon r(t+dt) = 
 * r(t) + v(t)*dt, en tenant compte de la périodicité.
 */
 
void move(	vector<vector<double>> &r,
			vector<vector<double>> &v,
			vector<double> boxDimensions,
			pairsIndDist pairs,
			double dt,
			double t)
{	
	int N = r.size(); // nombre de particules
	
	/* Traitement des collisions */
	
	/*
	 * On cherche les paires qui collisionnent et on traite
	 * la collision dans ce cas.
	 * 
	 * Remarque: Le véritable ordre physique des collisions à plus de 2 corps
	 * n'est pas respecté.
	 */ 
	
	for (int i=0; i<pairs.distances.size(); i++)
	{
		double d = pairs.distances[i];
		
		if (d < 2) // chevauchement => collision
		{
			int i1 = pairs.indicesPart1[i];
			int i2 = pairs.indicesPart2[i]; 
			
			collide(v[i1],v[i2],r[i1],r[i2],t);
		}
		else {break;}
	}
	
	/* Déplacement des particules */
	
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<3; j++)
		{
			r[i][j] = r[i][j] + dt*v[i][j];
			
			if (r[i][j] < 0) 
				{r[i][j] = r[i][j] + boxDimensions[j];}
			if (r[i][j] >= boxDimensions[j]) 
				{r[i][j] = r[i][j] - boxDimensions[j];}
		}
	}
}