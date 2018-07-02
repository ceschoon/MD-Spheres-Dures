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
 * La routine "move" avance le système de particules de "dt" dans le temps
 * en recalculant d'abord les vitesses si il y a collision entre les 
 * particules (chevauchement).	
 */ 
 
void move(	vector<vector<double>> &r,
			vector<vector<double>> &v,
			vector<double> boxDimensions,
			double dt);

#endif // MOVE_HPP