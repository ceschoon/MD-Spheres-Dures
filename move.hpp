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
#include <map>

using std::vector;
using std::map;

/*
 * Liste des copies voisines du système
 */

vector<vector<int>> getCopies();

/*
 * La routine "move" déplace toutes les particules de "t" dans le temps.
 */

void move(vector<vector<double>> &r,
		  vector<vector<double>> v,
		  double t);

/*
 * La routine "updateCollisions" met à jour les temps de collisions impliquant 
 * les particules d'indices compris dans la liste "indices". Ces temps sont 
 * listées dans la map "collisionTimes" reliant une paire de particules de la 
 * liste "r" à l'instant de collision entre ces deux-ci. Si la paire n'est pas
 * encore listée, celle-ci se verra ajoutée à la liste.
 * Cette routine met aussi à jour la copie de la particule 2 qui est la cible 
 * de collision. Cela est enregistré dans la map "collisionTarget", qui associe 
 * à chaque paire un vecteur {i,j,copyj} de 3 composantes contenant les indices
 * de la particule "i" incidente, de la particule cible "j" et de la copie
 * "copyj" de la particule "j" qui collisionne avec "i". 
 */
 
void updateCollisions(map<vector<int>,double,bool(*)(vector<int>,vector<int>)> 
						&collisionTimes,
					  map<vector<int>,vector<int>,bool(*)(vector<int>,
					  	vector<int>)> &collisionTarget,
				 	  vector<vector<double>> r,
				 	  vector<vector<double>> v, 
				 	  vector<int> indices,
				 	  vector<double> boxDimensions,
				 	  double tNow);

/*
 * La routine "nextCollision" recherche la prochaine collision parmi celles
 * listées dans "collisionTimes". 
 */

void nextCollision(map<vector<int>,double,bool(*)(vector<int>,vector<int>)> 
						collisionTimes,
				   map<vector<int>,vector<int>,bool(*)(vector<int>,
					  	vector<int>)> collisionTarget,
				   int N, 			// nombre de particules dans le système
				   double &tColl, 	// output, instant de la prochaine collision
				   int &i1, 		// output, indice particule #1 
				   int &i2, 		// output, indice particule #2
				   int &iCopy2,		// output, indice de la copie de la part #2
				   double tNow);
				 
/*
 * La routine "collide" réalise la collision entre deux particules 
 * Cette routine met à jour les positions et vitesses des deux particules.
 * Elle enregistre également le paramètre r12.v12 de la collision dans le 
 * fichier "data/collisionData.csv"
 */

void collide(vector<double> r1, // position de la particule #1 au présent
			 vector<double> &v1,
			 vector<double> r2,
			 vector<double> &v2,
			 vector<double> boxDimensions,
			 int iCopy2, 
			 double tNow); // pour dater la collision dans le fichier

/*
 * La routine "updateExits" met à jour les temps de sortie de la boîte pour
 * les particules d'indices compris dans la liste "indices". Elle indique 
 * également l'indice du plan par leque sortent les particules.
 */

void updateExits(map<int,double> &exitTimes,
				 map<int,int> &exitPlanes,
				 vector<vector<double>> r,
				 vector<vector<double>> v, 
				 vector<int> indices,
				 vector<double> boxDimensions,
				 double tNow);

/*
 * La routine "nextExit" recherche la prochaine sortie de particule parmi celles
 * listées dans "exitTimes" et "exitPlanes". 
 */

void nextExit(map<int,double> exitTimes,
			  map<int,int> exitPlanes,
			  int N, 			// nombre de particules dans le système
			  double &tExit, 	// output, durée avant prochaine sortie
			  int &exitPlane, 	// output, indice du plan traversé
			  int &iPart, 		// output, indice de la particule qui sort
			  double tNow);

/*
 * La routine "crossBorders" réalise le déplacement d'une particule qui 
 * sortirait de la boîte en traversant le plan "exitPlane" en la remettant sur 
 * le bord opposé à celui-ci.
 */

void crossBorder(vector<double> &r, // pos. actuelle de la part., à modifier
				 vector<double> boxDimensions,
				 int exitPlane); 

#endif // MOVE_HPP