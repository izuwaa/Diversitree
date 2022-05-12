/*
 * branchCoutScip.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "branchCountScip.h"
#include "diversitreenode.h"

/* depending on whether the GMP library is available we use a GMP data type or a SCIP_Longint */
#ifdef SCIP_WITH_GMP
#include <gmp.h>
typedef mpz_t Int;
#else
typedef SCIP_Longint         Int;
#endif

/* constraint handler properties */
#define CONSHDLR_NAME "countsols"
#define CONSHDLR_DESC "constraint to count feasible solutions"
#define CONSHDLR_ENFOPRIORITY -9999999 /**< priority of the constraint handler for constraint enforcing */
#define CONSHDLR_CHECKPRIORITY -9999999 /**< priority of the constraint handler for checking feasibility */
#define CONSHDLR_EAGERFREQ 100 /**< frequency for using all instead of only the useful constraints in separation,
   80                                          *   propagation and enforcement, -1 for no eager evaluations, 0 for first only */
#define CONSHDLR_NEEDSCONS FALSE /**< should the constraint handler be skipped, if no constraints are available? */

/** creates and adds a constraint which cuts off the solution from the feasibility region
 108  *
 109  *  input:
 110  *  - scip            : SCIP main data structure
 111  *  - sol             : solution to cut off
 112  *  - conshdlrdata    : constraint handler data
 113  */
#define CUTOFF_CONSTRAINT(x) SCIP_RETCODE x (SCIP* scip, SCIP_SOL* sol, SCIP_CONSHDLRDATA* conshdlrdata)

std::vector<SCIP_Real> nodeVisited;



/** constraint handler data */
struct SCIP_ConshdlrData
	{
		/* solution data and statistic variables */
		SCIP_SPARSESOL **solutions; /**< array to store all solutions */
		int nsolutions; /**< number of solution stored */
		int ssolutions; /**< size of the solution array */
		int feasST; /**< number of non trivial feasible subtrees */
		int nDiscardSols; /**< number of discarded solutions */
		int nNonSparseSols; /**< number of non sparse solutions */
		Int nsols; /**< number of solutions */
		CUTOFF_CONSTRAINT((*cutoffSolution)); /**< method for cutting of a solution */

		/* constraint handler parameters */
		SCIP_Longint sollimit; /**< counting stops, if the given number of solutions have been found (-1: no limit) */
		SCIP_Bool active; /**< constraint handler active */
		SCIP_Bool discardsols; /**< allow to discard solutions */
		SCIP_Bool sparsetest; /**< allow to check for sparse solutions */
		SCIP_Bool collect; /**< should the solutions be collected */

		SCIP_Bool warning; /**< has the warning message already been posted? */

		/* specific problem data */
		SCIP_HASHMAP *hashmap; /**< hashmap to store position of active transformed problem variable in our vars array */
		SCIP_VAR **allvars; /**< array containing a copy of all variables before presolving */
		SCIP_VAR **vars; /**< array containing a copy of all active variables (after presolving) */
		int nallvars; /**< number of all variables in the problem */
		int nvars; /**< number of all active variables in the problem */
		SCIP_Bool continuous; /**< are there continuous variables */
	};

/** free memory */
static
void freeInt(Int *value /**< pointer to the value to free */
)
	{ /*lint --e{715}*/
#ifdef SCIP_WITH_GMP
		mpz_clear(*value);
#endif
	}

