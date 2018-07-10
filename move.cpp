#include "move.hpp"
#include "utilities.hpp"
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
 * Liste des copies voisines du système
 */

vector<vector<double>> getCopies()
{
	vector<vector<double>> copies;
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

/* 
 * Morceau de code servant à trier un vecteur tout en conservant les indices.
 * Trouvé à l'adresse https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
 */
 
template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) 
{

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
	double lx = boxDimensions[0];
	double ly = boxDimensions[1];
	double lz = boxDimensions[2];
	
	vector<int> indicesPart1, indicesPart1Unique, indicesPart1Sorted;
	vector<int> indicesPart2, indicesPart2Unique, indicesPart2Sorted;
	vector<int> indicesCopyPart2, indicesCopyPart2Unique,
				indicesCopyPart2Sorted;
	vector<double> distances, distancesUnique, distancesSorted;
	
	/* 
	 * On commence par créer une liste des particules pouvant collisionner 
	 * avec une particule donnée de notre système. Étant donné la périodicité,
	 * une particules peut entrer en collision avec n'importe quelle 
	 * particule se trouvant dans une des 27 copies voisines de notre système.
	 */	
	
	vector<vector<double>> copies = getCopies();
	
	vector<vector<double>> r2;
	r2.reserve(N*27);
	
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
	 * de séparation d < dMax.
	 */
	
	for (int i=0; i<N; i++)
	{
		for (int j=i+1; j<27*N; j++)
		{
			double d = sqrt( pow(r[i][0]-r2[j][0],2)
							+pow(r[i][1]-r2[j][1],2)
							+pow(r[i][2]-r2[j][2],2));
			
			if (d < dMax) 
			{	
				indicesPart1.push_back(i);
				indicesPart2.push_back(j%N); // % pour indice ds copie (0,0,0)
				indicesCopyPart2.push_back(j/N); // / pour indice de la copie
				distances.push_back(d);
			}
		}
	}
	
	/* Tri des paires selon la distance */
	
	for (auto i: sort_indexes(distances)) 
	{
		indicesPart1Sorted.push_back(indicesPart1[i]);
		indicesPart2Sorted.push_back(indicesPart2[i]);
		indicesCopyPart2Sorted.push_back(indicesCopyPart2[i]);
		distancesSorted.push_back(distances[i]);
	}
	
	/* Retire les paires comptées plusieurs fois */
	
	int M = indicesPart1Sorted.size(); // nombre de paires
	vector<int> idx; // indices des paires en trop
	
	for (int i=0; i<M; i++)
	{
		for (int j=0; j<i; j++)
		{	
			// on retire la paire i si elle est équivalente à la paire j
			if ((indicesPart1Sorted[j]==indicesPart1Sorted[i]) &&
				(indicesPart2Sorted[j]==indicesPart2Sorted[i]))
			{
				idx.push_back(i);
				break;
			}
			else if ((indicesPart1Sorted[j]==indicesPart2Sorted[i]) &&
					 (indicesPart2Sorted[j]==indicesPart1Sorted[i]))
			{
				idx.push_back(i);
				break;
			}
			else {;}
		}
	}
	
	/*
	 * Ajoute la paire aux listes indicesPart*Unique, distancesUnique si 
	 * elle n'est pas dans la liste "idx" des paires excédentaires.
	 */
	 
	for (int i=0; i<M; i++)
	{
		if (std::find(idx.begin(), idx.end(), i) == idx.end()) // i n'est pas 
		 													   // dans idx
		{
			indicesPart1Unique.push_back(indicesPart1Sorted[i]);
			indicesPart2Unique.push_back(indicesPart2Sorted[i]);
			indicesCopyPart2Unique.push_back(indicesCopyPart2Sorted[i]);
			distancesUnique.push_back(distancesSorted[i]);
		}
	}
	
	return {indicesPart1Unique, indicesPart2Unique, indicesCopyPart2Unique, 
			distancesUnique};
}

/******************************************************************************/

/* Implémentation de la routine "collide", nécessaire à la routine "move" */

// implémentation nécessaire du produit scalaire
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
	
	vector<vector<double>> copies = getCopies();
	
	for (int i=0; i<pairs.distances.size(); i++)
	{
		double d = pairs.distances[i];
		
		if (d < 2) // chevauchement => collision
		{	
			int i1 = pairs.indicesPart1[i];
			int i2 = pairs.indicesPart2[i];
			int idxCopy = pairs.indicesCopyPart2[i];
			
			/*
			 * Remarque: Une particule entrera généralement en collision avec
			 * une copie d'une autre particule, il faut donc donner à la 
			 * routine "collide" les coordonnées de la bonne copie de i2.
			 * L'indice de la copie de i2 la plus proche de i1 est stockée
			 * dans "pairs.indicesCopyPart2".
			 */
			
			vector<double> r2(3); // position de la copie de la particule #2
			
			r2[0] = r[i2][0] + copies[idxCopy][0]*boxDimensions[0];
			r2[1] = r[i2][1] + copies[idxCopy][1]*boxDimensions[1];
			r2[2] = r[i2][2] + copies[idxCopy][2]*boxDimensions[2];
			
			collide(v[i1],v[i2],r[i1],r2,t);
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
			else if (r[i][j] >= boxDimensions[j]) 
				{r[i][j] = r[i][j] - boxDimensions[j];}
			else {;}
		}
	}
}