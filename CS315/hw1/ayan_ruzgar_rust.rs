fn main() {
    println!("Question 1. What types are legal for subscripts?");
    let mut array:[i32;5] =  [1,2,3,4,5];
    println!("array is {:?}", array);
    println!("array[0] is {}", array[0]);
    
    // All below gives the error
    // the trait `SliceIndex<[i32]>` is not implemented for `bool`/`{float}`/`&str`
    // This means only i32 is accepted for subscript tpye
    //println!("array[true] is {}", array[true]);
    //println!("array[1.2] is {}", array[1.2]);
    //println!("array['abc'] is {}", array["abc"]);
    
    
    println!("Question 2. Are subscripting expressions in element references range checked?");
    
    //Throws "index out of bounds: the length is 5 but the index is 5" at compilation
    //println!("array[5] is {}", array[5]);
    
    println!("Question 5. Are ragged or rectangular multidimensional arrays allowed, or both?");
    //Reference https://stackoverflow.com/questions/13212212/creating-two-dimensional-arrays-in-rust
    let mut rectangular:[[i32;3];3] = [[1,2,3],[4,5,6],[7,8,9]];
    println!("rectangular = {:?}", rectangular);
    
    let boxes: [Box<[u8]>; 3] = [
        Box::new([1]),
        Box::new([2,3]), 
        Box::new([4,5,6])
    ];
    let ragged: &[Box<[u8]>] = &boxes;
    println!("ragged = {:?}", ragged);
    println!("rectangular[1][1] = {}", rectangular[1][1]);
    println!("ragged[1][1] = {}", ragged[1][1]);
    
    println!("Setting rectangular[0][0] = 5");
    rectangular[0][0] = 5;
    println!("Setting rectangular[1][1] = 11");
    rectangular[1][1] = 11;
    println!("Setting rectangular[2][2] = rectangular[0][0] * rectangular[1][1]");
    rectangular[2][2] = rectangular[0][0] * rectangular[1][1];
    println!("rectangular[2][2] = {}", rectangular[2][2]);
    
    println!("Question 6. Can array objects be initialized?");
    let mut initialized_arr:[i32;5] =  [1,2,3,4,5];
    let mut initialized_arr2:[i32;100] =  [10;100]; //Array of 10's of size 100
	
    println!("initialized_arr2 = {:?}", initialized_arr2);
    
    println!("Question 7. Are any kind of slices supported?");
    //Reference https://doc.rust-lang.org/rust-by-example/primitives/array.html
    let mut array:[i32;5] =  [1,2,3,4,5];
    println!("array is {:?}", array); //[1, 2, 3, 4, 5]
    let mut slice = &array[2..4];
    println!("slice = array[2..4] is {:?}", slice); //[3, 4]
    slice = &array[3..5];
    println!("Now slice is {:?}", slice); //[4, 5]
    println!("array is {:?}", array); //Still not changed after modifying the slice.
    
    println!("Question 8. Which operators are provided?");
    
    let mut array:[i32;5] =  [1,2,3,4,5];
    let mut array2:[i32;5] =  [1,2,3,4,5];
    let mut array3:[i32;5] =  [5,4,3,2,1];
    let mut array4:[i32;3] =  [1,2,3];
    println!("{}", array == array2); //true
    println!("{}", array == array3); //false
    println!("{}", array3 > array); //true
    println!("{}", array3 < array); //false
    println!("{}", array <= array2); //true
    
    //error[E0369]: cannot add `[i32; 5]` to `[i32; 5]`
    //println!("{:?}", array + array2);
    
    //Error no implementation for `[i32; 5] == [i32; 3]`
    //println!("{}", array == array4); Must be the same size
}