#! /home/cedric/anaconda3/bin/python

import numpy,pandas,time,os

os.system("mkdir data_viriel")
os.system("touch data_viriel/pressures.csv")

while(True):
    
    start = time.time()
    
    # Simulation
    
    os.system("./simulationO3 3 3 3 0.0002 1 1000 0 _While")
    
    # Calcul de la pression
    
    N = 108
    T = 1

    collisionData = pandas.read_csv("data/collisionData_While.csv")
    [t,vDotr] = numpy.transpose(collisionData.as_matrix(['t','vDotr']))

    p = 1 + 1/(N*3*T*(t[-1]-t[0])) * numpy.sum(vDotr)
    
    # Enregistrement
    
    f = open("data_viriel/pressures.csv","a")
    f.write(str(p)+"\n")
    f.close()