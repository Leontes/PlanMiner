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
  Atom(const Atom & iAt):name(iAt.name){
    for(unsigned int i = 0; i < iAt.parameters.size(); i++){
      parameters.push_back(iAt.parameters[i] -> clone());
    }
  }

  ~Atom(){};

  void clearParam(std::string newValue, std::string value, std::vector<std::string> types);
  void clean();
  bool erasable();

  std::string getName(){
    return name;
  }

  double getValue(){
    return 1.0;
  }

  unsigned int length(){
    return 1;
  }


  void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity);

  virtual Atom *  clone(){
    return new Atom(*this);
  }

  std::ostream& print(std::ostream& os) const;







};

#endif
