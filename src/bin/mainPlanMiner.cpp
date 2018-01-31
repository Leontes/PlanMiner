#include <iostream>
#include <stdlib.h>
#include <utility>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex>

#include "ParsePT.hpp"
#include "groupTS.hpp"
#include "ToTable.hpp"
#include "RuleCreator.hpp"
#include "TypeHierarchyCalc.hpp"
#include "Cleaner.hpp"
#include "fuseRules.hpp"

const std::regex diffRE("^(\\( DIFF)(.)*(\\))");

int main(int argc, char const *argv[]) {
  if (argc > 1) {

    std::cout << std::endl << std::endl << std::endl << "/*********************************************************/" << std::endl;
    std::cout << "/****************** Launching PlanMiner ******************/" << std::endl;
    std::cout << "/*********************************************************/" << std::endl;


    std::cout << "#############################" << std::endl;
    std::cout << "Parsing pts..." << std::endl << std::endl;

    std::vector< PlanTrace * > * PTS = parse(argv[1]);
    bool typing = false, fluents = false, equality = false;

    /*
    for (unsigned int i = 0; i < PTS -> size(); i++) {
      std::cout << *((*PTS)[i]) << std::endl;
    }
    */


    //Calculamos jerarquia de tipos
    std::cout << "#############################" << std::endl;
    std::cout << "Processing types..." << std::endl << std::endl;

    std::vector < Type *> tipos = extractTypeHierarchy(PTS);
    if(tipos.size() > 0){
      typing = true;
    }

    std::cout << std::endl << "#############################" << std::endl << std::endl << std::endl;

    /*
    Este hueco queda libre para modificar las PT's con la información estructural del dominio HTN
    Para el futuro....
    */



    std::cout << "#############################" << std::endl;
    std::cout << "Processing plans..." << std::endl << std::endl;
    std::cout << "Plans processed:" << std::endl;

    unsigned int planLenght = 0, stateLength = 0;
    //Calculamos la forma esquematizada de cada TS
    for(unsigned int i = 0; i < PTS -> size(); i++){
      planLenght += (*PTS)[i] -> lenght();
      stateLength += (*PTS)[i] -> stateLenght();
      (*PTS)[i] -> schematize(tipos);
      std::cout << std::right << std::setw(6) << std::setfill('-') << i+1 << "/" << std::left << std::setw(6) << std::setfill('-') << (PTS -> size()) << " Lenght: " << (*PTS)[i] -> lenght() << endl;
    }
    std::cout << "#############################" << std::endl;
    std::cout << "Average plan lenght: " << (float)planLenght / (float)PTS -> size() << std::endl;
    std::cout << "Average state lenght: " << (float)stateLength / (float)PTS -> size() << std::endl;
    std::cout << "#############################" << std::endl << std::endl << std::endl;

    /*for (unsigned int i = 0; i < PTS -> size(); i++) {
      std::cout << *((*PTS)[i]) << std::endl;
    }*/

    //Extraemos los estados de cada TS y los agrupamos
    std::map < std::string, StatesLists > groupedStates = groupTaskStates(PTS);

    std::map < std::string, std::vector < std::vector < double > > > datasetMap;
    std::map < std::string, std::vector < std::string > > attribLabelsMap;

    std::vector < std::vector < double > > dataset;
    std::vector < std::string > attribLabels;
    //to_table
    for (std::map < std::string, StatesLists >::iterator it=groupedStates.begin(); it!=groupedStates.end(); it++){
      for (unsigned int i = 0; i < dataset.size(); i++) {
        dataset[i].clear();
      }
      dataset.clear();
      attribLabels.clear();


      to_table(it -> first, it -> second, &dataset, &attribLabels);

      datasetMap[it -> first] = dataset;
      attribLabelsMap[it -> first] = attribLabels;

    }




    /*
    for(std::map < std::string, std::vector < std::vector < double > > >::iterator it = datasetMap.begin(); it != datasetMap.end(); it++){

      std::cout << "\n\nTask: " << it -> first << std::endl;
      std::cout << "Attributes list:  " << std::endl;


      for (unsigned int i = 0; i < attribLabelsMap[it -> first].size(); i++) {
        std::cout << "Attr" +  std::to_string(i) << ":\t" << attribLabelsMap[it -> first][i] << std::endl;
      }

      for (unsigned int j = 0; j < attribLabelsMap[it -> first].size(); j++) {
        if(j==0){
          std::cout << std::endl<< std::flush;
          std::cout << std::left  << std::setw(6) << std::setfill(' ') << "|";
        }
        std::cout << std::left << std::setw(6) << std::setfill(' ') << "Attr" +  std::to_string(j) << std::setw(6) << std::setfill(' ') << "|";
      }
      std::cout << std::endl;

      unsigned int index = 0;



      do {
        for (unsigned int j = 0; j < it -> second.size(); j++) {
          if(j==0){
            std::cout << std::setw(6) << std::setfill(' ') << "|";
          }
          std::cout << std::left << std::setw(6) << std::setfill(' ') << it -> second[j][index]  << std::setw(6) << std::setfill(' ') << "|";
        }
        std::cout << std::endl;
        index++;
      } while(index < it -> second[0].size());


    } */


    /*
      Dataset cleaning
    */
    vector<string> tokens; // Create vector to hold our words
    std::string buf;

    std::cout << "#############################" << std::endl;
    std::cout << "Cleaning Datasets..." << std::endl << std::endl;
    std::cout << "Datasets cleaned:" << std::endl;

    unsigned int cont = 0;
    for(std::map < std::string, std::vector < std::vector < double > > >::iterator it = datasetMap.begin(); it != datasetMap.end(); it++){
      cont++;
      std::cout << " " << std::right << std::setw(6) << std::setfill('-') << cont << "/" << std::left << std::setw(6) << std::setfill('-') << (datasetMap.size()) << std::endl;

      tokens.clear();
      std::stringstream ss(it->first); // Insert the string into a stream

      while (ss >> buf){
        tokens.push_back(buf);
      }

      std::cout << tokens[1] << std::endl;
      cleanDataset(&(it->second), 0.05);
    }


    std::cout << "#############################" << std::endl << std::endl << std::endl;

    /*
      CSV creation section.
    */

    std::cout << "#############################" << std::endl;
    std::cout << "Creating Datasets..." << std::endl << std::endl;

    std::string sPath = "outputPM/" + string(argv[2]) + "/";
    struct stat sb;
    if (stat(sPath.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)){
      std::cout << "Datasets folder found." << std::endl;
    }
    else
    {
      std::cout << "Output folder not found. Creating " +  sPath  + " folder..." << std::endl;
      mode_t nMode = 0733; // UNIX style permissions
      int nError = 0;
      #if defined(_WIN32)
        nError = _mkdir("outputPM/");
        nError = _mkdir(sPath.c_str()); // can be used on Windows
        nError = _mkdir((sPath + "res/").c_str()); // can be used on Windows
      #else
        nError = mkdir("outputPM/",nMode);
        nError = mkdir(sPath.c_str(),nMode); // can be used on non-Windows
        nError = mkdir((sPath + "res/").c_str(),nMode); // can be used on non-Windows
      #endif

    }


    for(std::map < std::string, std::vector < std::vector < double > > >::iterator it = datasetMap.begin(); it != datasetMap.end(); it++){
      tokens.clear();
      std::stringstream ss(it->first); // Insert the string into a stream

      while (ss >> buf){
        tokens.push_back(buf);
      }

      std::ofstream output;
      output.open(sPath + tokens[1] + ".csv", std::ofstream::out | std::ofstream::trunc);

      for(unsigned int index = 0; index < attribLabelsMap[it -> first].size(); index++){
        if(index == 0){
          output << attribLabelsMap[it -> first][index];
        }
        else{
          output << ";" << attribLabelsMap[it -> first][index];
        }
      }
      output << std::endl;

      unsigned int maxK = datasetMap[it -> first].size();
      unsigned int maxL = datasetMap[it -> first][0].size();

      for (unsigned int l = 0; l < maxL; l++) {
        for (unsigned int k = 0; k < maxK; k++) {
          if(k == 0){
            output << datasetMap[it -> first][k][l];
          }
          else{
            output << ";" << datasetMap[it -> first][k][l];
          }
        }
        output << std::endl;
      }


      output.close();
      std::cout << "Dataset " +  tokens[1] + ".csv" + " created." << std::endl;

    }

    std::cout << "#############################" << std::endl << std::endl << std::endl;


    std::cout << "Launching INSLV..." << std::endl << std::endl << std::endl;


    ofstream pddl;
    std::string salidaStr;
    pddl.open(string(argv[2]) + ".pddl", std::ofstream::out | std::ofstream::trunc);
    pddl << "";
    salidaStr = string(argv[2]);
    std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
    pddl << "(define (domain " << salidaStr << ")" << endl;
    pddl << "\t(:requirements :strips";
    if(typing){
      pddl << " :typing";
    }
    if(fluents){
      pddl << " :action-costs";
    }
    if(equality){
        pddl << " :equality";
    }

    pddl << ")" << endl;



    std::map <std::string, std::vector <std::string> >mapaTipos;

    for (unsigned int tip = 0; tip < tipos.size(); tip++) {
      if(tipos[tip] -> parent != nullptr){
        mapaTipos[(tipos[tip] -> parent) -> name].push_back(tipos[tip] -> name);
      }
      else{
        mapaTipos["object"].push_back(tipos[tip] -> name);
      }
    }

    pddl << "\t(:types" << endl;

    for (std::map <std::string, std::vector <std::string> >::iterator mapIt= mapaTipos.begin(); mapIt != mapaTipos.end(); mapIt++) {
      pddl << "\t\t";
      for(unsigned int tip = 0; tip < (*mapIt).second.size(); tip++){
        salidaStr = (*mapIt).second[tip];
        std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
        pddl << salidaStr << " ";
      }
      salidaStr = (*mapIt).first;
      std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
      pddl << "- " << salidaStr << endl;
    }
    pddl << "\t)" << endl;

    pddl << "\t(:predicates" << endl;

    std::map <std::string, char > predsInc;

    for (std::map <std::string, std::vector <std::string> >::iterator mapIt= attribLabelsMap.begin(); mapIt != attribLabelsMap.end(); mapIt++){
      for (unsigned int iAttr = 0; iAttr < (mapIt -> second).size(); iAttr++){
        if((mapIt -> second)[iAttr] != "Class" and not regex_match((mapIt -> second)[iAttr],diffRE)){
          tokens.clear();
          std::stringstream ss((mapIt -> second)[iAttr]); // Insert the string into a stream
          while (ss >> buf){
            tokens.push_back(buf);
          }

          std::string tokensTip;
          tokensTip = tokens[1];
          for (unsigned int l = 4; l < tokens.size()-1; l+=2) {
            tokensTip += " " + tokens[l] ;
          }

          if(predsInc.find(tokensTip) == predsInc.end()){
            predsInc[tokensTip] = 0;

            salidaStr = "(" + tokens[1];
            int counter = 0;

            for(unsigned int tokIndex= 2; tokIndex < tokens.size()-1; tokIndex+=3){
              salidaStr += " ?p" + to_string(counter) + " ";
              salidaStr += tokens[tokIndex+1] + " " + tokens[tokIndex+2];
              counter++;
            }
            salidaStr += ")";

            std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
            pddl << "\t\t" << salidaStr << endl;
          }
        }
      }
    }

    pddl << "\t)" << endl;

    pddl.close();

    vector <list< pair<string, string> > > * reglas;
    // [ {n_ejemplos, [<attr, val>, <attr, val>, ... ,<attr, val>]}, ..., {n_ejemplos, [<attr, val>, <attr, val>, ... ,<attr, val>]} ]
    vector < pair <int, list< pair<string, string> > > > * reglasInp;


    for(std::map < std::string, std::vector < std::vector < double > > >::iterator it = datasetMap.begin(); it != datasetMap.end(); it++){
      tokens.clear();
      std::stringstream ss(it->first); // Insert the string into a stream

      while (ss >> buf){
        tokens.push_back(buf);
      }


      reglasInp = new vector < pair <int, list< pair<string, string> > > > ();

      reglas = new vector <list< pair<string, string> > >;
      //Fusion de reglas o la mejor de todas

      reglasInp = new vector < pair <int, list< pair<string, string> > > > ();
      inslv(string(argv[2]), tokens[1], sPath, &datasetMap[it -> first], &attribLabelsMap[it -> first], reglasInp);
      fuseRules(reglasInp, reglas, &datasetMap[it -> first], &attribLabelsMap[it -> first]);


      /*std::cout << "N rules " << reglas -> size() << std::endl;
      std::cout << "Size Pre " << (*reglas)[0].size() << std::endl;
      for (std::list< std::pair<std::string, std::string> >::iterator it=(*reglas)[0].begin(); it != (*reglas)[0].end(); ++it){
        std::cout << "(" <<(*it).first << " = " << (*it).second << ")" << std::endl;
      }


      std::cout << "Size Post " << (*reglas)[1].size() << std::endl;
      for (std::list< std::pair<std::string, std::string> >::iterator it=(*reglas)[1].begin(); it != (*reglas)[1].end(); ++it){
        std::cout << "(" <<(*it).first << " = " << (*it).second << ")" << std::endl;
      }*/


      delete reglasInp;


      pddl.open(string(argv[2]) + ".pddl", std::ofstream::out | std::ofstream::app);


      //Eliminamos los DIFF para que no aparezcan en las precondiciones...
      bool limpio = false;
      while(limpio == false){
        limpio = true;
        for (std::list< pair<string, string> >::iterator it=(*reglas)[0].begin(); it != (*reglas)[0].end(); ++it) {
          if(regex_match((*it).first,diffRE)){
            (*reglas)[0].erase(it);
            limpio = false;
            break;
          }
        }
      }

      salidaStr = tokens[1];
      std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);


      pddl << "\t(:action "<< salidaStr << std::endl;

      pddl << "\t\t:parameters (";

      for (unsigned int tokIndex = 2; tokIndex < tokens.size(); tokIndex++) {
        salidaStr = tokens[tokIndex];
        std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
        if(salidaStr != ")"){
          pddl << salidaStr;
          if(tokens[tokIndex+1] != ")"){
            pddl << " ";
          }
        }
      }

      pddl << ")" <<  endl;


      pddl << "\t\t:precondition" << std::endl;
      if((*reglas)[0].size() > 0){
        pddl << "\t\t\t(and" << endl;
      }else{
        pddl << "\t\t\t( )" << endl;
      }

      for (std::list< pair<string, string> >::iterator it=(*reglas)[0].begin(); it != (*reglas)[0].end(); ++it){
        if((*reglas)[0].size() > 1){
          pddl << "\t";
        }

        tokens.clear();
        std::stringstream ss((*it).first);
        while (ss >> buf){
          tokens.push_back(buf);
        }

        if((*it).second == "True"){
          pddl << "\t\t\t";

          for(unsigned int l = 0; l < tokens.size(); l++){
            if(tokens[l] == "-"){
              l++;
            }else{
              salidaStr = tokens[l];
              std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
              pddl << salidaStr;
              if(tokens[l] != "(" and tokens[l] != ")"){
                pddl << " ";
              }
            }
          }
          pddl << std::endl;
        }
        else{
          if((*it).second == "False"){
            /*pddl << "\t\t\t(not ";

            for(unsigned int l = 0; l < tokens.size(); l++){
              if(tokens[l] == "-"){
                l++;
              }
              else{
                salidaStr = tokens[l];
                std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
                pddl << salidaStr;
                if(tokens[l] != "(" and tokens[l] != ")"){
                  pddl << " ";
                }
              }
            }
            pddl<< ")" << std::endl;
          */}
          else{
            pddl << "\t\t\t(= ";

            for(unsigned int l = 0; l < tokens.size(); l++){
              if(tokens[l] == "-"){
                l++;
              }
              else{
                salidaStr = tokens[l];
                std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
                pddl << salidaStr;
                if(tokens[l] != "(" and tokens[l] != ")"){
                  pddl << " ";
                }
              }
            }
            pddl << " "  << (*it).second << " )" << std::endl;
          }
        }
      }

      if((*reglas)[0].size() > 0){
        pddl << ")" << endl;
      }


      pddl << "\t\t:effect" << std::endl;
      if((*reglas)[1].size() > 0){
        pddl << "\t\t\t(and" << endl;
      }
      else{
        pddl << "\t\t\t( )" << endl;
      }

      for (std::list< pair<string, string> >::iterator it=(*reglas)[1].begin(); it != (*reglas)[1].end(); ++it){
        bool noRepe = false;
        for (std::list< pair<string, string> >::iterator it2=(*reglas)[0].begin(); it2 != (*reglas)[0].end(); ++it2){
          if(((*it).first == (*it2).first) and ((*it).second == (*it2).second)){
            noRepe = true;
          }
        }
        if(noRepe == false){
          if((*reglas)[1].size() > 1){
            pddl << "\t";
          }

          tokens.clear();
          std::stringstream ss((*it).first);
          while (ss >> buf){
            tokens.push_back(buf);
          }

          if((*it).second == "True"){
            pddl << "\t\t\t";

            for(unsigned int l = 0; l < tokens.size(); l++){
              if(tokens[l] == "-"){
                l++;
              }else{
                salidaStr = tokens[l];
                std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
                pddl << salidaStr;
                if(tokens[l] != "( " and tokens[l] != ")"){
                  pddl << " ";
                }
              }
            }
            pddl << std::endl;
          }
          else{
            if((*it).second == "False"){
              pddl << "\t\t\t( not ";

              for(unsigned int l = 0; l < tokens.size(); l++){
                if(tokens[l] == "-"){
                  l++;
                }
                else{
                  salidaStr = tokens[l];
                  std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
                  pddl << salidaStr;
                  if(tokens[l] != "( " and tokens[l] != ")"){
                    pddl << " ";
                  }
                }
              }
              pddl<< ")" << std::endl;
            }
            else{
              if(regex_match((*it).first,diffRE)){
                pddl << "\t\t\t( ";
                if( /*std::stod((*it).second) > 0*/ 1){
                  pddl << "increase ( ";
                }else{
                  pddl << "decrease ( ";
                }
                salidaStr = tokens[1].substr(5);
                std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
                pddl << salidaStr;

                for(unsigned int l = 2; l < tokens.size()-1; l++){
                  if(tokens[l] == "-"){
                    l++;
                  }
                  salidaStr = tokens[l];
                  std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
                  pddl << salidaStr;
                }
                pddl << " ) " + (*it).second + " )\n";

              }else{
                pddl << "\t\t\t( = ";

                for(unsigned int l = 0; l < tokens.size(); l++){
                  if(tokens[l] == "-"){
                    l++;
                  }
                  else{
                    salidaStr = tokens[l];
                    std::transform(salidaStr.begin(), salidaStr.end(), salidaStr.begin(), ::tolower);
                    pddl << salidaStr;
                    if(tokens[l] != "( " and tokens[l] != ")"){
                      pddl << " ";
                    }
                  }
                }
                pddl << " " << (*it).second << " )" << std::endl;
              }
            }
          }
        }
      }

      if((*reglas)[1].size() > 0){
        pddl << "\t\t\t)" << endl;
      }


      pddl << "\t)" << std::endl;

      pddl.close();

      delete reglas;
    }

    pddl.open(string(argv[2]) + ".pddl", std::ofstream::out | std::ofstream::app);
    pddl << ")";
    pddl.close();


    exit(EXIT_SUCCESS);
  }
  else{
    exit(EXIT_FAILURE);
  }



}
