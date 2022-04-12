/*
 * resultWriter.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef RESULTWRITER_H_
#define RESULTWRITER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>
#include "currentStateVariables.h"
#include "solutionStoreMap.h"


/**
 * @brief The resultWriter class contains methods meant to help writing the multiple solutions captured to .csv file
 */
class resultWriter
{
private:
    std::string m_filename;
    std::vector<std::vector<std::string>> m_solutionVector;
    std::map<std::string, std::map<std::string, std::string>> m_solutionMap;
    int m_headerState;
public:

    resultWriter(std::string fileName, std::vector<std::vector<std::string>> solutionVector, int headState=0);
//    resultWriter(std::string fileName, std::vector<std::vector<std::string>> solutionVector, int headState=0, std::string diversityMethod);
    resultWriter(std::string fileName, std::map<std::string, std::map<std::string, std::string>> solutionMap, int headState=0);

    /**
     * writeSolutions_ToCsv writes all multiple solutions captured during the solving of the MIP to .csv file
     * @return 0 if all went well or -1 if something went wrong while writing the solutions to file.
     */
    int writeSolutions_ToCsv();

    /**
     * @brief setWrittenHeader : sets 0 if the header row of the .csv file has been written
     * @param headerSet 0 if we have not written the header row (e.g. if we are just beginning to write to file) 1 if the header row has been written
     */
    void setHeaderState(int headerState);

    /**
     * @brief getWrittenHeader : returns the state of the header row of the .csv file.
     * @return 0 if the header row of the .csv file has not yet been written and 1 if it has been written.
     */
    int getHeaderState();

    /**
     * @brief writeDiversity_ToCsv writes a record(s) to .csv file. Records include the diversity and other parameters of the captured during the solving process
     * @return 0 if all went well or -1 if something went wrong while writing the diversity to file.
     */
    int writeDiversity_ToCsv(solutionStoreMap record);
    int writeDiversity_ToCsv(solutionStoreMap record, std::string  FName);
};

#endif /* RESULTWRITER_H_ */
