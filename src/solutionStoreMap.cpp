/*
 * solutionStoreMap.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "solutionStoreMap.h"
//#include "solutionstoremap.h"

//std::map<std::string, std::map<std::string, float>> solutionStoreMap::m_solutionsMap{{"he",{{"cu", 0.0}}}};

void solutionStoreMap::setSolutionsMap(std::map<std::string, std::map<std::string, std::string>> newSolutionsMap)
{
     solutionStoreMap::m_solutionsMap = newSolutionsMap;
}

solutionStoreMap::solutionStoreMap()
{
    std::cout << "Starting  the store map ... \n";
    solutionStoreMap::m_solutionsMap.clear();
}


void solutionStoreMap::storeSolutionMap(std::map<std::string, std::map<std::string, std::string>> solutionsMap){
    solutionStoreMap::m_solutionsMap = solutionsMap;
}


void solutionStoreMap::updateSolution(std::string recordName, std::string attributeName, std::string attributeValue){
    solutionStoreMap::m_solutionsMap[recordName].insert(std::pair<std::string,std::string>(attributeName,attributeValue));
}


std::map<std::string, std::map<std::string, std::string>> solutionStoreMap::getSolutionsMap(){
    return solutionStoreMap::m_solutionsMap;
}


