/*
 * solutionsGenerator.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "solutionsGenerator.h"
//#include "solutionsgenerator.h"

solutionsGenerator::solutionsGenerator(optionsReader allOptions)
	{
		solutionsGenerator::m_allOptions = allOptions;
//    solutionsGenerator::m_currentState = currentState;
	}

//solutionStoreVector solutionsGenerator::solve(currentStateVariables &currentState){
//
//    // Check if GAMS or SCIP is selected
//    if (solutionsGenerator::m_allOptions.m_solverOption == "GAMS"){
//        std::cout << "\n We are using One Tree algorithm on GAMS and CPLEX  \n" ;
//
//        // Create the cplex.opt file based on the options set in the problemOptions.txt
//        std::string fileN = "cplex.opt";
//        std::cout << "\n Writing cplex.opt \n" << std::endl;
//        std::ofstream fileName(solutionsGenerator::m_allOptions.m_gamsOptDirectory + "/" +  fileN);
//        fileName << "*mip cplex" << "\n";
//        fileName << "solnpool generatedSoln.gdx" << "\n";
//        fileName << "solnpoolpop 2" << "\n";
//        fileName << "solnpoolintensity 4" << "\n";
//        fileName << "solnpoolcapacity " << currentState.requestedNumberOfSolutions() << "\n";
//        fileName << "solnpoolgap " << currentState.percentNearOptimal() << "\n";
//        fileName << "populatelim " << currentState.requestedNumberOfSolutions() << "\n";
//        fileName << "*ResLim 5" << "\n";
//        fileName << "*tilim 5" << "\n";
//        fileName.close();
//
//
//        //TODO This call to oneTree needs to be part of the loop above with the filenames given.
//        // Create a copy of the oneTree algorithm from GAMS and Cplex and call the generateMultiple solutions
//        solutionStoreVector generatedSolution;
//        oneTreeGams otg(generatedSolution, solutionsGenerator::m_allOptions, currentState);
//        //        oneTreeGams otg(generatedSolution, solutionsGenerator::m_allOptions.m_gamsWorkingDirectory, solutionsGenerator::m_allOptions.m_gamsSystemDirectory, solutionsGenerator::m_allOptions.m_gamsGMSFileDirectory, solutionsGenerator::m_currentState.optimizationProblems(), solutionsGenerator::m_allOptions.m_gamsOptDirectory);
//
//        return generatedSolution;
//
//    }
//    else if (solutionsGenerator::m_allOptions.m_solverOption == "SCIP"){
//        std::cout << "\n We are using DiversiTree algorithm on SCIP. It is not yet implemented \n" ;
//        EXIT_FAILURE;
//    }
//    else{
//        std::cout << "\n ERROR: Unknown solver selected. Fix this by including this line:- 'solver, GAMS, #, GAMS or SCIP' in the file 'inputParams.txt'.  \n" ;
//        EXIT_FAILURE;
//    }
//
//    EXIT_FAILURE;
//}
//

solutionStoreVector solutionsGenerator::generateSolutionsFromCSV(currentStateVariables &currentState)
	{
//solutionStoreVector solutionsGenerator::getSolutionsFromCSV(currentStateVariables &currentState){

// Try to read the CSVFile. If the file is good, then get the solutions from csv

		printf("Trying to read the input CSV file: %s \n", currentState.csvFiles().c_str());

		std::string line;
		float fileLength { };
		float fileWidth { };

		std::ifstream file(currentState.csvFiles());
		solutionStoreVector generatedSolution;

		if (file.good())
			{
				std::cout << " File ok \n ";

				while (std::getline(file, line))
					{
						std::stringstream ss(line);

						std::vector<std::string> tmp { };
						int counts { };

						while (std::getline(ss, line, ','))
							{

//                float ftmp { };
								std::string ftmp { };
								if (counts > 0 and fileLength > 0)
									{

//                    ftmp = std::stof(line);
										ftmp = line;
										tmp.push_back(ftmp);
										counts += 1;

									}
								else
									{
										counts += 1;
									}

							}

						if (tmp.size() > 0)
							{
								generatedSolution.updateSolution(tmp);
//                fileContent.push_back(tmp);
								fileLength += 1;
							}
						else if (fileLength == 0)
							{
								fileLength += 1;
							}

						if (counts > fileWidth)
							{
								fileWidth = counts - 1;
							}

					}

//Display All Input Values read:
				printf("CSV File has the following properties: \n");
				printf("File Length: %f \n", fileLength - 1);
				printf("File Width: %f \n", fileWidth);
				printf("\n continuing your task....... \n");

//        std::cout << " CSV File has the following properties: " << std::endl;
//        std::cout << "\n File Length: " << fileLength - 1;
//        std::cout << "\n File Width: " << fileWidth << " \n";
//        std::cout << "\n continuing your task....... \n";

				file.close();
				return generatedSolution;

			}
		else
			{
//		std::cout << " ERror with CSV";
				printf("\n File:ERROR: File not found .  \n");
				exit(1);
			}

	}

