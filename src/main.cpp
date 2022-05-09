//============================================================================
// Name        : main.cpp
// Author      : Izuwa Ahanor
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include "resultWriter.h"
#include "optionsReader.h"
#include "branchCountScip.h"
#include "solutionStoreVector.h"
#include "solutionStoreMap.h"
#include "currentStateVariables.h"
#include "diversityCalculator.h"
#include <dirent.h>
#include "utilities.h"

//using namespace std;

currentStateVariables curStateVariables { };

int main(int argc, char **argv)
	{

		/** THESE FILES HAVE ISSUES RUNNING
		 *
		 *  air03
		 *  mod010
		 *  vpm2
		 *
		 *  The input Params are:
		 *  fileName
		 *  File Type: One of problemOptions (Default) or solverOptions
		 *
		 */

		int fileAddCounter = 0;
		std::string inputFileName = argv[1];
		std::cout << " This is the name of the options file: " << inputFileName << " ........" << "\n";

		// Declare an options reader to read the problemOptions file and the solverOptions file
		optionsReader optReader;
		// Lets start by reading the problemOptions file
		char C = argv[3][0];
		optReader.readOptions(argv[1], argv[2], C);
		// Then we add the solverOptions file to the same object. This way, we have all our options in one Object
		C = argv[6][0];
		optReader.readOptions(argv[4], argv[5], C);

//		currentStateVariables currentState;
		curStateVariables.setSolverOption(optReader.m_solverOption);

		if (optReader.m_computeDiversityFromCsv == 0)
			{//computeDiversityFromCsv?  0 = NO, 1 = Yes

				for (unsigned long nearOptimalPercent = 0;
						nearOptimalPercent < optReader.m_percentNearOptimal.size(); nearOptimalPercent++)
					{// percentNearOptimal
						curStateVariables.setPercentNearOptimal(optReader.m_percentNearOptimal[nearOptimalPercent]);
						fileAddCounter = 0;

						for (unsigned long optiProblem = 0;
								optiProblem < optReader.m_optimizationProblems.size(); optiProblem++)
							{// optimizationProblem

								int pos = optReader.m_optimizationProblems[optiProblem].find(".");
								std::string optimizationProblem = optReader.m_optimizationProblems[optiProblem].substr(0, pos);
								curStateVariables.setOptimizationProblems(optimizationProblem);

								curStateVariables.setMObjectiveCalculated(0);
								curStateVariables.m_bestSolutionValues.clear();

								for (unsigned long reqNumSols = 0;
										reqNumSols < optReader.m_requestedNumberOfSolutions.size(); reqNumSols++)
									{//requestedNumberOfSolutions

										curStateVariables.setRequestedNumberOfSolutions(optReader.m_requestedNumberOfSolutions[reqNumSols]);

										curStateVariables.setComputeDiversity(optReader.m_computeDiversity);

										// Write the solutions or the Captured Diversity
										if (curStateVariables.computeDiversity() == 1)
											{//computeDiversity?  0 = Yes  Compute diversity, 1 = No Write captured solutions to file.

												std::string csvFile = "";
												// Generate the solutions using the options set
												if (optReader.m_solverOption == "GAMS")
													{
														/// We then Iterate through only the GAMS Related Items
														/// percentNearOptimal
														/// optimizationProblem
														/// requestedNumberSolutions
														/// computeDiversity
														/// computeDiversityFromCsv
														/// csvFile

														///Start by looping through the optimization problems
														/// Then for each optimizationProblem, generate the requested number of solutions given the percent near optimal
														/// Then either save the diversity or the generated solutions
														///

														std::string csvFile = optimizationProblem + "_gams" + "_percNO_" + std::to_string(curStateVariables.percentNearOptimal()) + "_reqNS_" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + ".csv";
														std::cout << "  Creating result file and storing it in: " << csvFile << " \n";

													}
												else
													{// We Iterate through the SCIP related items
														/// We then Iterate through only the GAMS Related Items
														/// percentNearOptimal
														/// optimizationProblem
														/// requestedNumberSolutions
														/// nodeType
														/// beta
														/// solCut
														/// depthCut
														/// alphaValue
														/// computeDiversity
														/// computeDiversityFromCsv
														/// csvFile

														for (unsigned long nodetype = 0; nodetype < optReader.m_nodeType.size();
																nodetype++)
															{
																curStateVariables.m_seenValues.clear();

																if (optReader.m_nodeType[nodetype] == 0)
																	{// WE ARE DEALING WITH DIVERSITREE - WE NEED ALPHA< BETA, SOLCUT, DEPTHCUT

																		curStateVariables.setMNodeType(optReader.getNodeType(optReader.m_nodeType[nodetype]));

																		for (unsigned long alpha = 0;
																				alpha < optReader.m_alphaValue.size(); alpha++)
																			{// SET ALPHA

																				//																				currentState.setAlphaValue(optReader.m_alphaValue[alpha]);

																				for (unsigned long beta = 0;
																						beta < optReader.m_betaValue.size(); beta++)
																					{// SET BETA

																						//																						currentState.setBetaValue(optReader.m_betaValue[beta]);

																						for (unsigned long solcut = 0;
																								solcut < optReader.m_solutioncutoff.size();
																								solcut++)
																							{
																								//																								currentState.setSolutioncutoff(optReader.m_solutioncutoff[solcut]);

																								for (unsigned long depthcut = 0;
																										depthcut < optReader.m_depthcutoff.size();
																										depthcut++)
																									{

																										curStateVariables.setAlphaValue(optReader.m_alphaValue[alpha]);
																										curStateVariables.setBetaValue(optReader.m_betaValue[beta]);
																										curStateVariables.setSolutioncutoff(optReader.m_solutioncutoff[solcut]);
																										curStateVariables.setDepthcutoff(optReader.m_depthcutoff[depthcut]);
																										//																										currentState.m_seenNodes.clear();
																										//																										currentState.m_seenNodesMap.clear();

																										if (curStateVariables.alphaValue() > 0 and (curStateVariables.alphaValue() + curStateVariables.betaValue()) > 1)
																											{
																												curStateVariables.setAlphaValue(1 - curStateVariables.betaValue());
																											}
																										// alphaValue, 0, 0.01, 0.90, 0.18, 0.95
																										// beta, 0.95, 0.06, 0.8, 1

																										//Get a string representing the value combination of alpha, beta, solcut and depthcut seen so far
																										std::string checkval = std::to_string(curStateVariables.alphaValue()) + "|" + std::to_string(curStateVariables.betaValue()) + "|" + std::to_string(curStateVariables.depthcutoff()) + "|" + std::to_string(curStateVariables.solutioncutoff());
																										int initialSize = curStateVariables.m_seenValues.size();
																										curStateVariables.m_seenValues.insert(checkval);
																										int newSize = curStateVariables.m_seenValues.size();

																										if ((newSize - initialSize) > 0)
																											{// We have a new alpha, beta, depthcut, solcut combination - start phase 2

																												branchCountScip multiSolGenerator(optReader);

																												solutionStoreVector generatedSolutions;// Holds the generated solutions
																												multiSolGenerator.generateSolutionsFromMIP(generatedSolutions, curStateVariables);

																												std::string csvFile = optimizationProblem + "_SCIP_" + std::string(curStateVariables.getMNodeType()) + "_percNO_" + std::to_string(curStateVariables.percentNearOptimal()) + "_reqNS_" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + "_alpha_" + std::to_string(curStateVariables.alphaValue()) + "_beta_" + std::to_string(curStateVariables.betaValue()) + "_solCut_" + std::to_string(curStateVariables.solutioncutoff()) + "_depthCut_" + std::to_string(curStateVariables.depthcutoff()) + ".csv";
																												std::cout << "  Creating result file and storing it in: " << csvFile << " \n";

																												std::chrono::duration<double, std::milli> timeSpent = curStateVariables.getStopTime() - curStateVariables.getStartTime();
																												curStateVariables.setTimeSpent(timeSpent.count());

																												resultWriter wr { csvFile, generatedSolutions.getSolutionsVector() };
																												wr.writeSolutions_ToCsv();

																												// NEW  CODE TO CAPTURE TO TIME AS WELL
																												solutionStoreMap solutionsMap;

																												// Add number of Solutions
																												solutionsMap.updateSolution(csvFile, "NumSols", std::to_string(curStateVariables.requestedNumberOfSolutions()));
																												// Add Time(ms)
																												solutionsMap.updateSolution(csvFile, "Time(ms)", std::to_string(curStateVariables.getTimeSpent()));

																												//THIS PORTION WRITES THE TIME IT TOOK TO GENERATE THE SOLUTIONS WRITTEN TO CSV
																												std::string runTimeCSVFile = std::string("SCIP_Runtime") + std::string("_percNO_") + std::to_string(curStateVariables.percentNearOptimal()) + std::string("_.csv");
																												if (fileAddCounter == 0)// fileAddCounter == 0 means we are just beginning to write in the file
																													{
																														resultWriter wr { csvFile, solutionsMap.getSolutionsMap() };

																														//                                std::cout << "This is k: " << k << " This is the headstate0 : " << wr.getHeaderState() << std::endl;
																														wr.writeDiversity_ToCsv(solutionsMap, runTimeCSVFile);
																														fileAddCounter = 1;

																													}
																												else
																													{
																														resultWriter wr { csvFile, solutionsMap.getSolutionsMap(), 2 };
																														//                                std::cout << "This is k: " << k <<  " This is the headstate2 : " << wr.getHeaderState() << std::endl;
																														wr.writeDiversity_ToCsv(solutionsMap, runTimeCSVFile);
																													}

																											}// END MAIN SOLVER PROCESS

																									}//DEPTHCUT

																							}//SOLCUT

																					}//BETA

																			}//ALPHA

																	}// NODETYPE DIVERSITREE
																else
																	{// WE ARE DEALING WITH REGULAR NODE SELECTION METHODS - HENCE NO NEED FOR ALPHA, BETA, SOLCUT, DEPTHCUT

																		curStateVariables.setMNodeType(optReader.getNodeType(optReader.m_nodeType[nodetype]));
																		curStateVariables.setAlphaValue(0);
																		curStateVariables.setBetaValue(0);
																		curStateVariables.setDepthcutoff(0);
																		curStateVariables.setSolutioncutoff(0);

																		branchCountScip multiSolGenerator(optReader);
																		solutionStoreVector generatedSolutions;// Holds the generated solutions
																		multiSolGenerator.generateSolutionsFromMIP(generatedSolutions, curStateVariables);

																		//																		std::string csvFile = optimizationProblem + "_SCIP" + "_percNO_" + std::to_string(curStateVariables.percentNearOptimal()) + "_reqNS_" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + "_alpha_" + std::to_string(curStateVariables.alphaValue()) + "_beta_" + std::to_string(curStateVariables.betaValue()) + "_solCut_" + std::to_string(curStateVariables.solutioncutoff()) + "_depthCut_" + std::to_string(curStateVariables.depthcutoff()) + ".csv";
																		std::string csvFile = optimizationProblem + "_SCIP_" + std::string(curStateVariables.getMNodeType()) + "_percNO_" + std::to_string(curStateVariables.percentNearOptimal()) + "_reqNS_" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + "_alpha_" + std::to_string(curStateVariables.alphaValue()) + "_beta_" + std::to_string(curStateVariables.betaValue()) + "_solCut_" + std::to_string(curStateVariables.solutioncutoff()) + "_depthCut_" + std::to_string(curStateVariables.depthcutoff()) + ".csv";
																		std::cout << "  Creating result file and storing it in: " << csvFile << " \n";

																		std::chrono::duration<double, std::milli> timeSpent = curStateVariables.getStopTime() - curStateVariables.getStartTime();
																		curStateVariables.setTimeSpent(timeSpent.count());

																		// TODO add a method here that if DBIN then we need to select on the Binary locations
																		resultWriter wr { csvFile, generatedSolutions.getSolutionsVector() };
																		wr.writeSolutions_ToCsv();

																		// NEW  CODE TO CAPTURE TO TIME AS WELL
																		solutionStoreMap solutionsMap;

																		// Add number of Solutions
																		solutionsMap.updateSolution(csvFile, "NumSols", std::to_string(curStateVariables.requestedNumberOfSolutions()));
																		// Add Time(ms)
																		solutionsMap.updateSolution(csvFile, "Time(ms)", std::to_string(curStateVariables.getTimeSpent()));

																		//THIS PORTION WRITES THE TIME IT TOOK TO GENERATE THE SOLUTIONS WRITTEN TO CSV
																		std::string runTimeCSVFile = std::string("SCIP_Runtime") + std::string("_percNO_") + std::to_string(curStateVariables.percentNearOptimal()) + std::string("_.csv");
																		if (fileAddCounter == 0)// fileAddCounter == 0 means we are just beginning to write in the file
																			{
																				resultWriter wr { csvFile, solutionsMap.getSolutionsMap() };

																				//                                std::cout << "This is k: " << k << " This is the headstate0 : " << wr.getHeaderState() << std::endl;
																				wr.writeDiversity_ToCsv(solutionsMap, runTimeCSVFile);
																				fileAddCounter = 1;

																			}
																		else
																			{
																				resultWriter wr { csvFile, solutionsMap.getSolutionsMap(), 2 };
																				//                                std::cout << "This is k: " << k <<  " This is the headstate2 : " << wr.getHeaderState() << std::endl;
																				wr.writeDiversity_ToCsv(solutionsMap, runTimeCSVFile);
																			}

																	}// NODETYPE REGULAR

															}// NODETYPE

													}// SCIP START

											}// END COMPUTE DIVERSITY == 1
										else
											{// START COMPUTE DIVERSITY == 0;  Store the diversity gotten and other parameters in the solutionStore then write them to the file

												std::string csvFile = "";
												if (optReader.m_solverOption == "GAMS")
													{
														/// We then Iterate through only the GAMS Related Items
														/// percentNearOptimal
														/// optimizationProblem
														/// requestedNumberSolutions
														/// computeDiversity
														/// computeDiversityFromCsv
														/// csvFile

														///Start by looping through the optimization problems
														/// Then for each optimizationProblem, generate the requested number of solutions given the percent near optimal
														/// Then either save the diversity or the generated solutions
														///

														std::string csvFile = optimizationProblem + "_gams_diversity" + "_percNO_" + std::to_string(curStateVariables.percentNearOptimal()) + "_reqNS_" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + ".csv";
														printf("Creating result file and storing it in: %s \n", csvFile.c_str());

													}// END GAMS; COMPUTE DIVERSITY
												else
													{// We Iterate through the SCIP related items
														/// We then Iterate through only the GAMS Related Items
														/// percentNearOptimal
														/// optimizationProblem
														/// requestedNumberSolutions
														/// nodeType
														/// beta
														/// solCut
														/// depthCut
														/// alphaValue
														/// computeDiversity
														/// computeDiversityFromCsv
														/// csvFile

														for (unsigned long nodetype = 0; nodetype < optReader.m_nodeType.size();
																nodetype++)
															{

																//																printf("\n This is the nodeTYPE %i: \n",optReader.m_nodeType[nodetype]);
																curStateVariables.m_seenValues.clear();

																if (optReader.m_nodeType[nodetype] == 0)
																	{// WE ARE DEALING WITH DIVERSITREE - WE NEED ALPHA< BETA, SOLCUT, DEPTHCUT

																		curStateVariables.setMNodeType(optReader.getNodeType(optReader.m_nodeType[nodetype]));

																		for (unsigned long alpha = 0;
																				alpha < optReader.m_alphaValue.size(); alpha++)
																			{// SET ALPHA

																				//																				currentState.setAlphaValue(optReader.m_alphaValue[alpha]);

																				for (unsigned long beta = 0;
																						beta < optReader.m_betaValue.size(); beta++)
																					{// SET BETA

																						//																						currentState.setBetaValue(optReader.m_betaValue[beta]);

																						for (unsigned long solcut = 0;
																								solcut < optReader.m_solutioncutoff.size();
																								solcut++)
																							{
																								//																								currentState.setSolutioncutoff(optReader.m_solutioncutoff[solcut]);

																								for (unsigned long depthcut = 0;
																										depthcut < optReader.m_depthcutoff.size();
																										depthcut++)
																									{

																										curStateVariables.setAlphaValue(optReader.m_alphaValue[alpha]);
																										curStateVariables.setBetaValue(optReader.m_betaValue[beta]);
																										curStateVariables.setSolutioncutoff(optReader.m_solutioncutoff[solcut]);
																										curStateVariables.setDepthcutoff(optReader.m_depthcutoff[depthcut]);
																										//																										curStateVariables.m_seenNodes.clear();
																										//																										curStateVariables.m_seenNodesMap.clear();

																										if (curStateVariables.alphaValue() > 0 and (curStateVariables.alphaValue() + curStateVariables.betaValue()) > 1)
																											{
																												curStateVariables.setAlphaValue(1 - curStateVariables.betaValue());
																											}

																										//Get a string representing the value combination of alpha, beta, solcut and depthcut seen so far
																										std::string checkval = std::to_string(curStateVariables.alphaValue()) + "|" + std::to_string(curStateVariables.betaValue()) + "|" + std::to_string(curStateVariables.depthcutoff()) + "|" + std::to_string(curStateVariables.solutioncutoff());
																										int initialSize = curStateVariables.m_seenValues.size();
																										curStateVariables.m_seenValues.insert(checkval);
																										int newSize = curStateVariables.m_seenValues.size();

																										//																										printf("\n INITIALSIZE: %i , NEWSIZE: %i: \n",initialSize, newSize);

																										if ((newSize - initialSize) > 0)
																											{// We have a new alpha, beta, depthcut, solcut combination - start phase 2

																												//																												printf("\n This is the nodeTYPE %i: \n",optReader.m_nodeType[nodetype]);

																												branchCountScip multiSolGenerator(optReader);
																												solutionStoreVector generatedSolutions;// Holds the generated solutions
																												multiSolGenerator.generateSolutionsFromMIP(generatedSolutions, curStateVariables);

																												//																												std::string csvFile = optimizationProblem + "_SCIP_diversity" + "_percNO_" + std::to_string(curStateVariables.percentNearOptimal()) + "_reqNS_" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + "_alpha_" + std::to_string(curStateVariables.alphaValue()) + "_beta_" + std::to_string(curStateVariables.betaValue()) + "_solCut_" + std::to_string(curStateVariables.solutioncutoff()) + "_depthCut_" + std::to_string(curStateVariables.depthcutoff()) + ".csv";
																												std::string csvFile = std::string("SCIP_diversity") + std::string("_percNO_") + std::to_string(curStateVariables.percentNearOptimal()) + std::string(".csv");
																												std::cout << "  Creating result file and storing it in: " << csvFile << " \n";

																												std::chrono::duration<double, std::milli> timeSpent = curStateVariables.getStopTime() - curStateVariables.getStartTime();
																												curStateVariables.setTimeSpent(timeSpent.count());

																												diversityCalculator divCalc(generatedSolutions);
																												divCalc.dbinAll(curStateVariables);

																												solutionStoreMap solutionsMap;
																												std::string recordName = curStateVariables.optimizationProblems() + "(" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + ")";
																												std::string solver = curStateVariables.getMNodeType() + "_";
																												// Add Diversity
																												solutionsMap.updateSolution(recordName, "diversity", std::to_string(curStateVariables.getDiversity()));
																												// Add number of Solutions
																												solutionsMap.updateSolution(recordName, "NumSols", std::to_string(curStateVariables.requestedNumberOfSolutions()));
																												// Add Time(ms)
																												solutionsMap.updateSolution(recordName, "Time(ms)", std::to_string(curStateVariables.getTimeSpent()));

																												solutionsMap.updateSolution(recordName, "Alpha", std::to_string(curStateVariables.alphaValue()));

																												solutionsMap.updateSolution(recordName, "Beta", std::to_string(curStateVariables.betaValue()));

																												solutionsMap.updateSolution(recordName, "SolCut", std::to_string(curStateVariables.solutioncutoff()));

																												solutionsMap.updateSolution(recordName, "DepthCut", std::to_string(curStateVariables.depthcutoff()));
																												// Add Nodes Processed
																												solutionsMap.updateSolution(recordName, "NodesProcessed", "0");
																												// Add Node Processing type
																												solutionsMap.updateSolution(recordName, "nodeType", curStateVariables.getMNodeType());
																												// Add NearOptimal Percent
																												solutionsMap.updateSolution(recordName, "PercentNearOptimal", std::to_string(curStateVariables.percentNearOptimal()));
																												// Add Number of solutions generated
																												solutionsMap.updateSolution(recordName, "NumberofGeneratedSolutions", std::to_string(curStateVariables.numberSolutionsGenerated()));

																												std::cout << "  This is the calculated diversity: " << curStateVariables.getDiversity() << " . It took " << curStateVariables.getTimeSpent() << " secs to compute." << "\n";

																												if (fileAddCounter == 0)
																													{
																														resultWriter wr { csvFile, solutionsMap.getSolutionsMap() };

																														//                                std::cout << "This is k: " << k << " This is the headstate0 : " << wr.getHeaderState() << std::endl;
																														wr.writeDiversity_ToCsv(solutionsMap);
																														fileAddCounter = 1;

																													}
																												else
																													{
																														resultWriter wr { csvFile, solutionsMap.getSolutionsMap(), 2 };
																														//                                std::cout << "This is k: " << k <<  " This is the headstate2 : " << wr.getHeaderState() << std::endl;
																														wr.writeDiversity_ToCsv(solutionsMap);
																													}

																											}// END MAIN SOLVER PROCESS

																									}//DEPTHCUT

																							}//SOLCUT

																					}//BETA

																			}//ALPHA

																	}// NODETYPE DIVERSITREE
																else
																	{// WE ARE DEALING WITH REGULAR NODE SELECTION METHODS - HENCE NO NEED FOR ALPHA, BETA, SOLCUT, DEPTHCUT

																		curStateVariables.setMNodeType(optReader.getNodeType(optReader.m_nodeType[nodetype]));
																		curStateVariables.setAlphaValue(0);
																		curStateVariables.setBetaValue(0);
																		curStateVariables.setDepthcutoff(0);
																		curStateVariables.setSolutioncutoff(0);

																		branchCountScip multiSolGenerator(optReader);
																		solutionStoreVector generatedSolutions;// Holds the generated solutions
																		multiSolGenerator.generateSolutionsFromMIP(generatedSolutions, curStateVariables);

																		//																		std::string csvFile = optimizationProblem + "_SCIP_diversity" + "_percNO_" + std::to_string(curStateVariables.percentNearOptimal()) + "_reqNS_" + std::to_string(currentState.requestedNumberOfSolutions()) + "_alpha_" + std::to_string(currentState.alphaValue()) + "_beta_" + std::to_string(currentState.betaValue()) + "_solCut_" + std::to_string(currentState.solutioncutoff()) + "_depthCut_" + std::to_string(currentState.depthcutoff()) + ".csv";
																		std::string csvFile = std::string("SCIP_diversity") + std::string("_percNO_") + std::to_string(curStateVariables.percentNearOptimal()) + std::string(".csv");
																		std::cout << "  Creating result file and storing it in: " << csvFile << " \n";

																		std::chrono::duration<double, std::milli> timeSpent = curStateVariables.getStopTime() - curStateVariables.getStartTime();
																		curStateVariables.setTimeSpent(timeSpent.count());

																		if (generatedSolutions.m_solutionsVector.size() >= 0){
																				diversityCalculator divCalc(generatedSolutions);
																				divCalc.dbinAll(curStateVariables);
																		}

																		solutionStoreMap solutionsMap;
																		std::string recordName = curStateVariables.optimizationProblems() + "(" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + ")";
																		std::string solver = curStateVariables.getMNodeType() + "_";
																		// Add Diversity
																		solutionsMap.updateSolution(recordName, "diversity", std::to_string(curStateVariables.getDiversity()));
																		// Add number of Solutions
																		solutionsMap.updateSolution(recordName, "NumSols", std::to_string(curStateVariables.requestedNumberOfSolutions()));
																		// Add Time(ms)
																		solutionsMap.updateSolution(recordName, "Time(ms)", std::to_string(curStateVariables.getTimeSpent()));

																		solutionsMap.updateSolution(recordName, "Alpha", std::to_string(curStateVariables.alphaValue()));

																		solutionsMap.updateSolution(recordName, "Beta", std::to_string(curStateVariables.betaValue()));

																		solutionsMap.updateSolution(recordName, "SolCut", std::to_string(curStateVariables.solutioncutoff()));

																		solutionsMap.updateSolution(recordName, "DepthCut", std::to_string(curStateVariables.depthcutoff()));
																		// Add Nodes Processed
																		solutionsMap.updateSolution(recordName, "NodesProcessed", "0");
																		// Add Node Processing type
																		solutionsMap.updateSolution(recordName, "nodeType", curStateVariables.getMNodeType());
																		// Add NearOptimal Percent
																		solutionsMap.updateSolution(recordName, "PercentNearOptimal", std::to_string(curStateVariables.percentNearOptimal()));
																		// Add Number of solutions generated
																		solutionsMap.updateSolution(recordName, "NumberofGeneratedSolutions", std::to_string(curStateVariables.numberSolutionsGenerated()));

																		std::cout << "  This is the calculated diversity: " << curStateVariables.getDiversity() << " . It took " << curStateVariables.getTimeSpent() << " secs to compute." << "\n";

																		if (fileAddCounter == 0)
																			{
																				resultWriter wr { csvFile, solutionsMap.getSolutionsMap() };

																				//                                std::cout << "This is k: " << k << " This is the headstate0 : " << wr.getHeaderState() << std::endl;
																				wr.writeDiversity_ToCsv(solutionsMap);
																				fileAddCounter = 1;

																			}
																		else
																			{
																				resultWriter wr { csvFile, solutionsMap.getSolutionsMap(), 2 };
																				//                                std::cout << "This is k: " << k <<  " This is the headstate2 : " << wr.getHeaderState() << std::endl;
																				wr.writeDiversity_ToCsv(solutionsMap);
																			}

																	}// NODETYPE REGULAR

															}// NODETYPE

													}// END SCIP COMPUTE DIVERSITY

											}// END COMPUTE DIVERSITY == 0

									}//requestedNumberOfSolutions

							}// optimizationProblem

					}// percentNearOptimal

			}
		else
			{//We have the CSV  -  generate the Diversities

				///
				/// Start by looping through the csvFiles
				/// Then for each csvFile, generate the diversity of the requestednumberofsolutions
				/// Then either save the diversity
				///

				fileAddCounter = 0;
				//		curStateVariablesVariables curStateVariables;
				std::vector<std::string> nameVector { };
				std::cout << "Number of csvFiles: " << optReader.m_csvFiles.size() << "\n";
				curStateVariables.setPMostDiverse(optReader.m_pMostDiverseSolutions);
				std::string csvFile = "nearOptimal_diversity_From_csv.csv";

				std::cout << "This is p Most Diverse: " << curStateVariables.pMostDiverse() << "\n";

				// ----------------------------------------------------------------------------------------------------------------------
				// ----------------------------------- USE THIS SECTION TO GET THE NAMES OF A LOT OF CSV --------------------------------
				// ----------------------------------- put  them in the allCSVFiles  Folder ---------------------------------------------
				// ----------------------------------------------------------------------------------------------------------------------
				DIR *directory;
				struct dirent *en;

				std::string path = optReader.m_gamsWorkingDirectory + "/allCSVFiles";
				std::vector<std::string> allcsvFiles { };

				directory = opendir(path.c_str());
				if (directory)
					{
						while ((en = readdir(directory)) != NULL)
							{
								//                printf("%s\n", en ->d_name);
								allcsvFiles.push_back(en->d_name);
							}
						printf("\n First bad Element: %s \n", allcsvFiles[0].c_str());
						printf("\n Second bad Element: %s \n", allcsvFiles[1].c_str());

						allcsvFiles.erase(allcsvFiles.begin());
						allcsvFiles.erase(allcsvFiles.begin());

						printf("\n First good Element: %s \n", allcsvFiles[0].c_str());

						std::sort(allcsvFiles.begin(), allcsvFiles.end());
						closedir(directory);
					}

				std::cout << "\n Number of csvFiles: " << allcsvFiles.size() << "\n";

				for (int i = 0; i < static_cast<int>(allcsvFiles.size()); i++)
					{
						std::cout << allcsvFiles[i] << ",";
						optReader.m_csvFiles.push_back(allcsvFiles[i]);
					}

				//                std::sort(optReader.m_csvFiles.begin(),optReader.m_csvFiles.end());
				std::cout << "\n Length of csvFiles: " << optReader.m_csvFiles.size() << "\n First file name is: " << optReader.m_csvFiles[0] << std::endl;

				// ----------------------------------------------------------------------------------------------------------------------
				// ----------------------------------------------------------------------------------------------------------------------
				// ----------------------------------------------------------------------------------------------------------------------

				//TODO change this line to optReader.m_csvFiles.size()
				for (int x = 0; x < static_cast<int>(optReader.m_csvFiles.size()); x++)
					{

						nameVector.clear();
						//TODO change this line to optReader.m_csvFiles[x]
						curStateVariables.setCsvFiles(optReader.m_csvFiles[x]);
						std::string fileName = curStateVariables.csvFiles();

						//            std::cout << "FName: " << fileName << std::endl;

						std::stringstream ss(fileName);
						std::string vals;
						char separator = '_';

						while (std::getline(ss, vals, separator))
							{
								vals.erase(remove_if(vals.begin(), vals.end(), isspace), vals.end());
								nameVector.push_back(vals);
							}

						//            pp08aCUTS.mps_100_CBFS_qp_0.01|0.00|0.06|0.05

						// TODO Change how this works; Make sure all files have same output name format.
						if (nameVector.size() == 5)
							{// We are stripping solutions from SCIP on the old system

								curStateVariables.setOptimizationProblems(nameVector[0]);
								curStateVariables.setRequestedNumberOfSolutions(std::stoi(nameVector[1]));
								curStateVariables.setSolverOption(nameVector[2]);

								std::string percentages = nameVector[4];

								std::vector<std::string> percentVector;

								std::stringstream ss(percentages);
								std::string vals;
								char separator = '|';

								while (std::getline(ss, vals, separator))
									{
										vals.erase(remove_if(vals.begin(), vals.end(), isspace), vals.end());
										percentVector.push_back(vals);
									}

								curStateVariables.setPercentNearOptimal(std::stof(percentVector[0]));
								curStateVariables.setAlphaValue(std::stof(percentVector[1]));
								curStateVariables.setBetaValue(std::stof(percentVector[2]));
								int pos = percentVector[3].find(".");
								curStateVariables.setSolutioncutoff(std::stof(percentVector[3].substr(0, pos + 3)));

							}
						else if (nameVector.size() == 6)
							{// We are stripping solutions from GAMS on the new system

								curStateVariables.setOptimizationProblems(nameVector[0]);
								curStateVariables.setSolverOption(nameVector[1]);
								curStateVariables.setPercentNearOptimal(std::stof(nameVector[3]));

								int pos = nameVector[5].find(".");

								curStateVariables.setRequestedNumberOfSolutions(std::stof(nameVector[5].substr(0, pos)));
								curStateVariables.setAlphaValue(0);
								curStateVariables.setBetaValue(0);
								curStateVariables.setSolutioncutoff(0);

							}

						solutionsGenerator multiSolGenerator(optReader);
						solutionStoreVector generatedSolutions;// Read the solutions from the CSV and holds the generated solutions
						//            generatedSolutions = multiSolGenerator.getSolutionsFromCSV(currentState);

						diversityCalculator divCalc(generatedSolutions);
						curStateVariables.setStartTime(std::chrono::high_resolution_clock::now());
						divCalc.dbinP(curStateVariables);
						curStateVariables.setStopTime(std::chrono::high_resolution_clock::now());

						std::chrono::duration<double, std::milli> timeSpent = curStateVariables.getStopTime() - curStateVariables.getStartTime();
						curStateVariables.setTimeSpent(timeSpent.count());

						solutionStoreMap solutionsMap;

						std::string recordName = curStateVariables.optimizationProblems() + "(" + std::to_string(curStateVariables.requestedNumberOfSolutions()) + ")";
						std::string nodeType = curStateVariables.solverOption();
						// Add Diversity
						solutionsMap.updateSolution(recordName, "diversity", std::to_string(curStateVariables.getDiversity()));
						// Add number of Solutions
						solutionsMap.updateSolution(recordName, "NumSols", std::to_string(curStateVariables.requestedNumberOfSolutions()));
						// Add pMostDiverse
						solutionsMap.updateSolution(recordName, "pMostDiverse", std::to_string(curStateVariables.pMostDiverse()));
						// Add Node Processing type
						solutionsMap.updateSolution(recordName, "nodeType", nodeType);
						// Add NearOptimal Percent
						solutionsMap.updateSolution(recordName, "PercentNearOptimal", std::to_string(curStateVariables.percentNearOptimal()));
						// Add Number of solutions generated
						solutionsMap.updateSolution(recordName, "NumberofGeneratedSolutions", std::to_string(curStateVariables.numberSolutionsGenerated()));
						// Add Number of solutions Alpha
						solutionsMap.updateSolution(recordName, "Alpha", std::to_string(curStateVariables.alphaValue()));
						// Add Number of solutions Beta
						solutionsMap.updateSolution(recordName, "Beta", std::to_string(curStateVariables.betaValue()));
						// Add Number of solutions SolCut
						solutionsMap.updateSolution(recordName, "SolCut", std::to_string(curStateVariables.solutioncutoff()));
						// Add Time(ms)
						solutionsMap.updateSolution(recordName, "LocSearchTime(ms)", std::to_string(curStateVariables.getTimeSpent()));

						if (fileAddCounter == 0)
							{
								resultWriter wr { csvFile, solutionsMap.getSolutionsMap() };
								//                std::cout << "This is k: " << k << " This is the headstate0 : " << wr.getHeaderState() << std::endl;
								wr.writeDiversity_ToCsv(solutionsMap);
								fileAddCounter = 1;

							}
						else
							{
								resultWriter wr { csvFile, solutionsMap.getSolutionsMap(), 2 };
								//                std::cout << "This is k: " << k <<  " This is the headstate2 : " << wr.getHeaderState() << std::endl;
								wr.writeDiversity_ToCsv(solutionsMap);
							}

						std::cout << "\n";

					}

			}

		//    return 0;
	}

