/*
 * oneTreeGams.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: izuwaahanor
 */

#include "oneTreeGams.h"

//#include "onetreegams.h"
#include <fstream>
#include <iostream>
#include <sstream>
//#include "gams.h"


//Main Method
oneTreeGams::oneTreeGams(solutionStoreVector &generatedSolution, optionsReader &allOptions, currentStateVariables &currentState)
{

//    gams::GAMSWorkspaceInfo wsInfo;
//    wsInfo.setWorkingDirectory(allOptions.m_gamsWorkingDirectory);
//    wsInfo.setSystemDirectory(allOptions.m_gamsSystemDirectory);
//    gams::GAMSWorkspace ws(wsInfo);
//
//    // Set Solver as CPLEX
//    gams::GAMSOptions opt =  ws.addOptions();
//    opt.setAllModelTypes("cplex");
//    opt.setOptFile(1);
//    opt.setOptDir(allOptions.m_gamsOptDirectory);
//
//
//    // Add the file I want to solve
//    gams::GAMSJob t1 = ws.addJobFromFile(allOptions.m_gamsGMSFileDirectory + "/" + currentState.optimizationProblems() + ".gms");
//
//
//    // Run the solver and add the run solutions to a .txt file.
//    std::ofstream resultFile("Result.txt");
//
//    // I need to time this run and add the start time and end time to the current state values.
//    currentState.setStartTime(std::chrono::high_resolution_clock::now());
//    t1.run(opt,resultFile);
//    currentState.setStopTime(std::chrono::high_resolution_clock::now());
//
//    // Capture the multiple solutions generated
//    gams::GAMSDatabase db = ws.addDatabaseFromGDX("generatedSoln.gdx");
//
//
//    //Solution map used to transfer the solutions to csv.
////    std::vector<std::vector<std::string>> solutionMap { };
//
//    //Create a solution store to hold the solutions and the binary locations
////    solutionStore solStore;
//    std::vector<bool> binaryLocations{};
//
////    for (gams::GAMSDatabaseIter rr = t1.outDB().begin();  rr != t1.outDB().end(); ++rr){
////        std::cout << rr.operator*().name() << "\n" ;
////    }
//
//    // Loop through the GAMSDatabase and populate the solution into the solution Map
//    for (gams::GAMSSetRecord rec2 : db.getSet("Index")){
//
//        gams::GAMSDatabase dbTemp = ws.addDatabaseFromGDX(rec2.text());     // I access the Solution record here
//        //        cout << "This is the DB: " << rec2.text() << endl;
//
//        std::vector<std::string> kt;
//        std::vector<std::string> tempValues;
//        std::vector<bool> binaryLocations{};
//        for (gams::GAMSVariableRecord rc : dbTemp.getVariable("xb")){    // xb Here gets only the Binary Varaibles and writes the values to file.
//            tempValues.push_back(rc.key(0));
//            tempValues.push_back(std::to_string( rc.level() ));
//            binaryLocations.push_back(true);
//
//        }
//
////        solutionMap.push_back(tempValues);   generatedSolution
////        solStore.updateSolution(tempValues, binaryLocations);
////        std::cout << " This is the ORIGINAL size of the binary vector: " << binaryLocations.size() << std::endl;
//        generatedSolution.updateSolution(tempValues, binaryLocations);
////        std::cout << "TempValue size: "  <<  tempValues.size() <<  std::endl;
//        std::cout << ". " ;
//        tempValues.clear();
//    }
//
//    // Delete the worker files. They would have a .gdx and .lst extension
//    std::cout << " \n Deleting .gdx and .lst worker files from working directory ..... \n " ;
//    system("rm *.gdx *.lst");
//
////    std::cout << " \n Total number of solutions collected: " << solutionMap.size() << "\n" << std::endl; //solStore
////    std::cout << " \n Total number of solutions collected: " << solStore.getSolutionsVector().size() << "\n" << std::endl; //solStore
//    std::cout << " \n Total number of solutions collected: " << generatedSolution.getSolutionsVector().size() << "\n" << "\n"; //solStore

}



//Overloaded Method - no function to track the time set
//TODO add a way to track the time in the below version or remove it.
oneTreeGams::oneTreeGams(solutionStoreVector &generatedSolution, std::string workingDirectory, std::string systemDirectory, std::string gmsFileDirectory,  std::string gmsFile,  std::string optionsDirectory)
{

//    gams::GAMSWorkspaceInfo wsInfo;
//    wsInfo.setWorkingDirectory(workingDirectory);
//    wsInfo.setSystemDirectory(systemDirectory);
//    gams::GAMSWorkspace ws(wsInfo);
//
//    // Set Solver as CPLEX
//    gams::GAMSOptions opt =  ws.addOptions();
//    opt.setAllModelTypes("cplex");
//    opt.setOptFile(1);
//    opt.setOptDir(optionsDirectory);
//
//
//    // Add the file I want to solve
//    gams::GAMSJob t1 = ws.addJobFromFile(gmsFileDirectory + "/" + gmsFile + ".gms");
//
//
//    // Run the solver and add the run solutions to a .txt file.
//    std::ofstream resultFile("Result.txt");
//
//    // I need to time this run and add the start time and end time to the current state values.
//
//    t1.run(opt,resultFile);
//
//
//    // Capture the multiple solutions generated
//    gams::GAMSDatabase db = ws.addDatabaseFromGDX("generatedSoln.gdx");
//
//
//    //Solution map used to transfer the solutions to csv.
////    std::vector<std::vector<std::string>> solutionMap { };
//
//    //Create a solution store to hold the solutions and the binary locations
////    solutionStore solStore;
//    std::vector<bool> binaryLocations{};
//
//    // Loop through the GAMSDatabase and populate the solution into the solution Map
//    for (gams::GAMSSetRecord rec2 : db.getSet("Index")){
//
//        gams::GAMSDatabase dbTemp = ws.addDatabaseFromGDX(rec2.text());     // I access the Solution record here
//        //        cout << "This is the DB: " << rec2.text() << endl;
//
//        std::vector<std::string> kt;
//        std::vector<std::string> tempValues;
//        std::vector<bool> binaryLocations{};
//        for (gams::GAMSVariableRecord rc : dbTemp.getVariable("xb")){    // xb Here gets only the Binary Varaibles and writes the values to file.
//            tempValues.push_back(rc.key(0));
//            tempValues.push_back(std::to_string( rc.level() ));
//            binaryLocations.push_back(true);
//
//        }
//
////        solutionMap.push_back(tempValues);   generatedSolution
////        solStore.updateSolution(tempValues, binaryLocations);
//        std::cout << " This is the ORIGINAL size of the binary vector: " << binaryLocations.size() << "\n";
//        generatedSolution.updateSolution(tempValues, binaryLocations);
//        std::cout << "TempValue size: "  <<  tempValues.size() <<  "\n";
//        tempValues.clear();
//    }
//
//    // Delete the worker files. They would have a .gdx and .lst extension
//    std::cout << " \n Deleting .gdx and .lst worker files from working directory ..... \n " ;
//    system("rm *.gdx *.lst");
//
////    std::cout << " \n Total number of solutions collected: " << solutionMap.size() << "\n" << std::endl; //solStore
////    std::cout << " \n Total number of solutions collected: " << solStore.getSolutionsVector().size() << "\n" << std::endl; //solStore
//    std::cout << " \n Total number of solutions collected: " << generatedSolution.getSolutionsVector().size() << "\n" << "\n"; //solStore


}


