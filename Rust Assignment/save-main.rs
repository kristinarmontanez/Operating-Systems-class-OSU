/*
    Name:           Kristina Montanez
    Date:           Dec 5, 2021
    Class:          CS344
    Project:        Assignment 6
    Description:    Rust Program that processes a user's input numbers in order to produce the 
                    sum of all numbers. This program is created using an example program 
                    provided in the assignment with unchanged comments that explain the 
                    functionality of the program and give directions on which parts of the code
                    that is allowed to be changed. 

                    Example Code to use:
                    "pa6_map_reduce.rs"
______________________________________________________________________________________________


______________________________________________________________________________________________
TABLE OF CONTENTS:

1)  print_partition_info
2)  generate_data
3)  partition_data_in_two
4)  map_data
5)  reduce_data
6)  main
7)  partition_data 
______________________________________________________________________________________________*/

// Use the libraries shown in the materials.
use std::thread; // use of
use std::env; // to get arugments passed to the program
//use std::JoinHandle; // for use of JoinHandle

/*
* Print the number of partitions and the size of each partition
* @param vs A vector of vectors
*/
fn print_partition_info(vs: &Vec<Vec<usize>>){
    println!("Number of partitions = {}", vs.len());
    for i in 0..vs.len(){
        println!("\tsize of partition {} = {}", i, vs[i].len());
    }
}

/*
* Create a vector with integers from 0 to num_elements -1
* @param num_elements How many integers to generate
* @return A vector with integers from 0 to (num_elements - 1)
*/
fn generate_data(num_elements: usize) -> Vec<usize>{
    let mut v : Vec<usize> = Vec::new();
    for i in 0..num_elements {
        v.push(i);
    }
    return v;
}

/*
* Partition the data in the vector v into 2 vectors
* @param v Vector of integers
* @return A vector that contains 2 vectors of integers

*/
fn partition_data_in_two(v: &Vec<usize>) -> Vec<Vec<usize>>{
    let partition_size = v.len() / 2;
    // Create a vector that will contain vectors of integers
    let mut xs: Vec<Vec<usize>> = Vec::new();

    // Create the first vector of integers
    let mut x1 : Vec<usize> = Vec::new();
    // Add the first half of the integers in the input vector to x1
    for i in 0..partition_size{
        x1.push(v[i]);
    }
    // Add x1 to the vector that will be returned by this function
    xs.push(x1);

    // Create the second vector of integers
    let mut x2 : Vec<usize> = Vec::new();
    // Add the second half of the integers in the input vector to x2
    for i in partition_size..v.len(){
        x2.push(v[i]);
    }
    // Add x2 to the vector that will be returned by this function
    xs.push(x2);
    // Return the result vector
    xs
}

/*
* Sum up the all the integers in the given vector
* @param v Vector of integers
* @return Sum of integers in v
* Note: this function has the same code as the reduce_data function.
*       But don't change the code of map_data or reduce_data.
*/
fn map_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/*
* Sum up the all the integers in the given vector
* @param v Vector of integers
* @return Sum of integers in v
*/
fn reduce_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/*
* A single threaded map-reduce program
*/
fn main() {

    // Use std::env to get arguments passed to the program
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        println!("ERROR: Usage {} num_partitions num_elements", args[0]);
        return;
    }
    let num_partitions : usize = args[1].parse().unwrap();
    let num_elements : usize = args[2].parse().unwrap();
    if num_partitions < 1{
      println!("ERROR: num_partitions must be at least 1");
        return;
    }
    if num_elements < num_partitions{
        println!("ERROR: num_elements cannot be smaller than num_partitions");
        return;
    }

    // Generate data.
    let v = generate_data(num_elements);

    // PARTITION STEP: partition the data into 2 partitions
    let xs = partition_data_in_two(&v);

    // Print info about the partitions
    print_partition_info(&xs);

    let mut intermediate_sums : Vec<usize> = Vec::new();

    // MAP STEP: Process each partition

    // CHANGE CODE START: Don't change any code above this line





/* 
______________________________________________________________________________________________
6)  main                    CHANGED CODE

Change the following code to create 2 threads that run concurrently and each of which uses 
map_data() function to process one of the two partitions
______________________________________________________________________________________________ */

