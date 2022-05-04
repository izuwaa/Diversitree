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
#include <map>

#define NODESEL_NAME             "DiversitreeBFSNodeSel"
#define NODESEL_DESC             "Diversitree node selection implementation"
#define NODESEL_STDPRIORITY      200001
#define NODESEL_MEMSAVEPRIORITY       100001

/** default values for user parameters */
#define INITIALSIZE             1024    /**< initial size of node visits array (increased dynamically if required) */
#define MAXNODELIMIT            20000000 /**< the maximum value for user parameter nodelimit */
#define JUSTCREATED							999.0
//#define CREATEDNOTUSED					995.0

//#define Optimum_Obj	getCurrentState().getMBestObjective()
//#define QPercent		getCurrentState().percentNearOptimal()
//#define alpha		getCurrentState().alphaValue()
//#define beta				getCurrentState().betaValue()

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

		//		float* nodeDiversities;      /**< array to store the diversity of each node as it is visited */
		int sizeNodeDiversities;     /**< the size of the nodeDiversities array */

		//		int*                  nodevisits;         /**< array to store the number of node visits so far for every node */
		//		int actualSizeNodeDiversities;     /**< the actual number of nodes seen so far */
		int maximumDepth; 					/**< Maximum depth seen so far */
		int countedSoFar;						/**< Number of solutions counted so far */
		//		std::vector<float>* seen;
		SCIP_Real* seen;

	};


/*
 * Local methods
 */

/** get the number times @p node has been visited so far */
static
float nodeGetDiversity(
		SCIP_NODESELDATA*     nodeseldata,        /**< node selector data */
		SCIP_NODE*            node                /**< the node in question */
)
	{
		int nodenumber;
		float result = 999.0;

		assert(nodeseldata != NULL);
		assert(node != NULL);

		//		std::vector<float>* localSeen =  nodeseldata->seen;

		/* nodes numbers start with 1 for the root node */
		nodenumber = (int)(SCIPnodeGetNumber(node) - 1);
		//		printf("looking for node %d :", nodenumber);

		//		if (nodenumber > nodeseldata->actualSizeNodeDiversities){
		if (nodenumber > nodeseldata->sizeNodeDiversities){
//				printf("Didn't find node %d and result is %f : \n \n", nodenumber, result);
				return JUSTCREATED;
		}else{
				//				result  = nodeseldata->seen->at(nodenumber);
				//				printf("Found node %d and result is %f :", nodenumber, result);
				//				return nodeseldata->seen->at(nodenumber);
				return nodeseldata->seen[nodenumber];
				//				return result;
		}

	}


/** increases the visits counter along the path from @p node to the root node */
static
void updateNodeDiversity(
		SCIP_NODESELDATA*     nodeseldata,        /**< node selector data */
		SCIP_NODE*            node,               /**< leaf node of path along which the visits are backpropagated */
		float diversity														/**< The diversity to store */
)
	{
		//		float* visits;


		assert(nodeseldata != NULL);
		assert(node != NULL);

		//		visits = nodeseldata->nodeDiversities;
		//		assert(visits != NULL);

		/* Change the diversity stored at the node number */
		int nodenumber;
		//		std::vector<float>* localSeen =  nodeseldata->seen;

		nodenumber = (int)(SCIPnodeGetNumber(node) - 1);
		//		nodeseldata->nodeDiversities[nodenumber] = diversity;

		//		printf("trying to update %d, %f \n", nodenumber, diversity);

		//		auto const result = nodeseldata->seen.insert(std::make_pair(nodenumber, 0));
		//		if (not result.second) { result.first->second = 0; }
		try{
				//				nodeseldata->seen->at(nodenumber) = diversity;
				nodeseldata->seen[nodenumber] = diversity;
		}
		catch (...){
				//				nodeseldata->seen->at(nodenumber) = diversity;
				nodeseldata->seen[nodenumber] = diversity;
				//				nodeseldata->seen.insert(std::pair<int,float>(nodenumber,0));
		}

	}

