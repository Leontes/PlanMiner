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
#include "Discretize.hpp"


const std::regex diffRE("^(DELTA - )\\((.)*\\)\\)");
const std::regex opRE("(^(opEQ - )\\((.)*\\))|(^(opGT-)\\((.)*\\))|(^(opLT-)\\((.)*\\))");


//const std::regex diffRE("^(\\( DELTA)(.)*(\\))");

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
      (*PTS)[i] -> schematize(/*tipos*/);
      std::cout << std::right << std::setw(6) << std::setfill('-') << i+1 << "/" << std::left << std::setw(6) << std::setfill('-') << (PTS -> size()) << " Lenght: " << (*PTS)[i] -> lenght() << endl;
    }
    std::cout << "#############################" << std::endl;
    std::cout << "Average plan lenght: " << (float)planLenght / (float)PTS -> size() << std::endl;
    std::cout << "Average state lenght: " << (float)stateLength / (float)PTS -> size() << std::endl;
    std::cout << "#############################" << std::endl << std::endl << std::endl;

    /*
    for (unsigned int i = 0; i < PTS -> size(); i++) {
      std::cout << *((*PTS)[i]) << std::endl;
    }
    */


    //Extraemos los estados de cada TS y los agrupamos
    std::map < std::string, StatesLists > groupedStates = groupTaskStates(PTS);

    std::map < std::string, std::vector < std::vector < std::vector < double > > > > datasetMap;
    std::map < std::string,  std::vector < std::vector < std::pair<std::string, std::string> > > > attribLabelsMap;

    std::vector < std::vector < std::vector < double > > > datasets;
    std::vector < std::vector < std::pair<std::string, std::string> > > attribLabelsVC;
    //to_table
    for (std::map < std::string, StatesLists >::iterator it=groupedStates.begin(); it!=groupedStates.end(); it++){
      for (unsigned int i = 0; i < datasets.size(); i++) {
        datasets[i].clear();
      }
      datasets.clear();
      attribLabelsVC.clear();

      to_table(it -> first, it -> second, &datasets, &attribLabelsVC);
      datasetMap[it -> first] = datasets;
      attribLabelsMap[it -> first] = attribLabelsVC;

    }







      //Calculamos jerarquia de tipos
      std::cout << "#############################" << std::endl;
      std::cout << "Processing types..." << std::endl << std::endl;
      std::map <std::string, std::vector <std::pair <std::string, std::string> > > cabeceras;
      std::vector < Type *> tipos = extractTypeHierarchy(PTS, &cabeceras);
      if(tipos.size() > 0){
        typing = true;
      }

      std::cout << std::endl << "#############################" << std::endl << std::endl << std::endl;






    for(auto it = datasetMap.begin(); it != datasetMap.end(); it++){

      std::cout << "\n\nTask: " << it -> first << std::endl;
      std::cout << "Attributes list:  " << std::endl;

      for (unsigned int iDataset = 0; iDataset < attribLabelsMap[it -> first].size(); iDataset++) {
        std::cout << "\nDataset nº " << iDataset+1 << std::endl;

        for (unsigned int attr = 0; attr < attribLabelsMap[it -> first][iDataset].size(); attr++) {
          std::cout << "Attr" +  std::to_string(attr) << ":\t" << attribLabelsMap[it -> first][iDataset][attr].first << std::endl;
        }



        std::cout << std::endl<< std::flush;
        std::cout << std::left  << std::setw(6) << std::setfill(' ') << "|";

        for (unsigned int attr = 0; attr < attribLabelsMap[it -> first][iDataset].size(); attr++){
          std::cout << std::left << std::setw(6) << std::setfill(' ') << "Attr" +  std::to_string(attr) << std::setw(6) << std::setfill(' ') << "|";
        }
        std::cout << std::endl;


        for (unsigned int i = 0; i < it -> second[iDataset][0].size(); i++) {
          for (unsigned int attr = 0; attr < attribLabelsMap[it -> first][iDataset].size(); attr++){
            if(attr==0){
              std::cout << std::setw(6) << std::setfill(' ') << "|";
            }
            std::cout << std::left << std::setw(6) << std::setfill(' ') << it -> second[iDataset][attr][i]  << std::setw(6) << std::setfill(' ') << "|";
          }
          std::cout << std::endl;
        }




      }
    }






    /*
      Dataset cleaning
    */
    vector<string> tokens;
    std::string buf;

    std::cout << "#############################" << std::endl;
    std::cout << "Cleaning Datasets..." << std::endl << std::endl;

    unsigned int cont = 0;
    for(auto it = datasetMap.begin(); it != datasetMap.end(); it++){
      cont++;
      std::cout << " " << std::right << std::setw(6) << std::setfill('-') << cont << "/" << std::left << std::setw(6) << std::setfill('-') << (datasetMap.size()) << std::endl;

      tokens.clear();
      std::stringstream ss(it->first); // Insert the string into a stream

      while (ss >> buf){
        tokens.push_back(buf);
      }
      std::cout << tokens[0] << std::endl;
      cleanDataset(&(it->second)[0], 0.05);
      cleanDataset(&(it->second)[1], 0.05);
    }


    std::cout << "Datasets cleaned." << std::endl;
    std::cout << "#############################" << std::endl << std::endl << std::endl;

    /*
      CSV creation section.
    */

    std::cout << "#############################" << std::endl;
    std::cout << "Creating Datasets..." << std::endl << std::endl;

    std::string sPath;
    if(argc == 3){
      sPath = "outputPM/" + string(argv[2]) + "/";
    }else{
      sPath = "outputPM/NODOMAINNAME/";
    }
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

    /*
    for(std::map < std::string, std::vector < std::vector < double > > >::iterator it = datasetMap.begin(); it != datasetMap.end(); it++){
      tokens.clear();
      std::stringstream ss(it->first); // Insert the string into a stream

      while (ss >> buf){
        tokens.push_back(buf);
      }

      std::ofstream output;
      output.open(sPath + it->first + ".csv", std::ofstream::out | std::ofstream::trunc);

      for(unsigned int index = 0; index < attribLabelsMap[it -> first].size(); index++){
        if(index == 0){
          output << attribLabelsMap[it -> first][index].first;
        }
        else{
          output << ";" << attribLabelsMap[it -> first][index].first;
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
      std::cout << "Dataset " +  it->first + ".csv" + " created." << std::endl;

    }
*/
    std::cout << "#############################" << std::endl << std::endl << std::endl;


    std::cout << "Launching INSLV..." << std::endl << std::endl << std::endl;


    ofstream pddl;
    std::string salidaStr;

    std::string nombreDom;
    if(argc == 3){
      nombreDom = string(argv[2]);
    }else{
      nombreDom = "noName";
    }

    pddl.open(nombreDom + ".pddl", std::ofstream::out | std::ofstream::trunc);
    pddl << "";
    salidaStr = nombreDom;
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
    std::map <std::string, char > predsInc;

    unsigned int nPreds = 0, nFunct = 0;
    for(auto itAttr = attribLabelsMap.begin(); itAttr != attribLabelsMap.end(); itAttr++){
      for (unsigned int iDataset = 0; iDataset < (itAttr -> second).size(); iDataset++){
        for (unsigned int iAttr = 0; iAttr < (itAttr -> second)[iDataset].size(); iAttr++){
          if((itAttr -> second)[iDataset][iAttr].second == "Logical"){
            nPreds++;
          }
          if((itAttr -> second)[iDataset][iAttr].second == "Numerical"){
            nFunct++;
          }
        }
      }
    }



    if(nPreds >0){
      pddl << "\t(:predicates" << endl;
      for (auto mapIt= attribLabelsMap.begin(); mapIt != attribLabelsMap.end(); mapIt++){
        for (unsigned int iDataset = 0; iDataset < (mapIt -> second).size(); iDataset++){
          for (unsigned int iAttr = 0; iAttr < (mapIt -> second)[iDataset].size(); iAttr++){
            if((mapIt -> second)[iDataset][iAttr].second == "Logical"){
              tokens.clear();
              std::stringstream ss((mapIt -> second)[iDataset][iAttr].first); // Insert the string into a stream
              while (ss >> buf){
                tokens.push_back(buf);
              }

              if((mapIt -> second)[iDataset][iAttr].first != "Class" and not (tokens[0] == "opEQ" or tokens[0] == "opGT" or tokens[0] == "opLT"  or tokens[0] == "opARITH")){
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
        }
      }
      pddl << "\t)" << endl;
    }


    if(nFunct >0){
      pddl << "\t(:functions" << endl;
      for (auto mapIt= attribLabelsMap.begin(); mapIt != attribLabelsMap.end(); mapIt++){
        for (unsigned int iDataset = 0; iDataset < (mapIt -> second).size(); iDataset++){
          for (unsigned int iAttr = 0; iAttr < (mapIt -> second).size(); iAttr++){
            if((mapIt -> second)[iDataset][iAttr].second == "Numerical"){
              tokens.clear();
              std::stringstream ss((mapIt -> second)[iDataset][iAttr].first); // Insert the string into a stream
              while (ss >> buf){
                tokens.push_back(buf);
              }

              if((mapIt -> second)[iDataset][iAttr].first != "Class" and not (tokens[0] == "DELTA" or tokens[0] == "opEQ" or tokens[0] == "opGT" or tokens[0] == "opLT"  or tokens[0] == "opARITH")){
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
        }
      }
      pddl << "\t)" << endl;
    }


    pddl.close();

    bool noRepe;
    vector <list< pair<string, string> > > reglas;
    // [ {n_ejemplos, [<attr, val>, <attr, val>, ... ,<attr, val>]}, ..., {n_ejemplos, [<attr, val>, <attr, val>, ... ,<attr, val>]} ]
    vector < pair <int, list< pair<string, string> > > > reglasInp1, reglasInp2;

    std::map < std::string, std::vector <FuzzSet> > conjuntosDiff;

    for(auto it = datasetMap.begin(); it != datasetMap.end(); it++){
      tokens.clear();
      std::stringstream ss(it->first); // Insert the string into a stream

      while (ss >> buf){
        tokens.push_back(buf);
      }

      reglasInp1.clear();
      reglasInp2.clear();
      reglas.clear();


      if(attribLabelsMap[it -> first][0].size() > 1 ){
        conjuntosDiff = discretize(&datasetMap[it -> first][0], &attribLabelsMap[it -> first][0]);
        inslv(nombreDom, it -> first, sPath, &datasetMap[it -> first][0], attribLabelsMap[it -> first][0], conjuntosDiff, &reglasInp1);
      }


      if(attribLabelsMap[it -> first][1].size() > 1 ){
        conjuntosDiff = discretize(&datasetMap[it -> first][1], &attribLabelsMap[it -> first][1]);
        inslv(nombreDom, it -> first, sPath, &datasetMap[it -> first][1], attribLabelsMap[it -> first][1], conjuntosDiff, &reglasInp2);
      }

      if(reglasInp2.begin() != reglasInp2.end()){
        reglasInp1.insert(reglasInp1.end(), reglasInp2.begin(), reglasInp2.end());
      }

      fuseRules(&reglasInp1, &reglas, &datasetMap[it -> first][0], attribLabelsMap[it -> first][0], &(cabeceras[it -> first]));


      noRepe = false;
      pddl.open(nombreDom + ".pddl", std::ofstream::out | std::ofstream::app);
      pddl << "\t(:action "<< it -> first << std::endl;
      pddl << "\t\t:parameters (";

      for(unsigned int paramT = 0; paramT < cabeceras[it -> first].size()-1; paramT++){
        pddl << "?" << cabeceras[it -> first][paramT].first << " - " << cabeceras[it -> first][paramT].second <<", ";
      }
      pddl << cabeceras[it -> first][cabeceras[it -> first].size()-1].first << " - " << cabeceras[it -> first][cabeceras[it -> first].size()-1].second;
      pddl << ")" <<  endl;


      pddl << "\t\t:precondition" << std::endl;
      if(reglas[0].size() > 0){
        pddl << "\t\t\t(and" << endl;
      }
      else{
        pddl << "\t\t\t( )" << endl;
      }

      for (std::list< pair<string, string> >::iterator it=reglas[0].begin(); it != reglas[0].end(); ++it){
        tokens.clear();
        std::stringstream ss((*it).first);
        while (ss >> buf){
          tokens.push_back(buf);
        }

        if(tokens[0] == "opEQ" or tokens[0] == "opGT" or tokens[0] == "opGET" or tokens[0] == "opLT" or tokens[0] == "opLET"){
          pddl << "\t\t\t\t(";
          if(tokens[0] == "opEQ"){
            pddl << "== ";
          }
          if(tokens[0] == "opGET"){
            pddl << ">= ";
          }
          if( tokens[0] == "opGT"){
            pddl << "> ";
          }
          if(tokens[0] == "opLET"){
            pddl << "<= ";
          }
          if(tokens[0] == "opLT"){
            pddl << "< ";
          }

          for(unsigned int i = 2; i < tokens.size(); i++){
            if(tokens[i] == "-" and tokens[i+1] != "("){
              i++;
            }
            else{
              pddl  << tokens[i] << " ";
            }

          }
          pddl << ")" << std::endl;
        }
        else{
          if((*it).second != "False"){
            pddl << "\t\t\t\t";
            for(unsigned int i = 0; i < tokens.size(); i++){
              if(tokens[i] != "-"){
                pddl  << tokens[i] << " ";
              }
              else{
                i++;
              }
            }
            pddl << "\n";
          }
        }
      }


    if(reglas[0].size() > 0){
      pddl << "\t\t\t)" << endl;
    }


    pddl << "\t\t:effect" << std::endl;
    if(reglas[0].size() > 0){
      pddl << "\t\t\t(and" << endl;
    }
    else{
      pddl << "\t\t\t( )" << endl;
    }



    for (std::list< pair<string, string> >::iterator it=reglas[1].begin(); it != reglas[1].end(); ++it){
      tokens.clear();
      std::stringstream ss((*it).first);
      while (ss >> buf){
        tokens.push_back(buf);
      }

      pddl << "\t\t\t\t";

      if(tokens[0] == "DELTA"){
        if( stod((*it).second) > 0){
          pddl << "( increase ";
        }
        else{
          pddl << "( decrease ";
        }

        for(unsigned int i = 2; i < tokens.size(); i++){
          if(tokens[i] != "-"){
            pddl  << tokens[i] << " ";
          }
          else{
            i++;
          }
        }

        pddl << " )\n";

      }
      else{
        if(tokens[0] == "opEQ"){
          pddl << "( assign ";
          for(unsigned int i = 2; i < tokens.size(); i++){
            if(tokens[i] != "-"){
              pddl  << tokens[i] << " ";
            }
            else{
              i++;
            }
          }
          pddl << " )\n";
        }
        else{
          if((*it).second == "False"){
            pddl << "( not ";
          }
          for(unsigned int i = 0; i < tokens.size(); i++){
            if(tokens[i] != "-"){
              pddl  << tokens[i] << " ";
            }
            else{
              i++;
            }

          }

          if((*it).second == "False"){
            pddl << " )";
          }
          pddl << "\n";
        }

      }
    }


    if(reglas[0].size() > 0){
      pddl << "\t\t\t)" << endl;
    }

    pddl << "\t)" << std::endl;
    pddl.close();
  }


  pddl.open(nombreDom + ".pddl", std::ofstream::out | std::ofstream::app);
  pddl << ")";
  pddl.close();

    exit(EXIT_SUCCESS);
  }
  else{
    exit(EXIT_FAILURE);
  }


}
