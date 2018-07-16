/****************************************************************************
 *																			*
 *	Fichier	contenant les prototypes des fonctions/routines réalisant 		*
 *		l'évaluation des observables physiques au cours de la simulation.	*
 *																			*
 *	Auteur: Cédric Schoonen													*
 *																			*
 ****************************************************************************/

#ifndef OBSERVABLES_HPP
#define OBSERVABLES_HPP

#include "move.hpp"
#include <vector>

using std::vector;

/*
 * La routine "pairDensity" compte le nombre de paires n(r) de 
 * particules séparées d'une distance comprise dans l'intervalle [r,r+dr[.
 * 
 * Cette routine enregistre l'histogramme de la distribution n(r) dans le 
 * fichier "data/pairDensity.csv".
 */ 
 
void pairDensity(pairsIndDist pairs, 
				 double dMax, // distance max où évaluer n(r)
				 double dr);

/*
 * La fonction "velocityQuadMean" calcule la vitesse quadratique moyenne <v^2>.
 */

double velocityQuadMean(vector<vector<double>> v);

#endif // OBSERVABLES_HPP