/*
 * diversityCalculator.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "diversityCalculator.h"

//#include "diversitycalculator.h"

diversityCalculator::diversityCalculator(solutionStoreVector solutions)
	{
		utilities ut;

//		printf("This is solutions Size: %d \n \n", solutions.m_solutionsVector.size());

		if (solutions.m_solutionsVector.size() > 0){
				try
				{
						std::stof(solutions.m_solutionsVector[0][0]);// if the first element in the inner vector is a float, then call utilities using "false" for vectorHasHeaders
						diversityCalculator::m_solutionMap = ut.stringSolutionsToFloat(solutions.m_solutionsVector, false);
				} catch (...)
					{
						diversityCalculator::m_solutionMap = ut.stringSolutionsToFloat(solutions.m_solutionsVector, true);
					}

				diversityCalculator::m_binaryLocations = solutions.m_binaryLocationsVector;
				//    diversityCalculator::m_diversityScore = 0;
		}
	}

float diversityCalculator::dbinQuick(std::vector<std::vector<float>> solutionVector)
	{

		if (solutionVector.size() > 0) {
				std::vector<float> results(solutionVector.size(), 0);
				float hammingDistance = 0;
				float dbinScore = 0;

				//    float a = 0;
				//    float b = 0;

				//    std::cout << "................. Computing diversity scores for " << solutionVector.size() << " solutions ......... \n \n";

				for (int x = 0; x < static_cast<int>(solutionVector.size()); x++)
					{
						//        std::cout << ".";
						std::vector<float> firstSolutionVector = solutionVector[x];
						std::valarray<float> firstSolutionValarry(firstSolutionVector.data(), firstSolutionVector.size());

						for (int t = x + 1; t < static_cast<int>(solutionVector.size()); t++)
							{
								//            hammingDistance = 0;

								std::vector<float> secondSolutionVector = solutionVector[t];
								std::valarray<float> secondSolutionValarry(secondSolutionVector.data(), secondSolutionVector.size());

								std::valarray<float> tmpResult = std::abs(secondSolutionValarry - firstSolutionValarry);

								hammingDistance = tmpResult.sum() / (float) firstSolutionValarry.size();
								dbinScore += hammingDistance;

							}
					}

				//    std::cout << "\n ......... Completed computing diversity scores for " << solutionVector.size() << " all solutions .......... " << std::endl;
				float tp = static_cast<float>(solutionVector.size());

				float m_diversityScore = (2 / (tp * (tp - 1))) * dbinScore;

				if (std::isnan(m_diversityScore))
					{
						m_diversityScore = 0;
					}

				//    std::cout << "This is the final diversity score: " << m_diversityScore << "\n";

				return m_diversityScore;
		}
		else{
				return 0;
		}

	}

///**<  We are using mean Diversity Score. Method to code DBin as described in the paper by Read Danna etal 2009.
// * It takes a vector of float vectors and computes all hamming distance between all vectors and returns a vector of the results */
//float dBinOneValarray(std::vector<std::vector<float>> *solutionMap, /**< Pointer to the vector of all solutions. */
//int solutionMapSize, /**< Size of solutionMap */
//std::vector<float> &nodeVarMap, /** Pointer to the vector of the Variables branched on the node we are checking */
//int nodeVarMapSize, /**< Size of nodeVarMap */
//float &diversityScore, /** reference to Final Diversity Score result  */
//std::vector<bool> &nodeValarraySelector /** column selector for the node  */
//) {
//
////	assert(solutionMap != NULL);
////	assert(diversityScore != NULL);
//
//	float hammingDistance = 0;
//	float dbinScore = 0;
//
////	float a2[nodeVarMap.size()];
////	std::copy(nodeVarMap.begin(), nodeVarMap.end(), a2);
//
//	//valarray<int> myVala ( myVect.data(), myVect.size() );
////	std::valarray<float> nd2Varray (a2,nodeVarMap.size());
//	std::valarray<float> nd2Varray(nodeVarMap.data(), nodeVarMap.size());
//
////	std::cout << " node array size: " << nd2Varray.size() << " Vector size: " << nodeVarMap.size() <<  "\n";
//
//	bool b2[nodeValarraySelector.size()];
//	std::copy(nodeValarraySelector.begin(), nodeValarraySelector.end(), b2);
//
//	std::valarray<bool> nd2VSelector(b2, nodeValarraySelector.size());
//
//	//	int a[9] = {1,2,3,4,5,6,7,8,9};
//	//	std::valarray<int> all(a,9);          // all = {1,2,3,4,5,6,7,8,9}
//	//
//	//	bool b[9]= {true,false,true,false,true,false,true,false,true};
//	//	std::valarray<bool> vb(b,9);
//	//	std::valarray<int> odd2 = all[vb];    // odd2 = {1,3,5,7,9}
//
//	hammingDistance = 0;
//	float nodeSprocessed = 0;
////		std::cout  <<  " Starting for loop: SolMapSize: " << solutionMap.size() << "length of things in [0] " << solutionMap[0].size()  ;
//
//	for (int i = 0; i < solutionMapSize; i++) {
////		for (std::vector<float> i : solutionMap){
//
////			vector<int> vect2(vect1.begin(), vect1.end());
////			std::vector<float> tmpVector (solutionMap->at(x).begin(), solutionMap->at(x).end()) ;
////			std::vector<float> tmpVector = solutionMap->at(i);
//
////			float a[i.size()];
////			std::copy(i.begin(), i.end(), a);
//
////			std::cout << " inner node array size: " << sizeof(a)/sizeof(a[0]) << " Vector size: " << i.size() <<  "\n";
//
//		std::valarray<float> allValarray(solutionMap->at(i).data(),
//				solutionMap->at(i).size());
////			std::valarray<float> allValarray (a,i.size());
////			std::cout << " Inner node array size: " << allValarray.size() << " Vector size: " << i.size() <<  "\n";
//		std::valarray<float> tmpValArray = allValarray[nd2VSelector];
//
////			std::cout << " Inner Final node array size: " << tmpValArray.size() << " Vector size: " << nd2Varray.size() <<  "\n";
//
//		std::valarray<float> tmpResult = std::abs(nd2Varray - tmpValArray);
//		hammingDistance += tmpResult.sum() / (float) nodeVarMapSize;
////			nodeSprocessed += nodeVarMapSize;
////		std::cout << " HD: " << hammingDistance;
//
//	}
////		std::cout << "\n";
////		std::cout  <<  " Ending for loop " ;
//
////		std::cout  <<  " This was  hamming distance:  " << hammingDistance  <<  "\n"  ;
//
//	//TODO Try Not using the mean and see what happens to the final outoput
//	dbinScore = hammingDistance;
////		std::cout << " DB: " << dbinScore;
//
//	float tp = static_cast<float>(solutionMapSize) + 1; // +1 is because of the node added; i.e. the node we are testing it's diversity
//	diversityScore = (2 / (tp * (tp - 1))) * dbinScore;
//
//	if (std::isnan(diversityScore) and nodeVarMap.size() == 0) {
//
////		std::cout << " dbinScore:" << dbinScore << " solutionMapSize:" << solutionMapSize << " nodeVarMapSize::" << nodeVarMap.size() << " numVars" << nodeValarraySelector.size() <<  " \n" ;
//		diversityScore = 1;
//	}
//
////	std::cout << "This is the Final Diversity Score: " << diversityScore
////			<< std::endl;
////	diversityScore = dbinScore;
//
//	return dbinScore;
//}

