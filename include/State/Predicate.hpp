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
  virtual std::ostream& print(std::ostream&) const {};
public:
  Predicate(){};
  Predicate(const Predicate & iPred){};
  virtual ~Predicate (){};

  virtual void clearParam(std::string newValue, std::string value, std::vector<std::string> types){}

  virtual void clean(){}

  virtual bool erasable(){}

  virtual std::string getName(){}

  virtual double getValue(){}

  virtual void setValue( double val ){}

  virtual double getCost(std::string pred){
    return -1;
  }

  virtual unsigned int length(){
    return 1;
  }

  virtual std::vector<std::string> getAllFunctions(){
    std::vector<std::string> out;
    return out;
  }

  virtual void setCost(std::string pred, double cost){}

  virtual void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity = true){};


  virtual Predicate *  clone(){
    return new Predicate(*this);
  }

  friend std::ostream& operator << (std::ostream& os, const Predicate& pred){
    return pred.print(os);
  };

protected:
  void extractSubVector(std::vector<std::string> &tokens, unsigned int *i, unsigned int *j);
};

#endif
