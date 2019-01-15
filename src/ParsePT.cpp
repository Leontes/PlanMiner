#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "ParsePT.hpp"

const std::regex planRE("New plan!!!");
const std::regex taskRE("##Tasks##");
const std::regex stateRE("##States##");

void parseGoals(){
  /*unsigned downDelim = str.find("(");
  unsigned upDelim = str.find(")");
  string task = str.substr (downDelim,upDelim);
  cout << */
}


Task * parseTask(std::string taskSTR){

  std::string strformateado;
  for(unsigned int i = 0; i < taskSTR.size(); i++){
    if(taskSTR[i] == '(' or taskSTR[i] == '['){
      strformateado += " ( ";
    }
    else{
      if(taskSTR[i] == ')' or taskSTR[i] == ']'){
        strformateado += " ) ";
      }
      else{
        strformateado += taskSTR[i];
      }
    }
  }


  if(strformateado.compare("") != 0){
    std::vector<std::string> tokens;
    boost::split( tokens, strformateado, boost::is_any_of(" "), boost::algorithm::token_compress_on);

    unsigned int contador = 0;
    while(tokens[contador] != ":"){
      contador++;
    }
    contador++;

    std::string name = tokens[contador+1];
    std::vector<pairParams> params;
    if(tokens.size() > 3){
      for (int i = contador+2; i < tokens.size()-2; i+=3) {
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

  std::string strformateado;
  for(unsigned int i = 0; i < stateSTR.size(); i++){
    if(stateSTR[i] == '('){
      strformateado += " ( ";
    }
    else{
      if(stateSTR[i] == ')'){
        strformateado += " ) ";
      }
      else{
        strformateado += stateSTR[i];
      }
    }
  }


  if(strformateado.compare("") != 0){
    std::vector<std::string> tokens;
    boost::split( tokens, strformateado, boost::is_any_of(" ") );
    return new State(tokens);
  }
  else{
    return nullptr;
  }
}




std::vector< PlanTrace * > * parse(const char * filename){
  std::ifstream ptFile;
  ptFile.open (filename);
  std::string rBuffer;
  std::vector< PlanTrace * > * pt = new std::vector< PlanTrace * >();
  std::map<std::string, State *> statesMap;
  std::vector<std::string> taskVec;

  Task * tareaParseada;

  unsigned int contador = 0;
  //Mientras el fichero tenga algo...
  while (!ptFile.eof()) {
    rBuffer = "";
    //Ignoramos las lineas vacías
    while (rBuffer.compare("") == 0 and !ptFile.eof()) {
      getline(ptFile, rBuffer);

      pt -> push_back(new PlanTrace());
      statesMap.clear();
      taskVec.clear();

      while(!regex_match(rBuffer,stateRE)){
          getline(ptFile, rBuffer);
          if(!regex_match(rBuffer,taskRE) and rBuffer != "" and !regex_match(rBuffer,stateRE)){
            taskVec.push_back(rBuffer);
          }
      }

      while(!regex_match(rBuffer, planRE) and !ptFile.eof()){
          getline(ptFile, rBuffer);
          if(!regex_match(rBuffer, planRE)){
            if(rBuffer != ""){
              for (contador = 0; contador < rBuffer.size(); contador++) {
                if(rBuffer[contador] == ':'){
                  break;
                }
              }

              statesMap[rBuffer.substr(0, contador)] = parseState(rBuffer.substr(contador+2));
            }
          }
      }

      std::string numPre = "", numPos = "";
      unsigned int indice = 0;
      for(unsigned int i = 0; i < taskVec.size(); i++){
        tareaParseada = parseTask(taskVec[i]);
        //Buscamos el primer numero
        indice = 1;
        numPre = "";
        while(taskVec[i][indice] != ','){
          numPre+= taskVec[i][indice];
          indice++;
        }

        //Buscamos el segundo numero
        indice += 2;
        numPos = "";
        while(taskVec[i][indice] != ']'){
          numPos+= taskVec[i][indice];
          indice++;
        }

        ((*pt)[pt -> size()-1]) -> addLink(new TSLinker(statesMap["[" + numPre + "]"], statesMap["[" + numPos + "]"], tareaParseada));
      }
    }

  }


  ptFile.close();
  return pt;
}
