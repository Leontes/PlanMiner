#ifndef FUNCTION_HPP
#define FUNCTION_HPP


#include "State/Predicate.hpp"


/** Class Predicate.
*  \brief Class to model numerical functions.
*/
class Function : public Predicate {
private:

public:
  Function():Predicate() {};
  virtual ~Function ();
};

#endif