static SCIP_RETCODE getExpandedSolutionsValarray(

		SCIP *scip, /**< SCIP data structure */
		SCIP_VAR **allvars, /**< SCIP variables */
		int nallvars, /**< number of all variables */
		SCIP_VAR **activevars, /**< SCIP variables */
		int nactivevars, /**< number of active variables */
		SCIP_HASHMAP *hashmap, /**< hashmap from active solution variable to the position in the active variables array */
		SCIP_SPARSESOL **sols, /**< sparse solutions to expands and write */
		int nsols /**< number of sparse solutions */
		//,std::vector<std::vector<float>> &solutionMap /**< Pointer to the return Solution Map */
		, solutionStoreVector &generatedSolution)
	{

		//	std::cout << " Expanding Solutions " << nallvars << std::endl;

		SCIP_SPARSESOL *sparsesol;
		SCIP_VAR **vars;
		SCIP_Real *scalars;
		SCIP_Longint *sol;
		SCIP_Longint solcnt;
		int s;
		int v;

		std::vector<bool> allBinaryLocations { };
		int numB = 0;// This is the total number of Binary Variables - Meant to check
		std::vector<float> tmpallVariables { };
		std::vector<std::string> ResultstmpallVariables { };

		if (nsols != 0)
			{
				assert(scip != NULL);
				assert(hashmap != NULL);
				assert(allvars != NULL || nallvars == 0);
				assert(activevars != NULL || nactivevars == 0);
				assert(sols != NULL || nsols == 0);

				solcnt = 0;

				/* get memory to store active solution */
				SCIP_CALL(SCIPallocBufferArray(scip, &sol, nactivevars));
				SCIP_CALL(SCIPallocBufferArray(scip, &vars, nactivevars));
				SCIP_CALL(SCIPallocBufferArray(scip, &scalars, nactivevars));

				/* loop over all sparse solutions */

				// ################################################################# IZUWA'S CODES START #######################################################################################################
				// POPULATE THE NAMES OF THE  VARIABLES
				std::vector<std::string> variableNames { };
				//		std::cout <<  " This is Nsols  from SolutionsToMap: " << nsols << std::endl;

				for (v = 0; v < nallvars; v++)
					{

						variableNames.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));

					}

				std::vector<float> allObjectives { };
				//	std::vector<std::vector<int>> allVariables { };

				float max_obj = -10000000000;// Set initial Max to a really small number.
				int maxobj_index = -10;

				// ################################################################# IZUWA'S CODES END ##########################################################################################################

				for (s = 0; s < nsols; ++s)
					{
						sparsesol = sols[s]; /*lint !e613*/
						assert(sparsesol != NULL);
						assert(SCIPsparseSolGetNVars(sparsesol) == nactivevars);

						/* get first solution of the sparse solution */
						SCIPsparseSolGetFirstSol(sparsesol, sol, nactivevars);
						//			std::cout<< "Outer Count: " << s  << "\n";
						//			int a = 0;

						do
							{
								//				std::cout << "This is solution index: " << SCIPsolGetIndex(sparsesol);
								//
								//				std::cout<< "inner Count: " << a << "\n" ;
								//				a += 1;

								SCIP_Real objval;

								solcnt++;

								/* print solution number
								 SCIPinfoMessage(scip, file, "%d(%" SCIP_LONGINT_FORMAT "), ", s+1, solcnt); */
								//			SCIPinfoMessage(scip, NULL, " %d (%" SCIP_LONGINT_FORMAT "),", s + 1, solcnt);
								objval = 0.0;

								//				tmpallVariables.resize(0); //clear(); This clears it
								tmpallVariables.clear();// resize(0, nallvars);   // Clears and reinitializes it

								/* write all variables */
								for (v = 0; v < nallvars; ++v)

									{
										SCIP_Real constant;
										SCIP_Real realvalue;
										int requiredsize;
										int nvars;
										int idx;
										int i;

										vars[0] = allvars[v]; /*lint !e613*/
										scalars[0] = 1.0;
										nvars = 1;
										constant = 0.0;

										SCIP_CALL(SCIPgetProbvarLinearSum(scip, vars, scalars, &nvars, nallvars, &constant, &requiredsize, TRUE));
										assert(requiredsize <= nallvars);
										assert(nvars <= nactivevars);

										realvalue = constant;

										for (i = 0; i < nvars; ++i)

											{
												assert(SCIPhashmapExists(hashmap, vars[i]));
												idx = SCIPhashmapGetImageInt(hashmap, vars[i]) - 1;
												assert(0 <= idx && idx < nactivevars);
												assert(activevars[idx] == vars[i]); /*lint !e613*/

												objval += SCIPvarGetObj(vars[i]) * sol[idx];
												realvalue += scalars[i] * sol[idx];
											}
										assert(SCIPisIntegral(scip, realvalue));

										/** SCIPinfoMessage(scip, file, "%g, ", realvalue); */
										// SCIPinfoMessage(scip, NULL, "%g,", realvalue);   //  #########################################################   This prints the variable Values
										//					tmpallVariables[SCIPvarGetName(SCIPgetOrigVars(scip)[v])] = realvalue;
										//					tmpallVariables.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));
										if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) != SCIP_VARTYPE_CONTINUOUS)

											{

												if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY)
													{
														numB += 1;
														allBinaryLocations.push_back(true);
														ResultstmpallVariables.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));
														ResultstmpallVariables.push_back(std::to_string(realvalue));
													}
												//												else
												//													{
												//														allBinaryLocations.push_back(false);
												//													}
												//
												//												ResultstmpallVariables.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));
												//												ResultstmpallVariables.push_back(std::to_string(realvalue));

											}
										//										tmpallVariables.push_back(std::to_string(realvalue));

									}

								/* transform objective value into original problem space */
								objval = SCIPretransformObj(scip, objval);

								/* output the objective value of the solution */
								/* SCIPinfoMessage(scip, file, "%g\n", objval); */

								//			SCIPinfoMessage(scip, NULL, "%g \n", objval);  // #########################################################   This prints the Objective Value
								// ################################################################# IZUWA'S CODES START - CAPTURE THE OBJECTIVE AND VAR VALUES IN A VECTOR ###########################################################################################
								allObjectives.push_back(objval);
								if (objval > max_obj)
									{
										max_obj = objval;
										maxobj_index = allObjectives.size() - 1;
									}
								generatedSolution.updateSolution(ResultstmpallVariables);
								//								solutionMap.push_back(tmpallVariables);

								// ################################################################# IZUWA'S CODES END - CAPTURE THE OBJECTIVE AND VAR VALUES IN A VECTOR ###########################################################################################

								//			 for (auto const &i: allVariables) {
								//			        std::cout << i << " ";
								//			    }

							} while (SCIPsparseSolGetNextSol(sparsesol, sol, nactivevars));
					}

				// ################################################################# IZUWA'S CODES START ################################################################################################################
				// I PRINT OUT ALL THE VARIABLES AND THE OBJECTIVES FOUND

				//		std::cout << "Expanded all Solutions Found into a Map \n" << std::endl;

				//TODO ADD A VERBOSITY REQUIREMENT TO PRINT THIS OUT

				// #####UNCOMMENT THIS LINE IF YOU WANT TO PRINT ALL VARIABLES OUT TO SCREEN
				//	// This  Prints out all the variable names
				//	for (auto const &i : variableNames) {
				//		std::cout << i << " ";
				//	}
				//	std::cout << "objVal" << " ";
				//	std::cout << "div[Dbin]" << " ";
				//	std::cout << "obj/maxObj" << std::endl;
				//

				// DONT NEED THIS ANY MORE SINCE I AM SETTING A CONSTRAINT ON THE SOLUTIONS OBJECTIVE ALREADY
				// Remove all additions not within qpercent of the optimal
				//
				//		for (int x = allObjectives.size() - 1; x >= 0; x--) {
				//
				//			if ((std::abs(allObjectives[x] - max_obj) / max_obj)
				//					> getQPercent()) {
				////			std::cout << "Before Size: " << allObjectives.size() << " \n ";
				//
				//				allObjectives.erase(allObjectives.begin() + x);
				//				solutionMap.erase(solutionMap.begin() + x);
				//
				////			std::cout << "After Size: " << allObjectives.size() << " \n ";
				//
				//			}
				//		}

			}
		else
			{

				// Add the Previous Objective to allVariables
				//		std::cout
				//				<< " No Solutions captured yet: Using Phase one Solution as Pool to get Diversity "
				//				<< nallvars << std::endl;

				//		tmpallVariables.resize(0);
				//				tmpallVariables = getBestObjectiveSolutionValarray();
				//				solutionMap.push_back(tmpallVariables);

				ResultstmpallVariables.clear();

				//				ResultstmpallVariables = currentState.getMBestSolutionValues();
				ResultstmpallVariables = curStateVariables.getMBestSolutionValues();
				generatedSolution.updateSolution(ResultstmpallVariables);
				//				solutionMap.push_back(tmpallVariables);

			}

		/* free buffer arrays */
		SCIPfreeBufferArray(scip, &scalars);
		SCIPfreeBufferArray(scip, &vars);
		SCIPfreeBufferArray(scip, &sol);

		tmpallVariables.resize(0);

		return SCIP_OKAY;
	}

