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

  void clearParam(std::string newValue, std::string value, std::vector<std::string> types);
  void clean();
  bool erasable();

  std::string getName(){
    return pred -> getName();
  }

  double getValue(){
    return value;
  }

  void setValue(double val){
    value = val;
  }

  unsigned int length(){
    return 1;
  }

  void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity);

  virtual Function *  clone(){
    return new Function(*this);
  }

  std::vector<std::string> getAllFunctions();

  std::ostream& print(std::ostream& os) const;

};

#endif
