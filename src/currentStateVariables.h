/*
 * currentStateVariables.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef SRC_CURRENTSTATEVARIABLES_H_
#define SRC_CURRENTSTATEVARIABLES_H_

#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_set>
#include "solutionStoreVector.h"

/**
 * @brief The currentStateVariables class Holds the current state varialbes of the problem been worked on.
 */
class currentStateVariables
	{
	public:
		std::string m_optionsFileName;
		std::string m_solverOption;
		std::string m_nodeType;
		float m_alphaValue;
		std::string m_optimizationProblems;
		int m_requestedNumberOfSolutions;
		float m_percentNearOptimal;
		int m_verbosity;
		float m_betaValue;
		std::string m_prioritizedParam;
		float m_solutioncutoff;
		float m_depthcutoff;
		int m_computeDiversity;
		int m_computeDiversityFromCsv;
		std::string m_csvFiles;
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> stopTime;
		float timeSpent;
		float diversity;
		float m_pMostDiverse;
		int m_numberSolutionsGenerated;
		float m_bestObjective;
		int m_objectiveCalculated;
		std::vector<std::string> m_bestSolutionValues;
		std::unordered_set<std::string> m_seenValues;
		int m_countedSolutionsSoFar;
		int m_maxTreeDepthSoFar;
		solutionStoreVector solutionPool;

//		std::unordered_set<int> m_seenNodes;
//		std::map<int,float> m_seenNodesMap;


		currentStateVariables();

		const std::string& optionsFileName() const;
		void setOptionsFileName(const std::string &newOptionsFileName);
		const std::string& solverOption() const;
		void setSolverOption(const std::string &newSolverOption);
		int nodeType() const;
		void setNodeType(int newNodeType);
		float alphaValue() const;
		void setAlphaValue(float newAlphaValue);
		const std::string& optimizationProblems() const;
		void setOptimizationProblems(const std::string &newOptimizationProblems);
		int requestedNumberOfSolutions() const;
		void setRequestedNumberOfSolutions(int newRequestedNumberOfSolutions);
		float percentNearOptimal() const;
		void setPercentNearOptimal(float newPercentNearOptimal);
		int verbosity() const;
		void setVerbosity(int newVerbosity);
		float betaValue() const;
		void setBetaValue(float newBetaValue);
		const std::string& prioritizedParam() const;
		void setPrioritizedParam(const std::string &newPrioritizedParam);
		float solutioncutoff() const;
		void setSolutioncutoff(float newSolutioncutoff);
		float depthcutoff() const;
		void setDepthcutoff(float newDepthcutoff);
		int computeDiversity() const;
		void setComputeDiversity(int newComputeDiversity);
		int computeDiversityFromCsv() const;
		void setComputeDiversityFromCsv(int newComputeDiversityFromCsv);
		const std::string& csvFiles() const;
		void setCsvFiles(const std::string &newCsvFiles);
		std::chrono::time_point<std::chrono::high_resolution_clock> getStartTime();
		void setStartTime(std::chrono::time_point<std::chrono::high_resolution_clock> newStartTime);
		std::chrono::time_point<std::chrono::high_resolution_clock> getStopTime();
		void setStopTime(std::chrono::time_point<std::chrono::high_resolution_clock> newStopTime);

		float getTimeSpent() const;
		void setTimeSpent(float newTimeSpent);
		float getDiversity() const;
		void setDiversity(float newDiversity);
		float pMostDiverse() const;
		void setPMostDiverse(float newPMostDiverse);
		int numberSolutionsGenerated() const;
		void setNumberSolutionsGenerated(int newNumberSolutionsGenerated);

		float getMBestObjective() const;
		void setMBestObjective(float mBestObjective);
		int getMObjectiveCalculated() const;
		void setMObjectiveCalculated(int mObjectiveCalculated);
		const std::vector<std::string>& getMBestSolutionValues() const;
		void setMBestSolutionValues(const std::vector<std::string> &mBestSolutionValues);
		const std::string& getMNodeType() const;
		void setMNodeType(const std::string &mNodeType);
		int getMCountedSolutionsSoFar();
		void setMCountedSolutionsSoFar(int mCountedSolutionsSoFar);
		int getMMaxTreeDepthSoFar();
		void setMMaxTreeDepthSoFar(int mMaxTreeDepthSoFar);
		const solutionStoreVector& getSolutionPool() const;
		void setSolutionPool(const solutionStoreVector &solutionPool);

//		const std::unordered_set<int>& getMSeenNodes() const;
//		void setMSeenNodes(const std::unordered_set<int> &mSeenNodes);
//
//		const std::map<int, float>& getMSeenNodesMap() const;
//		void setMSeenNodesMap(std::map<int, float> &mSeenNodesMap);

	};

#endif /* SRC_CURRENTSTATEVARIABLES_H_ */
