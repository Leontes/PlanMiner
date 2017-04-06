#ifndef ATOM_HPP
#define ATOM_HPP


#include "State/Predicate.hpp"
#include "Task/Parameter.hpp"

/** Class Predicate.
*  \brief Class to model logical atoms.
*/
class Atom : public Predicate {
private:
  std::string name;
  std::vector<Parameter*> parameters;
public:
  /** Atom Constructor
  *   @brief Atom class parameter constructor
  *   @param tokens  list of string preparsed tokens
  */
  Atom(std::vector <std::string> tokens);
  ~Atom(){};

  std::ostream& print(std::ostream& os) const;

};

#endif
