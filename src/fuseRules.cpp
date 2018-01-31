#include <iostream>
#include <queue>
#include "fuseRules.hpp"

class Comparator{
public:
  Comparator(){}
  bool operator() (const std::pair < int, std::pair < std::string, std::string > > lhs, std::pair < int, std::pair < std::string, std::string > > rhs) const{
    return (lhs.first<rhs.first);
  }
};

typedef std::priority_queue < std::pair < int, std::pair < std::string, std::string > > > rfPQ;


int conflict(std::vector< std::pair<std::string, std::string> > rule, std::pair<std::string, std::string> tuple){
  int conflicto = -1, i = 0;
  for(std::vector< std::pair<std::string, std::string> >::iterator it = rule.begin(); it != rule.end(); it++){
    if((*it).first == tuple.first and (*it).second != tuple.second){
      conflicto = i;
      break;
    }
    i++;
  }
  return conflicto;
}

unsigned int measureImpact(std::vector< std::pair<std::string, std::string> > ruleAux, std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels, double clase){
  unsigned int elem = 0, cont = 0;
  for (unsigned int j = 0; j < (*dataset)[0].size(); j++) {
    if((*dataset)[dataset -> size() - 1][j] == clase){
      for (unsigned int ant = 0; ant < ruleAux.size(); ant++){
        cont = 0;
        for(unsigned int attr = 0; attr < attribLabels -> size(); attr++){
          if((*attribLabels)[attr] == ruleAux[ant].first){
            //True
            if(ruleAux[ant].second  == "True" and (*dataset)[attr][j] == 1.0){
              cont++;
            }

            //False
            if(ruleAux[ant].second  == "False" and (*dataset)[attr][j] == 0.0){
              cont++;
            }

            //MVs
            if((*dataset)[attr][j] == -999999999.0){
              cont++;
            }

            //Numericos
            //WIP
            if(ruleAux[ant].second  == "" and (*dataset)[attr][j] == -999999.0 and 0){
              cont++;
            }
          }
        }
      }
      if(cont == ruleAux.size()){
        elem++;
      }
    }
  }
  return elem;
}

bool significative(unsigned int impactoBase, unsigned int impactoAux){
  return impactoAux > impactoBase * 0.9 and impactoAux < impactoBase * 1.1;
}

