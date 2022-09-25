package main

import (
	"fmt"
	"reflect"
)

func main() {
	/* It is not possible to define subprograms in this way
	func sayHi (name string) {
		fmt.Println("Hi", name)
	}
	*/

	//We need to assign a function into a variable.
	sayHi := func(name string) {
		fmt.Println("Hi", name)
	}
	sayHi("Rüzgar")
	fmt.Println("Type of sayHi:", reflect.TypeOf(sayHi))

	square := func(num int) int {
		return num * num
	}
	fmt.Println("square(5):", square(5))
	fmt.Println("Type of square:", reflect.TypeOf(square))

	//Can directly define a function and call it, but not very useful
	func(message string) {
		fmt.Println("Inside the directly called function. Message:", message)
	}("a special message")

	//This function returns another function to be used later
	generateHiFunction := func(name string) func() {
		return func() {
			fmt.Println("Hi", name, ",from the generated function")
		}
	}

	sayHiRuzgar := generateHiFunction("Rüzgar")
	sayHiRuzgar() //Prints "Hi Rüzgar ,from the generated function"

	//Subprograms with side-effects
	num := 2
	squareAndPrint := func() {
		num = num * num
		fmt.Println("After squaring, num:", num)
	}

	squareAndPrint() //4
	squareAndPrint() //16
	squareAndPrint() //256

	//Scope of local variables
	fmt.Println("\n\n")

	foo := "Main"
	subprogram1 := func() {
		foo = "SP1"
		subprogram2 := func() {
			foo = "SP2"
			fmt.Println("At SP2, foo:", foo)
		}
		fmt.Println("At SP1, foo:", foo)
		subprogram2()
		fmt.Println("At SP1 after calling SP2, foo:", foo)
	}
	fmt.Println("At Main, foo:", foo)
	subprogram1()
	fmt.Println("At Main after calling SP1, foo:", foo, "\n\n")

	//Checking if static or dynamic scoping is used
	foo = "Main"
	subprogram1 = func() {
		fmt.Println("At SP1, foo:", foo)

		//Gives undefined: specialVariable at compilation
		//If it were dynamic, would not be able decide this until runtime
		//fmt.Println("At SP1, specialVariable :", specialVariable)
	}
	subprogram2 := func() {
		foo := "SP2" //Define new local variable foo
		specialVariable := "specialVariable"
		fmt.Println("At SP2, foo:", foo)
		fmt.Println("At SP2, specialVariable:", specialVariable)
		subprogram1()
	}
	subprogram2()

	//Parameter passing methods
	fmt.Println("\n\n")

	num = 10
	modifyNum := func(numIn int) { numIn = 100 }
	fmt.Println("num before modification:", num)
	modifyNum(num)
	fmt.Println("num after modification:", num)
	fmt.Println()

	nums := [5]int{1, 2, 3, 4, 5}
	modifyArray := func(arr [5]int) { arr[0] = 100 }
	fmt.Println("nums before modification:", nums)
	modifyArray(nums)
	fmt.Println("nums after modification:", nums)
	fmt.Println()

	//Different than the array above, slice holds a pointer internally,
	//so when passed into a function by a value, we can still modify the actual elements through the pointer
	numsSlice := []int{1, 2, 3, 4, 5}
	modifySlice := func(slice []int) { slice[0] = 100 }
	fmt.Println("nums before modification:", numsSlice)
	modifySlice(numsSlice)
	fmt.Println("nums after modification:", numsSlice)
	fmt.Println()

	//This is still not pass by reference since pointers are also copied with pass by value.
	num = 10
	modifyNumByPointer := func(numIn *int) { *numIn = 100 }
	fmt.Println("num before modification by pointer:", num) //10
	modifyNumByPointer(&num)
	fmt.Println("num after modification by pointer:", num) //100
	fmt.Println()

	//Keyword and default parameters

	//Keyword parameters are not supported, but there is a similar feature for output parameters
	squareAndCube := func(num int) (square int, cube int) {
		square = num * num
		cube = square * num

		//Return empty and the current values of square and cube will be returned
		return
	}
	squareOf5, cubeOf5 := squareAndCube(5)
	fmt.Println("Square and cube of 5:", squareOf5, cubeOf5) //25 125

	//Default parameters are not supported
	//We can only simulate the behaviour by passing empty inputs such as empty string or nil
	simulateDefault := func(name string) {
		if name == "" {
			name = "Rüzgar"
		}
		fmt.Println("name is:", name)
	}

	simulateDefault("Ali")
	simulateDefault("") //Prints Rüzgar

	//Closure
	fmt.Println("\n\n")
	
	sequenceGenerator := func() (func(), func()) {
		i := 0

		increaseBy1 := func() {
			i = i + 1
			fmt.Println("Increased by 1, current value:", i)
		}

		increaseBy2 := func() {
			i = i + 2
			fmt.Println("Increased by 2, current value:", i)
		}

		return increaseBy1, increaseBy2
	}

	increaseBy1, increaseBy2 := sequenceGenerator()

	increaseBy1() //1
	increaseBy1() //2
	increaseBy1() //3
	increaseBy2() //5
	increaseBy2() //7
	increaseBy1() //8
}
