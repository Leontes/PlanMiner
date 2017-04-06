#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <vector>
#include <string>

#include "State/Predicate.hpp"


/** Class Predicate.
*  \brief Class to model logic predicates.
*/
class Predicate {
private:
  virtual std::ostream& print(std::ostream&) const = 0;
public:
  Predicate(){};
  virtual ~Predicate (){};

  friend std::ostream& operator << (std::ostream& os, const Predicate& pred){
    return pred.print(os);
  };

protected:
  void extractSubVector(std::vector<std::string> &tokens, unsigned int *i, unsigned int *j);
};

#endif
