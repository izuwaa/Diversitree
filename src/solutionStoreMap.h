/*
 * solutionStoreMap.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef SOLUTIONSTOREMAP_H_
#define SOLUTIONSTOREMAP_H_


#include <iostream>
#include <map>


class solutionStoreMap
{
public:
    std::map<std::string, std::map<std::string, std::string>> m_solutionsMap;

    solutionStoreMap();
    void storeSolutionMap(std::map<std::string, std::map<std::string, std::string>> solutionsMap); //  Store a full set of solutionsMap

//    template<class T>
    void updateSolution(std::string recordName, std::string attributeName, std::string attributeValue);  //  Store a record with recordName, attributeName and attributeValue

    std::map<std::string, std::map<std::string, std::string>> getSolutionsMap();
    void setSolutionsMap(std::map<std::string, std::map<std::string, std::string>> newSolutionsMap);
};


#endif /* SOLUTIONSTOREMAP_H_ */
