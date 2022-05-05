/*
 * resultWriter.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "resultWriter.h"
//#include "resultwriter.h"
#include <sstream>


resultWriter::resultWriter(std::string fileName, std::vector<std::vector<std::string>> solutionVector, int headState){

    resultWriter::m_filename = fileName;
    resultWriter::m_solutionVector = solutionVector;
    resultWriter::m_headerState =  headState; /// Set this head to zero during initialization.

}


resultWriter::resultWriter(std::string fileName, std::map<std::string, std::map<std::string, std::string>> solutionMap, int headState){

    resultWriter::m_filename = fileName;
    resultWriter::m_solutionMap = solutionMap;
    resultWriter::m_headerState =  headState; /// Set this head to zero during initialization.

}


int resultWriter::writeSolutions_ToCsv() {

    std::string fileN = resultWriter::m_filename;
    std::vector<std::vector<std::string>> solutionVector  = resultWriter::m_solutionVector;

    //TODO Use Verbosity Level to determine how to output this line
    //	std::cout << " Writing Results " << std::endl;
    std::cout << "Writing solutions to csv " << "\n";

    std::ofstream fileName(fileN); //, std::ios_base::app

    //	int headerSet = 0;
    std::vector<float> tmpR;
    int count = 0;
    int solVectorSize = solutionVector.size();

    std::cout << "\n Result Row Size: " << solVectorSize << " \n ";

    for (int v = 0; v < solVectorSize; v++) {
        int innerSize = solutionVector[v].size();

        // Iterate over the map using c++11 range based for loop
        for (int k = 0; k < innerSize; k+= 2) {
            // Accessing KEY from element

            std::string outerKey = solutionVector[v][k]; //element.first;
            // Accessing VALUE from element.
            float outerVal = std::stof(solutionVector[v][k+1]); //element.second;

            if (count == 0) {
                fileName << "SN" << ",";
                count += 1;
            }

            if (resultWriter::getHeaderState() == 0) {
                fileName << outerKey << ",";
                tmpR.push_back(outerVal);

            } else if (resultWriter::getHeaderState() == 1) {
                fileName << outerVal << ",";
            }

        }

        if ( resultWriter::getHeaderState() == 0) {
            fileName << "\n";
            fileName << count << ",";
            count += 1;

            for (unsigned long i = 0; i < tmpR.size(); i++) {
                fileName << tmpR[i] << ",";
            }

            resultWriter::setHeaderState(1);
        }

        fileName << "\n";
        fileName << count << ",";
        count += 1;

    }

    fileName.close();
    return 1;

}




int resultWriter::writeDiversity_ToCsv(solutionStoreMap record) {

    //TODO Use Verbosity Level to determine how to output this line
    printf( "Writing diversity results to csv \n" );
    printf( "headState: %i \n", resultWriter::getHeaderState() );

    //    std::cout << "Writing diversity results to csv " << "\n";
    //    std::cout << "headState: " << resultWriter::getHeaderState() <<  std::endl;

    std::stringstream ss;

    std::string fileN = resultWriter::m_filename;
    std::ofstream fileName(fileN, std::ios_base::app); //, std::ios_base::app

    //    std::ifstream infile(fileName);
    //    return infile.good();




    //	std::ofstream resultFile("results.csv");

    //    std::cout << "\n Result Row Size: " << record.getSolutionsMap().size();
    printf("\n Result Row Size: %i ", static_cast<int>( record.getSolutionsMap().size()  ));

    //	int headerSet = 0;
    std::vector<std::string> tmpR { };

    //    std::map<std::string, std::map<std::string, std::string>>::iterator it = record.getSolutionsMap().begin();

    // Iterate over the map using c++11 range based for loop
    for (const auto& element : record.getSolutionsMap()) { //start for loop
        // Accessing KEY from element
        std::string outerKey = element.first;
        // Accessing VALUE from element.
        std::map<std::string, std::string> outerVal = element.second;

        if (resultWriter::getHeaderState() == 0) {
            ss << "Problem";
            //            fileName << "Problem";
            resultWriter::setHeaderState(1);
        } else {

            ss << "\n";
            ss << outerKey;

            //            fileName << "\n";
            //            fileName << outerKey;
        }

        //        std::map<std::string, std::string>::iterator innerIt = outerVal.begin();

        for (const auto& innerElement : outerVal) {
            // Accessing KEY from element
            std::string innerKey = innerElement.first;
            // Accessing VALUE from element.
            std::string innerValue = innerElement.second;

            if (resultWriter::getHeaderState() == 1) {
                ss << "," << innerKey;
                //                fileName << "," << innerKey;
                tmpR.push_back(innerValue);
            }
            else {
                ss << "," << std::setprecision(12) << innerValue;
                //                fileName << "," << std::setprecision(12) << innerValue;
            }

        }

        if (resultWriter::getHeaderState() == 1) {
            ss << "\n";
            ss << outerKey;
            //            fileName << "\n";
            //            fileName << outerKey;

            for (int v = 0; v < static_cast<int>(tmpR.size()); v++) {
                ss << "," << std::setprecision(12) << tmpR[v];
                //                fileName << "," << std::setprecision(12) << tmpR[v];
            }
            resultWriter::setHeaderState(2); // setFileWriterHeadSet(2); // = 2;
        }

    } //start for loop


    fileName << ss.str();
    fileName.close();
    return 0;

}



int resultWriter::writeDiversity_ToCsv(solutionStoreMap record,  std::string FName) {

    //TODO Use Verbosity Level to determine how to output this line
    std::cout << "Writing diversity results to csv " << "\n";
    std::cout << "headState: " << resultWriter::getHeaderState() <<  "\n";

    std::string fileN = FName;

    //    std::ifstream infile(fileName);
    //    return infile.good();
    std::stringstream ss;
    std::ofstream fileName(fileN, std::ios_base::app); //, std::ios_base::app


    //	std::ofstream resultFile("results.csv");

    std::cout << "\n Result Row Size: " << record.getSolutionsMap().size();

    //	int headerSet = 0;
    std::vector<std::string> tmpR { };

    //    std::map<std::string, std::map<std::string, std::string>>::iterator it = record.getSolutionsMap().begin();

    // Iterate over the map using c++11 range based for loop
    for (const auto& element : record.getSolutionsMap()) { //start for loop
        // Accessing KEY from element
        std::string outerKey = element.first;
        // Accessing VALUE from element.
        std::map<std::string, std::string> outerVal = element.second;

        if (resultWriter::getHeaderState() == 0) {
            ss << "Problem";
            //            fileName << "Problem";
            resultWriter::setHeaderState(1);
        } else {
            ss << "\n";
            ss << outerKey;
            //            fileName << "\n";
            //            fileName << outerKey;
        }

        //        std::map<std::string, std::string>::iterator innerIt = outerVal.begin();

        for (const auto& innerElement : outerVal) {
            // Accessing KEY from element
            std::string innerKey = innerElement.first;
            // Accessing VALUE from element.
            std::string innerValue = innerElement.second;

            if (resultWriter::getHeaderState() == 1) {
                ss << "," << innerKey;
                //                fileName << "," << innerKey;
                tmpR.push_back(innerValue);
            }
            else {
                ss << "," << std::setprecision(12) << innerValue;
                //                fileName << "," << std::setprecision(12) << innerValue;
            }

        }

        if (resultWriter::getHeaderState() == 1) {
            ss << "\n";
            ss << outerKey;
            //            fileName << "\n";
            //            fileName << outerKey;

            for (int v = 0; v < static_cast<int>(tmpR.size()); v++) {
                ss << "," << std::setprecision(12) << tmpR[v];
                //                fileName << "," << std::setprecision(12) << tmpR[v];
            }
            resultWriter::setHeaderState(2); // setFileWriterHeadSet(2); // = 2;
        }

    } //start for loop

    fileName << ss.str();
    fileName.close();
    return 0;

}




/**
 * @brief setWrittenHeader : sets 0 if the header row of the .csv file has been written
 * @param headerSet 0 if we have not written the header row (e.g. if we are just beginning to write to file) 1 if the header row has been written
 */
void resultWriter::setHeaderState(int headerState){
    resultWriter::m_headerState = headerState;
}

/**
 * @brief getWrittenHeader : returns the state of the header row of the .csv file.
 * @return 0 if the header row of the .csv file has not yet been written and 1 if it has been written.
 */
int resultWriter::getHeaderState(){
    return resultWriter::m_headerState;
}
