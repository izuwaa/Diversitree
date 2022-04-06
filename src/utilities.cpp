/*
 * utilities.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "utilities.h"
//#include "utilities.h"

static currentStateVariables cState;

utilities::utilities()
{
    std::cout << "Starting Utility tools .... \n";
}



void setCurrentState(currentStateVariables &curState){
		cState = curState;
//		utilities::cState ;
}
currentStateVariables getCurrentState(){
		return cState;
}



std::vector<std::vector<float>> utilities::stringSolutionsToFloat(std::vector<std::vector<std::string>> stringedSolutions , bool vectorHasHeaders){

    std::vector<std::vector<float>> floatedSolutions;

    for (unsigned long x = 0; x < stringedSolutions.size(); x++){
        std::vector<float> tempSolution{};

        if(vectorHasHeaders){
            for (unsigned long t  = 1; t < stringedSolutions[x].size(); t+=2){
                tempSolution.push_back(std::stof(stringedSolutions[x][t]));
            }
        }else{ // vector does not have headers
            for (unsigned long t  = 0; t < stringedSolutions[x].size(); t++){
                tempSolution.push_back(std::stof(stringedSolutions[x][t]));
            }
        }


        floatedSolutions.push_back( tempSolution );

    }

    return floatedSolutions;
}


