#include "Task/Parameter.hpp"


std::string Parameter::to_string(){
  std::string str;
  if(value != "_NonD_"){
    str =  value + std::string(" ");
  }
  else{
    str = std::string("?") + name + std::string(" ");
  }
  if(types.size() > 1){
    str += std::string("- either( ");
    for (unsigned i = 0; i < types.size(); i++) {
      str += types[i] + std::string(" ");
    }
    str += std::string(")");
  }else{
    str += std::string("- ") + types[0] + std::string(" ");
  }
  return str;
}
