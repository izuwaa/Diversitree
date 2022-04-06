/*
 * solutionStoreVector.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "solutionStoreVector.h"
//#include "solutionstorevector.h"

solutionStoreVector::solutionStoreVector()
{

    solutionStoreVector::m_solutionsVector.clear();
    solutionStoreVector::m_binaryLocationsVector.clear();

}

void  solutionStoreVector::updateSolution(std::vector<std::string> newSolution, std::vector<bool> newBinaryLocations){
    solutionStoreVector::m_solutionsVector.push_back(newSolution);
    solutionStoreVector::m_binaryLocationsVector  = newBinaryLocations;
}


void  solutionStoreVector::updateSolution(std::vector<std::string> newSolution){
    solutionStoreVector::m_solutionsVector.push_back(newSolution);
}

void solutionStoreVector::updateSolutionBinaryLocations(std::vector<bool> newBinaryLocations){
		solutionStoreVector::m_binaryLocationsVector = newBinaryLocations;
}


void  solutionStoreVector::storeSolution(std::vector<std::vector<std::string>> solutionsVector, std::vector<bool> binaryLocationsVector){
    solutionStoreVector::m_solutionsVector = solutionsVector;
    solutionStoreVector::m_binaryLocationsVector  = binaryLocationsVector;
}


std::vector<std::vector<std::string>> solutionStoreVector::getSolutionsVector(){
    return solutionStoreVector::m_solutionsVector ;
}


std::vector<bool> solutionStoreVector::getBinaryLocationsVector(){
    return solutionStoreVector::m_binaryLocationsVector ;
}

void solutionStoreVector::clear(){

		solutionStoreVector::m_solutionsVector.clear();
		solutionStoreVector::m_binaryLocationsVector.clear();

}



