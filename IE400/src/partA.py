#!/usr/bin/env python
# coding: utf-8

import gurobipy as gp

from gurobipy import GRB
from gurobipy import quicksum

m = gp.Model()

X = {}
for i in range(1,8):
    X[i] = m.addVar(vtype=GRB.CONTINUOUS, name="X"+str(i))

P = {1:1, 2:0, 3:1, 4:0, 5:0, 6:0, 7:0, 8:0, 9:1}
Y = {1:1, 2:0, 3:1, 4:1, 5:0, 6:0, 7:1}
minDosages = {1:20, 2:10, 3:20, 4:10, 5:10, 6:20, 7:20}
maxDosages = {1:80, 2:50, 3:100, 4:100, 5:70, 6:90, 7:50}
totalDosageCl = 100

m.setObjective(5*-P[1] + 0.5*-P[2] + 12*-P[3] + 8*-P[4] + 5*-P[5] + 5*-P[6] + 1*-P[7] + 3*-P[8] + 2*-P[9]
               + 5*-Y[1] + 6*-Y[2] + 4*-Y[3] + 4*-Y[4] + 8*-Y[5] + 6*-Y[6] + 7*-Y[7]
               + 0.28*X[1] + 0.30*X[2] + 0.25*X[3] + 0.17*X[4] + 0.31*X[5] + 0.246*X[6] + 0.40*X[7]
               , GRB.MAXIMIZE)

fixX2 = m.addConstr(X[2] == 0)
fixX5 = m.addConstr(X[5] == 0)
fixX6 = m.addConstr(X[6] == 0)

minX1 = m.addConstr(X[1] >= minDosages[1])
maxX1 = m.addConstr(X[1] <= maxDosages[1])

minX3 = m.addConstr(X[3] >= minDosages[3])
maxX3 = m.addConstr(X[3] <= maxDosages[3])

minX4 = m.addConstr(X[4] >= minDosages[4])
maxX4 = m.addConstr(X[4] <= maxDosages[4])

minX7 = m.addConstr(X[7] >= minDosages[7])
maxX7 = m.addConstr(X[7] <= maxDosages[7])

totalDosage = m.addConstr(quicksum([X[i] for i in range(1,8)]) == totalDosageCl)

m.optimize()
m.printAttr('X')

