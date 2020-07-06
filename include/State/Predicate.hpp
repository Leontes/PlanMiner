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

  /** Function clearParam
  *   @brief Method to substitute a value of the parameters with a new one
  *   @param newValue string substituting string
  *   @param value string to be substituted
  *   @param types STD vector with the types of the objects of the parameters
  */
  virtual void clearParam(std::string newValue, std::string value, std::vector<std::string> types){}

  /** Function clean
  *   @brief Method to delete the parameters' list
  */
  virtual void clean(){}

  /** Function erasable
  *   @brief Method to check if the parameters' list has been fully cleared using the clearParam method
  *   retval bool cheking value
  */
  virtual bool erasable(){}

  /** Function getName
  *   @brief Method to get the name of the predicate
  *   @retval string atom name
  */
  virtual std::string getName(){}

  /** Function getValue
  *   @brief Method to get the value of the predicate
  *   @retval double atom value
  */
  virtual double getValue(){}

  /** Function setValue
  *   @brief Method to set the value of the predicate
  *   @param val new value
  */
  virtual void setValue( double val ){}

  /** Function length
  *   @brief Method to get the lenght of the predicate
  *   @retval double atom lenght
  */
  virtual unsigned int length(){
    return 1;
  }

  /** Function getAllFunctions
  *   @brief Method to get the name and parameters of the child predicates
  *   @retval STD vector with the predicates information
  */
  virtual std::vector<std::string> getAllFunctions(){
    std::vector<std::string> out;
    return out;
  }

  /** Function to_table
  *   @brief Method to display a collection of pairs pre-state/post-state as a attribute-value matrix.
  *   @param dataset Output pair of STD matrices of double values
  *   @param attribLabels Output pair of STD vector with the labels of the datasets' columns
  *   @param nStates unsigned int number of states of the dataset
  *   @param index unsigned int attribute index of the dataset's column
  *   @param polarity bool with the polarity of the atom
  */
  virtual void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity = true){};


  /** Function getTokens
  *   @brief Method to get the name and parameters of the child predicates
  *   @retval STD vector with the predicates information
  */
  virtual std::vector < std::vector <std::string> > getTokens(){
    std::vector < std::vector <std::string> > out;
    return out;
  }

  /** Function clone
  *   @brief Method to create a copy of the Predicate object
  *   @retval Function object copy
  */
  virtual Predicate * clone(){
    return new Predicate(*this);
  }

  /** Function operator <<
  *   @brief Method to output the Predicate object
  *   @retval ostream Predicate object info
  */
  friend std::ostream& operator << (std::ostream& os, const Predicate& pred){
    return pred.print(os);
  };

protected:
  void extractSubVector(std::vector<std::string> &tokens, unsigned int *i, unsigned int *j);
};

#endif
