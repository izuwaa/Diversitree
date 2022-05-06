/*
 * diversitreenode.h
 *
 *  Created on: Mar 24, 2022
 *      Author: izuwaahanor
 */

#ifndef DIVERSITREENODE_H_
#define DIVERSITREENODE_H_

#include <stdio.h>
#include <array>
#include <string>
#include <vector>
#include <tuple>
#include <numeric>
#include <map>
#include <chrono>
#include "scip/def.h"
#include "scip/type_retcode.h"
#include "scip/type_scip.h"
#include <iostream>
#include "scip/scip.h"
#include "scip/scipdefplugins.h"
#include "scip/dialog_default.h"
#include <algorithm>
#include "branchCountScip.h"
#include "optionsReader.h"
//#include "utilities.h"
#include "currentStateVariables.h"
//#include <chrono>


//#ifdef __cplusplus
extern "C" {
//#endif

/** creates the node selector for best first search and includes it in SCIP
 *
 *  @ingroup NodeSelectorIncludes
 */

static currentStateVariables *cState;
static std::vector<float> nodeVisits;

//static
//void setCurrentState(currentStateVariables *currentState);
//
//static
//currentStateVariables *getCurrentState();

SCIP_EXPORT
SCIP_RETCODE SCIPincludeDiversitreeBfs(SCIP *scip /**< SCIP data structure */, currentStateVariables *currentState
);

//#ifdef __cplusplus
}
//#endif


#endif /* DIVERSITREENODE_H_ */
