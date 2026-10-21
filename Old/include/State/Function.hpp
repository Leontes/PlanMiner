#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>
#include <string>


#include "State/Atom.hpp"

/** Class Predicate.
*  @brief Class to model numerical functions.
*/
class Function : public Predicate {
private:
  Atom * pred;
  double value;
public:
  /** Function Constructor
  *   @brief Function class parameter constructor
  *   @param tokens  list of string preparsed tokens
  */
  Function(std::vector <std::string> tokens);
  Function(const Function & iFunc):value(iFunc.value){
    pred = iFunc.pred -> clone();
  }

  ~Function (){
    delete pred;
  }

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

  /** Function erasable
  *   @brief Method to check if the parameters' list has been fully cleared using the clearParam method
  *   retval bool cheking value
  */
  bool erasable();

  /** Function getName
  *   @brief Method to get the name of the atomic predicate
  *   @retval string atom name
  */
  std::string getName(){
    return pred -> getName();
  }

  /** Function getValue
  *   @brief Method to get the value of the atomic predicate
  *   @retval double atom value
  */
  double getValue(){
    return value;
  }

  /** Function setValue
  *   @brief Method to set the value of the atomic predicate
  *   @param val new value
  */
  void setValue(double val){
    value = val;
  }

  /** Function length
  *   @brief Method to get the lenght of the atomic predicate (Is always 1)
  *   @retval double atom lenght
  */
  unsigned int length(){
    return 1;
  }

  /** Function to_table
  *   @brief Method to display a collection of pairs pre-state/post-state as a attribute-value matrix.
  *   @param dataset Output pair of STD matrices of double values
  *   @param attribLabels Output pair of STD vector with the labels of the datasets' columns
  *   @param nStates unsigned int number of states of the dataset
  *   @param index unsigned int attribute index of the dataset's column
  *   @param polarity bool with the polarity of the atom
  */
  void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity);

  /** Function clone
  *   @brief Method to create a copy of the Function object
  *   @retval Function object copy
  */
  virtual Function *  clone(){
    return new Function(*this);
  }

  /** Function getAllFunctions
  *   @brief Method to get the name and parameters of the child predicates
  *   @retval STD vector with the predicates information
  */
  std::vector<std::string> getAllFunctions();


  std::vector < std::vector <std::string> > getTokens(){
    return pred -> getTokens();
  }

  /** Function print
  *   @brief Method to output the Function object
  *   @retval ostream Function object info
  */
  std::ostream& print(std::ostream& os) const;

};

#endif
