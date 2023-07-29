#include "TSP.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <iostream>

// function that returns a std::vector of locations. This function needs to read in the locations from the input file.
std::vector<Location> ReadLocations(std::string_view inputFile) {

    std::ifstream in(inputFile.data());
	std::string line;

    std::vector<Location> locations;

    if (in.is_open()) {
		while (!in.eof()) {

			std::getline(in, line);
            
            int comma = line.find(',');

            std::string name = line.substr(0, comma);
            line.erase(0, comma + 1);

            comma = line.find(',');

            double latitude = stod(line.substr(0, comma));


            line.erase(0, comma + 1);

            comma = line.find(',');

            double longitude = stod(line.substr(0, comma));

            line.erase(0,  + comma + 1);

            Location loc;
            loc.mName = name;
            loc.mLatitude = latitude;
            loc.mLongitude = longitude;


            locations.emplace_back(loc);





		}
	}


    return locations;

}

// Function that generates the initial population for the genetic algorithm. The population consists of various permutations of the route sequence.
Population FillInitialPopulation (int popSize, std::mt19937& generator, size_t locationSize) {
    // Creating a population vector with size as the population size.
    std::vector<std::vector<int>> populations(popSize);

    // Fill each population member with a unique sequence of location indexes.
    std::generate(populations.begin(), populations.end(), [&generator, &locationSize]() {
        // Create an initial vector that contains the sequential values from 0 to n - 1, where n is the number of locations in the location file.
        // Note that there is not an extra 0 at the end, because it’s implied that you’ll go back to the first location.
        std::vector<int> v(locationSize) ;
        int i = 0;
        // Use generate() to fill the vector with sequence of numbers starting from 0 to n - 1.
        std::generate(v.begin(), v.end(), [&i]() {
            return i++;
        });
        
        // Shuffle the elements in the vector starting from second element to end to create a unique sequence, the first location (index 0) is kept constant as the starting and ending point.
        std::shuffle(v.begin() + 1, v.end(), generator);
        
        // Return the generated sequence.
        return v;
    });

    // Create a Population object and assign the generated population to its member field.
    Population pop;
    pop.mMembers = populations;

    // Return the created population object.
    return pop;
}

// this function outputs the population to output file for testing purposes
void OutputPopulationFile(std::string_view fileName, const Population& pop, std::string_view header) {
    std::ofstream out(fileName.data());


    out << header.data() << '\n';

    for(auto population: pop.mMembers) {
        out << '0';
        for(size_t j = 1; j < population.size(); j++) {
            out << ',' << std::to_string(population[j]);
        }
        out << '\n';

        
    }

}

void OutputFitnessFile(std::string_view fileName, const std::vector<std::pair<int,double>>& fits) {
    std::ofstream out;

    out.open(fileName.data(), std::ios_base::app);


    out << "FITNESS:" << '\n';

    for(auto fit: fits) {
        out << fit.first << ':' << fit.second << '\n';

        
    }

}

// Function to compute the fitness values of all the population members. The fitness value in this case is the total Haversine distance covered by the route sequence.
std::vector<std::pair<int,double>> computeFitnesses(const Population& population, const std::vector<Location>& locations) {
    // Vector to store the index and the corresponding fitness value for each member of the population.
    std::vector<std::pair<int,double>> fitnesses;

    int i = 0;  // Initialize index

    // Calculate the fitness value for each route sequence in the population.
    std::transform(population.mMembers.begin(), population.mMembers.end(), std::back_inserter(fitnesses), [&locations, &i](std::vector<int> from) {
        // Vector to store the distances between consecutive locations in the sequence.
        std::vector<double> diffs;

        // Calculate the difference between consecutive locations using Haversine formula
        std::adjacent_difference(from.begin(), from.end(), std::back_inserter(diffs), [&locations](int a, int b) {
            return GetHaversineDistance(locations[a].mLongitude, locations[a].mLatitude, locations[b].mLongitude, locations[b].mLatitude);
        });

        // For the initial element in the diffs array, calculate the distance between the first and the last location, because the route is a round trip.
        diffs[0] = GetHaversineDistance(locations[from[0]].mLongitude, locations[from[0]].mLatitude, locations[from[from.size()-1]].mLongitude, locations[from[from.size()-1]].mLatitude);

        // Sum all the Haversine distances in the 'diffs' vector to compute the total distance covered by the route sequence, which is the fitness value in this case.
        double fitness = std::accumulate(diffs.begin(), diffs.end(), 0.0, [](const double& a, const double& b) {
            return a + b;
        });

        // Pair the calculated fitness value with its index
        std::pair<int,double> fitPair(i, fitness);

        ++i;  // Increment the index for the next population member

        return fitPair;  // Return the index-fitness value pair
    });

    // Return the vector of index-fitness value pairs for the entire population.
    return fitnesses;
}


