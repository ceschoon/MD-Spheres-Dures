#! /home/cedric/anaconda3/bin/python

################################################################################
#																			   # 
# 	Ce programme permet d'effectuer des simulations en boucle            	   #
#																			   #
#	Utilisation: ./simulate.py num_threads 									   #
# 																			   #
#		num_threads		nombre de threads sur lequel effectuer des simulations #
#																			   #
#	Auteur du programme: Cédric Schoonen									   #
#	Auteur de ce fichier: Cédric Schoonen									   #
#																			   #
################################################################################


import numpy,pandas,time,os,sys,threading

num_threads = 1

if len(sys.argv)>=2:
	num_threads = int(sys.argv[1])

os.system("mkdir data_viriel")
os.system("touch data_viriel/pressures.dat")

def run_simulations(thread_id):

	while (True):
		
		start = time.time()
		
		# Simulation
		
		os.system("./simulationO3 3 3 3 0.0002 1 10000 0 _Thread"+thread_id)
		
		# Calcul de la pression
		
		N = 108
		T = 1
		t_Sim = 10000

		collisionData = pandas.read_csv("data/collisionData_Thread"+thread_id+".csv")
		[t,vDotr] = numpy.transpose(collisionData.as_matrix(['t','vDotr']))

		p = 1 + 1/(N*3*T*t_Sim) * numpy.sum(vDotr)  # mauvaise formule, il faut /(N-1) au lieu de /N -> à corriger dans l'analyse
		
		# Enregistrement
		
		f = open("data_viriel/pressures.dat","a")
		f.write(str(p)+"\n")
		f.close()

for i in range(num_threads):
	threading.Thread(target=run_simulations, args=(str(i),), name="thread"+str(i)).start()
