/*
 * diversitreenode.cpp
 *
 *  Created on: Mar 24, 2022
 *      Author: izuwaahanor
 */

#include "diversitreenode.h"

#include "scip/pub_message.h"
#include "scip/pub_nodesel.h"
#include "scip/pub_tree.h"
#include "scip/scip_mem.h"
#include "scip/scip_message.h"
#include "scip/scip_nodesel.h"
#include "scip/scip_numerics.h"
#include "scip/scip_param.h"
#include "scip/scip_solvingstats.h"
#include "scip/scip_tree.h"
#include "scip/type_misc.h"
#include <string.h>
#include <algorithm>
#include <iostream>
#include <valarray>
#include <fstream>

#define NODESEL_NAME             "DiversitreeBFSNodeSel"
#define NODESEL_DESC             "Diversitree node selection implementation"
#define NODESEL_STDPRIORITY      200001
#define NODESEL_MEMSAVEPRIORITY       100001

#define Optimum_Obj	getCurrentState().getMBestObjective()
#define QPercent		getCurrentState().percentNearOptimal()
#define alpha		getCurrentState().alphaValue()
#define beta				getCurrentState().betaValue()

/*
 * Default parameter settings
 */

#define MINPLUNGEDEPTH              100 /**< minimal plunging depth, before new best node may be selected (-1 for dynamic setting) */
#define MAXPLUNGEDEPTH              5000 /**< maximal plunging depth, before new best node is forced to be selected (-1 for dynamic setting) */
#define MAXPLUNGEQUOT             0.1 /**< maximal quotient (curlowerbound - lowerbound)/(cutoffbound - lowerbound) *   where plunging is performed */

/** node selector data for best first search node selection */
struct SCIP_NodeselData
	{
		SCIP_Real maxplungequot; /**< maximal quotient (curlowerbound - lowerbound)/(cutoffbound - lowerbound)
		 *   where plunging is performed */
		int minplungedepth; /**< minimal plunging depth, before new best node may be selected
		 *   (-1 for dynamic setting) */
		int maxplungedepth; /**< maximal plunging depth, before new best node is forced to be selected
		 *   (-1 for dynamic setting) */
	};

/*
 * Callback methods
 */

/** copy method for node selector plugins (called when SCIP copies plugins) */
static SCIP_DECL_NODESELCOPY(nodeselCopydiversitreenode)
	{ /*lint --e{715}*/
		assert(scip != NULL);
		assert(nodesel != NULL);
		assert(strcmp(SCIPnodeselGetName(nodesel), NODESEL_NAME) == 0);

		/* call inclusion method of node selector */
		SCIP_CALL(SCIPincludeDiversitreeBfs(scip));

		return SCIP_OKAY;
	}

/** destructor of node selector to free user data (called when SCIP is exiting) */
/**! [SnippetNodeselFreeBfs] */
static SCIP_DECL_NODESELFREE(nodeselFreediversitreenode)
	{ /*lint --e{715}*/
		SCIP_NODESELDATA *nodeseldata;

		assert(nodesel != NULL);
		assert(strcmp(SCIPnodeselGetName(nodesel), NODESEL_NAME) == 0);
		assert(scip != NULL);

		/* free user data of node selector */
		nodeseldata = SCIPnodeselGetData(nodesel);
		assert(nodeseldata != NULL);
		SCIPfreeBlockMemory(scip, &nodeseldata);
		SCIPnodeselSetData(nodesel, nodeseldata);

		return SCIP_OKAY;
	}
/**! [SnippetNodeselFreeBfs] */

