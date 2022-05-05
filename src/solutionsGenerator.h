/*
 * solutionsGenerator.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef SOLUTIONSGENERATOR_H_
#define SOLUTIONSGENERATOR_H_

#include "optionsReader.h"
#include "oneTreeGams.h"
#include <iostream>
#include <fstream>
#include "solutionStoreVector.h"
#include "currentStateVariables.h"


class solutionsGenerator
{
public:
    optionsReader m_allOptions;
//    currentStateVariables m_currentState;
    solutionsGenerator(optionsReader allOptions);
    solutionStoreVector generateSolutionsFromCSV(currentStateVariables &currentState);

};

#endif /* SOLUTIONSGENERATOR_H_ */
