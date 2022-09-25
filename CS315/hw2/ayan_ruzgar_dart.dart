int globalInt = 1;

bool printBool(String input) { 
    print("Printed at " + input + "!"); return true;
}
int printInt(String input) { 
    print("Printed at " + input + "!"); return 1;
}
int expensiveComputation(String name) { 
  int sum = 0;  
  for (int i = 0; i < 10000; i++) {
    sum += i;    
  }
  print("Did expensive computation at " + name);
  return sum;
}
bool sideEffectComputation() { 
  globalInt = 55;
  return true;
}

void main() {
  
  //Question 1.

  bool boolTrue = true;
  bool boolFalse = false;
  double negNumber = -1.1;
  int zeroNumber = 0;
  String emptyString = "";
  String nonemptyString = "abc";
  var undefinedVar;
  var nullVar = null;

  if (boolTrue) print("boolTrue"); //Prints
  if (boolFalse) print("boolFalse");
  
  //Error: A value of type '***' can't be assigned to a variable of type 'bool'
  /*
  if (negNumber) print("negNumber");
  if (zeroNumber) print("zeroNumber");
  if (emptyString) print("emptyString");
  if (nonemptyString) print("nonemptyString");
  */

  //Uncaught Error: TypeError: null: type 'JSNull' is not a subtype of type 'bool'
  //if (nullVar) print("nullVar");  
  //if (undefinedVar) print("undefinedVar");

 
  //Question 2.
  print("\n\n");
  
  bool and = (false && printBool("and"));
  bool or = (true || printBool("or"));
  int bitwiseAnd = (0 & printInt("bitwiseAnd")); //Prints
  int bitwiseOr = (~0 | printInt("bitwiseOr")); //Prints
  int multiplication = (0 * 5 * printInt("multiplication")); //Prints
  
  
  //Question 3.
  print("\n\n");
  
  //Prints a and b
  bool shortcircuit = printBool("a") && printBool("b") || printBool("c");
  //Prints a and b
  bool shortcircuit2 = false || printBool("a") && (printBool("b") || printBool("c"));
  
  //Question 4.
  print("\n\n");
  
  var smallNumber = 1;
  //With short-circuit
  if (smallNumber > 10 && expensiveComputation("if1") > 10000) {
    print("Inside if 1");
  }

  //Simulating the if above without short-circuit
  if (smallNumber > 10) {
    if (expensiveComputation("if2") > 10000) {
      print("Inside if 2");
    }
  }

  //Not doing short-circuit evaluation because of wrong order
  //Prints "Did expensive computation at if3" only for this one
  if (expensiveComputation("if3") > 10000 && smallNumber > 10) {
    print("Inside if 3");
  }
  
  
  //Question 5.
  print("\n\n");
  
  var var1 = smallNumber > 10 && sideEffectComputation();
  print(globalInt); //Prints 1, not changed
  var var2 = sideEffectComputation() && smallNumber > 10;
  print(globalInt); //Prints 55, changed this time

}
