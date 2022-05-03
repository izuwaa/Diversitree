/*
 * diversityCalculator.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef DIVERSITYCALCULATOR_H_
#define DIVERSITYCALCULATOR_H_

#include <iostream>
#include <vector>
#include <valarray>
#include "solutionStoreVector.h"
#include "utilities.h"
#include "currentStateVariables.h"

class diversityCalculator
{
public:
    std::vector<std::vector<float>> m_solutionMap{};
    std::vector<bool> m_binaryLocations;
//    float m_diversityScore;

    diversityCalculator(solutionStoreVector solutions);
    int dbinAll(currentStateVariables &currentState);
    int dbinP(currentStateVariables &currentState);
    float dbinQuick(std::vector<std::vector<float>> solutionVector);
    float dbinNode(std::vector<float> solutionVector);
//    float dbinNode(std::vector<float> solutionVector);

};

#endif /* DIVERSITYCALCULATOR_H_ */