// Creating thread #1 that will use map_data.
let xs_index_0 = xs[0].clone();
let sx_thread_0 = thread::spawn(move || map_data( &xs_index_0));
intermediate_sums.push( sx_thread_0.join().unwrap());

// Creating thread #2 that will use map_data.
let xs_index_1 = xs[1].clone();
let sx_thread_1 = thread::spawn(move || map_data( &xs_index_1));
intermediate_sums.push( sx_thread_1.join().unwrap());

/*____________________________________________________________________________________________ */








    // CHANGE CODE END: Don't change any code below this line until the next CHANGE CODE comment

    // Print the vector with the intermediate sums
    println!("Intermediate sums = {:?}", intermediate_sums);

    // REDUCE STEP: Process the intermediate result to produce the final result
    let sum = reduce_data(&intermediate_sums);
    println!("Sum = {}", sum);





/* 
______________________________________________________________________________________________
6)  main                    CHANGED CODE

1. Calls partition_data to partition the data into equal partitions
2. Calls print_partition_info to print info on the partitions that have been created
3. Creates one thread per partition and uses each thread to concurrently process one partition
4. Collects the intermediate sums from all the threads
5. Prints information about the intermediate sums
5. Calls reduce_data to process the intermediate sums
6. Prints the final sum computed by reduce_data
______________________________________________________________________________________________ */

// 1.   Partition the data into equal partitions.
let equal_part = partition_data( num_partitions, &v);
    
// 2. Print info on the partitions that have been created.
print_partition_info( &equal_part.clone());

// 3. Creates one thread per partition and uses each thread 
//    to concurrently process one partition.
let mut children : Vec<std::thread::JoinHandle<usize>> = Vec::new();

for partition in equal_part 
    {children.push( thread::spawn(move || map_data( &partition)));}

// 4. Collect intermediate sums from all threads.
let mut intermediate_sums_2 = Vec::new();
for i in children 
    {intermediate_sums_2.push( i.join().unwrap());} 
// 5. Print info on intermediate sums.
println!( "Intermediate sums = {:?}", intermediate_sums_2);
// 5. Process intermediate sums.
let final_sum = reduce_data( &intermediate_sums_2);
// 6. Print the final sum.
println!( "Sum = {}", final_sum);}

/*____________________________________________________________________________________________ */



/* 
______________________________________________________________________________________________
6)  partition_data          CHANGED CODE

Note: Don't change the signature of this function

Partitions the data into a number of partitions such that
the returned partitions contain all elements that are in the input vector
if num_elements is a multiple of num_partitions, then all partitions must have equal number of elements
if num_elements is not a multiple of num_partitions, some partitions can have one more element than other partitions

@param num_partitions The number of partitions to create
@param v The data to be partitioned
@return A vector that contains vectors of integers
______________________________________________________________________________________________ */
fn partition_data(num_partitions: usize, v: &Vec<usize>) -> Vec<Vec<usize>>
    
    // Make our vector counter.
    {let mut index = 0;

    // Divide num_partitions to get the length of our partitions,
    // and the remainder for what's left over. 
    let partition_size = v.len() / num_partitions;
    let mut remainder = v.len() % num_partitions;
    
    // Make our final vector that will hold vectors of integers.
    // This will allow us to push each sub_vector to final_vector
    // Once each time sub_vector is done being populated.
    let mut final_vector: Vec<Vec<usize>> = Vec::new();
    

    for _i in 0..num_partitions
        // Let's create our sub_vectors, 
        {let mut sub_vector : Vec<usize> = Vec::new();
        // let specific_partition_size
        let mut specific_partition_size = partition_size;
        
        
        if remainder > 0
            {specific_partition_size += 1;
            remainder -= 1;}


        // Populate it
        for i in 0..specific_partition_size
            {sub_vector.push(v[i+index]);}


        




        // Add it to vector of vectors
        final_vector.push(sub_vector);
        
        
        // Add the real partition size to our
        // vector counter.
        index += specific_partition_size;
    }



    final_vector}

