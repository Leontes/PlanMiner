#include "State/Predicate.hpp"


void Predicate::extractSubVector(std::vector<std::string> &tokens, unsigned int *i, unsigned int *j){
  int cont = 0;
  do{
    (*j)++;
    if(tokens[(*j)] == "("){
      cont ++;
    }
    if(tokens[(*j)] == ")"){
      cont --;
    }
  }while(cont >= 0);
  (*i)--;
  (*j)++;
}
