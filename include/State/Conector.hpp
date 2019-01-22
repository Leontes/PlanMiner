#ifndef CONECTOR_HPP
#define CONECTOR_HPP

#include <vector>
#include <string>

#include "State/Predicate.hpp"
#include "State/Atom.hpp"
#include "State/Function.hpp"


enum conectorType{NOT, OR, AND, XOR, XAND};

/** Class Predicate.
*  @brief Class to model logical conectors.
*/
class Conector : public Predicate {
private:
  conectorType type;
  std::vector<Predicate *> predicates;

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
    switch (type) {
      case NOT:
        return "not";
      case OR:
        return "or";
      case AND:
        return "and";
      case XOR:
        return "xor";
      case XAND:
        return "xand";
    }
    return "NaN";
  }

  /** Function getValue
  *   @brief Method to get the value of the atomic predicate
  *   @retval double atom value
  */
  double getValue(){
    return -1.0;
  }

  /** Function length
  *   @brief Method to get the lenght of the atomic predicate (Is always 1)
  *   @retval double atom lenght
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
  void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity);


  /** Function getAllFunctions
  *   @brief Method to get the name and parameters of the child predicates
  *   @retval STD vector with the predicates information
  */
  std::vector<std::string> getAllFunctions();

  /** Function print
  *   @brief Method to output the Conector object
  *   @retval ostream Conector object info
  */
  std::ostream& print(std::ostream& os) const;

public:
  /** State Constructor
  *   @brief State class parameter constructor
  *   @param tokens  list of string preparsed tokens
  *   @param iType type of the conector
  */
  Conector(std::vector <std::string> tokens, conectorType iType);
  Conector(const Conector & iCon):type(iCon.type){
    for(unsigned int i = 0; i < iCon.predicates.size(); i++){
      predicates.push_back(iCon.predicates[i] -> clone());
    }
  }
  ~Conector (){};

  /** Function clone
  *   @brief Method to create a copy of the Conector object
  *   @retval Conector object copy
  */
  virtual Conector *  clone(){
    return new Conector(*this);
  }


};

#endif
