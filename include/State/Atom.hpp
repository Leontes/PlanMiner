#ifndef CONECTOR_HPP
#define CONECTOR_HPP


#include "State/Predicate.hpp"


/** Class Predicate.
*  \brief Class to model logical atoms.
*/
class Atom : public Predicate {
private:

public:
  Atom():Predicate(){};
  virtual ~Atom ();
};

#endif
