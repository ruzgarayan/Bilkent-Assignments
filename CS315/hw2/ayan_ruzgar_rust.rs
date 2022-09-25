static mut globalVariable: &str = "Initial value";

fn main() {
    //Question 1.
    let boolTrue = true;
    let boolFalse = false;
    let string = "abc";
    let number = 1;
    
    if boolTrue { println!("boolTrue"); }
    if boolFalse { println!("boolFalse"); }
    
    //These below give error[E0308]: mismatched types
    //expected `bool`, found integer/&str
    // if string { println!("string"); }
    // if number { println!("number"); }
    // We understand that only boolean variables are used
   
    //Question 2.
    println!("\n");
    
    fn printBool(msg: &str) -> bool {
        println!("{}", msg);
        return true;
    }
    
    fn printInt(msg: &str) -> i32 {
        println!("{}", msg);
        return 1;
    }
    
    let and_ = (false && printBool("and"));
    let or_ = (true || printBool("or"));
    let bitwiseAnd = (0 & printInt("bitwiseAnd")); //Prints
    let bitwiseOr = (!0 | printInt("bitwiseOr")); //Prints
    let multiplication = (0 * 5 * printInt("multiplication")); //Prints
    
    
    //Question 3.
    println!("\n");
    
    //Prints a and b
    let shortcircuit = printBool("a") && printBool("b") || printBool("c");
    //Prints a and b
    let shortcircuit2 = false || printBool("a") && (printBool("b") || printBool("c"));
    
    //Question 4.
    println!("\n");
    
    fn expensiveComputation(name: &str) -> i32 { 
        let mut sum = 0;
        for i in 0..10000 {
            sum += i;
        }
        println!("Did expensive computation at {}", name);
        return sum;
    }

    let smallNumber = 1;
    //With short-circuit
    if smallNumber > 10 && expensiveComputation("if1") > 10000 {
        println!("Inside if 1");
    }
    
    //Simulating the if above without short-circuit
    if smallNumber > 10 {
        if expensiveComputation("if2") > 10000 {
        println!("Inside if 2");
        }
    }
    
    //Not doing short-circuit evaluation because of wrong order
    //Prints "Did expensive computation at if3" only for this one
    if expensiveComputation("if3") > 10000 && smallNumber > 10 {
        println!("Inside if 3");
    }
    
    //Question 5.
    println!("\n");
    
    fn sideEffectComputation() -> bool { 
        unsafe {
            globalVariable = "I have been changed";
        }
        return true;
    }

    unsafe {
        let var1 = smallNumber > 10 && sideEffectComputation();
        println!("{}", globalVariable); //Prints "Initial value"
        let var2 = sideEffectComputation() && smallNumber > 10;
        println!("{}", globalVariable); //Prints "I have been changed"
    }
}