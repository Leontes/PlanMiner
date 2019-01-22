#ifndef STATE_HPP
#define STATE_HPP

#include <vector>
#include <string>

#include "State/Conector.hpp"




/** Class State.
*  @brief Class to model logic states.
*/
class State {
private:
  std::vector<Predicate *> predicates;

public:
  /** State Constructor
  *   @brief State class parameter constructor
  *   @param tokens  list of string preparsed tokens
  */
  State(std::vector<std::string> tokens);
  State(const State & iState){
    for (unsigned int i = 0; i < iState.predicates.size(); i++) {
      predicates.push_back(iState.predicates[i] -> clone());
    }

  }
  ~State (){};

  /** Function clearParam
  *   @brief Method to substitute a value of the parameters with a new one
  *   @param newValue string substituting string
  *   @param value string to be substituted
  *   @param types STD vector with the types of the objects of the parameters
  */
  void clearParam(std::string newValue, std::string value, std::vector<std::string> types);

  /** Function clean
  *   @brief Method to delete the parameters' list
  */
  void clean();

  /** Function operator <<
  *   @brief Method to output the State object
  *   @retval ostream State object info
  */
  friend std::ostream& operator << (std::ostream& os, const State& state){
    for (unsigned i = 0; i < state.predicates.size(); i++) {
      os << *(state.predicates[i]);
    }
    os << std::endl;
    return  os;
  }

  /** Function length
  *   @brief Method to get the lenght of the state
  *   @retval double State lenght
  */
  unsigned int length(){
    unsigned int sLength = 0;
    for (unsigned i = 0; i < predicates.size(); i++) {
      sLength += predicates[i] -> length();
    }
    return sLength;
  }

  /** Function to_table
  *   @brief Method to display a collection of pairs pre-state/post-state as a attribute-value matrix.
  *   @param dataset Output pair of STD matrices of double values
  *   @param attribLabels Output pair of STD vector with the labels of the datasets' columns
  *   @param nStates unsigned int number of states of the dataset
  *   @param index unsigned int attribute index of the dataset's column
  *   @param polarity bool with the polarity of the atom
  */
  void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index);

  /** Function getAllFunctions
  *   @brief Method to get the name and parameters of the child predicates
  *   @retval STD vector with the predicates information
  */
  std::vector<std::string> getAllFunctions();

};

#endif
