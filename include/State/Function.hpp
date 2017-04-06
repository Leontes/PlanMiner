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
  ~Function (){
    delete pred;
  }
  std::ostream& print(std::ostream& os) const;

};

#endif