/** node selection method of node selector */
static SCIP_DECL_NODESELSELECT(nodeselSelectdiversitreenode)
	{ /*lint --e{715}*/
		SCIP_NODESELDATA *nodeseldata;
		int minplungedepth;
		int maxplungedepth;
		int plungedepth;
		SCIP_Real maxplungequot;

		assert(nodesel != NULL);
		assert(strcmp(SCIPnodeselGetName(nodesel), NODESEL_NAME) == 0);
		assert(scip != NULL);
		assert(selnode != NULL);

		*selnode = NULL;

		/* get node selector user data */
		nodeseldata = SCIPnodeselGetData(nodesel);
		assert(nodeseldata != NULL);

		/* calculate minimal and maximal plunging depth */
		minplungedepth = nodeseldata->minplungedepth;
		maxplungedepth = nodeseldata->maxplungedepth;
		maxplungequot = nodeseldata->maxplungequot;
		if (minplungedepth == -1)
			{
				minplungedepth = SCIPgetMaxDepth(scip) / 10;
				if (SCIPgetNStrongbranchLPIterations(scip) > 2 * SCIPgetNNodeLPIterations(scip))
					minplungedepth += 10;
				if (maxplungedepth >= 0)
					minplungedepth = MIN(minplungedepth, maxplungedepth);
			}
		if (maxplungedepth == -1)
			maxplungedepth = SCIPgetMaxDepth(scip) / 2;
		maxplungedepth = MAX(maxplungedepth, minplungedepth);

		/* check, if we exceeded the maximal plunging depth */
		plungedepth = SCIPgetPlungeDepth(scip);
		if (plungedepth >= maxplungedepth)
			{
				/* we don't want to plunge again: select best node from the tree */
				SCIPdebugMsg(scip, "plungedepth: [%d,%d], cur: %d -> abort plunging\n", minplungedepth, maxplungedepth, plungedepth);
				*selnode = SCIPgetBestNode(scip);
				SCIPdebugMsg(scip, "  -> best node   : lower=%g\n",
						*selnode != NULL ? SCIPnodeGetLowerbound(*selnode) : SCIPinfinity(scip));
			}
		else
			{
				SCIP_NODE *node;
				SCIP_Real maxbound;

				/* check, if plunging is forced at the current depth */
				if (plungedepth < minplungedepth)
					{
						maxbound = SCIPinfinity(scip);
						SCIPdebugMsg(scip, "plungedepth: [%d,%d], cur: %d => maxbound: infinity\n", minplungedepth, maxplungedepth, plungedepth);
					}
				else
					{
						SCIP_Real lowerbound;
						SCIP_Real cutoffbound;
						/* get global lower and cutoff bound */
						lowerbound = SCIPgetLowerbound(scip);
						cutoffbound = SCIPgetCutoffbound(scip);

						/* if we didn't find a solution yet, the cutoff bound is usually very bad:
						 * use only 20% of the gap as cutoff bound
						 */
						if (SCIPgetNSolsFound(scip) == 0)
							cutoffbound = lowerbound + 0.2 * (cutoffbound - lowerbound);
						/* calculate maximal plunging bound */
						maxbound = lowerbound + maxplungequot * (cutoffbound - lowerbound);

						SCIPdebugMsg(scip, "plungedepth: [%d,%d], cur: %d, bounds: [%g,%g], maxbound: %g\n", minplungedepth, maxplungedepth, plungedepth, lowerbound, cutoffbound, maxbound);
					}

				/* we want to plunge again: prefer children over siblings, and siblings over leaves,
				 * but only select a child or sibling, if its dual bound is small enough;
				 * prefer using nodes with higher node selection priority assigned by the branching rule
				 */
				node = SCIPgetPrioChild(scip);
				if (node != NULL && SCIPnodeGetLowerbound(node) < maxbound)
					{
						*selnode = node;
						SCIPdebugMsg(scip, "  -> selected prio child: lower=%g\n", SCIPnodeGetLowerbound(*selnode));
					}
				else
					{
						node = SCIPgetBestChild(scip);
						if (node != NULL && SCIPnodeGetLowerbound(node) < maxbound)
							{
								*selnode = node;
								SCIPdebugMsg(scip, "  -> selected best child: lower=%g\n", SCIPnodeGetLowerbound(*selnode));
							}
						else
							{
								node = SCIPgetPrioSibling(scip);
								if (node != NULL && SCIPnodeGetLowerbound(node) < maxbound)
									{
										*selnode = node;
										SCIPdebugMsg(scip, "  -> selected prio sibling: lower=%g\n", SCIPnodeGetLowerbound(*selnode));
									}
								else
									{
										node = SCIPgetBestSibling(scip);
										if (node != NULL && SCIPnodeGetLowerbound(node) < maxbound)
											{
												*selnode = node;
												SCIPdebugMsg(scip, "  -> selected best sibling: lower=%g\n", SCIPnodeGetLowerbound(*selnode));
											}
										else
											{
												*selnode = SCIPgetBestNode(scip);
												SCIPdebugMsg(scip, "  -> selected best leaf: lower=%g\n",
														*selnode != NULL ? SCIPnodeGetLowerbound(*selnode) : SCIPinfinity(scip));
											}
									}
							}
					}
			}

		return SCIP_OKAY;
	}