SCIP_RETCODE getAllExpandedSolutionsValarray(

		SCIP *scip, /**< SCIP data structure */
		SCIP_VAR **allvars, /**< SCIP variables */
		int nallvars, /**< number of all variables */
		SCIP_VAR **activevars, /**< SCIP variables */
		int nactivevars, /**< number of active variables */
		SCIP_HASHMAP *hashmap, /**< hashmap from active solution variable to the position in the active variables array */
		SCIP_SPARSESOL **sols, /**< sparse solutions to expands and write */
		int nsols, /**< number of sparse solutions */
		solutionStoreVector &generatedSolution)
	{

		SCIP_SPARSESOL *sparsesol;
		SCIP_VAR **vars;
		SCIP_Real *scalars;
		SCIP_Longint *sol;
		SCIP_Longint solcnt;
		int s;
		int v;

		std::vector<bool> allBinaryLocations { };
		int numB = 0;// This is the total number of Binary Variables - Meant to check
		std::vector<float> tmpallVariables { };
		std::vector<std::string> ResultstmpallVariables { };

		if (nsols != 0)
			{
				assert(scip != NULL);
				assert(hashmap != NULL);
				assert(allvars != NULL || nallvars == 0);
				assert(activevars != NULL || nactivevars == 0);
				assert(sols != NULL || nsols == 0);

				solcnt = 0;

				/* get memory to store active solution */
				SCIP_CALL(SCIPallocBufferArray(scip, &sol, nactivevars));
				SCIP_CALL(SCIPallocBufferArray(scip, &vars, nactivevars));
				SCIP_CALL(SCIPallocBufferArray(scip, &scalars, nactivevars));

				/* loop over all sparse solutions */

				// ################################################################# IZUWA'S CODES START #######################################################################################################
				// POPULATE THE NAMES OF THE  VARIABLES
				std::vector<std::string> variableNames { };
				//		std::cout <<  " This is Nsols  from SolutionsToMap: " << nsols << std::endl;

				for (v = 0; v < nallvars; v++)
					{

						variableNames.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));

					}

				std::vector<float> allObjectives { };
				//	std::vector<std::vector<int>> allVariables { };

				float max_obj = -10000000000;// Set initial Max to a really small number.
				int maxobj_index = -10;

				// ################################################################# IZUWA'S CODES END ##########################################################################################################

				for (s = 0; s < nsols; ++s)
					{
						sparsesol = sols[s]; /*lint !e613*/
						assert(sparsesol != NULL);
						assert(SCIPsparseSolGetNVars(sparsesol) == nactivevars);

						/* get first solution of the sparse solution */
						SCIPsparseSolGetFirstSol(sparsesol, sol, nactivevars);
						//			std::cout<< "Outer Count: " << s  << "\n";
						//			int a = 0;

						do
							{
								//				std::cout << "This is solution index: " << SCIPsolGetIndex(sparsesol);
								//
								//				std::cout<< "inner Count: " << a << "\n" ;
								//				a += 1;

								SCIP_Real objval;

								solcnt++;

								/* print solution number
								 SCIPinfoMessage(scip, file, "%d(%" SCIP_LONGINT_FORMAT "), ", s+1, solcnt); */
								//			SCIPinfoMessage(scip, NULL, " %d (%" SCIP_LONGINT_FORMAT "),", s + 1, solcnt);
								objval = 0.0;

								tmpallVariables.clear();
								ResultstmpallVariables.clear();
								allBinaryLocations.clear();

								/* write all variables */
								for (v = 0; v < nallvars; ++v)

									{
										SCIP_Real constant;
										SCIP_Real realvalue;
										int requiredsize;
										int nvars;
										int idx;
										int i;

										vars[0] = allvars[v]; /*lint !e613*/
										scalars[0] = 1.0;
										nvars = 1;
										constant = 0.0;

										SCIP_CALL(SCIPgetProbvarLinearSum(scip, vars, scalars, &nvars, nallvars, &constant, &requiredsize, TRUE));
										assert(requiredsize <= nallvars);
										assert(nvars <= nactivevars);

										realvalue = constant;

										for (i = 0; i < nvars; ++i)

											{
												assert(SCIPhashmapExists(hashmap, vars[i]));
												idx = SCIPhashmapGetImageInt(hashmap, vars[i]) - 1;
												assert(0 <= idx && idx < nactivevars);
												assert(activevars[idx] == vars[i]); /*lint !e613*/

												objval += SCIPvarGetObj(vars[i]) * sol[idx];
												realvalue += scalars[i] * sol[idx];
											}
										assert(SCIPisIntegral(scip, realvalue));

										/** SCIPinfoMessage(scip, file, "%g, ", realvalue); */
										// SCIPinfoMessage(scip, NULL, "%g,", realvalue);   //  #########################################################   This prints the variable Values
										//					tmpallVariables[SCIPvarGetName(SCIPgetOrigVars(scip)[v])] = realvalue;
										if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) != SCIP_VARTYPE_CONTINUOUS)

											{

												if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY)
													{
														numB += 1;
														allBinaryLocations.push_back(true);
														ResultstmpallVariables.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));
														ResultstmpallVariables.push_back(std::to_string(realvalue));
													}
												//												else
												//													{
												//														allBinaryLocations.push_back(false);
												//													}

												//												ResultstmpallVariables.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));
												//												ResultstmpallVariables.push_back(std::to_string(realvalue));

											}

										//										if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY)
										//											{
										//												ResultstmpallVariables.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));
										//												ResultstmpallVariables.push_back(std::to_string(realvalue));
										//											}
										tmpallVariables.push_back(realvalue);

									}

								/* transform objective value into original problem space */
								objval = SCIPretransformObj(scip, objval);

								/* output the objective value of the solution */
								/* SCIPinfoMessage(scip, file, "%g\n", objval); */

								//			SCIPinfoMessage(scip, NULL, "%g \n", objval);  // #########################################################   This prints the Objective Value
								// ################################################################# IZUWA'S CODES START - CAPTURE THE OBJECTIVE AND VAR VALUES IN A VECTOR ###########################################################################################
								allObjectives.push_back(objval);
								if (objval > max_obj)
									{
										max_obj = objval;
										maxobj_index = allObjectives.size() - 1;
									}

								//								solutionMap.push_back(tmpallVariables);
								//								ResultsolutionMap.push_back(ResultstmpallVariables);
								generatedSolution.updateSolution(ResultstmpallVariables, allBinaryLocations);

								// ################################################################# IZUWA'S CODES END - CAPTURE THE OBJECTIVE AND VAR VALUES IN A VECTOR ###########################################################################################

								//			 for (auto const &i: allVariables) {
								//			        std::cout << i << " ";
								//			    }

							} while (SCIPsparseSolGetNextSol(sparsesol, sol, nactivevars));
					}

				// ################################################################# IZUWA'S CODES START ################################################################################################################
				// I PRINT OUT ALL THE VARIABLES AND THE OBJECTIVES FOUND

				//		std::cout << "Expanded all Solutions Found into a Map \n" << std::endl;

				//TODO ADD A VERBOSITY REQUIREMENT TO PRINT THIS OUT

				// #####UNCOMMENT THIS LINE IF YOU WANT TO PRINT ALL VARIABLES OUT TO SCREEN
				//	// This  Prints out all the variable names
				//	for (auto const &i : variableNames) {
				//		std::cout << i << " ";
				//	}

			}
		else
			{
				//
				//// Add the Previous Objective to allVariables
				////		std::cout
				////				<< " No Solutions captured yet: Using Phase one Solution as Pool to get Diversity "
				////				<< nallvars << std::endl;

				tmpallVariables.clear();
				ResultstmpallVariables.clear();

				//				tmpallVariables = getBestObjectiveSolutionValarray();
				ResultstmpallVariables = curStateVariables.getMBestSolutionValues();
				generatedSolution.updateSolution(ResultstmpallVariables);
				//				solutionMap.push_back(tmpallVariables);
				//				ResultsolutionMap.push_back(ResultstmpallVariables);

			}

		/* free buffer arrays */
		SCIPfreeBufferArray(scip, &scalars);
		SCIPfreeBufferArray(scip, &vars);
		SCIPfreeBufferArray(scip, &sol);

		tmpallVariables.clear();

		return SCIP_OKAY;
	}