/** keeps visits array large enough to save visits for all nodes in the branch and bound tree */
static
SCIP_RETCODE expandMemorySize(
		SCIP*                 scip,               /**< SCIP data structure */
		SCIP_NODESELDATA*     nodeseldata         /**< node selector data */
)
	{
		assert(nodeseldata != NULL);

		/* if array has not been allocated yet, do this now with default initial capacity */
		//		if( nodeseldata->sizeNodeDiversities == 0 )
		if( nodeseldata->seen == NULL or nodeseldata->sizeNodeDiversities == 0 )
			{
				//				SCIP_CALL( SCIPallocClearMemoryArray(scip, &nodeseldata->nodeDiversities, INITIALSIZE) ); /*lint !e506*/
				//				nodeseldata->seen = new std::vector<float>(INITIALSIZE,JUSTCREATED);
				//				nodeseldata->sizeNodeDiversities = INITIALSIZE;

				SCIP_CALL( SCIPallocClearMemoryArray(scip, &nodeseldata->seen, INITIALSIZE) ); /*lint !e506*/
				nodeseldata->sizeNodeDiversities = INITIALSIZE;

				for (int i = 0; i< INITIALSIZE; ++i){
						nodeseldata->seen[i] = JUSTCREATED;
				}

			}

		//				assert(nodeseldata->nodelimit >= SCIPgetNNodes(scip));

		/* if user node limit has not been reached yet, resize the visits array if necessary */
		if( nodeseldata->sizeNodeDiversities < (int)(2 * SCIPgetNNodes(scip)))
			{
				int newcapacity;
				newcapacity = MIN(2 * nodeseldata->sizeNodeDiversities, MAXNODELIMIT);

				if (newcapacity < (int)(2 * SCIPgetNNodes(scip)) ){
						printf("Original new capacity was still not enough: newCap %d  :-- : numNodes %d ",newcapacity,SCIPgetNNodes(scip));

						newcapacity = MIN((int)(2 * SCIPgetNNodes(scip)), MAXNODELIMIT);

				}

				printf( "Resizing node visits array, old capacity: %d new capacity : %d\n", nodeseldata->sizeNodeDiversities, newcapacity);
				assert(newcapacity > nodeseldata->sizeNodeDiversities);

				SCIP_CALL( SCIPreallocMemoryArray(scip, &nodeseldata->seen, newcapacity) );
				BMSclearMemoryArray(&(nodeseldata->seen[nodeseldata->sizeNodeDiversities]), newcapacity - nodeseldata->sizeNodeDiversities); /*lint !e866*/

				for (int i = nodeseldata->sizeNodeDiversities; i< newcapacity; ++i){
						nodeseldata->seen[i] = JUSTCREATED;
				}


				//				std::vector<float> temp(newcapacity-nodeseldata->sizeNodeDiversities,JUSTCREATED);

				//				nodeseldata->seen->reserve(nodeseldata->seen->size()  + std::distance(temp.begin(), temp.end()));
				//				nodeseldata->seen->insert(nodeseldata->seen->end(), temp.begin(), temp.end());


				nodeseldata->sizeNodeDiversities = newcapacity;
			}

		return SCIP_OKAY;
	}



///** keeps visits array large enough to save visits for all nodes in the branch and bound tree */
//static
//SCIP_RETCODE ensureMemorySize(
//		SCIP*                 scip,               /**< SCIP data structure */
//		SCIP_NODESELDATA*     nodeseldata         /**< node selector data */
//)
//	{
//		assert(nodeseldata != NULL);
//
//		/* if array has not been allocated yet, do this now with default initial capacity */
//		if( nodeseldata->nodeDiversities == NULL )
//			{
//				SCIP_CALL( SCIPallocClearMemoryArray(scip, &nodeseldata->nodeDiversities, INITIALSIZE) ); /*lint !e506*/
//				nodeseldata->sizeNodeDiversities = INITIALSIZE;
//			}
//
//		//		assert(nodeseldata->nodelimit >= SCIPgetNNodes(scip));
//
//		/* if user node limit has not been reached yet, resize the visits array if necessary */
//		if( nodeseldata->sizeNodeDiversities < (int)(2 * SCIPgetNNodes(scip)))
//			{
//				int newcapacity;
//				newcapacity = MIN(2 * nodeseldata->sizeNodeDiversities, MAXNODELIMIT);
//
//				printf( "Resizing node visits array, old capacity: %d new capacity : %d\n", nodeseldata->sizeNodeDiversities, newcapacity);
//				assert(newcapacity > nodeseldata->sizeNodeDiversities);
//
//				SCIP_CALL( SCIPreallocMemoryArray(scip, &nodeseldata->nodeDiversities, newcapacity) );
//				BMSclearMemoryArray(&(nodeseldata->nodeDiversities[nodeseldata->sizeNodeDiversities]), newcapacity - nodeseldata->sizeNodeDiversities); /*lint !e866*/
//
//				nodeseldata->sizeNodeDiversities = newcapacity;
//			}
//
//		return SCIP_OKAY;
//	}






