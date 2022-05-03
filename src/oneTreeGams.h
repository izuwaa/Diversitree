/*
 * oneTreeGams.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef ONETREEGAMS_H_
#define ONETREEGAMS_H_

#include <iostream>
#include "solutionStoreVector.h"
#include "optionsReader.h"
#include "currentStateVariables.h"

class oneTreeGams
{
public:
    oneTreeGams(solutionStoreVector &generatedSolution, std::string workingDirectory, std::string systemDirectory, std::string gmsFileDirectory,  std::string gmsFile,  std::string optionsDirectory = "NA");
    oneTreeGams(solutionStoreVector &generatedSolution, optionsReader &allOptions, currentStateVariables &currentState);

};

#endif /* ONETREEGAMS_H_ */
