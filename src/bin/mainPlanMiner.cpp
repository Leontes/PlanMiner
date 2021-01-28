#include <iostream>
#include <stdlib.h>
#include <utility>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex>


#include "PtLib.hpp"
#include "DataSLib.hpp"
#include "RuleLib.hpp"


const std::regex diffRE("^(DELTA - )\\((.)*\\)\\)");
const std::regex opRE("(^(opEQ - )\\((.)*\\))|(^(opGT-)\\((.)*\\))|(^(opLT-)\\((.)*\\))");


//const std::regex diffRE("^(\\( DELTA)(.)*(\\))");

int main(int argc, char const *argv[]) {
  bool existeFL;
  if(argc > 1){
    std::ifstream infile(argv[1]);
    existeFL = infile.good();
    infile.close();
  }
  else{
    existeFL = false;
  }


  if (argc > 1 and existeFL != false) {

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
      planLenght += (*PTS)[i] -> getNTS();
      stateLength += (*PTS)[i] -> stateLenght();
      (*PTS)[i] -> schematize(/*tipos*/);
      std::cout << std::right << std::setw(6) << std::setfill('-') << i+1 << "/" << std::left << std::setw(6) << std::setfill('-') << (PTS -> size()) << " Lenght: " << (*PTS)[i] -> getNTS() << endl;
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


    //Calculamos jerarquia de tipos
    std::cout << "#############################" << std::endl;
    std::cout << "Processing types..." << std::endl << std::endl;
    std::map <std::string, std::vector <std::pair <std::string, std::string> > > cabeceras;
    std::map <std::string, std::vector <std::pair <std::string, std::string> > > predicados;
    std::vector < Type *> tipos = extractTypeHierarchy(PTS, &cabeceras, &predicados);
    if(tipos.size() > 0){
      typing = true;
    }

    std::cout << std::endl << "#############################" << std::endl << std::endl << std::endl;

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

    std::cout << "\n\n#############################" << std::endl;
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
      lowFreq(&(it->second)[0], 0.05);
      // discretizar(&(it->second)[1], &(attribLabelsMap[(it->first)][1]));
      std::cout << "Siguiente accion\n";
    }
    std::cout << "Datasets cleaned." << std::endl;
    std::cout << "#############################" << std::endl << std::endl << std::endl;


    /*
      Information Discovery
    */
    std::cout << "\n\n#############################" << std::endl;
    std::cout << "Discovering Information..." << std::endl << std::endl;
    for(auto it = datasetMap.begin(); it != datasetMap.end(); it++){
      std::cout << it->first << std::endl;
      discoverInfo(&(it->second)[1], &(attribLabelsMap[(it->first)][1]));
    }
    std::cout << "\n\n#############################" << std::endl;

    /*
      CSV creation section.
    */

    std::cout << "#############################" << std::endl;
    std::cout << "Creating Datasets..." << std::endl << std::endl;

    std::string sPath;
    if(argc == 3){
      sPath = "outputPM/SLV/" + string(argv[2]) + "/";
    }else{
      sPath = "outputPM/SLV/NODOMAINNAME/";
    }
    struct stat sb;
    if (stat(sPath.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)){
      std::cout << "Datasets folder found." << std::endl;
    }
    else{
      std::cout << "Output folder not found. Creating " +  sPath  + " folder..." << std::endl;
      mode_t nMode = 0733; // UNIX style permissions
      int nError = 0;
      #if defined(_WIN32)
        nError = _mkdir("outputPM/");
        nError = _mkdir("outputPM/SLV/");
        nError = _mkdir(sPath.c_str()); // can be used on Windows
        nError = _mkdir((sPath + "Logic/").c_str()); // can be used on Windows
        nError = _mkdir((sPath + "Number/").c_str()); // can be used on Windows
        nError = _mkdir((sPath + "Logic/res/").c_str()); // can be used on Windows
        nError = _mkdir((sPath + "Number/res/").c_str()); // can be used on Windows
      #else
        nError = mkdir("outputPM/",nMode);
        nError = mkdir("outputPM/SLV/",nMode);
        nError = mkdir(sPath.c_str(),nMode); // can be used on non-Windows
        nError = mkdir((sPath + "Logic/").c_str(),nMode); // can be used on non-Windows
        nError = mkdir((sPath + "Number/").c_str(),nMode); // can be used on non-Windows
        nError = mkdir((sPath + "Logic/res/").c_str(),nMode); // can be used on non-Windows
        nError = mkdir((sPath + "Number/res/").c_str(),nMode); // can be used on non-Windows
      #endif
    }

    std::ofstream output;
    for(auto it = datasetMap.begin(); it != datasetMap.end(); it++){
      for (unsigned int iDataset = 0; iDataset < attribLabelsMap[it -> first].size(); iDataset++){
        if(iDataset == 1){
          output.open(sPath + "Number/" + it->first + ".csv", std::ofstream::out | std::ofstream::trunc);
        }
        else{
          output.open(sPath + "Logic/" + it->first + ".csv", std::ofstream::out | std::ofstream::trunc);
        }

        for (unsigned int attr = 0; attr < attribLabelsMap[it -> first][iDataset].size() - 1; attr++){
          output << attribLabelsMap[it -> first][iDataset][attr].first <<";";
        }
        output << attribLabelsMap[it -> first][iDataset][attribLabelsMap[it -> first][iDataset].size() - 1].first << std::endl;



        for (unsigned int i = 0; i < it -> second[iDataset][0].size(); i++){
          for (unsigned int attr = 0; attr < attribLabelsMap[it -> first][iDataset].size() - 1; attr++){
            output << it -> second[iDataset][attr][i] << "; ";
          }
          output << it -> second[iDataset][attribLabelsMap[it -> first][iDataset].size() - 1][i] << std::endl;
        }
        output.close();
        std::cout << "Dataset " +  it->first + to_string(iDataset) + ".csv" + " created." << std::endl;
      }

    }

    output.open(sPath + "headers.txt", std::ofstream::out | std::ofstream::trunc);
    for (auto cabeceras_it =  cabeceras.begin(); cabeceras_it != cabeceras.end(); cabeceras_it++){
      output << cabeceras_it -> first + " ";

      for (size_t m = 0; m < (cabeceras_it -> second).size(); m++) {
        output << (cabeceras_it -> second)[m].first + " - " + (cabeceras_it -> second)[m].second + " ";
      }
      output<< std::endl;
    }
    output.close();



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

    for (std::map <std::string, std::vector <std::string> >::iterator mapIt= mapaTipos.begin(); mapIt != mapaTipos.end(); mapIt++){
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
                if(predsInc.find(tokens[1]) == predsInc.end()){
                  predsInc[tokens[1]] = 0;
                  std::transform(tokens[1].begin(), tokens[1].end(), tokens[1].begin(), ::tolower);
                  pddl << "\t\t(" << tokens[1];
                  std::transform(tokens[1].begin(), tokens[1].end(), tokens[1].begin(), ::toupper);
                  for(unsigned int pr = 0; pr < predicados[tokens[1]].size(); pr++){
                    std::transform(predicados[tokens[1]][pr].second.begin(), predicados[tokens[1]][pr].second.end(), predicados[tokens[1]][pr].second.begin(), ::tolower);
                    pddl << " ?" << predicados[tokens[1]][pr].first << " - " << predicados[tokens[1]][pr].second;
                  }
                  pddl << ")" << endl;
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
          for (unsigned int iAttr = 0; iAttr < (mapIt -> second)[iDataset].size(); iAttr++){
            if((mapIt -> second)[iDataset][iAttr].second == "Numerical"){
              tokens.clear();
              std::stringstream ss((mapIt -> second)[iDataset][iAttr].first); // Insert the string into a stream
              while (ss >> buf){
                tokens.push_back(buf);
              }
              if((mapIt -> second)[iDataset][iAttr].first != "Class" and not (tokens[0] == "opEQ" or tokens[0] == "opGT" or tokens[0] == "opLT"  or tokens[0] == "opARITH" or tokens[0] == "TIMESTAMP")){
                if(predsInc.find(tokens[1]) == predsInc.end()){
                  predsInc[tokens[1]] = 0;
                  std::transform(tokens[1].begin(), tokens[1].end(), tokens[1].begin(), ::tolower);
                  pddl << "\t\t(" << tokens[1];
                  std::transform(tokens[1].begin(), tokens[1].end(), tokens[1].begin(), ::toupper);
                  for(unsigned int pr = 0; pr < predicados[tokens[1]].size(); pr++){
                    std::transform(predicados[tokens[1]][pr].second.begin(), predicados[tokens[1]][pr].second.end(), predicados[tokens[1]][pr].second.begin(), ::tolower);
                    pddl << " ?" << predicados[tokens[1]][pr].first << " - " << predicados[tokens[1]][pr].second;
                  }
                  pddl << ")" << endl;
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
        conjuntosDiff = labeler(&datasetMap[it -> first][0], &attribLabelsMap[it -> first][0]);
        inslv(nombreDom, it -> first, sPath + "Logic/", &datasetMap[it -> first][0], attribLabelsMap[it -> first][0], conjuntosDiff, &reglasInp1);
      }

      if(attribLabelsMap[it -> first][1].size() > 1 ){
        conjuntosDiff = labeler(&datasetMap[it -> first][1], &attribLabelsMap[it -> first][1]);
        inslv(nombreDom, it -> first, sPath  + "Number/", &datasetMap[it -> first][1], attribLabelsMap[it -> first][1], conjuntosDiff, &reglasInp2);
      }

      if(reglasInp2.begin() != reglasInp2.end() and reglasInp2.size() > 1){
        reglasInp1.insert(reglasInp1.end(), reglasInp2.begin(), reglasInp2.end());
      }

      std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~\n";
      for (size_t p = 0; p < reglasInp1.size(); p++) {
        std::cout << reglasInp1[p].first << std::endl;
        for (auto itLista = reglasInp1[p].second.begin(); itLista != reglasInp1[p].second.end(); itLista++) {
          std::cout << itLista -> first << " => "  << itLista -> second << std::endl;
        }
      }
      std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~\n";




      fuseRules(&reglasInp1, &reglas, &(cabeceras[it -> first]), attribLabelsMap[it -> first]);

      if(reglas.size() > 0){
        noRepe = false;
        pddl.open(nombreDom + ".pddl", std::ofstream::out | std::ofstream::app);
        pddl << "\t(:action "<< it -> first << std::endl;
        pddl << "\t\t:parameters (";



        for(unsigned int paramT = 0; paramT < cabeceras[it -> first].size()-1; paramT++){
          std::transform(cabeceras[it -> first][paramT].second.begin(), cabeceras[it -> first][paramT].second.end(), cabeceras[it -> first][paramT].second.begin(), ::tolower);

          pddl << "?" << cabeceras[it -> first][paramT].first << " - " << cabeceras[it -> first][paramT].second <<" ";
        }

        std::transform(cabeceras[it -> first][cabeceras[it -> first].size()-1].second.begin(), cabeceras[it -> first][cabeceras[it -> first].size()-1].second.end(), cabeceras[it -> first][cabeceras[it -> first].size()-1].second.begin(), ::tolower);
        pddl << "?" << cabeceras[it -> first][cabeceras[it -> first].size()-1].first << " - " << cabeceras[it -> first][cabeceras[it -> first].size()-1].second;



        pddl << ")" <<  endl;


        for (std::list< pair<string, string> >::iterator it=reglas[1].begin(); it != reglas[1].end(); ++it){
          tokens.clear();
          std::stringstream ss((*it).first);
          while (ss >> buf){
            tokens.push_back(buf);
          }

          if (tokens[0] == "DELTA" and tokens[2] == "TIMESTAMP"){
            // pddl << "\t\t:duration (= ?duration ";
            //
            // for(unsigned int i = 3; i < tokens.size(); i++){
            //   if(tokens[i] != "~"){
            //     pddl  << tokens[i] << " ";
            //   }
            //   else{
            //     i++;
            //   }
            // }
            //
            // pddl << ")\n";
          }
        }


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
          if(tokens[2] != "TIMESTAMP"){
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
                if(tokens[i] == "~" and tokens[i+1] != "("){
                  i++;
                }
                else{
                  std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);
                  if(tokens[i] == "delta"){
                    //pddl  << " AQUI VA UN DELTA ";
                  }
                  else{
                    pddl  << tokens[i] << " ";
                  }
                }

              }
              pddl << ")" << std::endl;
            }
            else{
              if((*it).second != "False"){
                pddl << "\t\t\t\t";
                for(unsigned int i = 0; i < tokens.size(); i++){
                  if(tokens[i] != "~"){
                    std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);
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
        }

        if(reglas[0].size() > 0){
          pddl << "\t\t\t)" << endl;
        }

        pddl << "\t\t:effect" << std::endl;
        if(reglas[1].size() > 0){
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

          if(tokens[2] != "TIMESTAMP"){
            pddl << "\t\t\t\t";
          }

          if(tokens[0] == "DELTA"){
            if(tokens[2] != "TIMESTAMP"){
              if(stod((*it).second) > 0.0){
                pddl << "( increase ";
              }
              else{
                pddl << "( decrease ";
              }

              for(unsigned int i = 2; i < tokens.size(); i++){
                if(tokens[i] != "~"){
                  std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);
                  pddl  << tokens[i] << " ";
                }
                else{
                  i++;
                }
              }
              pddl << " )\n";
            }
          }
          else{
            if(tokens[0] == "opEQ"){
              pddl << "( assign ( ";
              for(unsigned int i = 2; i < tokens.size(); i++){
                if(tokens[i] != "~"){
                  std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);
                  pddl  << tokens[i] << " ";
                }
                else{
                  i++;
                }
              }
              pddl << ")\n";
            }
            else{
              if((*it).second == "False"){
                pddl << "( not ";
              }
              for(unsigned int i = 0; i < tokens.size(); i++){
                if(tokens[i] != "~"){
                  std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);
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


        if(reglas[1].size() > 0){
          pddl << "\t\t\t)" << endl;
        }

        pddl << "\t)" << std::endl;
        pddl.close();
      }
    }


    pddl.open(nombreDom + ".pddl", std::ofstream::out | std::ofstream::app);
    pddl << ")";
    pddl.close();

    exit(EXIT_SUCCESS);
  }
  else{
    std::cout << "Input PT file not specified or does not exist" << std::endl;
    exit(EXIT_FAILURE);
  }


}