/** execution method of dialog for writing all solutions */
SCIP_RETCODE branchCountScip::captureSolutions(SCIP *scip, solutionStoreVector &generatedSolution)
	{

		SCIP_Longint nsols;//Captures the total number of solutions
		SCIP_Bool valid;

		std::vector<bool> allBinaryLocations { };
		int numB = 0;// This is the total number of Binary Variables - Meant to check

		assert(scip != NULL);

		switch (SCIPgetStage(scip))
			{
			case SCIP_STAGE_INIT:
				SCIPdialogMessage(scip, NULL, "no problem available\n");
				break;
			case SCIP_STAGE_PROBLEM:
			case SCIP_STAGE_TRANSFORMING:
			case SCIP_STAGE_FREETRANS:
				SCIPdialogMessage(scip, NULL, "the counting process was not started yet\n");
				break;
			case SCIP_STAGE_TRANSFORMED:
			case SCIP_STAGE_INITPRESOLVE:
			case SCIP_STAGE_PRESOLVING:
			case SCIP_STAGE_EXITPRESOLVE:
			case SCIP_STAGE_PRESOLVED:
			case SCIP_STAGE_INITSOLVE:
			case SCIP_STAGE_SOLVING:
			case SCIP_STAGE_SOLVED:
			case SCIP_STAGE_EXITSOLVE:
				{

					SCIP_CONSHDLR *conshdlr;
					SCIP_CONSHDLRDATA *conshdlrdata;
					int nsparsesols;

					valid = FALSE;
					nsols = SCIPgetNCountedSols(scip, &valid);

					/* find the countsols constraint handler */
					conshdlr = SCIPfindConshdlr(scip, CONSHDLR_NAME);
					assert(conshdlr != NULL);

					conshdlrdata = SCIPconshdlrGetData(conshdlr);
					assert(conshdlrdata != NULL);

					nsparsesols = conshdlrdata->nsolutions;

					if (!valid)
						{
							/* too many solutions, output not "possible" */
							char *buffer;
							int buffersize;
							int requiredsize;

							buffersize = SCIP_MAXSTRLEN;

							SCIP_CALL(SCIPallocBufferArray(scip, &buffer, buffersize));
							SCIPgetNCountedSolsstr(scip, &buffer, buffersize, &requiredsize);

							if (requiredsize > buffersize)
								{
									buffersize = requiredsize;
									SCIP_CALL(SCIPreallocBufferArray(scip, &buffer, requiredsize));
									SCIPgetNCountedSolsstr(scip, &buffer, buffersize, &requiredsize);
								}

							assert(buffersize >= requiredsize);
							SCIPdialogMessage(scip, NULL, "no output, because of too many feasible solutions : %s\n", buffer);

							SCIPfreeBufferArray(scip, &buffer);
						}
					else if (nsols == 0)
						{
//							SCIPdialogMessage(scip, NULL, "there are no counted solutions\n");
							printf("There are no counted solutions - Changed \n");
						}
					else if (nsparsesols == 0)
						{
							SCIPdialogMessage(scip, NULL, "there is no solution collect (set parameter <constraints/countsols/collect> to TRUE)\n");
						}
					else
						{

							SCIP_SPARSESOL **sparsesols;
							SCIP_VAR **origvars;
							SCIP_VAR **allvars;
							int norigvars;
							int nvars;
							int v;

							SCIP_RETCODE retcode;

							/* get sparse solutions defined over the active variables */
							nvars = conshdlrdata->nvars;
							sparsesols = conshdlrdata->solutions;

							/* get original problem variables */
							retcode = SCIPallocBufferArray(scip, &origvars, SCIPgetNOrigVars(scip));
							if (retcode != SCIP_OKAY)
								{
									//                   fclose(file);
									SCIP_CALL(retcode);
								}

							norigvars = 0;
							//			std::cout << "These are the number of Original Variables "
							//					<< std::to_string(SCIPgetNOrigVars(scip)) << std::endl;

							//#pragma omp parallel for shared(allBinaryLocations, origvars, scip) private(v) reduction(+:norigvars)
							for (v = 0; v < SCIPgetNOrigVars(scip); ++v)
								{

									if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) != SCIP_VARTYPE_CONTINUOUS)

										{

											if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY)
												{
													numB += 1;
													allBinaryLocations.push_back(true);
												}
											else
												{
													allBinaryLocations.push_back(false);
												}
											//#pragma omp critical
											origvars[norigvars] = SCIPgetOrigVars(scip)[v];
											norigvars++;

											// ################################ HOW TO GET THE VARIABLE NAMES #####################################################################
											//printf("This is what is going on inside that loop = %s  \n", SCIPvarGetName(SCIPgetOrigVars(scip)[v]));
										}
								}

							std::cout << " This is numB: " << numB << "\n" << std::endl;
							std::cout << " This is norigvars: " << norigvars << "\n" << std::endl;
							std::cout << " This is conshdlrdata->nallvars: " << conshdlrdata->nallvars << "\n" << std::endl;

							assert(norigvars == conshdlrdata->nallvars);

							retcode = SCIPduplicateBufferArray(scip, &allvars, conshdlrdata->allvars, norigvars);
							if (retcode != SCIP_OKAY)
								{
									//                   fclose(file); /*lint !e449*/
									SCIP_CALL(retcode);
								}

							/* sort original variables array and the corresponding transformed variables w.r.t. the problem index */
							//                SCIPsortDownPtrPtr((void**)allvars, (void**)origvars, varCompProbindex, norigvars);
							//			SCIPdialogMessage(scip, NULL,
							//					"Processing  %" SCIP_LONGINT_FORMAT " (%d) feasible solutions\n",
							//					nsols, nsparsesols);
							/* first row: output the names of the variables in the given ordering */
							//                SCIPinfoMessage(scip, file, "#, ");
							// Printing the output
							// SCIPinfoMessage(scip, NULL, "#, ");
							//			static
							SCIP_VAR *transvar;
							//#pragma omp parallel for shared(scip, norigvars, origvars, allvars) private(transvar)
							for (v = 0; v < norigvars; ++v)
								{//This Prints the  names of the variables to the screen
#ifndef NDEBUG
									{
										/* check if the original variable fits to the transformed variable the constraint handler has stored */
										SCIP_VAR *transvar;
										//					SCIP_CALL(
										//							SCIPgetTransformedVar(scip, origvars[v],
										//									&transvar));

										SCIPgetTransformedVar(scip, origvars[v], &transvar);
										assert(transvar != NULL);
										assert(transvar == allvars[v]);

									}
#endif

								}

							//Convert the Solutions to a Vector
							//							std::vector<std::vector<float>> solMap;
							//							std::vector<std::vector<std::string>> resultsolMap;//Solution map for results; need to change this so I dont need this anymore

							//TODO remove the below two lines:

							printf("\n This is nsols: %d", nsols);
							printf("getCountedSolutions: %d", curStateVariables.getMCountedSolutionsSoFar());

							//							std::cout << "\n This is nsols: " << nsols;
							//							std::cout << "getCountedSolutions: " << currentState.getMCountedSolutionsSoFar() << std::endl;

							//			retcode = SM.expandSolutions(scip, allvars, conshdlrdata->nallvars, conshdlrdata->vars, nvars, conshdlrdata->hashmap, sparsesols, nsparsesols, solMap);
							retcode = getAllExpandedSolutionsValarray(scip, allvars, conshdlrdata->nallvars, conshdlrdata->vars, nvars, conshdlrdata->hashmap, sparsesols, nsparsesols, generatedSolution);

							//							std::cout << " \n This is what is inside SolMap " << solMap[0].size();
							std::cout << " \n This is what is inside allBinaryLocations " << allBinaryLocations.size();

							if (retcode != SCIP_OKAY)
								{
									//                   fclose(file);
									SCIP_CALL(retcode);
								}

							/* free conshdlrdata */
							////							conshdlrdata->nsols; /*lint !e545*/
							//							delete[] &conshdlrdata->nsols;
							//							assert(conshdlrdata->solutions == NULL);
							//							assert(conshdlrdata->nsolutions == 0);
							//							assert(conshdlrdata->ssolutions == 0);
							//							conshdlr = NULL;
							//							conshdlrdata = SCIPconshdlrGetData(conshdlr);
							//							assert(conshdlrdata != NULL);
							//
							//							/* free conshdlrdata */
							//							freeInt(&conshdlrdata->nsols); /*lint !e545*/
							//
							//							assert(conshdlrdata->solutions == NULL);
							//							assert(conshdlrdata->nsolutions == 0);
							//							assert(conshdlrdata->ssolutions == 0);
							//
							//							SCIPfreeBlockMemory(scip, &conshdlrdata);
							//							SCIPconshdlrSetData(conshdlr, NULL);
							//							SCIPfreeBlockMemory(scip, &conshdlrdata);
							//							SCIPconshdlrSetData(conshdlr, NULL);
							//							SCIPfreeBlockMemory(scip, &conshdlrdata);
							//						   SCIPconshdlrSetData(conshdlr, NULL);
							//							SCIP_CONSHDLR *conshdlr;
							//							SCIP_CONSHDLRDATA *conshdlrdata;
							//							int nsparsesols;
							//
							//							valid = FALSE;
							//							nsols = SCIPgetNCountedSols(scip, &valid);
							//
							//							/* find the countsols constraint handler */
							//							conshdlr = SCIPfindConshdlr(scip, CONSHDLR_NAME);
							//							assert(conshdlr != NULL);
							//
							//							conshdlrdata = SCIPconshdlrGetData(conshdlr);
							//							assert(conshdlrdata != NULL);
							//
							//							nsparsesols = conshdlrdata->nsolutions;
							//
						}
					break;
				}
			case SCIP_STAGE_FREE:
				SCIPerrorMessage("invalid call during SCIP_STAGE_FREE\n")
				;
				return SCIP_ERROR;
			}

		//    *nextdialog = SCIPdialoghdlrGetRoot(dialoghdlr);

		//	// ##################################################################################
		//							assert(conshdlr != NULL);
		//							assert(strcmp(SCIPconshdlrGetName(conshdlr), CONSHDLR_NAME) == 0);
		//
		//							/* free constraint handler data */
		//							conshdlrdata = SCIPconshdlrGetData(conshdlr);
		//							assert(conshdlrdata != NULL);
		//
		//							/* free conshdlrdata */
		//							freeInt(&conshdlrdata->nsols); /*lint !e545*/
		//
		//							assert(conshdlrdata->solutions == NULL);
		//							assert(conshdlrdata->nsolutions == 0);
		//							assert(conshdlrdata->ssolutions == 0);
		//
		//							SCIPfreeBlockMemory(scip, &conshdlrdata);
		//							SCIPconshdlrSetData(conshdlr, NULL);
		//
		//							// ###############################################################################

		return SCIP_OKAY;
	}

