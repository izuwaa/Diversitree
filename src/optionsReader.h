/*
 * optionsReader.h
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#ifndef OPTIONSREADER_H_
#define OPTIONSREADER_H_

#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

class optionsReader
	{

	public:

// MAP For the NodeTypes
		std::map<int, std::string> mp_nodeType = { { 0, "DiversiTree" }, { 1, "bfs" }, { 2, "dfs" }, { 3, "uct" }, { 4, "hybridestim" }, { 5, "breadthfirst" } };

// Option Parameters needed for solving the problems : problemParameter
		std::string m_optionsFileName;
		std::string m_solverOption;
		std::vector<int> m_nodeType;
		std::vector<float> m_alphaValue;
		std::vector<std::string> m_optimizationProblems;
		std::vector<int> m_requestedNumberOfSolutions;
		std::vector<float> m_percentNearOptimal;
		int m_verbosity;
		std::vector<float> m_betaValue;
		std::string m_prioritizedParam;
		std::vector<float> m_solutioncutoff;
		std::vector<float> m_depthcutoff;
		int m_computeDiversity;
		int m_computeDiversityFromCsv;
		std::vector<std::string> m_csvFiles;
		int m_pMostDiverseSolutions;

// Option Parameters needed for the GAMS and SCIP Solver
		std::string m_gamsWorkingDirectory = "";//TODO need to assert that this is not null when solving with GAMS
		std::string m_gamsSystemDirectory = "";//TODO need to assert that this is not null when solving with GAMS
		std::string m_gamsOptDirectory = "";
		std::string m_gamsGMSFileDirectory = "";// TODO need to assert that this is not null when solving with GAMS
		std::string m_scipWorkingDirectory = "";// TODO need to assert that this is not null when solving with SCIP
		std::string m_scipSystemDirectory = "";// TODO need to assert that this is not null when solving with SCIP
		std::string m_scipOptDirectory = "";
		std::string m_scipMPSFileDirectory = "";// TODO need to assert that this is not null when solving with SCIP

		optionsReader();
		void readOptions(std::string optionsFile, std::string fileType = "problemOptions", char separator = ',');
		std::string getNodeType(int originalNodeType);

	};

#endif /* OPTIONSREADER_H_ */