// Function to compute the Haversine distance between two locations given their longitudes and latitudes.
double GetHaversineDistance(const double& lon1, const double& lat1, const double& lon2, const double& lat2) {
    // Convert the degrees to radians
    double lon1Rad = lon1 * 0.0174533;
    double lon2Rad = lon2 * 0.0174533;
    double lat1Rad = lat1 * 0.0174533;
    double lat2Rad = lat2 * 0.0174533;

    // Calculate the differences in coordinates
    double dlon = lon1Rad - lon2Rad;
    double dlat = lat1Rad - lat2Rad;

    // Calculate the Haversine distance
    double a = pow((sin(dlat/2)), 2) + cos(lat1Rad) * cos(lat2Rad) * pow((sin(dlon/2)),2);
    double c = 2 * atan2( sqrt(a), sqrt(1-a) );

    // Convert to miles
    double distance = 3961 * c;
    return distance;
}



// Function to select parents for the next generation based on the fitness values of the population members. 
std::vector<std::pair<int,int>> Select(std::vector<std::pair<int,double>>& fitnesses, std::mt19937& generator, int popSize) {
    // Sort the fitness vector in ascending order. The individual with the lowest score (shortest distance) is considered as the "most fit".
    std::sort(fitnesses.begin(), fitnesses.end(), [](std::pair<int,double> a, std::pair<int,double> b) {
        return a.second < b.second;
    });

    std::vector<double> probabilities(popSize);

    // Initialize each element in the probability vector to 1.0 / popSize. This represents the initial chance of being selected for reproduction for each individual.
    std::generate(probabilities.begin(), probabilities.end(), [popSize]() {
        return 1.0/popSize;
    });

    // The probability for the two fittest individuals is multiplied by 6. This gives them a higher chance of being selected for reproduction.
    probabilities[fitnesses[0].first] *= 6.0;
    probabilities[fitnesses[1].first] *= 6.0;

    // The remainder of the top half of the fit individuals (from rank 2 to rank size / 2 – 1) should have their probability multiplied by 3. This also gives them a higher chance of being selected, but less than the two fittest individuals.
    for (int j = 2; j < (popSize / 2); j++) {
        probabilities[fitnesses[j].first] *= 3.0;
    }

    // Renormalize the probability vector to sum to 1.0. This is necessary because we have changed the probabilities for the top half of the individuals.
    double probSum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0, [](const double& a, const double& b) {
            return a + b;
        });

    probabilities = divEachBy(probabilities, probSum);  // Note: The function "divEachBy" should divide each element in the probabilities vector by probSum.

    std::vector<std::pair<int,int>> selections(popSize);

    // Generate a pair of parents for each new individual in the next generation.
    std::generate(selections.begin(), selections.end(), [&generator, &probabilities]() {
        std::pair<int,int> parents;
        std::uniform_real_distribution<double> uniformDist(0.0, 1.0);

        // Select the first parent using roulette wheel selection. This means that individuals with higher fitness have a higher chance of being selected.
        double randDouble = uniformDist(generator);

        double sum = probabilities[0];

        int i = 0;

        while (sum < randDouble) {
            ++i;
            sum += probabilities[i];
        }

        parents.first = i;

        // Select the second parent. The same roulette wheel selection process is used, so individuals with higher fitness again have a higher chance of being selected.
        randDouble = uniformDist(generator);

        sum = probabilities[0];

        i = 0;

        while (sum < randDouble) {
            ++i;
            sum += probabilities[i];
        }

        parents.second = i;

        return parents;
    });

    // Return the vector of parent pairs for the next generation.
    return selections;
}


// Function to divide each element of a vector by a given denominator
std::vector<double> divEachBy(const std::vector<double>& v, double denominator) {
    std::vector<double> ret;
    // std::transform is used to perform an operation on each element of the vector 'v'.
    // In this case, each element of the vector is divided by the 'denominator' 
    // and the result is stored in the 'ret' vector.
    std::transform(v.begin(), v.end(), std::back_inserter(ret), [denominator](const double& a) {
        return a / denominator;
    });
    // The vector 'ret', containing the result of the division operation for each element of 'v', is returned.
    return ret;
}

