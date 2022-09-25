<?php //php 7.2.24

    //Question 1.
    $boolTrue = True;
    $boolFalse = False;
    $negNumber = -1.1;
    $zeroNumber = 0;
    $posNumber = 1.1;
    $emptyString = "";
    $nonemptyString = "abc";
    $undefinedVar;
    $nullVar = NULL;
    $empty_array = [];
    $nonempty_array = ["abc", 1, 2];

    if ($boolTrue) echo "boolTrue\n"; //Prints
    if ($boolFalse) echo "boolTrue\n";
    if ($negNumber) echo "negNumber\n"; //Prints
    if ($zeroNumber) echo "zeroNumber\n";
    if ($posNumber) echo "posNumber\n"; //Prints
    if ($emptyString) echo "emptyString\n";
    if ($nonemptyString) echo "nonemptyString\n"; //Prints
    if ($undefinedVar) echo "undefinedVar\n";
    if ($nullVar) echo "nullVar\n";
    if ($empty_array) echo "empty_array\n";
    if ($nonempty_array) echo "nonempty_array\n"; //Prints

    //Question 2.
    echo "\n\n";

    function printMsg($msg) {
        echo "$msg \n";
        return True;
    }

    $and_ = (False && printMsg("and"));
    $and2_ = (False and printMsg("and2"));
    $or_ = (True || printMsg("or"));
    $or2_ = (True or printMsg("or2"));
    $bitwiseAnd = (0 & printMsg("bitwiseAnd")); //Prints
    $bitwiseOr = (~0 | printMsg("bitwiseOr")); //Prints
    $multiplication = (0 * 5 * printMsg("multiplication")); //Prints

    //Question 3.
    echo "\n\n";

    //Prints a and b
    $shortcircuit = printMsg("a") && printMsg("b") || printMsg("c");
    //Prints a and b
    $shortcircuit2 = False || printMsg("a") && (printMsg("b") || printMsg("c"));

    //Question 4.
    echo "\n\n";

    function expensiveComputation($name) { 
        $sum = 0;  
        for ($i = 0; $i <= 10000; $i++) {
            $sum += $i;    
        }
        echo "Did expensive computation at $name \n";
        return $sum;
    }

    $smallNumber = 1;
    //With short-circuit
    if ($smallNumber > 10 && expensiveComputation("if1") > 10000) {
        echo "Inside if 1\n";
    }

    //Simulating the if above without short-circuit
    if ($smallNumber > 10) {
        if (expensiveComputation("if2") > 10000) {
          echo "Inside if 2\n";
        }
    }

    //Not doing short-circuit evaluation because of wrong order
    //Prints "Did expensive computation at if3" only for this one
    if (expensiveComputation("if3") > 10000 && $smallNumber > 10) {
        echo "Inside if 3\n";
    }

    //Question 5.
    echo "\n\n";

    $globalVariable = "Initial value";
    function sideEffectComputation() { 
        global $globalVariable;
        $globalVariable = "I have been changed";
        return True;
    }
    $var1 = $smallNumber > 10 && sideEffectComputation();
    echo "$globalVariable \n"; //Prints "Initial value"
    $var2 = sideEffectComputation() && $smallNumber > 10;
    echo "$globalVariable \n"; //Prints "I have been changed"

?>