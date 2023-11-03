# Genetic Algorithm to Solve Traveling Salesman Problem

Dylan Andrews

## Table of Contents

- [Background](#background)
  - [The Traveling Salesman Problem](#the-traveling-salesman-problem)
  - [Genetic Algorithms](#genetic-algorithms)
- [Implementation](#implementation)
  - [Initialize](#initialize)
  - [Calculate Fitness](#calculate-fitness)
  - [Select](#select)
  - [Crossover](#crossover)
- [Sample](#sample)
  - [Input](#input)
  - [Output](#output)


## Background

### The Traveling Salesman Problem
The Traveling Salesman Problem (TSP) is a classic algorithmic problem in the field of computer science and operations research. In this problem, a salesman is given a list of cities to travel to, and the salesman must find the shortest route that visits each city and ends up at the same location that they started in. Although the problem is easy to understand, the challenge of the problem comes from the fact that the number of possible routes increases rapidly as more cities are added (this number is factorial to the number of cities). For instance, if there are 5 cities, there are 120 possible routes; but if there are 10 cities, this number balloons to over 3.6 million. Because of this complexity, there is no known efficient solution for the worst-case scenario, and thus requires some sort of heuristic or optimization-based algorithm to solve the problem. For our problem, we have two sets of locations: one for Los Angles and one for New York (Also to avoid the complexity of streets, we are assuming we can fly in a helicopter.

Example route for Los Angeles:

![alt text](https://github.com/dandrews19/TravelingSalesman/blob/main/example.jpeg?raw=true)

### Genetic Algorithms
Genetic Algorithms take the idea of Darwinism and apply it as a heuristic algorithm. In layman's terms, we start with some random guesses at the potential solution and then try to pick the "fittest" guesses (in our case, the guesses that minimize the distance traveled) to evolve into better guesses. After enough iterations, we stop and end up having a fairly good (though not always perfect) solution. Genetic Algorithms are a good option when:
 - You have a problem where you want the "best possible" solution (in our case, the route that minimizes distance)
 - There's no known efficient algorithm to find the optimal solution (in our case brute force is n!, and the best algorithm is still NP)
 - You can quantify how good or bad a solution is (in our case, the better solutions have shorter routes)
 - You have a way to combine parts of two (or more) solutions (in our case, we can combine parts of multiple routes, more on this in the implementation section).

Here's a general overview of the steps a genetic algorithm follows:
![alt text](https://github.com/dandrews19/TravelingSalesman/blob/main/overview.jpg?raw=true)

- Initialize: Generate a random initial population
- Calculate Fitness: Calculate the fitness function for each member of the population (for us, "more fit" = shorter distance = better)
- Selection: Based on fitness rankings, select pairs of individuals to reproduce, giving some (but not all) preference to fitter individuals
- Crossover/Mutation: Reproduce selected pairs by "crossing over" attributes, might also introduce some random mutations
- Next Generation: After calculating crossover/mutation for all pairs, we'll have a new generation for the "current population"
- Repeat: keep repeating until reaching a condition of termination

## Implementation

The following outlines how the genetic algorithm solution to this problem was implemented. It is also important to note that it was implemented in a functional style in an effort to reduce the need for custom classes, reduce the side effects of functions, and reduce iteration by using functions like std::transform, std::accumulate, std::generate, std::adjacent_difference. 

### Initialize
Initial and future populations are represented as a vector of ints corresponding to the index of the location (the first location in the file is 0). For example, this vector: [0, 2, 5, 4, 3, 1] means start at location 0, then go to 2, then 5, then 4, then 3, then 1, and then back to 0 (implied) 

To carry out the population initialization, a function called FillInitialPopulation in TSP.cpp does the following:
- Initializes population vector with a size of the desired population size
- uses std::generate to fill each population with a sequence of numbers from 1 to n-1 and then uses std::shuffle to shuffle each population member create a random starting sequence for each member.

### Calculate Fitness

To quantify fitness, we use the distance of the route, with a shorter distance equalling better fitness. For this project, Haversine Distance was used to calculate the distance between two points to more accurately get the distance between the two points on a sphere.

To carry out the fitness calculations, a function called computeFitness in TSP.cpp does the following:
- Uses std::transform and std::back_inserter to fill a vector that stores the fitness of each route alongside its index in a std::pair
- Inside the std::transform, std::adjacent_difference is used to compute the Haversine distance between each stop in the route, and std::accumulate is used to compute the sum of those distances to get a final fitness score
- There is a separate Haversine Distance calculator function that computes the Haversine distances


### Select
The Selections are represented as a vector of a pair of ints, with the ints representing the "parents" chosen to reproduce for the next generation. 

To carry out the selection process, a function called Select in TSP.cpp does the following:
1. Uses std::sort to sort the population members by fitness scores (smallest to largest)
2. Calculate Probabiity of Reproduction
     1. Fills a vector of probabilities, representing the probability of each member being chosen for reproduction. Each member is   given the same initial probability (1/populationSize)**
     2. Multiply the two fittest members' probabilities by 6 (6 is an arbitrary number)
     3. Multiply the remainder of the top half members' probabilities by 3 (3 is arbitrary)
     4. Use std::accumulate and std::transform to renormalize the probabilities by summing them up and dividing each probabilty by the sum
3. Use std::generate to generate a pair of parents for each new individual in the next generation by picking from a normal distribution so that members with higher fitness scores are more likely to be chosen.

** Why not select only the fittest? We want to avoid something known as generational drift, which could lead us to a solution that is locally optimal but not globally optimal, so we want to keep some other members in there in case they have characteristics which may be advantageous down the line.


### Crossover
In this step, we reproduce selected pairs by "crossing over" the paths. Random mutations also may be introduced, as is the case with real evolution.

To carry out the crossover process, a function called Crossover in TSP.cpp does the following:
- Use std::transform along with std::back_inserter to fill a vector representing a new population
- For each of the selected parent pairs, we choose a random point from a normal distribution to "crossover" at. Once this "crossover" point is chosen, we flip a coin to decide which parent will fill up the new route up until the crossover point, and then copy that portion using std::copy_n
- For the remainder of the new path, we copy the remaining points from the parent not selected for the first half, skipping any points that are already present in the first half using std::copy_if and std::find
- Decide whether to apply a mutation by choosing number from uniform distribution
    - If a mutation occurs, two random indices are chosen (not including 0) and their indexes are swapped
 


## Sample

Below is a sample input/output from the program:

### Input

locations.txt:

```
LAX Airport,33.941845,-118.408635
USC,34.020547,-118.285397
Coliseum,34.014156,-118.287923
Chinese Theatre,34.102021,-118.340946
Whiskey a Go Go,34.090839,-118.385725
Getty Center,34.078062,-118.473892
Getty Villa,34.045868,-118.564850
Disneyland,33.812110,-117.918921
The Huntington Library,34.129178,-118.114556
Rose Bowl,34.161373,-118.167646
Griffith Observatory,34.118509,-118.300414
Hollywood Sign,34.134124,-118.321548
Magic Mountain,34.425392,-118.597230
Third Street Promenade,34.016297,-118.496838
Venice Beach,33.985857,-118.473167
Catalina Island,33.394698,-118.415119
Staples Center,34.043097,-118.267351
Dodger Stadium,34.072744,-118.240594
La Brea Tar Pits,34.063814,-118.355466
Zuma Beach,34.015489,-118.822160
```

Inputs:
```cpp

const char* argv[] = {
			"tests/tests", 
			"input/locations.txt", // name of input file
			"8", // population size
			"5", // number of generations algorithm should produce
			"10", // percent chance of mutation happening
			"1337" // seed for random number generator
		};
```

### Output

log.txt:

```
INITIAL POPULATION:
0,16,8,10,1,3,18,15,5,11,17,7,9,12,13,19,6,4,2,14
0,9,3,8,14,2,18,6,4,5,13,19,16,17,11,12,15,7,1,10
0,8,14,7,11,13,2,10,3,6,9,4,16,12,19,17,15,1,18,5
0,14,15,9,5,1,16,19,10,18,12,4,11,8,17,7,6,3,2,13
0,2,9,14,19,12,18,11,16,4,15,17,8,1,5,3,10,6,13,7
0,1,14,12,8,11,4,16,7,9,3,17,6,2,13,19,5,10,15,18
0,2,7,14,11,19,17,3,12,15,6,9,8,4,5,10,16,13,18,1
0,16,19,3,5,17,2,12,6,1,14,15,10,4,7,11,18,8,9,13
FITNESS:
0:338.403
1:359.009
2:414.252
3:385.012
4:366.093
5:381.913
6:395.9
7:397.543
SELECTED PAIRS:
(3,4)
(1,1)
(3,5)
(4,5)
(4,7)
(0,1)
(4,4)
(1,0)
GENERATION: 1
0,14,15,9,5,1,16,19,10,18,12,4,2,11,17,8,3,6,13,7
0,9,3,8,14,2,18,6,4,5,13,19,16,17,11,12,15,7,1,10
0,14,15,1,12,8,11,4,16,7,9,3,17,6,2,13,19,5,10,18
0,2,9,14,19,12,18,11,16,4,15,17,8,1,5,3,10,6,13,7
0,2,9,14,19,12,18,11,16,3,5,17,6,1,15,10,4,7,8,13
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,2,9,14,19,12,18,11,16,4,15,17,8,1,5,3,10,6,13,7
0,16,8,10,1,3,18,15,5,11,17,7,9,14,2,6,4,13,19,12
FITNESS:
0:381.779
1:359.009
2:361.716
3:366.093
4:383.62
5:358.315
6:366.093
7:363.312
SELECTED PAIRS:
(5,5)
(7,6)
(1,7)
(5,3)
(5,7)
(3,2)
(1,5)
(0,2)
GENERATION: 2
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,2,9,14,19,12,18,11,16,8,10,1,3,15,5,17,7,6,4,13
0,9,3,8,14,2,18,6,4,5,13,19,16,17,11,12,15,7,10,1
0,2,9,14,19,12,18,11,16,4,15,3,8,6,5,13,17,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,2,9,14,19,12,18,11,16,4,15,17,8,1,5,7,3,6,13,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,14,15,9,5,1,16,19,10,18,12,8,11,4,7,3,17,6,2,13
FITNESS:
0:358.315
1:370.836
2:359.189
3:375.563
4:358.315
5:382.782
6:358.315
7:403.84
SELECTED PAIRS:
(4,4)
(4,4)
(2,0)
(2,0)
(4,4)
(4,0)
(0,2)
(3,0)
GENERATION: 3
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,9,3,16,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,18,2,14,6,4,5,13,19,17,11,12,15,7,10,1
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,10,1
0,16,9,3,8,14,2,18,19,12,11,4,15,6,5,13,17,7,1,10
FITNESS:
0:358.315
1:358.315
2:361.503
3:344.842
4:358.315
5:358.315
6:358.495
7:362.746
SELECTED PAIRS:
(1,0)
(3,1)
(1,4)
(4,4)
(3,4)
(0,1)
(5,3)
(4,3)
GENERATION: 4
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,10,1
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,18,14,2,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,10,1
FITNESS:
0:358.315
1:358.315
2:358.315
3:358.315
4:358.495
5:358.315
6:357.444
7:358.495
SELECTED PAIRS:
(2,2)
(0,6)
(7,0)
(7,2)
(0,2)
(6,1)
(0,6)
(4,2)
GENERATION: 5
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,10,1
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,10,1
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,18,2,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,18,14,2,6,4,5,13,19,17,11,12,15,7,1,10
0,16,9,3,8,14,2,18,6,4,5,13,19,17,11,12,15,7,1,10
FITNESS:
0:358.315
1:358.315
2:358.495
3:358.495
4:358.315
5:360.112
6:357.444
7:358.315
SOLUTION:
LAX Airport
Staples Center
Rose Bowl
Chinese Theatre
The Huntington Library
La Brea Tar Pits
Venice Beach
Coliseum
Getty Villa
Whiskey a Go Go
Getty Center
Third Street Promenade
Zuma Beach
Dodger Stadium
Hollywood Sign
Magic Mountain
Catalina Island
Disneyland
USC
Griffith Observatory
LAX Airport
DISTANCE: 357.444 miles
```
