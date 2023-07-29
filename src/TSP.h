#pragma once
#include <string>
#include <vector>
#include <random>

struct Location
{
	std::string mName;
	double mLatitude = 0.0;
	double mLongitude = 0.0;
};

struct Population
{
	std::vector<std::vector<int>> mMembers;
};

std::vector<Location> ReadLocations(std::string_view inputFile);

Population FillInitialPopulation (int popSize, std::mt19937& generator, size_t locationSize);

void OutputPopulationFile(std::string_view fileName, const Population& pop, std::string_view header);

std::vector<std::pair<int,double>> computeFitnesses(const Population& population, const std::vector<Location>& locations);

double GetHaversineDistance(const double& lon1, const double& lat1, const double& lon2, const double& lat2);

void OutputFitnessFile(std::string_view fileName, const std::vector<std::pair<int,double>>& fits);

std::vector<std::pair<int,int>> Select(std::vector<std::pair<int,double>>& fitnesses, std::mt19937& generator, int popSize);

std::vector<double> divEachBy(const std::vector<double>& v, double denominator);

void OutputSelectedPairs(std::string_view fileName, const std::vector<std::pair<int,int>>& selections);

Population Crossover(const std::vector<std::pair<int,int>>& selections, const std::vector<Location>& locations, std::mt19937& generator, int popSize, const Population& currentPop, int mutationChanceInt);


void OutputGeneration(std::string_view fileName, int genNumber, const Population& pop);

void OutputSolution(std::string_view fileName, const std::vector<Location>& locations, const std::vector<int>& minDistanceVector, double minDistance);