//
//
//

//branchCountScip::branchCountScip(int message)
//	{
//		printf("Starting empty branchCountScip: \n ");
//	}

//
//
//

/**< Solves the problem loaded on SCIP and sets the bestObjective and bestSolution variables to be used by other methods */
SCIP_RETCODE branchCountScip::phaseOne(SCIP *scip, /**< SCIP data structure */
		const char *variableNameForScip, const char *filename, const char *fileFormat, solutionStoreVector &generatedSolution)
	{

		assert(scip != NULL);

		// Create a basic Problem
		SCIP_CALL(SCIPcreateProbBasic(scip, variableNameForScip));

		// Set Necessary Parameters
		//    SCIP_CALL(oneTreePhaseOneParams(scip));
		/* include default SCIP plugins */
		SCIP_CALL(SCIPincludeDefaultPlugins(scip));
		SCIP_CALL(SCIPsetIntParam(scip, "display/verblevel", 0));
		//	SCIP_CALL(SCIPsetBoolParam(scip, "constraints/countsols/discardsols", FALSE));
		SCIP_CALL(SCIPsetBoolParam(scip, "constraints/countsols/collect", TRUE));

		// Read the Problem file
		//     SCIP_CALL(readFile(scip, filename, fileFormat));
		if (filename != NULL)
			{
				if (SCIPfileExists(filename))
					{
						std::cout << "Reading Input File < " << filename << ">" << std::endl;
						SCIP_CALL(SCIPreadProb(scip, filename, fileFormat));
					}
				else
					{
						std::cout << "Input File <" << filename << "> Not Found " << std::endl;
					}
			}

		// Call SCIPSolve which solves problem
		SCIP_CALL(SCIPsolve(scip));

		//Get the best Solution and best Objective from PhaseOne
		SCIP_Sol *bestSolution = (SCIPgetBestSol(scip));
		//    float bestObjective = SCIPgetSolOrigObj(scip, bestSolution);
		//		currentState.setMBestObjective(SCIPgetSolOrigObj(scip, bestSolution));
		curStateVariables.setMBestObjective(SCIPgetSolOrigObj(scip, bestSolution));

		printf("\n *********************************  This is the Complete Objective: %f ********************************* \n", curStateVariables.getMBestObjective());

		std::vector<std::string> bestSolutionValues;
		//    std::vector<float> bestSolutionValarray;
		std::string bestSolutionString;
		bestSolutionString = "";

		// Set the Best Objective Value - This is set in VarsAcrossModules so it can be used across all Modules
		//    setBestObjective(bestObjective);

		// Convert the solution into  a Map<string, float>

		for (int v = 0; v < SCIPgetNOrigVars(scip); ++v)
			{

				//TODO Try to see if this change makes any difference
				if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) != SCIP_VARTYPE_CONTINUOUS)

					{

						std::string valName = SCIPvarGetName(SCIPgetOrigVars(scip)[v]);
						float val = SCIPgetSolVal(scip, bestSolution, SCIPgetOrigVars(scip)[v]);

						if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY)
							{

								//			bestSolutionValues.insert(
								//					std::pair<std::string, float>(valName, val));

								bestSolutionValues.push_back(valName);
								curStateVariables.m_bestSolutionValues.push_back(valName);

								bestSolutionValues.push_back(std::to_string(val));
								curStateVariables.m_bestSolutionValues.push_back(std::to_string(val));
							}
						//						currentState.m_bestSolutionValues.push_back(valName);
						//						currentState.m_bestSolutionValues.push_back(std::to_string(val));
					}

			}

		// Set the Best Solution - This is set in VarsAcrossModules so it can be used across all Modules

		printf("\n bestSol Length: %f  \n", bestSolutionValues.size());

		//		utilities ut;
		//		setCurrentState(currentState);
		//		utilities::setCurrentState(currentState);
		generatedSolution.updateSolution(bestSolutionValues);

		//	currentState.setMBestSolutionValues(bestSolutionValues);

		return SCIP_OKAY;

	}

