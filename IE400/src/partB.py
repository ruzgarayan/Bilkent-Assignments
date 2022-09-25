#!/usr/bin/env python
# coding: utf-8

import gurobipy as gp

from gurobipy import GRB
from gurobipy import quicksum

m = gp.Model()

Y = {}
for i in range(1,8):
    Y[i] = m.addVar(vtype=GRB.BINARY, name="Y"+str(i))

I = {}
for i in range(1,8):
    I[i] = m.addVar(vtype=GRB.CONTINUOUS, name="I"+str(i))
    
D = {}
for i in range(1,8):
    D[i] = m.addVar(vtype=GRB.CONTINUOUS, name="D"+str(i))
    
X = {}
for i in range(1,8):
    X[i] = m.addVar(vtype=GRB.CONTINUOUS, name="X"+str(i))

P = {1:1, 2:0, 3:1, 4:0, 5:0, 6:0, 7:0, 8:0, 9:1}
QOL_P_parameters = {1:-5, 2:-0.5, 3:-12, 4:-8, 5:-5, 6:-5, 7:-1, 8:-3, 9:-2}
QOL_Y_parameters = {1:-5, 2:-6, 3:-4, 4:-4, 5:-8, 6:-6, 7:-7}
QOL_X_parameters = {1:0.28, 2:0.30, 3:0.25, 4:0.17, 5:0.31, 6:0.246, 7:0.40}

minDosages = {1:20, 2:10, 3:20, 4:10, 5:10, 6:20, 7:20}
maxDosages = {1:80, 2:50, 3:100, 4:100, 5:70, 6:90, 7:50}
baseRegimenDosages = {1:20, 2:0, 3:30, 4:15, 5:0, 6:0, 7:35}
totalDosageCl = 275

Q_threshold = 35

fixedCosts = {1:25, 2:50, 3:10, 4:25, 5:20, 6:30, 7:40}
unitCosts = {1:1, 2:2, 3:1, 4:3, 5:2, 6:1, 7:1}

isDrugInBaseRegimen = {1:True, 2:False, 3:True, 4:True, 5:False, 6:False, 7:True}
m.setObjective( quicksum([(fixedCosts[i] * (1 - Y[i]) if isDrugInBaseRegimen[i] else fixedCosts[i] * Y[i]) for i in range(1,8)])
               + quicksum([unitCosts[i] * (I[i] + D[i]) for i in range(1,8)])
               , GRB.MINIMIZE)


helperXConstraints = {}
minDrugConstraint = {}
maxDrugConstraint = {}
for i in range(1,8):
    helperXConstraints[i] = m.addConstr(X[i] == baseRegimenDosages[i] + I[i] - D[i])
    minDrugConstraint[i] = m.addConstr(X[i] >= minDosages[i] * Y[i])
    maxDrugConstraint[i] = m.addConstr(X[i] <= maxDosages[i] * Y[i])
    
totalDosageConstraint = m.addConstr(quicksum([X[i] for i in range(1,8)]) == totalDosageCl)

QOFConstraint = m.addConstr(quicksum([QOL_P_parameters[i] * P[i] for i in range(1,10)])
                            + quicksum([QOL_Y_parameters[i] * Y[i] for i in range(1,8)])
                            + quicksum([QOL_X_parameters[i] * X[i] for i in range(1,8)])
                            >= Q_threshold
)


m.optimize()
m.printAttr('X')