static//std::map<std::string, float>
SCIP_RETCODE getNodeBranchVarMap(SCIP *scip, SCIP_NODE *node, std::vector<std::string> &nodeBranchMap)
	{

		SCIP_VAR **branchvars;
		SCIP_Real *branchbounds;
		SCIP_BOUNDTYPE *boundtypes;
		int nbranchvars;
		int size;

		size = SCIPnodeGetDepth(node);

		/* allocate memory for all branching decisions */
//	SCIP_RETCODE retcode;
		SCIP_CALL(SCIPallocBufferArray(scip, &branchvars, size));
		SCIP_CALL(SCIPallocBufferArray(scip, &branchbounds, size));
		SCIP_CALL(SCIPallocBufferArray(scip, &boundtypes, size));

		/* we assume that we only have one branching decision at each node */
		SCIPnodeGetAncestorBranchings(node, branchvars, branchbounds, boundtypes, &nbranchvars, size);

		/* check if did not have enough memory */
		if (nbranchvars > size)
			{
				size = nbranchvars;
				SCIP_CALL(SCIPallocBufferArray(scip, &branchvars, size));
				SCIP_CALL(SCIPallocBufferArray(scip, &branchbounds, size));
				SCIP_CALL(SCIPallocBufferArray(scip, &boundtypes, size));

				/* now getting all information */
				SCIPnodeGetAncestorBranchings(node, branchvars, branchbounds, boundtypes, &nbranchvars, size);
			}

//	std::cout << "This is the number of Branching this Node : " << nbranchvars
//			<< std::endl;

//	static std::vector<std::string> TnodeBranchMap;

//#pragma omp parallel for shared(TnodeBranchMap) private(nbranchvars, branchvars, branchbounds)
		for (int v = 0; v < nbranchvars; v++)
			{
//		SCIP_Var k  = nd1Var[v];
//		SCIPfindVar(scip,SCIPvarGetName(branchvars[v]));
				if (SCIPvarGetType(branchvars[v]) == SCIP_VARTYPE_BINARY)
					{
//			std::string pr = SCIPvarGetName(branchvars[v]);
//		std::cout << pr.substr(2, pr.length()) << " " << branchbounds[v] << " ";
//			nodeBranchMap[pr.substr(2, pr.length())] = branchbounds[v];
//#pragma omp critical
						nodeBranchMap.push_back(std::string(SCIPvarGetName(branchvars[v])).substr(2, std::string(SCIPvarGetName(branchvars[v])).length()));
//#pragma omp critical
						nodeBranchMap.push_back(std::to_string(branchbounds[v]));

					}

			}
//	std::cout << std::endl;

//	return nodeMap;

//	nodeBranchMap = TnodeBranchMap;
		/* free memory for all branching decisions */
		SCIPfreeBufferArray(scip, &boundtypes);
		SCIPfreeBufferArray(scip, &branchbounds);
		SCIPfreeBufferArray(scip, &branchvars);
//	TnodeBranchMap.clear();

		return SCIP_OKAY;

	}