// function that outputs selected pairs to log file
void OutputSelectedPairs(std::string_view fileName, const std::vector<std::pair<int,int>>& selections) {
    std::ofstream out;

    out.open(fileName.data(), std::ios_base::app);


    out << "SELECTED PAIRS:" << '\n';

    for(auto select: selections) {
        out << '(' << select.first << ',' << select.second << ')' << '\n';

        
    }
}

// This function implements the crossover and mutation operations for the genetic algorithm.
// It takes as inputs the pairs of parents selected for crossover, the list of locations, a random number generator, 
// the size of the population, the current population, and the chance of mutation.
Population Crossover(const std::vector<std::pair<int,int>>& selections, const std::vector<Location>& locations, std::mt19937& generator, 
                     int popSize, const Population& currentPop, int mutationChanceInt) {

    std::vector<std::vector<int>> newPop; // Vector to hold the new population.

    // std::transform applies a function to each member of the selections vector.
    // The function performs crossover between pairs of parents and applies mutation.
    std::transform(selections.begin(), selections.end(), std::back_inserter(newPop), 
                   [&generator, popSize, &locations, &currentPop, mutationChanceInt](const std::pair<int,int>& parents) {
        
        // Generate a random index for the crossover point.
        std::uniform_int_distribution<int> distribution(1, locations.size() - 2);
        int crossoverIndex = distribution(generator);
        std::vector<int> newMem; // Vector to hold the new member of the population.

        // Randomly select which parent will contribute the first part of the genome.
        std::uniform_int_distribution<int> binaryOut(0,1);
        int chooser = binaryOut(generator);
        int firstParent = 0;
        int secondParent = 0;
        if (chooser == 0) {
            firstParent = parents.second;
            secondParent = parents.first;
        }
        else {
            firstParent = parents.first;
            secondParent = parents.second;                                                                                                                           
        }

        // Copy the first part of the genome from the first parent.
        std::copy_n(currentPop.mMembers[firstParent].begin(), crossoverIndex + 1, std::back_inserter(newMem));

        // Copy the remaining part of the genome from the second parent, skipping any genes already present.
        std::copy_if(currentPop.mMembers[secondParent].begin(), currentPop.mMembers[secondParent].end(), std::back_inserter(newMem), [&newMem](const int& i) {
            return std::find(newMem.begin(), newMem.end(), i) == newMem.end();
        });

        // Decide whether to apply mutation.
        std::uniform_real_distribution<double> mutation;
        double mutationDoub = mutation(generator);
        double mutationChance = static_cast<double>(mutationChanceInt)/100.0;

        // If the randomly generated value is less than or equal to the mutation chance, apply mutation.
        if (mutationDoub <= mutationChance) {
            std::uniform_int_distribution<int> mutationSwap(1, newMem.size() - 1);
            int randomFirstIndex = mutationSwap(generator);
            int randomSecondIndex = mutationSwap(generator);
            std::swap(newMem[randomFirstIndex], newMem[randomSecondIndex]); // Swap two genes to apply mutation.
        }

        return newMem; // Return the new member of the population.
    });

   Population returnPop; // Create a new Population object.
   returnPop.mMembers = newPop; // Set the members of the new population.

   return returnPop; // Return the new population.
}


// functions that output the generations and solutions to the log file
void OutputGeneration(std::string_view fileName, int genNumber, const Population& pop){
    std::ofstream out(fileName.data(), std::ios_base::app);


    out << "GENERATION: " << genNumber << '\n';

    for(auto population: pop.mMembers) {
        out << '0';
        for(size_t j = 1; j < population.size(); j++) {
            out << ',' << std::to_string(population[j]);
        }
        out << '\n';

        
    }
}

void OutputSolution(std::string_view fileName, const std::vector<Location>& locations, const std::vector<int>& minDistanceVector, double minDistance) {
    std::ofstream out(fileName.data(), std::ios_base::app);


    out << "SOLUTION: " <<  '\n';

    for(auto element: minDistanceVector) {
        out << locations[element].mName << '\n';
        
        

        
    }

    out << locations[0].mName << '\n';
    out << "DISTANCE: " << minDistance << " miles";

}