# Genetic Algorithm to Solve Traveling Salesman Problem

## Background: The Traveling Salesman Problem
The Traveling Salesman Problem (TSP) is a classic algorithmic problem in the field of computer science and operations research. In this problem, a salesman is given a list of cities to travel to, and the salesman must find the shortest route that visits each city and ends up at the same location that they started in. Although the problem is easy to understand, the challenge of the problem comes from the fact that the number of possible routes increases rapidly as more cities are added (this number is factorial to the number of cities). For instance, if there are 5 cities, there are 120 possible routes; but if there are 10 cities, this number balloons to over 3.6 million. Because of this complexity, there is no known efficient solution for the worst-case scenario, and thus requires some sort of heuristic or optimization-based algorithm to solve the problem. For our problem, we have two sets of locations: one for Los Angles and one for New York (Also to avoid the complexity of streets, we are assuming we can fly in a helicopter.

Example route for Los Angeles:

![alt text](https://github.com/dandrews19/TravelingSalesman/blob/main/example.jpg?raw=true)

## Background: Genetic Algorithms
This is a Genetic Algorithm (GA) implementation designed to solve an optimization problem. The algorithm uses standard GA operators such as selection, crossover, and mutation. The specific problem solved by the algorithm appears to be a variant of the Traveling Salesman Problem (TSP).

## Input
The input to the program is given through the command line with the following parameters:

1. **inputFile**: The path to the file containing the problem data.
2. **popSize**: The size of the population in each generation of the GA.
3. **numGenerations**: The number of generations for which the GA should run.
4. **mutationChance**: The chance of a mutation occurring in each generation, represented as an integer (1 equals a 1% chance).
5. **seed**: The seed for the random number generator used in the GA.

For example, to run the program with a population size of 100, for 200 generations, with a 5% mutation chance, and a seed of 1234, on a problem data file named 'problem.dat', you would use:
