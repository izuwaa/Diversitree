/*
 * solutionStoreVector.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef SOLUTIONSTOREVECTOR_H_
#define SOLUTIONSTOREVECTOR_H_

#include <vector>
#include <map>
#include <iostream>


/**
 * @brief The solutionStore class
 * Holds all computed solutions for operations in other classes
 */
class solutionStoreVector
{
public:
    std::vector<std::vector<std::string>> m_solutionsVector;
    std::vector<bool> m_binaryLocationsVector;

    solutionStoreVector();
    void storeSolution(std::vector<std::vector<std::string>> solutionsVector, std::vector<bool> binaryLocationsVector); //  Store a full set of solutionsVector and binaryLocationsVector
    void updateSolution(std::vector<std::string> newSolution, std::vector<bool> newBinaryLocations);  //  Store a set of solutionsVector and binaryLocationsVector one vector at a time
    void updateSolution(std::vector<std::string> newSolution);  //  Store a set of solutionsVector without the binaryLocationsVector one vector at a time
    void updateSolutionBinaryLocations(std::vector<bool> newBinaryLocations);  //  Store a set of binary locations without the solutionsVector
    std::vector<std::vector<std::string>> getSolutionsVector();
    std::vector<bool> getBinaryLocationsVector();
    void clear();

};

#endif /* SOLUTIONSTOREVECTOR_H_ */
