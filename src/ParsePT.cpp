#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "ParsePT.hpp"

const std::regex planRE("New Plan!!!");
const std::regex taskRE("^T(.)*:");
const std::regex stateRE("^S(.)*:");

void parseGoals(){
  /*unsigned downDelim = str.find("(");
  unsigned upDelim = str.find(")");
  string task = str.substr (downDelim,upDelim);
  cout << */
}


Task * parseTask(std::string taskSTR){
    std::vector<std::string> tokens;
    boost::split( tokens, taskSTR, boost::is_any_of(" ") );
    if(tokens.size() > 1){
      std::string name = tokens[1];

      std::vector<pairParams> params;
      if(tokens.size() > 3){
        for (int i = 2; i < tokens.size()-1; i+=3) {
          params.push_back(pairParams(tokens[i],tokens[i+2]));
        }
      }
      return new PrimitiveTask(name, &params);
    }
    else{
      return nullptr;
    }

}


State * parseState(std::string stateSTR){
  std::vector<std::string> tokens;
  boost::split( tokens, stateSTR, boost::is_any_of(" ") );
  if(tokens.size() > 1){
    return new State(tokens);
  }
  else{
    return nullptr;
  }


}




std::vector<std::vector< PlanTrace * > > * parse(const char * filename){
  std::ifstream ptFile;
  ptFile.open (filename);
  std::string rBuffer;
  std::vector< PlanTrace * > * pt = new std::vector< PlanTrace * >();

  State * pre = nullptr, * post = nullptr;
  PrimitiveTask * parsedTask;

  while (!ptFile.eof()) {
     getline(ptFile, rBuffer);
     if(regex_match(rBuffer,planRE)){
       pt -> push_back(new PlanTrace());
     }

     if(regex_match(rBuffer,stateRE)){
       getline(ptFile, rBuffer);
       if(pre == nullptr){
         pre = parseState(rBuffer);
       }else{
         post = pre;
         pre = parseState(rBuffer);
         //añadimos un TSLinker a la PT
       }

       if(pre != nullptr){
         std::cout << *(pre);
       }
       else{
         std::cout << "Missing state" << std::endl;
       }
     }


     if(regex_match(rBuffer,taskRE)){
       getline(ptFile, rBuffer);
       parsedTask = (PrimitiveTask*) parseTask(rBuffer);
       if(parsedTask != nullptr){
         std::cout << *(parsedTask);
       }
       else{
         std::cout << "Missing task" << std::endl;
       }
     }
  }
  ptFile.close();

}
