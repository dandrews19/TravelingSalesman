# Genetic Algorithm to Solve Traveling Salesman Problem

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
     1. Fills a vector of probabilities, representing the probability of each member being chosen for reproduction. Each member is   given the same initial probability (1/populationSize)
     2. Multiply the two fittest members' probabilities by 6 (6 is an arbitrary number)
     3. Multiply the remainder of the top half members' probabilities by 3 (3 is arbitrary)
     4. Use std::accumulate and std::transform to renormalize the probabilities by summing them up and dividing each probabilty by the sum
3. A
