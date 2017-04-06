#include <iostream>

#include "State/State.hpp"

State::State(std::vector<std::string> tokens){
  predicates.push_back(new Conector(tokens, AND));
}

/**
(
  ( at a1 - aircraft c1 - city )
  ( not
    ( at p1 - person c1 - city )
  )
  ( at p2 - person c1 - city )
  ( in p1 - person a1 - aircraft )
  ( fueled a1 - aircraft )
)
**/
