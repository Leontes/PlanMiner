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

  void clearParam(std::string newValue, std::string value, std::vector<std::string> types);
  void clean();
  bool erasable();

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

  double getValue(){
    return -1.0;
  }

  double getCost(std::string pred);

  void setCost(std::string pred, double cost);

  unsigned int length(){
    unsigned int sLength = 0;
    for (unsigned i = 0; i < predicates.size(); i++) {
      sLength += predicates[i] -> length();
    }
    return sLength;
  }

  void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity);

  std::vector<std::string> getAllFunctions();

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

  virtual Conector *  clone(){
    return new Conector(*this);
  }


};

#endif
