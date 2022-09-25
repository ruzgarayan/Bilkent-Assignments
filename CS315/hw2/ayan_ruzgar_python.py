
#Question 1.
boolTrue = True;
boolFalse = False;
negNumber = -1.1;
zeroNumber = 0;
posNumber = 1.1;
emptyString = "";
nonemptyString = "abc";
#undefinedVar; not possible in Python
nullVar = None;
empty_array = [];
nonempty_array = ["abc", 1, 2];

if (boolTrue) :
    print("boolTrue") #Prints
if (boolFalse) :
    print("boolFalse")
if (negNumber) :
    print("negNumber") #Prints
if (zeroNumber) :
    print("zeroNumber")
if (posNumber) :
    print("posNumber") #Prints
if (emptyString) :
    print("emptyString")
if (nonemptyString) :
    print("nonemptyString") #Prints
if (nullVar) :
    print("nullVar")
if (empty_array) :
    print("empty_array")
if (nonempty_array) :
    print("nonempty_array") #Prints

#Question 2.
print("\n")
def printMsg(msg):
    print(msg)
    return True

and_ = (False and printMsg("and"))
or_ = (True or printMsg("or"))
bitwiseAnd = (0 & printMsg("bitwiseAnd")) #Prints
bitwiseOr = (~0 | printMsg("bitwiseOr")) #Prints
multiplication = (0 * 5 * printMsg("multiplication")) #Prints

#Question 3.
print("\n")
#Prints a and b
shortcircuit = printMsg("a") and printMsg("b") or printMsg("c")
#Prints a and b
shortcircuit2 = False or printMsg("a") and (printMsg("b") or printMsg("c"))

#Question 4.
print("\n")

def expensiveComputation(name): 
    sum = 0
    for i in range(10000):
        sum = sum + i
    print("Did expensive computation at " + name)
    return sum

smallNumber = 1;
#With short-circuit
if smallNumber > 10 and expensiveComputation("if1") > 10000:
    print("Inside if 1")

#Simulating the if above without short-circuit
if smallNumber > 10:
    if expensiveComputation("if2") > 10000:
        print("Inside if 2");

#Not doing short-circuit evaluation because of wrong order
#Prints "Did expensive computation at if3" only for this one
if expensiveComputation("if3") > 10000 and smallNumber > 10:
    print("Inside if 3")

#Question 5.
print("\n")

globalVariable = "Initial value";
def sideEffectComputation(): 
    global globalVariable
    globalVariable = "I have been changed"
    return True

var1 = smallNumber > 10 and sideEffectComputation()
print(globalVariable) #Prints "Initial value"
var2 = sideEffectComputation() and smallNumber > 10
print(globalVariable) #Prints "I have been changed"