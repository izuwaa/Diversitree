/*
 * utilities.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <vector>
#include <iostream>
#include "currentStateVariables.h"

class utilities
	{
	public:

		utilities();
		std::vector<std::vector<float>> stringSolutionsToFloat(std::vector<std::vector<std::string>> stringedSolutions, bool vectorHasHeaders);

	};

//void setCurrentState(currentStateVariables &curState);
//currentStateVariables getCurrentState();

#endif /* UTILITIES_H_ */