/* Sets Parameters for the second phase of  the one-tree algorithm */
SCIP_RETCODE branchCountScip::phaseTwo(SCIP *scip, /**< SCIP data structure */
		const char *variableNameForScip, const char *filename, const char *fileFormat, solutionStoreVector &generatedSolution)
	{

		assert(scip != NULL);

		// INITIALIZE PLUGINS AND SET NECESSARY PARAMETERS

		curStateVariables.setMCountedSolutionsSoFar(0);
		curStateVariables.setMMaxTreeDepthSoFar(0);
		curStateVariables.solutionPool.clear();

		//		utilities ut;
		//		setCurrentState(currentState);
		//		utilities::setCurrentState(currentState);

		//		utilities::cState.setMCountedSolutionsSoFar(0);
		//		utilities::cState.setMMaxTreeDepthSoFar(0);
		//		utilities::cState.solutionPool.clear();

		try
		{
				/* include default SCIP plugins */
				SCIP_CALL(SCIPincludeDefaultPlugins(scip));
				//	SCIP_CALL(SCIPsetIntParam(scip, "limits/solutions", 130));
				SCIP_CALL(SCIPsetIntParam(scip, "display/verblevel", 0));
				//	SCIP_CALL(SCIPsetBoolParam(scip, "constraints/countsols/discardsols", FALSE));

		} catch (...)
			{
				printf("Plugins already initialized: Continuing ....");
			}

		SCIP_CALL(SCIPsetBoolParam(scip, "constraints/countsols/collect", TRUE));

		//TODO MAke this timelimit a  parameter
		SCIP_CALL(SCIPsetRealParam(scip, "limits/time", 1800));//value is in seconds 1500

		if (curStateVariables.getMNodeType() == "DiversiTree")
			{
				printf("########################## SETTING NODE SELECTION RULE TO %s ################################# : \n", curStateVariables.getMNodeType().c_str());
				//				SCIP_CALL(SCIPincludeDiversitreeBfs(scip, currentState));
				SCIP_CALL(SCIPincludeDiversitreeBfs(scip));
				//				CS = currentState;

				SCIP_CALL(SCIPsetBoolParam(scip, "constraints/countsols/collect", TRUE));
				//				SCIP_CALL(SCIPsetBoolParam(scip, "constraints/countsols/discardsols", FALSE));
			}
		else
			{
				printf("########################## SETTING NODE SELECTION RULE TO %s ################################# : \n", curStateVariables.getMNodeType().c_str());

				std::string stdMode = std::string("nodeselection/") + curStateVariables.getMNodeType() + std::string("/stdpriority");
				std::string prtMode = std::string("nodeselection/") + curStateVariables.getMNodeType() + std::string("/memsavepriority");
				std::string minPlungeDepth = std::string("nodeselection/") + curStateVariables.getMNodeType() + std::string("/minplungedepth");
				std::string maxPlungeDepth = std::string("nodeselection/") + curStateVariables.getMNodeType() + std::string("/maxplungedepth");
				std::string maxPlungeQuot = std::string("nodeselection/") + curStateVariables.getMNodeType() + std::string("/maxplungequot");

				SCIP_CALL(SCIPsetIntParam(scip, stdMode.c_str(), 200001));// For Standard Mode
				SCIP_CALL(SCIPsetIntParam(scip, prtMode.c_str(), 100001));// For Priority Mode

				SCIP_CALL(SCIPsetIntParam(scip, minPlungeDepth.c_str(), 100));
				SCIP_CALL(SCIPsetIntParam(scip, maxPlungeDepth.c_str(), 5000));
				SCIP_CALL(SCIPsetRealParam(scip, maxPlungeQuot.c_str(), 0.1));

			}

		//	std::cout << " **************** Starting Phase Two ******************** "
		//			<< std::endl;
		//	std::cout
		//			<< " ^^^^^^^^^^^^^^^^^^^^^^^^^   NOW COUNTING ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ "
		//			<< std::endl;

		SCIP_RETCODE retcode;
		SCIP_Longint nsols;
		SCIP_Bool valid;

		SCIP_CALL(SCIPcreateProbBasic(scip, variableNameForScip));

		// #############################################   Read the Problem file #############################################################
		if (filename != NULL)
			{
				if (SCIPfileExists(filename))
					{
						std::cout << "Reading Input File < " << filename << ">" << std::endl;
						SCIP_CALL(SCIPreadProb(scip, filename, fileFormat));
					}
				else
					{
						std::cout << "Input File <" << filename << "> Not Found " << std::endl;
					}
			}

		// #############################################   SET THE OBJECTIVE BOUND BASED ON Q% #############################################################
		float ObjBound = curStateVariables.getMBestObjective() + (curStateVariables.getMBestObjective() * curStateVariables.percentNearOptimal());
		SCIPsetObjlimit(scip, ObjBound);

		//		printf("This is objlimit %f: \n", ObjBound);
		//
		//	SCIP_Real objLimit;
		//	objLimit = SCIPgetObjlimit(scip);
		//	std::cout << "This is the Objective Limit: " << objLimit << std::endl;

		// #############################################   PERFORM PARAMETER CHECKS  #############################################################
		// Determine the number of solutions required
		if (curStateVariables.requestedNumberOfSolutions() > 0 || curStateVariables.requestedNumberOfSolutions() == -1)
			{

				std::string tmpStr = "";

				if (curStateVariables.requestedNumberOfSolutions() > 0)
					{
						tmpStr = std::to_string(curStateVariables.requestedNumberOfSolutions());
					}
				else
					{
						tmpStr = "Unlimited (time bounded)";
					}

				printf("Counting %s solutions within %f % of the optimal \n", tmpStr.c_str(), curStateVariables.percentNearOptimal() * 100);
				//				std::cout << " Counting " << tmpStr << " solutions within " << qpercent * 100 << "% Of the optimal " << std::endl;
				SCIP_CALL(SCIPsetLongintParam(scip, "constraints/countsols/sollimit", curStateVariables.requestedNumberOfSolutions()));

			}
		else
			{
				printf("Required number of solutions can only be -1 (unlimited) or greater than zero \n");
				//				std::cout << " Required number of solutions can only  be -1 (unlimited) or greater than zero " << std::endl;

				return SCIP_PARAMETERWRONGTYPE;
			}

		// #############################################   THIS IS WHERE COUNT TAKES PLACE  #############################################################
		// #############################################   ######################  ######################################################################

		//    // I need to time this run and add the start time and end time to the current state values.
		printf("Time Starting \n");
		curStateVariables.setStartTime(std::chrono::high_resolution_clock::now());
		printf("Count Starting \n");
		retcode = SCIPcount(scip);// Required if you want to count solutions
		printf("Count Ended \n");
		curStateVariables.setStopTime(std::chrono::high_resolution_clock::now());
		printf("Time Ended \n");
		nsols = SCIPgetNCountedSols(scip, &valid);// Required if you want to count solutions
		printf("Tried to capture nsols here \n");

		if (retcode != SCIP_OKAY)
			{
				printf("\n Count Failed");
				SCIPinfoMessage(scip, NULL, "\n Count Failed");
			}
		else
			{
				//		SCIPdialogMessage(scip, NULL, "Count went ok %ll \n", nsols);
				printf("\n count went ok: %d", nsols);
				//				std::cout << "\n Count went okies: " << nsols << std::endl;
			}

		// #############################################   ######################  ######################################################################
		// #############################################   ######################  ######################################################################

		//		setFileName(filename);// Set the  FileName
		//		setNodeSelectionMethod (nodeSelectionRule);// Set the Node Selection Type

		// ############################################# START THE NEAR OPTIMAL SOLUTIONS GENERATION PROCESS ###########################################################
		//	std::cout << SCIP_capture_Solutions(scip, qpercent);
		//	getAllDiversity(scip);
		printf("Starting Capture \n");
		captureSolutions(scip, generatedSolution);
		printf("Ended Capture Solutions \n");

		// #####################  END: THESE CODES ARE REQUIRED IF YOU WANT TO COUNT THE NUMBER OF SOLUTIONS

		return SCIP_OKAY;

	}

