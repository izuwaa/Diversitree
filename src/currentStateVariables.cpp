/*
 * currentStateVariables.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "currentStateVariables.h"

//#include "currentstatevariables.h"

const std::string& currentStateVariables::optionsFileName() const
	{
		return m_optionsFileName;
	}

void currentStateVariables::setOptionsFileName(const std::string &newOptionsFileName)
	{
		m_optionsFileName = newOptionsFileName;
	}

const std::string& currentStateVariables::solverOption() const
	{
		return m_solverOption;
	}

void currentStateVariables::setSolverOption(const std::string &newSolverOption)
	{
		m_solverOption = newSolverOption;
	}

float currentStateVariables::alphaValue() const
	{
		return m_alphaValue;
	}

void currentStateVariables::setAlphaValue(float newAlphaValue)
	{
		m_alphaValue = newAlphaValue;
	}

const std::string& currentStateVariables::optimizationProblems() const
	{
		return m_optimizationProblems;
	}

void currentStateVariables::setOptimizationProblems(const std::string &newOptimizationProblems)
	{
		m_optimizationProblems = newOptimizationProblems;
	}

int currentStateVariables::requestedNumberOfSolutions() const
	{
		return m_requestedNumberOfSolutions;
	}

void currentStateVariables::setRequestedNumberOfSolutions(int newRequestedNumberOfSolutions)
	{
		m_requestedNumberOfSolutions = newRequestedNumberOfSolutions;
	}

float currentStateVariables::percentNearOptimal() const
	{
		return m_percentNearOptimal;
	}

void currentStateVariables::setPercentNearOptimal(float newPercentNearOptimal)
	{
		m_percentNearOptimal = newPercentNearOptimal;
	}

int currentStateVariables::verbosity() const
	{
		return m_verbosity;
	}

void currentStateVariables::setVerbosity(int newVerbosity)
	{
		m_verbosity = newVerbosity;
	}

float currentStateVariables::betaValue() const
	{
		return m_betaValue;
	}

void currentStateVariables::setBetaValue(float newBetaValue)
	{
		m_betaValue = newBetaValue;
	}

const std::string& currentStateVariables::prioritizedParam() const
	{
		return m_prioritizedParam;
	}

void currentStateVariables::setPrioritizedParam(const std::string &newPrioritizedParam)
	{
		m_prioritizedParam = newPrioritizedParam;
	}

float currentStateVariables::solutioncutoff() const
	{
		return m_solutioncutoff;
	}

void currentStateVariables::setSolutioncutoff(float newSolutioncutoff)
	{
		m_solutioncutoff = newSolutioncutoff;
	}

float currentStateVariables::depthcutoff() const
	{
		return m_depthcutoff;
	}

void currentStateVariables::setDepthcutoff(float newDepthcutoff)
	{
		m_depthcutoff = newDepthcutoff;
	}

int currentStateVariables::computeDiversity() const
	{
		return m_computeDiversity;
	}

void currentStateVariables::setComputeDiversity(int newComputeDiversity)
	{
		m_computeDiversity = newComputeDiversity;
	}

int currentStateVariables::computeDiversityFromCsv() const
	{
		return m_computeDiversityFromCsv;
	}

void currentStateVariables::setComputeDiversityFromCsv(int newComputeDiversityFromCsv)
	{
		m_computeDiversityFromCsv = newComputeDiversityFromCsv;
	}

const std::string& currentStateVariables::csvFiles() const
	{
		return m_csvFiles;
	}

void currentStateVariables::setCsvFiles(const std::string &newCsvFiles)
	{
		m_csvFiles = newCsvFiles;
	}

std::chrono::time_point<std::chrono::high_resolution_clock> currentStateVariables::getStartTime()
	{
		return startTime;
	}

void currentStateVariables::setStartTime(std::chrono::time_point<std::chrono::high_resolution_clock> newStartTime)
	{
		startTime = newStartTime;
	}

std::chrono::time_point<std::chrono::high_resolution_clock> currentStateVariables::getStopTime()
	{
		return stopTime;
	}

void currentStateVariables::setStopTime(std::chrono::time_point<std::chrono::high_resolution_clock> newStopTime)
	{
		stopTime = newStopTime;
	}

float currentStateVariables::getTimeSpent() const
	{
		return timeSpent;
	}

void currentStateVariables::setTimeSpent(float newTimeSpent)
	{
		timeSpent = newTimeSpent;
	}

float currentStateVariables::getDiversity() const
	{
		return diversity;
	}

void currentStateVariables::setDiversity(float newDiversity)
	{
		diversity = newDiversity;
	}

float currentStateVariables::pMostDiverse() const
	{
		return m_pMostDiverse;
	}

void currentStateVariables::setPMostDiverse(float newPMostDiverse)
	{
		m_pMostDiverse = newPMostDiverse;
	}

int currentStateVariables::numberSolutionsGenerated() const
	{
		return m_numberSolutionsGenerated;
	}

void currentStateVariables::setNumberSolutionsGenerated(int newNumberSolutionsGenerated)
	{
		m_numberSolutionsGenerated = newNumberSolutionsGenerated;
	}

float currentStateVariables::getMBestObjective() const
	{
		return m_bestObjective;
	}

void currentStateVariables::setMBestObjective(float mBestObjective)
	{
		m_bestObjective = mBestObjective;
	}

int currentStateVariables::getMObjectiveCalculated() const
	{
		return m_objectiveCalculated;
	}

const std::vector<std::string>& currentStateVariables::getMBestSolutionValues() const
	{
		return m_bestSolutionValues;
	}

const std::string& currentStateVariables::getMNodeType() const
	{
		return m_nodeType;
	}



int currentStateVariables::getMCountedSolutionsSoFar()
	{
		return m_countedSolutionsSoFar;
	}

void currentStateVariables::setMCountedSolutionsSoFar(int mCountedSolutionsSoFar)
	{
		m_countedSolutionsSoFar = mCountedSolutionsSoFar;
	}

int currentStateVariables::getMMaxTreeDepthSoFar()
	{
		return m_maxTreeDepthSoFar;
	}

void currentStateVariables::setMMaxTreeDepthSoFar(int mMaxTreeDepthSoFar)
	{
		m_maxTreeDepthSoFar = mMaxTreeDepthSoFar;
	}

const solutionStoreVector& currentStateVariables::getSolutionPool() const
	{
		return solutionPool;
	}

void currentStateVariables::setSolutionPool(const solutionStoreVector &solutionPool)
	{
		this->solutionPool = solutionPool;
	}


void currentStateVariables::setMNodeType(const std::string &mNodeType)
	{
		m_nodeType = mNodeType;
	}

void currentStateVariables::setMBestSolutionValues(const std::vector<std::string> &mBestSolutionValues)
	{
		m_bestSolutionValues = mBestSolutionValues;
	}

void currentStateVariables::setMObjectiveCalculated(int mObjectiveCalculated)
	{
		m_objectiveCalculated = mObjectiveCalculated;
	}


const std::unordered_set<int>& currentStateVariables::getMSeenNodes() const
	{
		return m_seenNodes;
	}

void currentStateVariables::setMSeenNodes(const std::unordered_set<int> &mSeenNodes)
	{
		m_seenNodes = mSeenNodes;
	}

const std::map<int, float>& currentStateVariables::getMSeenNodesMap() const
	{
		return m_seenNodesMap;
	}

void currentStateVariables::setMSeenNodesMap(std::map<int, float> &mSeenNodesMap)
	{
		m_seenNodesMap = mSeenNodesMap;
	}



currentStateVariables::currentStateVariables()
	{

		currentStateVariables::m_optionsFileName = "";
		currentStateVariables::m_solverOption = "";
		currentStateVariables::m_alphaValue = 0;
		currentStateVariables::m_optimizationProblems = "";
		currentStateVariables::m_requestedNumberOfSolutions = 0;
		currentStateVariables::m_percentNearOptimal = 0;
		currentStateVariables::m_verbosity = 1;
		currentStateVariables::m_betaValue = 0;
		currentStateVariables::m_prioritizedParam = "";
		currentStateVariables::m_solutioncutoff = 0;
		currentStateVariables::m_depthcutoff = 0;
		currentStateVariables::m_computeDiversity = 0;
		currentStateVariables::m_computeDiversityFromCsv = 0;
		currentStateVariables::m_csvFiles = "";

//TODO change how this time variables are initialized
		currentStateVariables::startTime;
		currentStateVariables::stopTime;

		currentStateVariables::timeSpent = 0;
		currentStateVariables::diversity = 0;
		currentStateVariables::m_pMostDiverse = 0;
		currentStateVariables::m_numberSolutionsGenerated = 0;

//TODO Change how this variable is initialized
		currentStateVariables::m_nodeType = "";

		currentStateVariables::m_bestObjective = 0;
		currentStateVariables::m_objectiveCalculated = 0;

		currentStateVariables::m_bestSolutionValues.clear();
		currentStateVariables::m_seenNodesMap.clear();
		currentStateVariables::m_seenNodes.clear();
		currentStateVariables::m_seenValues.clear();

		currentStateVariables::m_countedSolutionsSoFar = 0;
		currentStateVariables::m_maxTreeDepthSoFar = 0;

	}

