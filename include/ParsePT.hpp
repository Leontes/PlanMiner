#ifndef PARSEPT_HPP
#define PARSEPT_HPP



#include "PlanTrace.hpp"




void parseGoals();


/** Method ParseTask
*   @brief Method to parse an string and create a pointer to an Task object.
*   @param taskSTR  String to be parsed.
*   @retval Pointer to Task type object
*/
Task * parseTask(std::string taskSTR);

/** Method ParseState
*   @brief Method to parse an string and create a pointer to an State object.
*   @param taskSTR  String to be parsed.
*   @retval Pointer to State type object
*/
State * parseState(std::string stateSTR);




std::vector< PlanTrace * > * parse(const char * filename);


#endif