static//std::map<std::string, float>
SCIP_RETCODE getNodeBranchVarMapValarray(SCIP *scip, SCIP_NODE *node, std::vector<float> &nodeBranchMap, std::vector<
		bool> &nodeSelector, int &numBinVar)
	{

		SCIP_VAR **branchvars;
		SCIP_Real *branchbounds;
		SCIP_BOUNDTYPE *boundtypes;
		int nbranchvars;
		int size;
		std::vector<float> tmpBranchMap { };

		size = SCIPnodeGetDepth(node);

		/* allocate memory for all branching decisions */
//	SCIP_RETCODE retcode;
		SCIP_CALL(SCIPallocBufferArray(scip, &branchvars, size));
		SCIP_CALL(SCIPallocBufferArray(scip, &branchbounds, size));
		SCIP_CALL(SCIPallocBufferArray(scip, &boundtypes, size));

		/* we assume that we only have one branching decision at each node */
		SCIPnodeGetAncestorBranchings(node, branchvars, branchbounds, boundtypes, &nbranchvars, size);

		/* check if did not have enough memory */
		if (nbranchvars > size)
			{
				size = nbranchvars;
				SCIP_CALL(SCIPallocBufferArray(scip, &branchvars, size));
				SCIP_CALL(SCIPallocBufferArray(scip, &branchbounds, size));
				SCIP_CALL(SCIPallocBufferArray(scip, &boundtypes, size));

				/* now getting all information */
				SCIPnodeGetAncestorBranchings(node, branchvars, branchbounds, boundtypes, &nbranchvars, size);
			}

		std::vector<std::string> variableNames { };

		for (int v = 0; v < nbranchvars; v++)
			{

//		variableNames.push_back(SCIPvarGetName(SCIPgetOrigVars(scip)[v]));
				variableNames.push_back(std::string(SCIPvarGetName(branchvars[v])).substr(2, std::string(SCIPvarGetName(branchvars[v])).length()));

			}

//	std::cout << "This is the number of Branching this Node : " << nbranchvars
//			<< "\n";

//	static std::vector<std::string> TnodeBranchMap;

//#pragma omp parallel for shared(TnodeBranchMap) private(nbranchvars, branchvars, branchbounds)
//	int branchVarCount = 0;
//	bool booleanSelector[SCIPgetNOrigVars(scip)]; // {}; // = {false,true};

//	std::cout << " Starting for loop \n";
		numBinVar = 0;
		for (int v = 0; v < SCIPgetNOrigVars(scip); ++v)
			{

				if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) != SCIP_VARTYPE_CONTINUOUS)
					{

						std::vector<std::string>::iterator ret = std::find(variableNames.begin(), variableNames.end(), SCIPvarGetName(SCIPgetOrigVars(scip)[v]));

						if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY and ret != variableNames.end())
							{

								numBinVar += 1;
								nodeSelector.push_back(true);
								nodeBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
//				nodeBranchMap.push_back(std::string(SCIPvarGetName(branchvars[v])).substr(2, std::string(SCIPvarGetName(branchvars[v])).length()));
//								nodeBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
//								tmpBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
//				std::cout << " Branhc var num: " << (int)(ret - variableNames.begin()) << '\n';
//				branchVarCount += 1;

							}
						else if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY)
							{
//								nodeBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
//								tmpBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
								nodeSelector.push_back(false);
							}
					}
			}

//		printf("Number of Binlocations in this node: %d \n",numBinVar);
//		nodeBranchMap.push_back(tmpBranchMap);

		/* free memory for all branching decisions */
		SCIPfreeBufferArray(scip, &boundtypes);
		SCIPfreeBufferArray(scip, &branchbounds);
		SCIPfreeBufferArray(scip, &branchvars);
		variableNames.clear();
//	TnodeBranchMap.clear();

//	std::cout  <<  " I ended node  branch  " ;
		return SCIP_OKAY;

	}

/** Used by node compare, it normalizes the Lower bound value  using the Lowerbound of the two nodes been compared, the bestEstimate found in phase-one of  the
 * tree pass and the maximum optimal objective value desired. Returns the normalized lowerbound for both nodes at the same time by reference.
 */
void getLBScore(float node1LB, float node2LB, float objectiveLimit, float &node1Score, float &node2Score)
	{

		float minVal = std::min( { (float) Optimum_Obj, node1LB, node2LB });

		node1Score = (node1LB - minVal) / (objectiveLimit - minVal);
		node2Score = (node2LB - minVal) / (objectiveLimit - minVal);

	}

/** Used by node compare, it normalizes the Lower bound value  using the Lowerbound of the two nodes been compared, the bestEstimate found in phase-one of  the
 * tree pass and the maximum optimal objective value desired. Returns the normalized lowerbound for both nodes at the same time by reference.
 */
void getDepthScore(float node1Depth, float node2Depth, float minDepth, float maxDepth, float &node1Score, float &node2Score)
	{

// We want to use minDepth as 0

//TODO Try minDepth as zero or minplungedepth
		minDepth = 0;
		float minVal = std::min( { minDepth, node1Depth, node2Depth });

		node1Score = (node1Depth - minVal) / (maxDepth - minVal);
		node2Score = (node2Depth - minVal) / (maxDepth - minVal);

	}

