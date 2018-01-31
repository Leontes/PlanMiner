#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include <string>
#include <vector>

#include <iostream>

/** Class Parameter.
*  @brief Class to model task's parameters.
*/
class Parameter {
private:
  std::string name;
  std::vector<std::string> types;
  std::string value;


public:
  /** Task Constructor
  *   @brief Parameter class parameter constructor
  *   @param name  parameter name
  *   @param type  Base type of the paramater
  *   @param value Paramater value
  */
  Parameter(std::string name, std::string value, std::string type):name(name), value(value){
    types.push_back(type);
  };
  Parameter(const Parameter & iParam):name(iParam.name),types(iParam.types),value(iParam.value){}
  ~Parameter(){};

  std::string getName(){
    return name;
  }

  void setName(std::string iName){
    name = iName;
  }

  std::vector<std::string>getTypes(){
    return types;
  }

  std::string getValue(){
    return value;
  }

  void setValue(std::string iValue){
    value = iValue;
  }

  /** Methos addType
  *   @brief Adds another type to the parameter
  *   @param type  Base type of the paramater
  */
  void addType(std::string type){
    types.push_back(type);
  }

  friend std::ostream& operator << (std::ostream& os, const Parameter& param){
    if(param.value != "_NonD_"){
      os << param.value << std::string(" ");
    }
    else{
      os << std::string("?") << param.name << std::string(" ");
    }
    if(param.types.size() > 1){
      os << std::string("- either( ");
      for (unsigned i = 0; i < param.types.size(); i++) {
        os << param.types[i] << std::string(" ");
      }
      os << std::string(")");
    }else{
      os << std::string("- ") << param.types[0] << std::string(" ");
    }
    return os;
  }

  std::string to_string();

  virtual Parameter *  clone(){
    return new Parameter(*this);
  }



};

#endif
