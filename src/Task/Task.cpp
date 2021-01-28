#include "Task/Task.hpp"

std::string Task::to_string(){
  //Meto un comentario para Github
  std::string str = std::string("( ") + taskName + std::string(" ");
  for (unsigned i = 0; i < parameters.size(); i++) {
    str += parameters[i] -> to_string();
  }

  str += std::string(")\n");
  return str;
}