/** node comparison method of node selector */
static
SCIP_DECL_NODESELCOMP(nodeselCompdiversitreenode)
	{ /*lint --e{715}*/

		SCIP_NODESELDATA *nodeseldata;
		nodeseldata = SCIPnodeselGetData(nodesel);

		SCIP_Real lowerbound1;
		SCIP_Real lowerbound2;

		SCIP_Longint nsols;
		SCIP_Bool valid;

		valid = FALSE;
		nsols = SCIPgetNCountedSols(scip, &valid);

//	std::cout<< "nsols: " << nsols;
//	std::cout<< "getCountedSolutions: " << getCountedSolutions() << std::endl;

// Get the Node Map of Variables  NODE 1 - if diversityUtilization > 0

		float node1Div { };
		float node2Div { };

		int minplungedepth = nodeseldata->minplungedepth;
		int maxplungedepth = nodeseldata->maxplungedepth;

// SET Max tree Depth
		float maxDepth = std::max( { getCurrentState().getMMaxTreeDepthSoFar(), (float) SCIPnodeGetDepth(node2), (float) SCIPnodeGetDepth(node1) });
		getCurrentState().setMMaxTreeDepthSoFar(maxDepth);

		bool depthCutOffTest = getCurrentState().depthcutoff() == 0 or maxDepth >= getCurrentState().depthcutoff();
		bool solCutOffTest = getCurrentState().solutioncutoff() == 0 or nsols >= (getCurrentState().solutioncutoff() * getCurrentState().requestedNumberOfSolutions());
		bool alphaTest = alpha > 0 or beta > 0;

//	file_out << " depthCutOffTest:" << depthCutOffTest ;
//	file_out << " solCutOffTest:" << solCutOffTest ;
//	file_out << " diversityTest:" << diversityTest ;

		if (alphaTest and solCutOffTest and depthCutOffTest)
			{

// New Method - using Val Array
// Get the Node Map of Variables  NODE 1
//		std::cout << " Starting 1 ";
				std::vector<float> nd1Map { };

				std::vector<bool> nd1Selector { };
				int numBin { };
				getNodeBranchVarMapValarray(scip, node1, nd1Map, nd1Selector, numBin);
//				getNodeBranchVarMapValarray(scip, node, nodeBranchMap, nodeSelector, numBinVar);

// New Method - using Val Array
// Get the Node Map of Variables  NODE 2
				std::vector<float> nd2Map { };
				std::vector<bool> nd2Selector { };
				;//   =  getNodeVarMap(scip, node2);
				int numBin2 { };
				getNodeBranchVarMapValarray(scip, node2, nd2Map, nd2Selector, numBin2);

//				printf("Number of Binlocations Node1: %d  and for Node2: %d \n",numBin,numBin2);
//
//
				optionsReader opt;
				branchCountScip bcs(opt);
				bcs.getNodeDiversityValarray(scip, nd1Map, nd2Map, nsols == getCurrentState().getMCountedSolutionsSoFar(), node1Div, node2Div, (int) SCIPnodeGetNumber(node1), (int) SCIPnodeGetNumber(node2), nd1Selector, nd2Selector);
//				getNodeDiversityValarray(scip, std::vector<float> , std::vector<float> , bool ,float , float , int, int , std::vector<bool> , std::vector<bool>);
//
//
			}

		getCurrentState().setMCountedSolutionsSoFar(nsols);
//		setCountedSolutions(nsols);

		assert(nodesel != NULL);
		assert(strcmp(SCIPnodeselGetName(nodesel), NODESEL_NAME) == 0);
		assert(scip != NULL);

		float scaledLB1 { };
		float scaledLB2 { };

		float scaledDepth1 { };
		float scaledDepth2 { };

		getLBScore((float) SCIPnodeGetLowerbound(node1), (float) SCIPnodeGetLowerbound(node2), (float) SCIPgetObjlimit(scip), scaledLB1, scaledLB2);
		getDepthScore((float) SCIPnodeGetDepth(node1), (float) SCIPnodeGetDepth(node2), (float) minplungedepth, (float) maxplungedepth, scaledDepth1, scaledDepth2);

//	float alpha {};
//
//	if ((diversity + beta) >=  1 ){
//		alpha = 1 - beta;
//	}else{
//		alpha = diversity;
//	}

		float lbAlpha { };

		lbAlpha = 1 - (alpha + beta);
		if (lbAlpha < 0)
			{
				lbAlpha = 0;
			}

//// NEW DR MEDAL METHOD
//	lowerbound1 = ((1 - (scaledDepth1*diversity)) * scaledLB1) + ((scaledDepth1*diversity) * node1Div);
//	lowerbound2 = ((1 - (scaledDepth2*diversity)) * scaledLB2) + ((scaledDepth2*diversity) * node2Div);

///// ALPHA * DIV + BETA * DEPTH - WHAT  I SHOULD HAVE USED
		lowerbound1 = (lbAlpha) * scaledLB1 + (alpha * (2 * node1Div)) + (beta * scaledDepth1);
		lowerbound2 = (lbAlpha) * scaledLB2 + (alpha * (2 * node2Div)) + (beta * scaledDepth2);

/////////// ALPHA TIMES MAX (DIVERSITY,DEPTH) - WHAT I USED  SO FAR
//	lowerbound1 = (lbAlpha) * scaledLB1
//			+ (diversity * std::max(node1Div, scaledDepth1)); // + (beta*scaledDepth1) ;
//	lowerbound2 = (lbAlpha) * scaledLB2
//			+ (diversity * std::max(node2Div, scaledDepth2)); // + (beta*scaledDepth2) ;

		if (SCIPisLT(scip, lowerbound1, lowerbound2))
			return -1;
		else if (SCIPisGT(scip, lowerbound1, lowerbound2))
			return +1;
		else
			{

//				SCIP_Real estimate1;
//				SCIP_Real estimate2;
//
////		estimate1 = (1-diversity)*SCIPnodeGetEstimate(node1) + (diversity * node1Div);
////		estimate2 = (1-diversity)*SCIPnodeGetEstimate(node2) + (diversity * node2Div);
//
////		estimate1 = (1-0.9)*SCIPnodeGetEstimate(node1) + (diversity*node1Div);
////		estimate2 = (1-0.9)*SCIPnodeGetEstimate(node2) + (diversity*node2Div);
//
////		if ((diversity + beta) >=  1 ){
////			alpha = 1 - beta;
////		}else{
////			alpha = diversity;
////		}
//
////		float lbAlpha {};
//				lbAlpha = 1 - (alpha + beta);
//				if (lbAlpha < 0)
//					{
//						lbAlpha = 0;
//					}
//
//				float scaledLBEstimate1 { };
//				float scaledLBEstimate2 { };
//
//				getLBScore((float) SCIPnodeGetEstimate(node1), (float) SCIPnodeGetEstimate(node2), (float) SCIPgetObjlimit(scip), scaledLBEstimate1, scaledLBEstimate2);
//				getDepthScore((float) SCIPnodeGetDepth(node1), (float) SCIPnodeGetDepth(node2), (float) minplungedepth, (float) maxplungedepth, scaledDepth1, scaledDepth2);
//
///// NEW DR MEDAL
//
////		estimate1 = ((1 - (scaledDepth1*diversity)) * scaledLBEstimate1) + ((scaledDepth1*diversity) * node1Div);
////		estimate2 = ((1 - (scaledDepth2*diversity)) * scaledLBEstimate2) + ((scaledDepth2*diversity) * node2Div);
//
/////// ALPHA * DIV + BETA * DEPTH - WHAT  I SHOULD HAVE USED
//				estimate1 = (lbAlpha) * scaledLBEstimate1 + (alpha * (2 * node1Div)) + (beta * scaledDepth1);
//				estimate2 = (lbAlpha) * scaledLBEstimate2 + (alpha * (2 * node2Div)) + (beta * scaledDepth2);
//
///////////// ALPHA TIMES MAX (DIVERSITY,DEPTH) - WHAT I USED  SO FAR
////		estimate1 = (lbAlpha) * scaledLBEstimate1
////				+ (diversity * std::max(node1Div, scaledDepth1)); // + (beta*scaledDepth1) ;
////		estimate2 = (lbAlpha) * scaledLBEstimate2
////				+ (diversity * std::max(node2Div, scaledDepth2)); // + (beta*scaledDepth2) ;
//
//				if ((SCIPisInfinity(scip, estimate1) && SCIPisInfinity(scip, estimate2)) || (SCIPisInfinity(scip, -estimate1) && SCIPisInfinity(scip, -estimate2)) || SCIPisEQ(scip, estimate1, estimate2))
//					{
//						SCIP_NODETYPE nodetype1;
//						SCIP_NODETYPE nodetype2;
//
//						nodetype1 = SCIPnodeGetType(node1);
//						nodetype2 = SCIPnodeGetType(node2);
//						if (nodetype1 == SCIP_NODETYPE_CHILD && nodetype2 != SCIP_NODETYPE_CHILD)
//							return -1;
//						else if (nodetype1 != SCIP_NODETYPE_CHILD && nodetype2 == SCIP_NODETYPE_CHILD)
//							return +1;
//						else if (nodetype1 == SCIP_NODETYPE_SIBLING && nodetype2 != SCIP_NODETYPE_SIBLING)
//							return -1;
//						else if (nodetype1 != SCIP_NODETYPE_SIBLING && nodetype2 == SCIP_NODETYPE_SIBLING)
//							return +1;
//						else
//							{
//								int depth1;
//								int depth2;
//
//								depth1 = SCIPnodeGetDepth(node1);
//								depth2 = SCIPnodeGetDepth(node2);
//								if (depth1 < depth2)
//									return -1;
//								else if (depth1 > depth2)
//									return +1;
//								else return 0;
//							}
//					}


//				if (SCIPisLT(scip, estimate1, estimate2))
//					return -1;
//
//				if (SCIPisGT(scip, estimate1, estimate2))
//					return +1;

				/* depths are equal; prefer node with smaller number */
				SCIP_Longint number1;
				SCIP_Longint number2;

				number1 = SCIPnodeGetNumber(node1);
				number2 = SCIPnodeGetNumber(node2);
				assert(number1 != number2);

				if (number1 < number2)
					return -1;
				else return +1;


			}
	}

