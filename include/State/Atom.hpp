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

  /** clearParam
  *   @brief Method to substitute a value of the parameters with a new one
  *   @param newValue string substituting string
  *   @param value string to be substituted
  *   @param types STD vector with the types of the objects of the parameters
  */
  void clearParam(std::string newValue, std::string value, std::vector<std::string> types);

  /** Function clean
  *   @brief Method to delete the parameters' list
  */
  void clean();

  /** Function erasable
  *   @brief Method to check if the parameters' list has been fully cleared using the clearParam method
  *   retval bool cheking value
  */
  bool erasable();

  /** Function getName
  *   @brief Method to get the name of the atomic predicate
  *   @retval string atom name
  */
  std::string getName(){
    return name;
  }

  /** Function getValue
  *   @brief Method to get the value of the atomic predicate (Is always 1)
  *   @retval double atom value
  */
  double getValue(){
    return 1.0;
  }

  /** Function length
  *   @brief Method to get the lenght of the atomic predicate (Is always 1)
  *   @retval double atom lenght
  */
  unsigned int length(){
    return 1;
  }

  /** Function to_table
  *   @brief Method to display a collection of pairs pre-state/post-state as a attribute-value matrix.
  *   @param dataset Output pair of STD matrices of double values
  *   @param attribLabels Output pair of STD vector with the labels of the datasets' columns
  *   @param nStates unsigned int number of states of the dataset
  *   @param index unsigned int attribute index of the dataset's column
  *   @param polarity bool with the polarity of the atom
  */
  void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity);

  /** Function clone
  *   @brief Method to create a copy of the Atom object
  *   @retval Atom object copy
  */
  virtual Atom * clone(){
    return new Atom(*this);
  }

  /** Function print
  *   @brief Method to output the Atom object
  *   @retval ostream Atom object info
  */
  std::ostream& print(std::ostream& os) const;







};

#endif
