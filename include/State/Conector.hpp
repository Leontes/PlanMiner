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

  std::ostream& print(std::ostream& os) const;

public:
  /** State Constructor
  *   @brief State class parameter constructor
  *   @param tokens  list of string preparsed tokens
  *   @param iType type of the conector
  */
  Conector(std::vector <std::string> tokens, conectorType iType);
  ~Conector (){};
};

#endif