SCIP_RETCODE branchCountScip::runSCIP(solutionStoreVector &generatedSolution)
	{
		SCIP *scip = NULL;
		//		if (currentState.getMObjectiveCalculated() == 0)
		//			{

		SCIP_CALL(SCIPcreate(&scip));
		//TODO We need to get the file extension from the file name
		std::string fName = branchCountScip::m_allOptions.m_scipMPSFileDirectory + "/" + curStateVariables.m_optimizationProblems + ".mps";

		if (curStateVariables.getMObjectiveCalculated() == 1){

				printf("\n Already calculated best objective for this problem  \n");
				printf("\n *********************************  This is the Complete Objective: %f ********************************* \n", curStateVariables.getMBestObjective());


		}
		else{


				printf("\n This is the name of the scipMPSFileDirectory: %s  \n", branchCountScip::m_allOptions.m_scipMPSFileDirectory.c_str());
				printf("\n Calculating PhaseOne: Objective Function:  \n");


				SCIP_CALL(branchCountScip::phaseOne(scip, "countPhaseOne", fName.c_str(), "mps", generatedSolution));
				curStateVariables.setMObjectiveCalculated(1);

		}

		scip = NULL;
		SCIP_CALL(SCIPcreate(&scip));

		SCIP_CALL(branchCountScip::phaseTwo(scip, "countPhaseTwo", fName.c_str(), "mps", generatedSolution));
		//				SCIP_CALL(SCIP_DECL_CONSFREE(scip,consFreeCountsols));

		//			}
		//		else
		//			{
		//				scip = NULL;
		//				SCIP_CALL(SCIPcreate(&scip));
		////TODO We need to get the file extension from the file name
		//
		//				std::string fName = branchCountScip::m_allOptions.m_scipMPSFileDirectory + "/" + currentState.m_optimizationProblems + ".mps";

		//				printf("\n Skipping PhaseOne: Objective function already exists.  \n");
		//				printf("\n This is the Objective: %f  \n", currentState.getMBestObjective());

		//		SCIP_CALL(branchCountScip::phaseTwo(scip, "countPhaseTwo", fName.c_str(), "mps", generatedSolution, currentState));
		//			}

		scip = NULL;
		SCIP_CALL(SCIPcreate(&scip));

		SCIP_CALL(SCIPfree(&scip));
		BMScheckEmptyMemory();

		return SCIP_OKAY;
	}

branchCountScip::branchCountScip(optionsReader &allOptions) : solutionsGenerator(allOptions)
	{

	}

SCIP_RETCODE branchCountScip::generateSolutionsFromMIP(solutionStoreVector &generatedSolution, currentStateVariables &currentState)
	{
		//std::cout<< "I got this far" ;
		//		setCurrentState(currentState);

		SCIP_RETCODE retcode = branchCountScip::runSCIP(generatedSolution);

		//		currentState = getCurrentState();

		return retcode;
	}