float diversityCalculator::dbinNode(std::vector<float> solutionVector)
	{

		if (solutionVector.size() == 0)
			{
				return 0;
			}
		else
			{

				std::vector<float> results(diversityCalculator::m_solutionMap.size(), 0);
				//		curState.setNumberSolutionsGenerated(diversityCalculator::m_solutionMap.size());
				float hammingDistance = 0;
				float dbinScore = 0;

				bool b2[diversityCalculator::m_binaryLocations.size()];
				//				printf("This is Bool Size: %d, and this is solVec Size: %d:",diversityCalculator::m_binaryLocations.size(),solutionVector.size());
				std::copy(diversityCalculator::m_binaryLocations.begin(), diversityCalculator::m_binaryLocations.end(), b2);

				std::valarray<bool> binaryValuesColumnSelector(b2, diversityCalculator::m_binaryLocations.size());

				std::valarray<float> firstSolutionValarry(solutionVector.data(), solutionVector.size());
				//				std::valarray<float> binaryValuesFirstSolutionValarry = firstSolutionValarry[binaryValuesColumnSelector];
				std::valarray<float> binaryValuesFirstSolutionValarry(solutionVector.data(), solutionVector.size());

				for (int x = 0; x < static_cast<int>(diversityCalculator::m_solutionMap.size()); x++)
					{

						std::vector<float> secondSolutionVector = diversityCalculator::m_solutionMap[x];
						std::valarray<float> secondSolutionValarry(secondSolutionVector.data(), secondSolutionVector.size());
						std::valarray<float> binaryValuesSecondSolutionValarry = secondSolutionValarry[binaryValuesColumnSelector];

						//						printf("This is first Valarray Size: %d, and this is sec Valarray Size: %d:",binaryValuesFirstSolutionValarry.size(),binaryValuesSecondSolutionValarry.size());

						std::valarray<float> tmpResult = std::abs(binaryValuesSecondSolutionValarry - binaryValuesFirstSolutionValarry);


						hammingDistance = tmpResult.sum() / (float)(firstSolutionValarry.size());
						dbinScore += hammingDistance;


					}

				//    std::cout << "\n ......... Completed computing diversity scores for all solutions .......... \n";
				float tp = static_cast<float>(diversityCalculator::m_solutionMap.size());

				float m_diversityScore = (2 / (tp * (tp - 1))) * dbinScore;

				if (std::isnan(m_diversityScore))
					{
						m_diversityScore = 0;
					}

				return m_diversityScore;
			}
	}

