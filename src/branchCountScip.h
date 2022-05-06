/*
 * branchCoutScip.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef BRANCHCOUNTSCIP_H_
#define BRANCHCOUNTSCIP_H_

#include <iostream>
#include "solutionStoreVector.h"
#include "solutionsGenerator.h"
#include "optionsReader.h"
#include "currentStateVariables.h"
#include "utilities.h"
#include "diversityCalculator.h"

#include "scip/scip.h"
#include "scip/scip_nodesel.h"
#include "scip/scip_numerics.h"
#include "scip/scipdefplugins.h"
#include "scip/dialog_default.h"
#include <fstream>
#include <sstream>
#include <valarray>
#include <stdio.h>
#include <array>
#include <string>
#include <vector>
#include <tuple>
#include <numeric>
#include <map>
#include <chrono>

class branchCountScip: public solutionsGenerator
	{
	public:
		branchCountScip(optionsReader &allOptions);
		//		branchCountScip(int message);
		void generateSolutionsFromMIP(solutionStoreVector &generatedSolution, currentStateVariables &currentState);
		SCIP_RETCODE runSCIP(solutionStoreVector &generatedSolution, currentStateVariables &currentState);

		//TODO ADD fileFormat as a parameter that can be passed
		SCIP_RETCODE phaseOne(SCIP *scip, const char *variableNameForScip, const char *filename, const char *fileFormat, solutionStoreVector &generatedSolution, currentStateVariables &currentState);
		SCIP_RETCODE phaseTwo(SCIP *scip, const char *variableNameForScip, const char *filename, const char *fileFormat, solutionStoreVector &generatedSolution, currentStateVariables &currentState);
		SCIP_RETCODE captureSolutions(SCIP *scip, solutionStoreVector &generatedSolution, currentStateVariables &currentState);
		static float getNodeDiversityValarray(SCIP *scip, std::vector<float> &node1BranchMap /**< This is Node 1 Branch */, std::vector<
				float> &node2BranchMap /**< This is Node 2 Branch */,
				bool computeNode1, /**< If this is True, then compute diversity for node 1 */
				bool computeNode2, /**< If this is True, then compute diversity for node 2 */
				bool solCountChanged, /**< If this is False, then use old solutions pool, else, update new solution pool */
				float &node1Div, float &node2Div, int node1Key, int node2Key, std::vector<bool> &node1ValarraySelector, std::vector<
				bool> &node2ValarraySelector, currentStateVariables &currentState);
	};

#endif /* BRANCHCOUNTSCIP_H_ */