void fuseRules(std::vector < std::pair <int, std::list< std::pair<std::string, std::string > > > > * reglasInp,
  std::vector <std::list< std::pair<std::string, std::string> > > * reglasOut, std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels){

    /*
      |  Ordenar las tuplas <attr, val> segun su numero de ejemplos y guardarlas en una lista de tuplas
      |
      |  regla = []
      |  Impacto = 0
      |
      |  Mientras la lista de tuplas tenga elementos:
      |  |  tupla = sacarElem(listaTuplas)
      |  |  si no hay conflicto entre regla y tupla:
      |  |  |  incluir tupla a regla
      |  |  |
      |  |  si lo hay:
      |  |  |  reglaAux = regla
      |  |  |  sustituirConflicto(reglaAux, tupla)
      |  |  |
      |  |  |  Impacto = medirImpacto(reglaAux, dataset, attribLabels)
      |  |  |  Si EsSignificativo(Impacto):
      |  |  |  |  delete(reglaAux)
      |  |  |  Si no:
      |  |  |  |  delete(tupla)
      |  |  |  |  delete(conflicto)
      | return regla
    */

    std::cout << "\n\n#########################################################################" << std::endl;
    std::cout << "Fusing rules..." << std::endl;

    rfPQ prePQ;
    rfPQ postPQ;
    std::vector <std::pair < int, std::pair < std::string, std::string > > > preVec;
    std::vector <std::pair < int, std::pair < std::string, std::string > > > postVec;
    std::vector <std::pair < int, std::pair < std::string, std::string > > >::iterator itPQ;

    bool esta = false;

    // Ordenar las tuplas <attr, val> segun su numero de ejemplos y guardarlas en una lista de tuplas
    for (unsigned int i = 0; i < reglasInp -> size(); i++) {
      if((*reglasInp)[i].first < 0){
        for(std::list< std::pair<std::string, std::string> >::iterator it = (*reglasInp)[i].second.begin(); it != (*reglasInp)[i].second.end(); it++){
            itPQ= preVec.begin();
            esta=false;
            for (; itPQ != preVec.end(); itPQ++){
              if((*itPQ).second.first == (*it).first and (*itPQ).second.second == (*it).second){
                esta = true;
                break;
              }
            }
            if(esta == true){
              (*itPQ).first -= (*reglasInp)[i].first;
            }
            else{
              preVec.push_back(  std::pair < int, std::pair < std::string, std::string > > (-(*reglasInp)[i].first, *it));
            }
        }

      }else{
        for(std::list< std::pair<std::string, std::string> >::iterator it = (*reglasInp)[i].second.begin(); it != (*reglasInp)[i].second.end(); it++){
            itPQ= postVec.begin();
            esta = false;
            for (; itPQ != postVec.end(); itPQ++){
              if((*itPQ).second.first == (*it).first and (*itPQ).second.second == (*it).second){
                esta = true;
                break;
              }
            }
            if(esta == true){
              (*itPQ).first += (*reglasInp)[i].first;
            }
            else{
              postVec.push_back(  std::pair < int, std::pair < std::string, std::string > > ((*reglasInp)[i].first, *it));
            }
        }
      }
    }

    //std::cout << "Pre:" <<std::endl;
    for(unsigned int i = 0; i < preVec.size(); i++ ){
        //std::cout <<"[" << preVec[i].first <<"] " << preVec[i].second.first << " = " << preVec[i].second.second << std::endl;
        prePQ.push(preVec[i]);
    }

    //std::cout << "Post:" <<std::endl;
    for(unsigned int i = 0; i < postVec.size(); i++ ){
        //std::cout <<"[" << postVec[i].first <<"] " << postVec[i].second.first << " = " << postVec[i].second.second << std::endl;
        postPQ.push(postVec[i]);
    }


    //  regla = []
    std::vector< std::pair<std::string, std::string> > rule;
    std::vector< std::pair<std::string, std::string> > ruleAux;
    //  Impacto = 0
    unsigned int impact =0, impactAux =0;

    std::pair<std::string, std::string> tuple;
    int elem;


    //Mientras la lista de tuplas tenga elementos:
    while (!prePQ.empty()) {
      //tupla = sacarElem(listaTuplas)
      tuple.first = prePQ.top().second.first;
      tuple.second = prePQ.top().second.second;

      //si no hay conflicto entre regla y tupla:
      elem = conflict(rule, tuple);
      if(elem == -1){
        //incluir tupla a regla
        rule.push_back(tuple);
        impact = measureImpact(ruleAux, dataset, attribLabels, 0);
      }else{
        //si lo hay:
        //std::cout << "Hay un conflicto con: " << tuple.first << " = " << tuple.second << std::endl;
        //reglaAux = regla
        ruleAux = rule;
        //sustituirConflicto(reglaAux, tupla)
        ruleAux[elem] = tuple;

        /*std::cout << "Regla ANTES de la susticion" << std::endl;
        for(unsigned int i = 0; i < rule.size(); i++){
          std::cout << "(" <<rule[i].first << " = " << rule[i].second << ")" << std::endl;
        }

        std::cout << "Regla DESPUES de la susticion" << std::endl;
        for(unsigned int j = 0; j < ruleAux.size(); j++){
          std::cout << "(" <<ruleAux[j].first << " = " << ruleAux[j].second << ")" << std::endl;
        }*/

        //Impacto = medirImpacto(reglaAux, dataset, attribLabels)
        impactAux = measureImpact(ruleAux, dataset, attribLabels, 0);


        //Si EsSignificativo(Impacto):
        if(significative(impact, impactAux)){
          //delete(reglaAux)
          ruleAux.clear();
        }
        //Si no:
        else{
          //delete(reglaAux)
          ruleAux.clear();
          //delete(conflicto)
          rule.erase(rule.begin() + elem);
        }
      }

      prePQ.pop();
    }

    std::cout << "Rule created for the pre-state model:" << std::endl;
    for(unsigned int i = 0; i < rule.size(); i++){
      std::cout << "(" <<rule[i].first << " = " << rule[i].second << ")" << std::endl;
    }

    reglasOut -> push_back(std::list < std::pair< std::string, std::string> > (rule.begin(), rule.end()));


    rule.clear();
    ruleAux.clear();

    //Mientras la lista de tuplas tenga elementos:
    while (!postPQ.empty()) {
      //tupla = sacarElem(listaTuplas)
      tuple.first = postPQ.top().second.first;
      tuple.second = postPQ.top().second.second;

      //si no hay conflicto entre regla y tupla:
      elem = conflict(rule, tuple);
      if(elem == -1){
        //incluir tupla a regla
        //std::cout << "No hay un conflicto con: " << tuple.first << " = " << tuple.second << std::endl;
        rule.push_back(tuple);
        impact = measureImpact(ruleAux, dataset, attribLabels, 1);
      }else{
        //si lo hay:
        //std::cout << "Hay un conflicto con: " << tuple.first << " = " << tuple.second << std::endl;
        //reglaAux = regla
        ruleAux = rule;
        //sustituirConflicto(reglaAux, tupla)
        ruleAux[elem] = tuple;

        /*std::cout << "Regla ANTES de la susticion" << std::endl;
        for(unsigned int i = 0; i < rule.size(); i++){
          std::cout << "(" <<rule[i].first << " = " << rule[i].second << ")" << std::endl;
        }

        std::cout << "Regla DESPUES de la susticion" << std::endl;
        for(unsigned int j = 0; j < ruleAux.size(); j++){
          std::cout << "(" <<ruleAux[j].first << " = " << ruleAux[j].second << ")" << std::endl;
        }*/

        //Impacto = medirImpacto(reglaAux, dataset, attribLabels)
        impactAux = measureImpact(ruleAux, dataset, attribLabels, 1);


        //Si EsSignificativo(Impacto):
        if(significative(impact, impactAux)){
          //delete(reglaAux)
          ruleAux.clear();
        }
        //Si no:
        else{
          //delete(reglaAux)
          ruleAux.clear();
          //delete(conflicto)
          rule.erase(rule.begin() + elem);
        }
      }

      postPQ.pop();
    }

    std::cout << "\nRule created for the post-state model:" << std::endl;
    for(unsigned int i = 0; i < rule.size(); i++){
      std::cout << "(" <<rule[i].first << " = " << rule[i].second << ")" << std::endl;
    }
    reglasOut -> push_back(std::list < std::pair< std::string, std::string> > (rule.begin(), rule.end()));



    std::cout << "#########################################################################" << std::endl;
  }
