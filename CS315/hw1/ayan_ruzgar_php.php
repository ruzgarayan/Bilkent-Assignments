<?php //php 7.2.24

    echo "Question 1. What types are legal for subscripts?";
    $mixed_array = ["Spring", "Winter", "Fall", "Summer", 2.2 => "Double index", "xyz" => "abc", true => "false"];
    print_r($mixed_array);
    echo "mixed_array[2] = $mixed_array[2]\n";
    echo "mixed_array['xyz'] = "; echo $mixed_array["xyz"]; echo "\n";

    //All below gives Undefined index error.
    //echo "mixed_array[true] = $mixed_array[true]";
    //echo "mixed_array[1.2] = $mixed_array[1.2]";

    echo "Question 2. Are subscripting expressions in element references range checked?\n";

    //PHP Notice:  Undefined offset: 10 in 762760953/source.php on line 14
    echo "mixed_array[10] = $mixed_array[10]\n";
    //PHP Notice:  Undefined index: xyz2 in 1330870507/source.php on line 19
    echo "mixed_array['xyz2'] = "; echo $mixed_array["xyz2"]; echo "\n";

    echo "Question 3. When are subscript ranges bound?\n";
    $mixed_array[10] = "New Element";
    echo "mixed_array[10] = $mixed_array[10]\n";

    echo "Question 4. When does allocation take place?\n";

    echo "Question 5. Are ragged or rectangular multidimensional arrays allowed, or both?\n";
    $multidim_array = [[1,2,3],[4,5]];
    $multidim_array2 = [[1,2,3],[4,5,6]];
    print_r($multidim_array);
    print_r($multidim_array2);

    echo "Question 6. Can array objects be initialized?\n";
    $initialized_arr = ["banana", "orange"];
    print_r($initialized_arr);
    $initialized_arr2 = array_fill(100, 10, "fruit");
    print_r($initialized_arr2); //Keys 100..109 have the value "fruit"
    

    echo "Question 7. Are any kind of slices supported?\n";
    //Reference https://www.php.net/manual/en/function.array-slice.php
    $original_array = [1,2,3,4,5];
    $sliced_array = array_slice($original_array, 2, 2, true);
    echo "sliced array ";
    print_r($sliced_array);
    $sliced_array[0] = "Modified element";
    echo "original array after modifying its slice ";
    print_r($original_array); //Doesn't change, array_slice just creates a copy

    echo "Question 8. Which operators are provided?\n";
    //Reference https://www.php.net/manual/en/language.operators.array.php

    echo "[1,2] + [3,4,5] = \n";
    print_r([1,2] + [3,4,5]);
    //Result is [1,2,5], the first array overwrites the same indices when finding the union
    
    $a = [1 => "First", 2 => "Second"]; 
	$b = [2 => "Second", 1 => "First"];
    var_dump($a == $b); //TRUE
    var_dump($a === $b); //FALSE
    var_dump($a != $b); //FALSE
    var_dump($a <> $b); //FALSE
    var_dump($a !== $b); //TRUE
?>