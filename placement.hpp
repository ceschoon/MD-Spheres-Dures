/****************************************************************************
 									    									
 	Fichier	contenant les prototypes des fonctions/routines plaçant sphères 	  
    dures dans un réseau cubique faces centrées et donnant aux noeuds
	une vitesse initiale distribuée selon une Maxwellienne.				    					
 																			
 	Auteur: Maxime Jamotte						    						
 																			
 ****************************************************************************/


#ifndef PLACEMENT_HPP
#define PLACEMENT_HPP

#include <vector>

using std::vector;
/* 
 * 
 *
 */

vector<vector<double>> placementR(vector<int> sizes, double a);




vector<vector<double>> placementV(int N, double T);

#endif // PLACEMENT_HPP 