/** execution method of dialog for writing all solutions */
float branchCountScip::getNodeDiversityValarray(
		SCIP *scip,
		std::vector<float> &node1BranchMap /**< This is Node 1 Branch */,
		std::vector<float> &node2BranchMap /**< This is Node 2 Branch */,
		bool computeNode1, /**< If this is True, then compute diversity for node 1 */
		bool computeNode2, /**< If this is True, then compute diversity for node 2 */
		bool solCountChanged, /**< If this is False, then use old solutions pool, else, update new solution pool */
		float &node1Div,
		float &node2Div,
		int node1Key,
		int node2Key,
		std::vector<bool> &node1ValarraySelector,
		std::vector<bool> &node2ValarraySelector)
	{

		SCIP_Longint nsols;//Captures the total number of solutions
		SCIP_Bool valid;
		//	float divScore;
		float finalDiversityScoreNode1 { };
		float finalDiversityScoreNode2 { };
		//		utilities ut;

		solutionStoreVector solMap = curStateVariables.getSolutionPool();

		//		printf(" \n This is the solMap length: %d \n", solMap.m_solutionsVector.size() );

		std::vector<bool> allBinaryLocations { };
		int numB = 0;// This is the total number of Binary Variables - Meant to check

		assert(scip != NULL);
		//		assert(cst != NULL);

		SCIP_STAGE stage = SCIPgetStage(scip);

		if (stage == SCIP_STAGE_SOLVING)
			{

				SCIP_CONSHDLR *conshdlr;
				SCIP_CONSHDLRDATA *conshdlrdata;
				int nsparsesols;

				valid = FALSE;
				nsols = SCIPgetNCountedSols(scip, &valid);

				/* find the countsols constraint handler */
				conshdlr = SCIPfindConshdlr(scip, CONSHDLR_NAME);
				assert(conshdlr != NULL);

				conshdlrdata = SCIPconshdlrGetData(conshdlr);
				assert(conshdlrdata != NULL);

				nsparsesols = conshdlrdata->nsolutions;

				if (!valid)
					{
						/* too many solutions, output not "possible" */
						char *buffer;
						int buffersize;
						int requiredsize;

						buffersize = SCIP_MAXSTRLEN;

						SCIP_CALL(SCIPallocBufferArray(scip, &buffer, buffersize));
						SCIPgetNCountedSolsstr(scip, &buffer, buffersize, &requiredsize);

						if (requiredsize > buffersize)
							{
								buffersize = requiredsize;
								SCIP_CALL(SCIPreallocBufferArray(scip, &buffer, requiredsize));
								SCIPgetNCountedSolsstr(scip, &buffer, buffersize, &requiredsize);
							}

						assert(buffersize >= requiredsize);
						SCIPdialogMessage(scip, NULL, "no output, because of too many feasible solutions : %s\n", buffer);

						SCIPfreeBufferArray(scip, &buffer);
					}
				else if (nsols == 0)
					{

						// SINCE NO SOLUTIONS YET, USE THE PREVIOUS SOLUTION FOUND

						std::cout << ".";

						//TODO Remove this line to ensure we are using from the first Solutions
						finalDiversityScoreNode1 = 0;
						finalDiversityScoreNode2 = 0;

						//			node1Div = finalDiversityScoreNode1;
						//			node2Div = finalDiversityScoreNode2;

					}
				else if (nsparsesols == 0)
					{
						SCIPdialogMessage(scip, NULL, "there is no solution collect (set parameter <constraints/countsols/collect> to TRUE)\n");
					}
				else
					{

						SCIP_SPARSESOL **sparsesols;
						SCIP_VAR **origvars;
						SCIP_VAR **allvars;
						int norigvars;
						int nvars;
						int v;

						SCIP_RETCODE retcode;

						//			if ((getNodeDiversityStore(node1Key) < 0)
						//					or (getNodeDiversityStore(node2Key) < 0)) {

						if (solCountChanged or (solMap.m_solutionsVector.size() == 0) )
							{

								/* get sparse solutions defined over the active variables */
								nvars = conshdlrdata->nvars;
								sparsesols = conshdlrdata->solutions;

								/* get original problem variables */
								retcode = SCIPallocBufferArray(scip, &origvars, SCIPgetNOrigVars(scip));
								if (retcode != SCIP_OKAY)
									{
										//                   fclose(file);
										SCIP_CALL(retcode);
									}

								norigvars = 0;

								for (v = 0; v < SCIPgetNOrigVars(scip); ++v)
									{

										if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) != SCIP_VARTYPE_CONTINUOUS)

											{

												if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY)
													{
														numB += 1;
														allBinaryLocations.push_back(true);
													}
												else
													{
														allBinaryLocations.push_back(false);
													}

												origvars[norigvars] = SCIPgetOrigVars(scip)[v];
												norigvars++;

											}
									}

								assert(norigvars == conshdlrdata->nallvars);

								retcode = SCIPduplicateBufferArray(scip, &allvars, conshdlrdata->allvars, norigvars);
								if (retcode != SCIP_OKAY)
									{
										//                   fclose(file); /*lint !e449*/
										SCIP_CALL(retcode);
									}

								for (v = 0; v < norigvars; ++v)
									{//This Prints the  names of the variables to the screen
#ifndef NDEBUG
										{
											/* check if the original variable fits to the transformed variable the constraint handler has stored */
											SCIP_VAR *transvar;
											;

											SCIPgetTransformedVar(scip, origvars[v], &transvar);
											assert(transvar != NULL);
											assert(transvar == allvars[v]);
										}
#endif

									}

								//								std::vector<std::vector<float>> solMap;
								//								solutionStoreVector solMap;
								solMap.clear();

								//								currentStateVariables cst  = utilities::getCurrentState();
								//								currentStateVariables cst = getCurrentState();

								retcode = getAllExpandedSolutionsValarray(scip, allvars, conshdlrdata->nallvars, conshdlrdata->vars, nvars, conshdlrdata->hashmap, sparsesols, nsparsesols, solMap);
								//								retcode = getExpandedSolutionsValarray(scip, allvars, conshdlrdata->nallvars, conshdlrdata->vars, nvars, conshdlrdata->hashmap, sparsesols, nsparsesols, solMap);

								if (retcode != SCIP_OKAY)
									{
										//                   fclose(file);
										SCIP_CALL(retcode);
									}

								// set new Solutions  pool

								//								utilities::getCurrentState().setSolutionPool(solMap);
								curStateVariables.setSolutionPool(solMap);

								if ( (node1BranchMap.size() > 0) and (computeNode1 == true) )
									{
										//																				printf("Node1 MapSize: %d ",node1BranchMap.size());
										solMap.updateSolutionBinaryLocations(node1ValarraySelector);
										//																				printf("Starting Diversity Calc: ");
										diversityCalculator divCalc(solMap);
										finalDiversityScoreNode1 = divCalc.dbinNode(node1BranchMap);//Diversity for node 1
										//																				printf("Ended Diversity Calc: %f ", finalDiversityScoreNode1);

									}
								else
									{
										finalDiversityScoreNode1 = 0;
									}

								//
								//
								//
								if ( (node2BranchMap.size() > 0) and (computeNode2 == true) )
									{
										//																				printf("    Node2 MapSize: %d \n",node2BranchMap.size());
										solMap.updateSolutionBinaryLocations(node2ValarraySelector);
										//																				printf("Starting Diversity Calc: ");
										diversityCalculator divCalc(solMap);
										finalDiversityScoreNode2 = divCalc.dbinNode(node2BranchMap);//Diversity for node 2
										//																				printf("Ending Diversity Calc: %f \n", finalDiversityScoreNode2);
									}
								else
									{
										finalDiversityScoreNode2 = 0;
									}
								//
								//
								//
								node1Div = finalDiversityScoreNode1;
								node2Div = finalDiversityScoreNode2;

								SCIPfreeBufferArray(scip, &allvars);
								SCIPfreeBufferArray(scip, &origvars);
								//								solMap.clear();

							}
						else
							{// Sol Count Didn't change
								//								currentStateVariables cst = utilities::getCurrentState();
								//								currentStateVariables cst = getCurrentState();
								//								solutionStoreVector solMap = cst->getSolutionPool();
								//								solMap = cst->getSolutionPool();
								// set new Solutions  pool

								//
								//
								//TODO Make a node diversity map so that we dont have to compute diversity everytime solutions are updated

								//								printf("Soln Didn't Change side : \n ");

								if ( (node1BranchMap.size() > 0) and (computeNode1 == true) )
									{
										solMap.updateSolutionBinaryLocations(node1ValarraySelector);
										diversityCalculator divCalc(solMap);
										finalDiversityScoreNode1 = divCalc.dbinNode(node1BranchMap);//Diversity for node 1
									}
								else
									{
										finalDiversityScoreNode1 = 0;
									}
								//}
								//
								//
								if ( (node2BranchMap.size() > 0) and (computeNode2 == true) )
									{
										solMap.updateSolutionBinaryLocations(node2ValarraySelector);
										diversityCalculator divCalc(solMap);
										finalDiversityScoreNode2 = divCalc.dbinNode(node2BranchMap);//Diversity for node 2
									}
								else
									{
										finalDiversityScoreNode2 = 0;
									}
								//
								//
								//

								//								if ((getNodeDiversityStore(node1Key)[0] < 0) or (getNodeDiversityStore(node1Key)[1] != getCountedSolutions()))
								//									{
								//
								//										dBinOneValarray(&solMap, solMap.size(), node1BranchMap, node1BranchMap.size(), finalDiversityScoreNode1, node1ValarraySelector);//Diversity for node 1
								//										updateNodeDiversityStore(node1Key, finalDiversityScoreNode1, getCountedSolutions());
								//
								//									}
								//								else
								//									{
								//										finalDiversityScoreNode1 = getNodeDiversityStore(node1Key)[0];
								//									}
								//
								//								if ((getNodeDiversityStore(node2Key)[0] < 0) or (getNodeDiversityStore(node2Key)[1] != getCountedSolutions()))
								//									{
								//										dBinOneValarray(&solMap, solMap.size(), node2BranchMap, node2BranchMap.size(), finalDiversityScoreNode2, node2ValarraySelector);//Diversity for node 2
								//										updateNodeDiversityStore(node2Key, finalDiversityScoreNode2, getCountedSolutions());
								//
								//									}
								//								else
								//									{
								//										finalDiversityScoreNode2 = getNodeDiversityStore(node2Key)[0];
								//									}

								node1Div = finalDiversityScoreNode1;
								node2Div = finalDiversityScoreNode2;

								//				SCIPfreeBufferArray(scip, &allvars);
								//				SCIPfreeBufferArray(scip, &origvars);

							}

						//			}   // End of Checks if we already have the Node Diversity Scores

					}// End of main If clause
			}
		else
			{
				SCIPdialogMessage(scip, NULL, "I am Not Solving  \n");
				//		finalDiversityScore = 0;
			}

		//	std::cout  <<  " I ended getNodeDiversityValarray  " ;
		return 1;
	}
