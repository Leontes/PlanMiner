#include "groupTS.hpp"




//pair.first = presstates
//pair.second = poststates



std::map < std::string, StatesLists > groupTaskStates(std::vector< PlanTrace * > * PTS){

  std::map < std::string, StatesLists > output;
  std::map < std::string, StatesLists >::iterator it;
  std::string taskName;


  for(unsigned int i = 0; i < PTS -> size(); i++){
    for (unsigned int j = 0; j < (*PTS)[i] -> getNTS(); j++) {
      //taskName = (((*PTS)[i] -> getTS(j)) -> getTask()) -> to_string();
      taskName = (((*PTS)[i] -> getTS(j)) -> getTask()) -> get_Tname();
      it = output.find(taskName);
      if (it != output.end()){
        //Esta
        if(((*PTS)[i] -> getTS(j)) -> getPreS() != nullptr){
          output[taskName].first.push_back(((*PTS)[i] -> getTS(j)) -> getPreS());
        }

        if(((*PTS)[i] -> getTS(j)) -> getPostS() != nullptr){
          output[taskName].second.push_back(((*PTS)[i] -> getTS(j)) -> getPostS());
        }

      }else{
        //No esta
        output[taskName] = std::pair<std::list<State*>, std::list<State*> >();
        if(((*PTS)[i] -> getTS(j)) -> getPreS() != nullptr){
          output[taskName].first.push_back(((*PTS)[i] -> getTS(j)) -> getPreS());
        }

        if(((*PTS)[i] -> getTS(j)) -> getPostS() != nullptr){
          output[taskName].second.push_back(((*PTS)[i] -> getTS(j)) -> getPostS());
        }
      }
    }
  }

  /*for (it=output.begin(); it!=output.end(); ++it){
    std::cout << "Key: " << it->first;

    std::cout << "\tNumber of Prestates: " << (it -> second).first.size() << std::endl;
    cont = 0;
    for (std::list<State *>::iterator stIt = (it -> second).first.begin(); stIt != (it -> second).first.end(); stIt ++){
        std::cout << "Prestate: " << cont << std::endl;
        std::cout << *(*stIt) << std::endl;
        cont ++;
    }


    std::cout << "\tNumber of Poststates: " << (it -> second).second.size() << std::endl;
    cont = 0;
    for (std::list<State *>::iterator stIt = (it -> second).second.begin(); stIt != (it -> second).second.end(); stIt ++){
        std::cout << "Poststate: " << cont << std::endl;
        std::cout << *(*stIt) << std::endl;
        cont++;
    } s
  } */

  return output;

}