/*
 * Callback methods
 */

/** copy method for node selector plugins (called when SCIP copies plugins) */
static SCIP_DECL_NODESELCOPY(nodeselCopydiversitreenode)
																																																																																																																																																																			{ /*lint --e{715}*/
		assert(scip != NULL);
		assert(nodesel != NULL);
		//		assert(cState != NULL);
		assert(strcmp(SCIPnodeselGetName(nodesel), NODESEL_NAME) == 0);

		/* call inclusion method of node selector */
		SCIP_CALL(SCIPincludeDiversitreeBfs(scip, cState));
		//		SCIP_CALL(SCIPincludeDiversitreeBfs(scip));

		return SCIP_OKAY;
																																																																																																																																																																			}




/** solving process initialization method of node selector (called when branch and bound process is about to begin) */


/** solving process deinitialization method of node selector (called when branch and bound process data gets freed) */
static
SCIP_DECL_NODESELEXITSOL(nodeselExitdiversitreenode)
  										 {


//		printf("Exiting nodeSel \n");
		SCIP_NODESELDATA* nodeseldata;
		assert(scip != NULL);
		assert(nodesel != NULL);

		nodeseldata = SCIPnodeselGetData(nodesel);

		assert(nodeseldata != NULL);

		if( nodeseldata->sizeNodeDiversities > 0 )
			{
//				printf("Trying to exit");
				//				delete nodeseldata->seen;
				SCIPfreeMemoryArray(scip, &nodeseldata->seen);
//				printf("exited");
			}
		nodeseldata->sizeNodeDiversities = 0;
		nodeseldata->countedSoFar = 0;
		nodeseldata->maximumDepth = 0;


		return SCIP_OKAY;
  										 }



/** solving process initialization method of node selector (called when branch and bound process is about to begin) */
static
SCIP_DECL_NODESELINITSOL(nodeselInitdiversitreenode)
  																																							 {
//		printf("initializing nodeSel \n");
		SCIP_NODESELDATA* nodeseldata;
		assert(scip != NULL);
		assert(nodesel != NULL);

		nodeseldata = SCIPnodeselGetData(nodesel);

		assert(nodeseldata != NULL);

		nodeseldata->countedSoFar = 0;
		//		nodeseldata->actualSizeNodeDiversities = INITIALSIZE;
		nodeseldata->maximumDepth = 0;

		//		printf("Trying to allocate");
		//		nodeseldata->seen = new std::vector<float>(INITIALSIZE,JUSTCREATED);
		//		printf("allocated");
		//		nodeseldata->seen = {};
		//		nodeseldata->sizeNodeDiversities = INITIALSIZE;
		nodeseldata->sizeNodeDiversities = 0;

		return SCIP_OKAY;
  																																							 }




