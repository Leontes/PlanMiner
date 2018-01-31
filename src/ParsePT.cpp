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
  if(stateSTR.compare("") != 0){
    std::vector<std::string> tokens;
    boost::split( tokens, stateSTR, boost::is_any_of(" ") );
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

  State * parsedPre = nullptr, * parsedPost = nullptr;
  PrimitiveTask * parsedTask = nullptr;
  bool esperandoEstado = false, esperandoTarea = true;
  char nStates = 0, a;

//Mientras el fichero tenga algo...
  while (!ptFile.eof()) {


    if(nStates == 2){
      nStates = 1;
      //std::cout << parsedPre << " " << parsedPost << " " << parsedTask << std::endl;
      ((*pt)[pt -> size() - 1]) -> addLink(new TSLinker(parsedPre, parsedPost, parsedTask));
      //std::cout << *((*pt)[pt -> size() - 1]) -> getTS(((*pt)[pt -> size() - 1]) -> lenght() -1 ) << std::endl;
    }

    rBuffer = "";
    //Ignoramos las lineas vacías
    while (rBuffer.compare("") == 0 and !ptFile.eof()) {
      getline(ptFile, rBuffer);
    }

    //std::cout << rBuffer << std::endl;
    //std::cin >> a;

    //Si la linea es el inicio de un nuevo plan creamos el objeto necesario
    if(regex_match(rBuffer,planRE)){
      pt -> push_back(new PlanTrace());
      parsedPre = nullptr;
      parsedPost = nullptr;
      parsedTask = nullptr;
      esperandoEstado = false;
      esperandoTarea = false;
      nStates = 0;
    }
    else{
      //Si no comprobamos si lo siguiente es un estado
      if(regex_match(rBuffer,stateRE)){
        esperandoEstado = true;
        //std::cout << "Esperando un estado..." << std::endl;
      }
      else{
        //Si no comprobamos si lo siguiente es una tarea
        if(regex_match(rBuffer,taskRE)){
          esperandoTarea = true;
          //std::cout << "Esperando una tarea..." << std::endl;
          if(esperandoEstado == true){
            //std::cout << "Estaba esperando un estado y lo que ocurrió a continuacion te sorprendera..." << std::endl;
            esperandoEstado = false; //<-- Si esperabamos un estado y nos hemos encontrado con el inicio de una tarea... el estado esta perdido
            parsedPre = parsedPost;
            parsedPost = nullptr;
            nStates++;
          }
        }
        else{
          //Si esperabamos una tarea la parseamos
          if(esperandoTarea == true){
            esperandoTarea = false;
            parsedTask = (PrimitiveTask*) parseTask(rBuffer);
          }

          if(esperandoEstado == true){
            esperandoEstado = false;
            parsedPre = parsedPost;
            parsedPost = parseState(rBuffer);
            nStates++;
          }
         }
       }
     }





  }



  /*while (!ptFile.eof()) {
    rBuffer = "";
    while (rBuffer.compare("") == 0 and !ptFile.eof()) {
      getline(ptFile, rBuffer);
    }
    if(regex_match(rBuffer,planRE)){
       pt -> push_back(new PlanTrace());
       parsedPre = nullptr;
       parsedPost = nullptr;
       parsedTask = nullptr;
     }

     if(regex_match(rBuffer,stateRE)){
       rBuffer = "";
       while (rBuffer.compare("") == 0 and !ptFile.eof()) {
         getline(ptFile, rBuffer);
       }
       parsedPre = parsedPost;
       parsedPost = parseState(rBuffer);

       if(tareaLeida == true){
         ((*pt)[pt -> size() - 1]) -> addLink(new TSLinker(parsedPre, parsedPost, parsedTask));
         tareaLeida = false;
       }

       /*if(parsedPre != nullptr){
         std::cout << *(parsedPre);
       }
       else{
         std::cout << "Missing state" << std::endl;
       }
     }

     if(regex_match(rBuffer,taskRE)){
       rBuffer = "";
       while (rBuffer.compare("") == 0 and !ptFile.eof()) {
         getline(ptFile, rBuffer);
       }
       parsedTask = (PrimitiveTask*) parseTask(rBuffer);
       tareaLeida = true;

       /*if(parsedTask != nullptr){
         std::cout << *(parsedTask);
       }
       else{
         std::cout << "Missing task" << std::endl;
       }
     }
  }*/


  ptFile.close();

  return pt;

}
