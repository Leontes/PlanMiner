#ifndef CONECTOR_HPP
#define CONECTOR_HPP


#include "State/Predicate.hpp"


/** Class Predicate.
*  \brief Class to model logical conectors.
*/
class Conector : public Predicate {
private:

public:
  Conector():Predicate(){};
  virtual ~Conector ();
};

#endif