/** destructor of node selector to free user data (called when SCIP is exiting) */
/**! [SnippetNodeselFreeBfs] */
static SCIP_DECL_NODESELFREE(nodeselFreediversitreenode)
																																																																																																																																																																			{ /*lint --e{715}*/

//		printf("Freeing nodeSel \n");
		SCIP_NODESELDATA *nodeseldata;

		assert(scip != NULL);
		assert(nodesel != NULL);
		assert(strcmp(SCIPnodeselGetName(nodesel), NODESEL_NAME) == 0);


		/* free user data of node selector */
		nodeseldata = SCIPnodeselGetData(nodesel);
		//		assert(nodeseldata != NULL);

		//		std::cout << "\n \n Complete node size: " << nodeseldata->sizeNodeDiversities << " \n";
		//		printf(" \n \n Complete node size: float %f :--: dec %d  \n  ",nodeseldata->sizeNodeDiversities,nodeseldata->sizeNodeDiversities);

		if (nodeseldata->sizeNodeDiversities > 0){
				//				printf(" \n \n Trying to delete Dec: %d , Int %i, float %f \n \n ", nodeseldata->sizeNodeDiversities, nodeseldata->sizeNodeDiversities, nodeseldata->sizeNodeDiversities);
				//				delete nodeseldata->seen;
				//				assert(nodeseldata->seen != NULL);
				//				if( nodeseldata->seen == NULL){
				////						printf("It was null \n");
				//				}
				//				else{
				//						printf("It was not null \n");

				//						delete nodeseldata->seen;
				nodeseldata->seen = NULL;
				//						SCIPfreeMemoryArray(scip, nodeseldata->seen);
				//						BMSfreeMemoryArray(nodeseldata->seen);
				//						free(&nodeseldata->seen);
				//						BMSfreeMemory(&node);

				//				}

				//				printf("Deleted");
		}

		nodeseldata->sizeNodeDiversities = 0;
		nodeseldata->countedSoFar = 0;
		nodeseldata->maximumDepth = 0;

		SCIPfreeBlockMemory(scip, &nodeseldata);
		SCIPnodeselSetData(nodesel, NULL);

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
SCIP_RETCODE getNodeBranchVarMapValarray(
		SCIP *scip,
		SCIP_NODE *node,
		std::vector<float> &nodeBranchMap,
		std::vector<bool> &nodeSelector,
		int &numBinVar)
	{

		SCIP_VAR **branchvars;
		SCIP_Real *branchbounds;
		SCIP_BOUNDTYPE *boundtypes;
		int nbranchvars;
		int size;
		//		std::vector<float> tmpBranchMap { };

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
								nodeBranchMap.push_back((float)(branchbounds[(int)(ret - variableNames.begin())])  );
								//								nodeBranchMap.push_back( 0 );
								//				nodeBranchMap.push_back(std::string(SCIPvarGetName(branchvars[v])).substr(2, std::string(SCIPvarGetName(branchvars[v])).length()));
								//								nodeBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
								//								tmpBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
								//				std::cout << " Branhc var num: " << (int)(ret - variableNames.begin()) << '\n';
								//				branchVarCount += 1;

							}
						else if (SCIPvarGetType(SCIPgetOrigVars(scip)[v]) == SCIP_VARTYPE_BINARY and ret == variableNames.end() )
							{
								//								nodeBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
								//								tmpBranchMap.push_back((float) branchbounds[(int) (ret - variableNames.begin())]);
								//								nodeBranchMap.push_back(9999.0);
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

		//		float minVal = std::min( { (float) Optimum_Obj, node1LB, node2LB });
		float minVal = std::min( { (float) cState->getMBestObjective(), node1LB, node2LB });

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


		//				printf(" comparing ..... \n");
		SCIP_NODESELDATA *nodeseldata;
		nodeseldata = SCIPnodeselGetData(nodesel);

		SCIP_Real lowerbound1;
		SCIP_Real lowerbound2;

		SCIP_Longint nsols;
		SCIP_Bool valid;

		valid = FALSE;
		nsols = SCIPgetNCountedSols(scip, &valid);

		//		printf("Nsols: %d \n ", nsols);

		//	std::cout<< "nsols: " << nsols;
		//	std::cout<< "getCountedSolutions: " << getCountedSolutions() << std::endl;

		// Get the Node Map of Variables  NODE 1 - if diversityUtilization > 0

		float node1Div { };
		float node2Div { };

		float computeNode1 = nodeGetDiversity(nodeseldata, node1);
		float computeNode2 = nodeGetDiversity(nodeseldata, node2);

		bool cNode1 = false;
		bool cNode2 = false;
		bool changed = false;


		int minplungedepth = nodeseldata->minplungedepth;
		int maxplungedepth = nodeseldata->maxplungedepth;

		// SET Max tree Depth
		//		int maxDepth = std::max( { cState->getMMaxTreeDepthSoFar(), SCIPnodeGetDepth(node2), SCIPnodeGetDepth(node1) });
		//		cState->setMMaxTreeDepthSoFar(maxDepth);

		int maxDepth = std::max( { nodeseldata->maximumDepth, SCIPnodeGetDepth(node2), SCIPnodeGetDepth(node1) });
		nodeseldata->maximumDepth = maxDepth;


		bool depthCutOffTest = cState->depthcutoff() == 0 or maxDepth >= cState->depthcutoff();
		bool solCutOffTest = cState->solutioncutoff() == 0 or ( nsols > 0 and  nsols >= (cState->solutioncutoff() * cState->requestedNumberOfSolutions()  ) );
		bool alphaTest = cState->alphaValue() > 0 or cState->betaValue() > 0;



		SCIP_CALL( expandMemorySize(scip, nodeseldata) );


		//		printf("Node nums and Capacities: %d :-: %d :-: %d",  (int) SCIPnodeGetNumber(node1) ,(int) SCIPnodeGetNumber(node2), nodeseldata->sizeNodeDiversities);

		if (alphaTest and solCutOffTest and depthCutOffTest)
			{


				//				printf(" AlphaTest \n");
				/* make sure that UCT node selection data is large enough to store node visits */
				//				SCIP_CALL( ensureMemorySize(scip, nodeseldata) );

				// New Method - using Val Array
				// Get the Node Map of Variables  NODE 1
				//		std::cout << " Starting 1 ";
				std::vector<float> nd1Map { };

				std::vector<bool> nd1Selector { };
				int numBin { };


				// New Method - using Val Array
				// Get the Node Map of Variables  NODE 2
				std::vector<float> nd2Map { };
				std::vector<bool> nd2Selector { };
				;//   =  getNodeVarMap(scip, node2);
				int numBin2 { };

				//				nodeGetDiversity(nodeseldata, node1, computeDiversity);

				//				printf("This is Counted so far BEFORE: %d :--: %d  :--: %d ", cState->getMCountedSolutionsSoFar(), nodeseldata->countedSoFar, nsols);


				if (nodeseldata->countedSoFar != nsols){

						getNodeBranchVarMapValarray(scip, node1, nd1Map, nd1Selector, numBin);
						getNodeBranchVarMapValarray(scip, node2, nd2Map, nd2Selector, numBin2);
						cNode1 = true;
						cNode2 = true;

						//						printf(" Not equal: %d  %d", nodeseldata->countedSoFar, nsols);

				}
				else{
						//						printf(" Equal: %d  %d", nodeseldata->countedSoFar, nsols);

						if (computeNode1 >= 999.0){
								//								printf(" First Node1: %d ", (int) SCIPnodeGetNumber(node1));
								getNodeBranchVarMapValarray(scip, node1, nd1Map, nd1Selector, numBin);
								cNode1 = true;
						}
						else{

								node1Div = computeNode1;
								//								printf(" BSeen Node1: %d ", computeNode1 );
						}


						if (computeNode2 >= 999.0){
								getNodeBranchVarMapValarray(scip, node2, nd2Map, nd2Selector, numBin2);
								//								printf(" First Node2: %d ", (int) SCIPnodeGetNumber(node2));
								cNode2 = true;
						}
						else{

								node2Div = computeNode2;
								//								printf(" BSeen Node2: %d ", computeNode2 );
						}


				}

				//				getNodeBranchVarMapValarray(scip, node1, nd1Map, nd1Selector, numBin);
				//				getNodeBranchVarMapValarray(scip, node2, nd2Map, nd2Selector, numBin2);


				if ( (cNode1 == false) and (cNode2==false) ){ // We dont need to compute anything for both nodes
						//						printf("No need to compute Valarray");
				}
				else{

						//						printf("This is Counted so far BEFORE: %d :--: %d  :--: %d ", cState->getMCountedSolutionsSoFar(), nodeseldata->countedSoFar, nsols);

						optionsReader opt;
						branchCountScip bcs(opt);
						bcs.getNodeDiversityValarray(scip, nd1Map, nd2Map, cNode1, cNode2, nsols != nodeseldata->countedSoFar, node1Div, node2Div, (int) SCIPnodeGetNumber(node1), (int) SCIPnodeGetNumber(node2), nd1Selector, nd2Selector, cState);

						nodeseldata->countedSoFar = nsols;
						cState->setMCountedSolutionsSoFar(nsols);

						//						printf("This is Counted so far AFTER: %d :--: %d  :--: %d \n", cState->getMCountedSolutionsSoFar(), nodeseldata->countedSoFar, nsols);

				}


				//				printf("node1Div %f :--: %f ", node1Div, computeNode1);
				//				printf("node2Div %f :--: %f \n", node2Div, computeNode2);

				if (cNode1 == true){
						//						printf(" Updating computeNode1 ");
						updateNodeDiversity(nodeseldata, node1, node1Div);
				}


				if (cNode2 == true){
						//						printf(" Updating computeNode2 ");
						updateNodeDiversity(nodeseldata, node2, node2Div);
				}


			}


		//		printf("This is Counted so far: %d,,,, %d ", cState->getMCountedSolutionsSoFar(), nodeseldata->countedSoFar);
		//		printf("Node1: %d ", SCIPnodeGetNumber(node1));
		//		printf("Node2: %d \n", SCIPnodeGetNumber(node2));
		//		seenNodes.insert(SCIPnodeGetNumber(node1));
		//		seenNodes.insert(SCIPnodeGetNumber(node2));
		//		printf("new seenNodes  Length: %d \n", seenNodes.size());
		//		cState->setMSeenNodesMap(seenNodeMaps);
		//		cState->setMSeenNodes(seenNodes);
		//		printf("Previous seenNodes  Length: %d ", seenNodes.size());
		//		printf("This is Nsols so far: %d \n", nsols);
		//		setCountedSolutions(nsols);


		assert(nodesel != NULL);
		assert(strcmp(SCIPnodeselGetName(nodesel), NODESEL_NAME) == 0);
		assert(scip != NULL);



		//		printf("This is the best Objective %f \n", cState->getMBestObjective());
		//		printf("This is the objlimit %f \n", (float) SCIPgetObjlimit(scip));


		//		getLBScore((float) SCIPnodeGetLowerbound(node1), (float) SCIPnodeGetLowerbound(node2), (float) SCIPgetObjlimit(scip), scaledLB1, scaledLB2);


		float lbAlpha { };

		if (alphaTest and solCutOffTest and depthCutOffTest){

				float scaledLB1 { };
				float scaledLB2 { };

				float scaledDepth1 { };
				float scaledDepth2 { };

				getLBScore((float) SCIPnodeGetLowerbound(node1), (float) SCIPnodeGetLowerbound(node2), cState->getMBestObjective(), scaledLB1, scaledLB2);
				getDepthScore((float) SCIPnodeGetDepth(node1), (float) SCIPnodeGetDepth(node2), (float) minplungedepth, (float) maxplungedepth, scaledDepth1, scaledDepth2);

				//		lbAlpha = 1 - (alpha + beta);
				lbAlpha = 1 - (cState->alphaValue() + cState->betaValue());
				if (lbAlpha < 0)
					{
						lbAlpha = 0;
					}

				lowerbound1 = (lbAlpha) * scaledLB1 + (cState->alphaValue() * (2 * node1Div)) + (cState->betaValue() * scaledDepth1);
				lowerbound2 = (lbAlpha) * scaledLB2 + (cState->alphaValue() * (2 * node2Div)) + (cState->betaValue() * scaledDepth2);


		}
		else{

				lowerbound1 = SCIPnodeGetLowerbound(node1);
				lowerbound2 = SCIPnodeGetLowerbound(node2);
		}

		//// NEW DR MEDAL METHOD
		//	lowerbound1 = ((1 - (scaledDepth1*diversity)) * scaledLB1) + ((scaledDepth1*diversity) * node1Div);
		//	lowerbound2 = ((1 - (scaledDepth2*diversity)) * scaledLB2) + ((scaledDepth2*diversity) * node2Div);

		///// ALPHA * DIV + BETA * DEPTH - WHAT  I SHOULD HAVE USED
		//		lowerbound1 = (lbAlpha) * scaledLB1 + (alpha * (2 * node1Div)) + (beta * scaledDepth1);
		//		lowerbound1 = (lbAlpha) * scaledLB1 + (cState->alphaValue() * (2 * node1Div)) + (cState->betaValue() * scaledDepth1);
		//		lowerbound2 = (lbAlpha) * scaledLB2 + (alpha * (2 * node2Div)) + (beta * scaledDepth2);
		//		lowerbound2 = (lbAlpha) * scaledLB2 + (cState->alphaValue() * (2 * node2Div)) + (cState->betaValue() * scaledDepth2);

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

				SCIP_Real estimate1;
				SCIP_Real estimate2;


				if (alphaTest and solCutOffTest and depthCutOffTest){

						float scaledLBEstimate1 { };
						float scaledLBEstimate2 { };

						float scaledDepth1 { };
						float scaledDepth2 { };

						lbAlpha = 1 - (cState->alphaValue() + cState->betaValue());
						if (lbAlpha < 0)
							{
								lbAlpha = 0;
							}



						//				getLBScore((float) SCIPnodeGetEstimate(node1), (float) SCIPnodeGetEstimate(node2), (float) SCIPgetObjlimit(scip), scaledLBEstimate1, scaledLBEstimate2);
						getLBScore((float) SCIPnodeGetEstimate(node1), (float) SCIPnodeGetEstimate(node2), cState->getMBestObjective(), scaledLBEstimate1, scaledLBEstimate2);
						getDepthScore((float) SCIPnodeGetDepth(node1), (float) SCIPnodeGetDepth(node2), (float) minplungedepth, (float) maxplungedepth, scaledDepth1, scaledDepth2);

						estimate1 = (lbAlpha) * scaledLBEstimate1 + (cState->alphaValue() * (2 * node1Div)) + (cState->betaValue() * scaledDepth1);
						estimate2 = (lbAlpha) * scaledLBEstimate2 + (cState->alphaValue() * (2 * node2Div)) + (cState->betaValue() * scaledDepth2);

				}
				else{

						estimate1 = SCIPnodeGetEstimate(node1);
						estimate2 = SCIPnodeGetEstimate(node2);

				}



				/// NEW DR MEDAL

				//		estimate1 = ((1 - (scaledDepth1*diversity)) * scaledLBEstimate1) + ((scaledDepth1*diversity) * node1Div);
				//		estimate2 = ((1 - (scaledDepth2*diversity)) * scaledLBEstimate2) + ((scaledDepth2*diversity) * node2Div);

				///// ALPHA * DIV + BETA * DEPTH - WHAT  I SHOULD HAVE USED
				//				estimate1 = (lbAlpha) * scaledLBEstimate1 + (cState->alphaValue() * (2 * node1Div)) + (cState->betaValue() * scaledDepth1);
				//				estimate2 = (lbAlpha) * scaledLBEstimate2 + (cState->alphaValue() * (2 * node2Div)) + (cState->betaValue() * scaledDepth2);

				/////////// ALPHA TIMES MAX (DIVERSITY,DEPTH) - WHAT I USED  SO FAR
				//		estimate1 = (lbAlpha) * scaledLBEstimate1
				//				+ (diversity * std::max(node1Div, scaledDepth1)); // + (beta*scaledDepth1) ;
				//		estimate2 = (lbAlpha) * scaledLBEstimate2
				//				+ (diversity * std::max(node2Div, scaledDepth2)); // + (beta*scaledDepth2) ;

				if ((SCIPisInfinity(scip, estimate1) && SCIPisInfinity(scip, estimate2)) || (SCIPisInfinity(scip, -estimate1) && SCIPisInfinity(scip, -estimate2)) || SCIPisEQ(scip, estimate1, estimate2))
					{
						SCIP_NODETYPE nodetype1;
						SCIP_NODETYPE nodetype2;

						nodetype1 = SCIPnodeGetType(node1);
						nodetype2 = SCIPnodeGetType(node2);
						if (nodetype1 == SCIP_NODETYPE_CHILD && nodetype2 != SCIP_NODETYPE_CHILD)
							return -1;
						else if (nodetype1 != SCIP_NODETYPE_CHILD && nodetype2 == SCIP_NODETYPE_CHILD)
							return +1;
						else if (nodetype1 == SCIP_NODETYPE_SIBLING && nodetype2 != SCIP_NODETYPE_SIBLING)
							return -1;
						else if (nodetype1 != SCIP_NODETYPE_SIBLING && nodetype2 == SCIP_NODETYPE_SIBLING)
							return +1;
						else
							{
								int depth1;
								int depth2;

								depth1 = SCIPnodeGetDepth(node1);
								depth2 = SCIPnodeGetDepth(node2);
								if (depth1 < depth2)
									return -1;
								else if (depth1 > depth2)
									return +1;
								else return 0;
							}
					}

				if (SCIPisLT(scip, estimate1, estimate2))
					return -1;

				if (SCIPisGT(scip, estimate1, estimate2))
					return +1;

				/* depths are equal; prefer node with smaller number */
				//				SCIP_Longint number1;
				//				SCIP_Longint number2;
				//
				//				number1 = SCIPnodeGetNumber(node1);
				//				number2 = SCIPnodeGetNumber(node2);
				//				assert(number1 != number2);
				//
				//				if (number1 < number2)
				//					return -1;
				//				else return +1;

			}
																																																							}

