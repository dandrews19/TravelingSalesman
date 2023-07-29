#include "SrcMain.h"
#include <iostream>
#include <random>
#include "TSP.h"
#include <fstream>
#include <algorithm>

// A function to process the command line arguments and initiate the genetic algorithm.
void ProcessCommandArgs(int argc, const char* argv[]) {

    // Parsing the input command line arguments.
	std::string inputFile = argv[1]; // The first argument is the file name for the input file.
	std::string popSizeStr = argv[2]; // The second argument is the population size for the genetic algorithm.
	int popSizeInt = stoi(popSizeStr); // Converting the population size to an integer.
	std::string numGenerationsStr = argv[3]; // The third argument is the number of generations the genetic algorithm should run for.
	int numGenerationsInt = stoi(numGenerationsStr); // Converting the number of generations to an integer.
	std::string mutationChanceStr = argv[4]; // The fourth argument is the chance of a mutation happening.
	int mutationChanceInt = stoi(mutationChanceStr); // Converting the mutation chance to an integer.
	std::string seedStr = argv[5]; // The fifth argument is the seed for the random number generator.
	int seedInt = stoi(seedStr) ; // Converting the seed to an integer.

    // Initializing the random number generator with the given seed.
	std::mt19937 generator(seedInt);

    // Reading the locations from the input file.
	std::vector<Location> locations = ReadLocations(inputFile);

    // Creating the initial population.
	Population initialPopulation = FillInitialPopulation(popSizeInt, generator, locations.size());

    // Logging the initial population to a file named "log.txt".
	OutputPopulationFile("log.txt", initialPopulation, "INITIAL POPULATION:");

    // Declarations of populationFitnesses and selections to be used in the genetic algorithm.
	std::vector<std::pair<int,double>> populationFitnesses;
	std::vector<std::pair<int,int>> selections;

    // Running the genetic algorithm for the specified number of generations.
	for (int genNumber = 1; genNumber <= numGenerationsInt; genNumber++ ) {
	    // Computing the fitnesses for the current population.
	    populationFitnesses = computeFitnesses(initialPopulation, locations);
	    // Logging the fitnesses to the "log.txt" file.
	    OutputFitnessFile("log.txt",populationFitnesses);
	    // Performing the selection step of the genetic algorithm.
	    selections = Select(populationFitnesses, generator, popSizeInt);
	    // Logging the selected pairs to the "log.txt" file.
	    OutputSelectedPairs("log.txt",selections);
	    // Generating the new population by crossover (and possibly mutation).
	    initialPopulation = Crossover(selections, locations, generator, popSizeInt, initialPopulation, mutationChanceInt);
	    // Logging the current generation to the "log.txt" file.
	    OutputGeneration("log.txt", genNumber, initialPopulation);
	}

    // Computing the fitnesses for the final population.
	populationFitnesses = computeFitnesses(initialPopulation, locations);

    // Logging the final fitnesses to the "log.txt" file.
	OutputFitnessFile("log.txt",populationFitnesses);

    // Finding the minimum (best) distance.
	auto minDistanceIterator = std::min_element(populationFitnesses.begin(), populationFitnesses.end(), [](const std::pair<int,double>& lhs, const std::pair<int,double>& rhs){
		return lhs.second < rhs.second;
	});

    // Identifying the index of the minimum distance and its corresponding value.
	auto minDistanceElement = populationFitnesses[std::distance(populationFitnesses.begin(), minDistanceIterator)].first;
	auto minDistance = populationFitnesses[std::distance(populationFitnesses.begin(), minDistanceIterator)].second;

    // Getting the route with the minimum distance.
	auto minDistanceVector= initialPopulation.mMembers[minDistanceElement];

    // Logging the best solution found by the genetic algorithm to the "log.txt" file.
	OutputSolution("log.txt", locations, minDistanceVector, minDistance);
}
