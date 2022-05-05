/*
 * optionsReader.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "optionsReader.h"
//#include "optionsreader.h"

optionsReader::optionsReader()
	{
		optionsReader::m_optionsFileName = "";
	}

std::string optionsReader::getNodeType(int originalNodeType)
	{
		std::string stringNode = mp_nodeType.at(originalNodeType);
		return stringNode;
	}

void optionsReader::readOptions(std::string optionsFile, std::string fileType, char separator)
	{
		std::cout << "\n Trying to read the " << fileType << " file from " << optionsFile << " using '" << separator << "' as file separator. \n ";

		optionsReader::m_optionsFileName = optionsFile;

		std::map<std::string, int> inputMap { { "solver", 1 }, { "nodeType", 2 }, { "alphaValue", 3 }, { "optimizationProblem", 4 }, { "requestedNumberSolutions", 5 }, { "percentNearOptimal", 6 }, { "Verbosity", 7 }, { "beta", 8 }, { "priorotizedParam", 9 }, { "solutioncutoff", 10 }, { "depthcutoff", 11 }, { "computeDiversity", 12 }, { "computeDiversityFromCsv", 13 }, { "csvFile", 14 }, { "gamsWorkingDirectory", 15 }, { "gamsSystemDirectory", 16 }, { "gamsOptDirectory", 17 }, { "gamsGMSFileDirectory", 18 }, { "scipWorkingDirectory", 19 }, { "scipSystemDirectory", 20 }, { "scipOptDirectory", 21 }, { "scipMPSFileDirectory", 22 }, { "pMostDiverseSolutions", 23 } };

		std::string line;
		std::ifstream file(m_optionsFileName);

		if (file.good())
			{

				while (std::getline(file, line))
					{

//  Here I read a line from the file
						std::stringstream ss(line);
						std::string vals;
						int curRead = 0;
						int readingIndex { };

						while (std::getline(ss, vals, separator))
							{

								vals.erase(remove_if(vals.begin(), vals.end(), isspace), vals.end());
								if (vals.compare("#") == 0)
									{
										break;
									}
								else
									{

										if (curRead == 0)
											{
// We are just starting to read this parameter
												std::map<std::string, int>::iterator it = inputMap.find(vals);
												if (it == inputMap.end())
													{
														readingIndex = 1000;
														std::cout << "  Didnt find: " << vals << "\n";
													}
												else
													{
														curRead = 1;
														readingIndex = inputMap[vals];
														std::cout << "\n Reading: " << vals << " : " << "\n";
													}

											}
										else
											{
// We are populating the values of this parameter
												switch (readingIndex)
													{
													case 1:
														{
															optionsReader::m_solverOption = vals;
															std::cout << vals << ", ";
															break;
														}
													case 2:
														{
															optionsReader::m_nodeType.push_back(std::stoi(vals));
															std::cout << vals << ", ";
															break;
														}
													case 3:
														{
															optionsReader::m_alphaValue.push_back(std::stof(vals));
															std::cout << vals << ", ";
															break;
														}
													case 4:
														{
															optionsReader::m_optimizationProblems.push_back(vals);
															std::cout << vals << ", ";
															break;
														}
													case 5:
														{
															optionsReader::m_requestedNumberOfSolutions.push_back(std::stoi(vals));
															std::cout << vals << ", ";
															break;
														}
													case 6:
														{
															optionsReader::m_percentNearOptimal.push_back(std::stof(vals));
															std::cout << vals << ", ";
															break;
														}
													case 7:
														{
															optionsReader::m_verbosity = std::stoi(vals);
															std::cout << vals << ", ";
															break;
														}
													case 8:
														{
															optionsReader::m_betaValue.push_back(std::stof(vals));
															std::cout << vals << ", ";
															break;
														}
													case 9:
														{
															optionsReader::m_prioritizedParam = vals;
															std::cout << vals << ", ";
															break;
														}
													case 10:
														{
															optionsReader::m_solutioncutoff.push_back(std::stof(vals));
															std::cout << vals << ", ";
															break;
														}
													case 11:
														{
															optionsReader::m_depthcutoff.push_back(std::stof(vals));
															std::cout << vals << ", ";
															break;
														}
													case 12:
														{
															optionsReader::m_computeDiversity = std::stoi(vals);
															std::cout << vals << ", ";
															break;
														}
													case 13:
														{
															optionsReader::m_computeDiversityFromCsv = std::stoi(vals);
															std::cout << vals << ", ";
															break;
														}
													case 14:
														{
															optionsReader::m_csvFiles.push_back(vals);
															std::cout << vals << ", ";
															break;
														}
													case 15:
														{
															optionsReader::m_gamsWorkingDirectory = vals;
															std::cout << vals << ", ";
															break;
														}
													case 16:
														{
															optionsReader::m_gamsSystemDirectory = vals;
															std::cout << vals << ", ";
															break;
														}
													case 17:
														{
															optionsReader::m_gamsOptDirectory = vals;
															std::cout << vals << ", ";
															break;
														}
													case 18:
														{
															optionsReader::m_gamsGMSFileDirectory = vals;
															std::cout << vals << ", ";
															break;
														}
													case 19:
														{
															optionsReader::m_scipWorkingDirectory = vals;
															std::cout << vals << ", ";
															break;
														}
													case 20:
														{
															optionsReader::m_scipSystemDirectory = vals;
															std::cout << vals << ", ";
															break;
														}
													case 21:
														{
															optionsReader::m_scipOptDirectory = vals;
															std::cout << vals << ", ";
															break;
														}
													case 22:
														{
															optionsReader::m_scipMPSFileDirectory = vals;
															std::cout << vals << ", ";
															break;
														}
													case 23:
														{
															optionsReader::m_pMostDiverseSolutions = std::stoi(vals);
															std::cout << vals << ", ";
															break;
														}
													default:
														std::cout << "Unknown Parameter. Please use correct input template \n ";
													}

											}

									}

							}
					}

			}

	}