int diversityCalculator::dbinAll(currentStateVariables &curState)
	{

		if ( diversityCalculator::m_solutionMap.size() >0 ){


				std::vector<float> results(diversityCalculator::m_solutionMap.size(), 0);
				curState.setNumberSolutionsGenerated(diversityCalculator::m_solutionMap.size());
				float hammingDistance = 0;
				float dbinScore = 0;
				float numBin = 0;

				bool b2[diversityCalculator::m_binaryLocations.size()];
				std::copy(diversityCalculator::m_binaryLocations.begin(), diversityCalculator::m_binaryLocations.end(), b2);

				std::valarray<bool> binaryValuesColumnSelector(b2, diversityCalculator::m_binaryLocations.size());

				for (int x = 0; x < static_cast<int>(diversityCalculator::m_solutionMap.size()); x++)
					{

						std::vector<float> firstSolutionVector = diversityCalculator::m_solutionMap[x];
						std::valarray<float> firstSolutionValarry(firstSolutionVector.data(), firstSolutionVector.size());
						std::valarray<float> binaryValuesFirstSolutionValarry = firstSolutionValarry[binaryValuesColumnSelector];

						for (int t = x + 1; t < static_cast<int>(diversityCalculator::m_solutionMap.size()); t++)
							{
								//            hammingDistance = 0;

								std::vector<float> secondSolutionVector = diversityCalculator::m_solutionMap[t];
								std::valarray<float> secondSolutionValarry(secondSolutionVector.data(), secondSolutionVector.size());
								std::valarray<float> binaryValuesSecondSolutionValarry = secondSolutionValarry[binaryValuesColumnSelector];

								std::valarray<float> tmpResult = std::abs(binaryValuesSecondSolutionValarry - binaryValuesFirstSolutionValarry);

								hammingDistance = tmpResult.sum() / (float) binaryValuesFirstSolutionValarry.size();
								dbinScore += hammingDistance;

							}
					}

				//    std::cout << "\n ......... Completed computing diversity scores for all solutions .......... \n";
				float tp = static_cast<float>(diversityCalculator::m_solutionMap.size());

				float m_diversityScore = (2 / (tp * (tp - 1))) * dbinScore;

				if (std::isnan(m_diversityScore))
					{
						curState.setDiversity(0);
					}
				else
					{
						curState.setDiversity(m_diversityScore);
					}
		}
		else{
				curState.setDiversity(0);
		}

		//    std::cout << "This is the final diversity score: " << curState.getDiversity() << std::endl;
		//    std::cout << "This is the final diversity score: " << m_diversityScore << std::endl;

		return 0;
	}

int diversityCalculator::dbinP(currentStateVariables &curState)
	{

		//    std::vector<float> results(diversityCalculator::m_solutionMap.size(), 0);
		//    std::cout << "................. Computing diversity scores for " << " ......... \n \n";
		printf("................. Computing diversity scores for  ......... \n \n");

		if ( diversityCalculator::m_solutionMap.size() >0 ){
		std::vector<std::vector<float>> results { };
		float diversityScore { };
		curState.setNumberSolutionsGenerated(diversityCalculator::m_solutionMap.size());

		for (int x = 0; x < static_cast<int>(diversityCalculator::m_solutionMap.size()); x++)
			{

				if (results.size() < curState.pMostDiverse())
					{
						results.push_back(diversityCalculator::m_solutionMap[x]);
					}
				else
					{

						int bestExitIndex { };
						float bestDiversity { };
						diversityScore = diversityCalculator::dbinQuick(results);
						bestDiversity = diversityScore;
						bestExitIndex = -1;

						//            std::cout << "\n This is the initial length of the vector: " << results.size();
						//            printf( "\n This is the initial length of the vector: %i  \n" , static_cast<int>(results.size()) );
						results.push_back(diversityCalculator::m_solutionMap[x]);

						for (int j = 0; j < static_cast<int>(curState.pMostDiverse()); j++)
							{

								std::vector<float> tmp { };

								tmp = results[j];
								results.erase(results.begin() + j);

								diversityScore = diversityCalculator::dbinQuick(results);

								if (diversityScore > bestDiversity)
									{
										bestDiversity = diversityScore;
										bestExitIndex = j;
									}

								results.insert(results.begin() + j, tmp);
							}

						if (bestExitIndex == -1)
							{
								results.pop_back();
							}
						else
							{
								results.erase(results.begin() + bestExitIndex);
							}
					}
			}

		diversityScore = diversityCalculator::dbinQuick(results);

		//    printf( " Number of Items in List: %i  \n" , static_cast<int>(results.size()) );
		printf("\n Final Diversity: %f  \n", diversityScore);

		//    std::cout << " Number of Items in List: " << results.size();
		//    std::cout << "\n Final Diversity: " << diversityScore;

		curState.setDiversity(diversityScore);
		// .....................................................................................................
		}
		else{
				curState.setDiversity(0);
		}
		return 0;
	}

