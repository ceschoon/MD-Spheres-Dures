/****************************************************************************
 *																			*
 *	Fichier	contenant les prototypes des fonctions/routines réalisant 		*
 *		l'évolution temporelle d'un système de N sphères dures aux 			*
 * 		positions r et vitesses v.											*
 *																			*
 *	Auteur: Cédric Schoonen													*
 *																			*
 ****************************************************************************/

#ifndef MOVE_HPP
#define MOVE_HPP

#include <vector>

using std::vector;

/*
 * La fonction "pairList" renvoie une structure contenant des informations
 * (distance, indices des particules) sur chaque paire de particules de la
 * liste "r" séparées d'une distance d'au plus "dMax".
 * 
 * Les listes de la structure "pairsIndDist" sont triées selon la distance
 * entre les particules, par ordre croissant.
 */
 
struct pairsIndDist
{
	vector<int> indicesPart1; // indice de la particule #1 de la paire
	vector<int> indicesPart2;
	vector<int> indicesCopyPart2; // indice de la copie du système dans
								  // laquelle se trouve la particule #2
								  // qui collisionne avec la particule #1.
	vector<double> distances; // distance séparant les deux particules
};
 
pairsIndDist pairList(vector<vector<double>> r,
					  vector<double> boxDimensions,
					  double dMax);

/*
 * La routine "move" avance le système de particules de "dt" dans le temps
 * en recalculant d'abord les vitesses si il y a collision entre les 
 * particules (chevauchement). 
 * 
 * Cette routine enregistre également les grandeurs suivantes: 
 * 	-	Le produit vij.rij pour chaque collision. Cette 
 * 		variable est écrite dans le fichier "data/collisionData.csv"
 */ 
 
void move(	vector<vector<double>> &r,
			vector<vector<double>> &v,
			vector<double> boxDimensions,
			pairsIndDist pairs,
			double dt,
			double t); // temps avant +dt

#endif // MOVE_HPP