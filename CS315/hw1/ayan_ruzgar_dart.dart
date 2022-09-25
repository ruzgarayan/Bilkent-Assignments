void main() {
	print("Question 1. What types are legal for subscripts?");
	var array = ['a','b','c','d','e']; 
	print("Initial array:"); print(array); 
	print("array[0]:"); print(array[0]); //Integer is legal

	//When we try to use string, compilation error
	// Error: A value of type 'String' can't be assigned to a variable of type 'int'.
	// So, only int types are allowed for subscript
	//print("array['xyz']:"); print(array["xyz"]);

	print("Question 2. Are subscripting expressions in element references range checked?");

	//Yes, expressions are range checked
	//Gives "Uncaught Error: RangeError (index): Index out of range: index should be less than 5: 10"
	print("array[5]:"); //print(array[5]);

	print("Question 3. When are subscript ranges bound?");
	array.add('x');
	print("array[5]:"); print(array[5]); //x
	
	print("Question 5. Are ragged or rectangular multidimensional arrays allowed, or both?");
	List<List<int>> rectangular = [[1,2,3], [4,5,6], [7,8,9]];
	List<List<int>> ragged = [[1], [2,3], [4,5,6]];
	print(rectangular);
	print(ragged);
	
	print("Question 6. Can array objects be initialized?");
	var initialized_arr = [1,2,3,4,5];
	List<int> initialized_arr2 = List.filled(100, 10, growable: true);
	print(initialized_arr2);
	
	print("Question 7. Are any kind of slices supported?");
	var arr = [1,2,3,4,5];
	var slice = arr.sublist(1,3);
	print(slice); //[2,3]
	
	print("Question 8. Which operators are provided?");
	print([1,2] + [3,4]); //[1,2,3,4]
  
	var arr1 = [1,2];
	var arr2 = [1,2];
	print(arr1 == arr2); //false
	print(arr1 == arr1); //true
}