/*
 * bfs specific interface methods
 */

/** creates the node selector for best first search and includes it in SCIP */
SCIP_RETCODE SCIPincludeDiversitreeBfs(SCIP *scip /**< SCIP data structure */
)
	{
		SCIP_NODESELDATA *nodeseldata;
		SCIP_NODESEL *nodesel;

		/* allocate and initialize node selector data; this has to be freed in the destructor */
		SCIP_CALL(SCIPallocBlockMemory(scip, &nodeseldata));

		/* include node selector */
		SCIP_CALL(SCIPincludeNodeselBasic(scip, &nodesel, NODESEL_NAME, NODESEL_DESC, NODESEL_STDPRIORITY, NODESEL_MEMSAVEPRIORITY, nodeselSelectdiversitreenode, nodeselCompdiversitreenode, nodeseldata));

		assert(nodesel != NULL);

		SCIP_CALL(SCIPsetNodeselCopy(scip, nodesel, nodeselCopydiversitreenode));
		SCIP_CALL(SCIPsetNodeselFree(scip, nodesel, nodeselFreediversitreenode));

		/* add node selector parameters */
		SCIP_CALL(SCIPaddIntParam(scip, "nodeselection/diversitreenode/minplungedepth", "minimal plunging depth, before new best node may be selected (-1 for dynamic setting)", &nodeseldata->minplungedepth, TRUE, MINPLUNGEDEPTH, -1, INT_MAX, NULL, NULL));//-1
		SCIP_CALL(SCIPaddIntParam(scip, "nodeselection/diversitreenode/maxplungedepth", "maximal plunging depth, before new best node is forced to be selected (-1 for dynamic setting)", &nodeseldata->maxplungedepth, TRUE, MAXPLUNGEDEPTH, -1, INT_MAX, NULL, NULL));//-1
		SCIP_CALL(SCIPaddRealParam(scip, "nodeselection/diversitreenode/maxplungequot", "maximal quotient (curlowerbound - lowerbound)/(cutoffbound - lowerbound) where plunging is performed", &nodeseldata->maxplungequot, TRUE, MAXPLUNGEQUOT, 0.0, SCIP_REAL_MAX, NULL, NULL));//0.0

		return SCIP_OKAY;
	}

