#include "move.hpp"
#include <math.h>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;

double dot(vector<double> x, vector<double> y)
{
	return x[0]*y[0]+x[1]*y[1]+x[2]*y[2];
}

/*
 * La fonction "collide" renvoie la vitesse V1 d'une particule située en r1,
 * initialement à une vitesse v1, après collision avec une autre particule 
 * située en r2, initialement à une vitesse v2.
 *
 * On a V1 = v1 + proj , avec proj la projection de v2-v1 sur r2-r1
 */

vector<double> collide(	vector<double> v1, vector<double> v2,
					  	vector<double> r1, vector<double> r2)
{	
	vector<double> r12(3,0); // r2-r1
	for (int i=0; i<3; i++) {r12[i] = r2[i]-r1[i];}
	
	vector<double> v12(3,0); // v2-v1
	for (int i=0; i<3; i++) {v12[i] = v2[i]-v1[i];}
	
	double lambda = dot(v12, r12)/dot(r12,r12); 
	
	vector<double> V1(3,0);
	for (int i=0; i<3; i++) {V1[i] = v1[i] + lambda*r12[i];}
	
	return V1;
}

/*
 * La routine "move" déplace d'abord toutes les particules selon 
 * r(t+dt) = r(t) + v(t)*dt, en tenant compte de la périodicité.
 * Ensuite, les vitesses sont recalculées pour les particules subissant une
 * collision. 
 */
 
void move(	vector<vector<double>> &r,
			vector<vector<double>> &v,
			vector<double> boxDimensions,
			double dt)
{	
	int N = r.size(); // nombre de particules
	int lx = boxDimensions[0];
	int ly = boxDimensions[1];
	int lz = boxDimensions[2];
	
	/* Traitement des collisions */
	
	/* 
	 * On commence par créer une liste des particules pouvant collisionner 
	 * avec une particule donnée de notre système. Étant donné la périodicité,
	 * une particules peut entrer en collision avec n'importe quelle 
	 * particule se trouvant dans une des 27 copies voisines de notre système.
	 */	
	 
	vector<vector<double>> r2; r2.clear();
	vector<vector<double>> copies; copies.clear(); 
	
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
	 * Remarque: Si il y a des collision à plus de 2 corps, l'ordre de 
	 * traitement des collisions sera fixé par l'ordre d'apparition des
	 * particules dans les vecteurs r et v. Le véritable ordre physique des
	 * collisions est ignoré.
	 */ 
	
	for (int i=0; i<N; i++)
	{
		for (int j=i+1; j<27*N; j++)
		{
			double d = sqrt( pow(r[i][0]-r2[j][0],2)
							+pow(r[i][1]-r2[j][1],2)
							+pow(r[i][2]-r2[j][2],2));				
			
			if (d < 2)
			{	
				v[i] = collide(v[i],v[j],r[i],r2[j]);
				v[j] = collide(v[j],v[i],r2[j],r[i]);
			}
		}
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