/*
 * bfs specific interface methods
 */

/** creates the node selector for best first search and includes it in SCIP */
SCIP_RETCODE SCIPincludeDiversitreeBfs(SCIP *scip /**< SCIP data structure */, currentStateVariables *currentState)
	{
		SCIP_NODESELDATA *nodeseldata;
		SCIP_NODESEL *nodesel;

		/* allocate and initialize node selector data; this has to be freed in the destructor */
		SCIP_CALL(SCIPallocBlockMemory(scip, &nodeseldata));

		/* include node selector */
		SCIP_CALL(SCIPincludeNodeselBasic(scip, &nodesel, NODESEL_NAME, NODESEL_DESC, NODESEL_STDPRIORITY, NODESEL_MEMSAVEPRIORITY, nodeselSelectdiversitreenode, nodeselCompdiversitreenode, nodeseldata));

		assert(nodesel != NULL);


		SCIP_CALL(SCIPsetNodeselCopy(scip, nodesel, nodeselCopydiversitreenode));
		SCIP_CALL(SCIPsetNodeselInitsol(scip, nodesel, nodeselInitdiversitreenode));
		SCIP_CALL(SCIPsetNodeselFree(scip, nodesel, nodeselFreediversitreenode));
		SCIP_CALL(SCIPsetNodeselExitsol(scip,nodesel,nodeselExitdiversitreenode));

		cState = currentState;

		/* add node selector parameters */
		SCIP_CALL(SCIPaddIntParam(scip, "nodeselection/diversitreenode/minplungedepth", "minimal plunging depth, before new best node may be selected (-1 for dynamic setting)", &nodeseldata->minplungedepth, TRUE, MINPLUNGEDEPTH, -1, INT_MAX, NULL, NULL));//-1
		SCIP_CALL(SCIPaddIntParam(scip, "nodeselection/diversitreenode/maxplungedepth", "maximal plunging depth, before new best node is forced to be selected (-1 for dynamic setting)", &nodeseldata->maxplungedepth, TRUE, MAXPLUNGEDEPTH, -1, INT_MAX, NULL, NULL));//-1
		SCIP_CALL(SCIPaddRealParam(scip, "nodeselection/diversitreenode/maxplungequot", "maximal quotient (curlowerbound - lowerbound)/(cutoffbound - lowerbound) where plunging is performed", &nodeseldata->maxplungequot, TRUE, MAXPLUNGEQUOT, 0.0, SCIP_REAL_MAX, NULL, NULL));//0.0

		return SCIP_OKAY;
	}