//    wsInfo.setWorkingDirectory("/Library/Frameworks/GAMS.framework/Versions/38/Resources/apifiles/C++/IzuwaTest/tGams/testGAMS");
//    wsInfo.setSystemDirectory("/Library/Frameworks/GAMS.framework/Versions/38/Resources");
//    opt.setOptDir("/Library/Frameworks/GAMS.framework/Versions/38/Resources/apifiles/C++/IzuwaTest/tGams/testGAMS/cplexOptions");
//    gams::GAMSJob t1 = ws.addJobFromFile("/Library/Frameworks/GAMS.framework/Versions/38/Resources/apifiles/C++/IzuwaTest/tGams/testGAMS/gmsFiles/lseu.gms");

//solver, GAMS, #, GAMS or SCIP
//nodeType, 1,5, #, 0:DiversiTree, 1:BFS, 2:DFS, 3:UCT, 4:hybridestim, 5:BreadthFirst
//alphaValue, 0.9, #, Real numbers between 0 and 1
//optimizationProblem, bell5.mps, blend2.mps, dcmulti.mps, egout.mps,fiber.mps, fixnet6.mps, gen.mps, gesa3.mps, gt2.mps, khb05250.mps, l152lav.mps, lseu.mps,misc03.mps,misc06.mps, mod008.mps, p0033.mps, p0201.mps,p0548.mps, pp08a.mps,pp08aCUTS.mps, qnet1_o.mps,qnet1.mps,rgn.mps, set1ch.mps,stein27.mps,stein45.mps,vpm1.mps,vpm2.mps, #, MIP Problems
//requestedNumberSolutions, 15
//percentNearOptimal, 0.1
//Verbosity, 1
//beta, 0.04
//priorotizedParam, beta, #, The prioritized parameter is set at given value
//solutioncutoff, 0.8
//depthcutoff, 0
//computeDiversity, 0, #, 0 means compute the diversities of captured solutions 1 means write the captured solutions to .csv file
//computeDiversityFromCsv, 0 , #, 0 means we DON'T Have a .csv file hence run GAMS or SCIP, 1 means we are given a .csv file with solutions hence just compute the diversity of the k most diverse solutions; k is represented by the parameter - requestedNumberSolutions
//csvFile, dcmulti_100.csv, #, .csv files with solutions that we wish to compute the diversity of the k most diverse solutions. k is represented by the parameter - requestedNumberSolutions
//pMostDiverseSolutions, 10

//solver, GAMS
//nodeType, 1,5
//alphaValue, 0.9
//optimizationProblem, bell5.mps, blend2.mps, dcmulti.mps, egout.mps,fiber.mps, fixnet6.mps, gen.mps, gt2.mps, khb05250.mps, l152lav.mps, lseu.mps,misc03.mps,misc06.mps, mod008.mps, p0033.mps, p0201.mps, p0548.mps, pp08a.mps,pp08aCUTS.mps, rgn.mps, stein27.mps,stein45.mps,vpm1.mps,vpm2.mps
//requestedNumberSolutions, 10, 50, 100, 200, 1000
//percentNearOptimal, 0.10
//Verbosity, 1
//beta, 0.04
//priorotizedParam, beta, #, The prioritized parameter is set at given value
//solutioncutoff, 0.8
//depthcutoff, 0
//computeDiversity, 1, #, 0 means compute the diversities of captured solutions 1 means write the captured solutions to .csv file
//computeDiversityFromCsv, 1, #, 0 means we DON'T Have a .csv file hence run GAMS or SCIP, 1 means we are given a .csv file with solutions hence just compute the diversity of the k most diverse solutions; k is represented by the parameter - requestedNumberSolutions
//csvFile,
//pMostDiverseSolutions, 10
