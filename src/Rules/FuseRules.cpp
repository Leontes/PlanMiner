#include <iostream>
#include <queue>
#include <sstream>
#include <limits>
#include <map>

#include "Rules/FuseRules.hpp"


bool isnumber(const std::string& s){
  if(s[0] == '-' and s.size() == 1){
    return false;
  }
  else{
    return(s.find_first_not_of( "-.0123456789" ) == std::string::npos);
  }
}

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

bool significative(unsigned int impactoBase, unsigned int impactoAux){
  return impactoAux > impactoBase * 0.9 and impactoAux < impactoBase * 1.1;
}

bool en(std::string A, std::string B){

  //std::cout << "¿Esta " << B << " en " << A << "?\n";
  if (A.find(B) != std::string::npos) {
    //std::cout << "\t Siiii!" << '\n';
      return true;
  }
  else{
    //std::cout << "\t Noooo!" << '\n';
    return false;
  }
}

std::list < std::pair< std::string, std::string> > borrarRedundancias(std::list < std::pair< std::string, std::string> > regla){
  std::list < std::pair< std::string, std::string> > aux;
  std::map < std::string, std::list < std::string> > dictIgualdades;
  //std::cout << "\n\n\n////////////////////////////////////////////////////////////\n";
  std::vector<std::string> tokens, op1, op2;
  std::string buf, op1STR, op2STR;
  unsigned int i = 0;
  for (auto it = regla.begin(); it != regla.end(); it++) {
    //std::cout << (*it).first << " <-----> " << (*it).second << "\n";
    tokens.clear();
    std::stringstream ss(it->first);
    while (ss >> buf){
      tokens.push_back(buf);
    }

    if(tokens[0] == "opGT" or tokens[0] == "opLT" or tokens[0] == "opEQ"){
      if(tokens[2] == "("){
        i = 2;
        op1.clear();
        while(tokens[i] != ")"){
          op1.push_back(tokens[i]);
          i++;
        }
        op1.push_back(tokens[i]);
        i++;
      }
      else{
        i = 2;
        op1.clear();
        op1.push_back(tokens[i]);
        i++;
      }

      if(tokens[i] == "("){
        op2.clear();
        while(i < tokens.size()){
          op2.push_back(tokens[i]);
          i++;
        }
      }
      else{
        op2.clear();
        op2.push_back(tokens[i]);
        i++;
      }

      op1STR = "";
      for (size_t j = 0; j < op1.size(); j++) {
        op1STR += op1[j];
        op1STR += " ";
      }

      op2STR = "";
      for (size_t j = 0; j < op2.size(); j++) {
        op2STR += op2[j];
        op2STR += " ";
      }

      if(dictIgualdades.find(op1STR) == dictIgualdades.end()){
        dictIgualdades[op1STR] = std::list < std::string>();
        dictIgualdades[op1STR].push_back(op2STR);
      }
      else{
        dictIgualdades[op1STR].push_back(op2STR);
      }
    }
  }


  std::list < std::string> listaPreds;
  std::vector < std::string> predsAEliminar;
  std::vector <bool> contenidoEn;
  std::string predA, predB;
  for(auto it = dictIgualdades.begin(); it != dictIgualdades.end(); it++){
    //std::cout << (*it).first << std::endl;
    listaPreds = (*it).second;
    predsAEliminar.clear();
    for(auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++){
      //std::cout << "\t*** " << (*it2) << std::endl;
      contenidoEn.clear();
      predA = (*it2);
      for(auto it3 = (*it).second.begin(); it3 != (*it).second.end(); it3++){
        predB = (*it3);
        if(it2 != it3){
          if(en(predA, predB)){
            predsAEliminar.push_back(((*it).first + predB));
            predsAEliminar[predsAEliminar.size()-1].erase((predsAEliminar[predsAEliminar.size()-1]).end()-1, (predsAEliminar[predsAEliminar.size()-1]).end());
          }
          contenidoEn.push_back(en(predA, predB));
        }
        else{
          contenidoEn.push_back(false);
        }
      }
    }

    for (auto it2 = regla.begin(); it2 != regla.end(); it2++){
      if(((*it2).first.find("opGT") != std::string::npos) or ((*it2).first.find("opLT") != std::string::npos) or ((*it2).first.find("opEQ") != std::string::npos)){
        for(unsigned int i = 0; i < predsAEliminar.size(); i++){
          if((*it2).first.find(predsAEliminar[i]) != std::string::npos){
            (*it2).second = "ELIMINAR";
          }
        }
      }
    }

  }

  for (auto it = regla.begin(); it != regla.end(); it++){
    if((*it).second != "ELIMINAR"){
      aux.push_back((*it));
    }
  }

  return aux;
}

std::list < std::pair< std::string, std::string> > clearDeltas (std::list < std::pair< std::string, std::string> > regla, std::vector < std::vector < std::pair<std::string, std::string> > > labels){
  std::string buf, pred, aux;
  std::vector<std::string> tokens;
  bool fin;
  for(auto i = regla.begin(); i != regla.end(); i++){
    std::cout << (*i).first << std::endl;
    std::stringstream ss(i->first);
    while (ss >> buf){
      tokens.push_back(buf);
    }
    fin = false;
    for(unsigned int j = 0; j < tokens.size() and (fin == false); j++){
      if ((tokens[j] == "DELTA" or tokens[j] == "delta") and (j > 0)){
        while(tokens[j] != ")"){
          pred += tokens[j];
          pred += " ";
          j++;
        }
        pred += tokens[j];

        for(unsigned int k = 0; k < labels[1].size(); k++){
          if((labels[1][k].first.find(pred) != std::string::npos) and (labels[1][k].first.find("op") == std::string::npos)){
            aux = labels[1][k].first;
            fin = true;
            break;
          }
        }


      }
    }
    
    if(aux.size() > 0) {
      std::cout << "------------>" << aux << "\n";
      std::cout << "------------>" << pred << "\n";
      aux = aux.substr(aux.find("(", 9));
      std::cout << "------------>" << (*i).first.replace((*i).first.find(")") + 1, std::string::npos, aux) << "\n";
    }


  }

    for(unsigned int i = 0; i < labels[1].size(); i++){
      std::cout << labels[1][i].first << "\n";
    }

  return regla;

}



void fuseRules(std::vector < std::pair <int, std::list< std::pair<std::string, std::string > > > > * reglasInp,
  std::vector <std::list< std::pair<std::string, std::string> > > * reglasOut,
    std::vector <std::pair <std::string, std::string> > * cabeceras,
      std::vector < std::vector < std::pair<std::string, std::string> > > attribLabels){

    /*
      |  Modificar los predicados de las reglas con los conflictos entre objetos calculados
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

    std::vector <std::pair < int, std::pair < std::string, std::string > > > preVec;
    std::vector <std::pair < int, std::pair < std::string, std::string > > > postVec;
    std::vector <std::pair < int, std::pair < std::string, std::string > > > selectedPreds;


    bool esta = false;

    std::string buf,aux;
    std::vector<std::string> tokens;
    //Modificar los predicados con los objetos de las cabeceras calculados
    for(unsigned int i = 0; i < cabeceras -> size(); i++){
      for (size_t j = 0; j < reglasInp -> size(); j++) {
        for(auto it = (*reglasInp)[j].second.begin(); it != (*reglasInp)[j].second.end(); it++){
          aux = "";
          std::stringstream ss(it->first); // Insert the string into a stream

          while (ss >> buf){
            tokens.push_back(buf);
          }
          for (size_t k = 0; k < tokens.size(); k++) {
            if(tokens[k] == ("?" + (*cabeceras)[i].first)){
              aux += tokens[k];
              aux += " ";
              k++;
              aux += tokens[k];
              aux += " ";
              k++;
              aux += "?";
              aux += (*cabeceras)[i].second;
              aux += " ";
            }else{
              aux += tokens[k];
              aux += " ";
            }

          }
          //aux += "\n";
          it->first = aux;
          tokens.clear();
        }
      }
    }

    std::list< std::pair<std::string, std::string> > lstAux;
    for (unsigned int i = 0; i < reglasInp -> size(); i++) {
      lstAux.clear();
      for(std::list< std::pair<std::string, std::string> >::iterator it1 = (*reglasInp)[i].second.begin(); it1 != (*reglasInp)[i].second.end(); it1++){
        esta=false;
        for(std::list< std::pair<std::string, std::string> >::iterator it2 = lstAux.begin(); it2 != lstAux.end(); it2++){
          if((((*it1).first == (*it2).first) and ((*it1).second == (*it2).second))){
            esta=true;
            break;
          }
        }
        if(esta != true){
          lstAux.push_back((*it1));
        }
      }
      (*reglasInp)[i].second = lstAux;
    }


    std::vector <std::pair < int, std::pair < std::string, std::string > > >::iterator itVc = preVec.begin();
    //Ordenar las tuplas <attr, val> segun su numero de ejemplos y guardarlas en una lista de tuplas
    for (unsigned int i = 0; i < reglasInp -> size(); i++) {
      //Pre
      if((*reglasInp)[i].first < 0){
        for(std::list< std::pair<std::string, std::string> >::iterator it = (*reglasInp)[i].second.begin(); it != (*reglasInp)[i].second.end(); it++){
            esta=false;
            itVc = preVec.begin();
            for(; itVc != preVec.end(); itVc++){
              if((*itVc).second.first == (*it).first and (*itVc).second.second == (*it).second){
                esta = true;
                break;
              }
            }
            if(esta == true){
              (*itVc).first -= (*reglasInp)[i].first;
            }
            else{
              preVec.push_back(std::pair < int, std::pair < std::string, std::string > > (-(*reglasInp)[i].first, *it));
            }
        }

      }
      //Post
      else{
        for(std::list< std::pair<std::string, std::string> >::iterator it = (*reglasInp)[i].second.begin(); it != (*reglasInp)[i].second.end(); it++){
            esta = false;
            std::vector <std::pair < int, std::pair < std::string, std::string > > >::iterator itVc = postVec.begin();

            itVc = postVec.begin();
            for (; itVc != postVec.end(); itVc++){
              if((*itVc).second.first == (*it).first and (*itVc).second.second == (*it).second){
                esta = true;
                break;
              }
            }
            if(esta == true){
              (*itVc).first += (*reglasInp)[i].first;
            }
            else{
              postVec.push_back(  std::pair < int, std::pair < std::string, std::string > > ((*reglasInp)[i].first, *it));
            }
        }
      }
    }

    //Filtrar los elementos del post-estado para eliminar aquellas tuplas que aparezcan en el pre-estado
    // std::vector <std::pair < int, std::pair < std::string, std::string > > > selectedPreds2;
    // bool meter = true;
    // for(unsigned int i = 0; i < postVec.size(); i++ ){
    //   meter = true;
    //   for(unsigned int j = 0; j < preVec.size(); j++ ){
    //     if((postVec[i].second.first == preVec[j].second.first) and (postVec[i].second.second == preVec[j].second.second)){
    //         meter = false;
    //     }
    //   }
    //   if(meter == true){
    //     selectedPreds2.push_back(postVec[i]);
    //   }
    // }
    // postVec = selectedPreds2;
    //
    std::cout << "Predicados seleccionados para el prestado:" <<std::endl;
    for(unsigned int i = 0; i < preVec.size(); i++ ){
        std::cout <<"[" << preVec[i].first <<"] " << preVec[i].second.first << " = " << preVec[i].second.second << std::endl;
    }


    //  regla = []
    std::vector< std::pair<std::string, std::string> > rule;
    int conflicto = -1;
    int w1, w2;

    std::vector<bool> elemVistos(preVec.size(), false);
    for(unsigned int i = 0; i < preVec.size(); i++){
      if(elemVistos[i] == false){
        elemVistos[i] = true;
        conflicto = -1;
        for(unsigned int j = 0; j < preVec.size(); j++ ){
          if(elemVistos[j] == false){
            if(preVec[i].second.first == preVec[j].second.first){
              conflicto = j;
              elemVistos[j] = true;
            }
          }
        }

        if(conflicto != -1){
          w1 = preVec[i].first;
          w2 = preVec[conflicto].first;

          if((w1 > w2) and (((double)w1/(double)(w1+w2)) > 0.05)){
            //i gana -> se incluye en la regla
            //rule.push_back(std::pair<std::string, std::string> (preVec[i].second.first, preVec[i].second.second));
            selectedPreds.push_back(preVec[i]);
          }
          else{
            if((w2 > w1) and (((double)w2/(double)(w1+w2)) > 0.05)){
              //j gana -> se incluye en la regla
              //rule.push_back(std::pair<std::string, std::string> (preVec[conflicto].second.first, preVec[conflicto].second.second));
              selectedPreds.push_back(preVec[conflicto]);
            }
            else{
              //Empate. A tomar por culo las dos
            }
          }
        }
        else{
          //No hay conflicto -> se incluye en la regla el elemento
          //rule.push_back(std::pair<std::string, std::string> (preVec[i].second.first, preVec[i].second.second));
          selectedPreds.push_back(preVec[i]);
        }
      }
    }


    std::map<std::pair<std::string, std::string>, std::vector <std::vector <std::string> > > mapComps;
    std::vector <std::vector <std::string> > auxMat;
    std::vector <std::string> auxVec;
    std::string pred, otros;
    unsigned int contParent;

    //std::cout << "\nTOKENS:" <<std::endl;
    for(unsigned int i = 0; i < selectedPreds.size(); i++ ){
      std::stringstream ss(selectedPreds[i].second.first);
      tokens.clear();
      while (ss >> buf){
        tokens.push_back(buf);
      }

      auxVec.clear();
      if((selectedPreds[i].second.second != "True") and (selectedPreds[i].second.second != "False")){
        if(tokens[0] != "DELTA"){
          auxVec.push_back("opEQ");
          auxVec.push_back("-");
          auxVec.insert(auxVec.end(), tokens.begin(), tokens.end());
          tokens = auxVec;
          tokens.push_back(selectedPreds[i].second.second);
        }
      }

      if(tokens[0] == "opEQ" or tokens[0] == "opLT" or tokens[0] == "opGT"){
        pred="";
        unsigned int j = 2;
        while(tokens[j] != ")"){
          pred+= tokens[j] + " ";
          j++;
        }
        pred+= tokens[j];
        j++;
        if(isnumber(tokens[tokens.size()-1])){
          otros="NUMERO";
        }
        else{
          otros="";
          contParent = 0;



          while(j < tokens.size()){
            if(tokens[j] == "("){
              contParent++;
            }
            otros+= tokens[j] + " ";
            j++;
          }

          //otros+= tokens[j];
        }

        if(mapComps.find(std::pair<std::string, std::string>(pred, otros)) == mapComps.end()){
          auxVec.clear();
          auxVec.push_back(tokens[0]);
          if(otros == "NUMERO"){
            auxVec.push_back(tokens[tokens.size()-1]);
          }
          else{
            auxVec.push_back(otros);
          }
          auxVec.push_back(selectedPreds[i].second.second);

          auxMat.clear();
          auxMat.push_back(auxVec);

          mapComps[std::pair<std::string, std::string>(pred, otros)] = auxMat;
        }else{
          auxVec.clear();
          auxVec.push_back(tokens[0]);
          if(otros == "NUMERO"){
            auxVec.push_back(tokens[tokens.size()-1]);
          }
          else{
            auxVec.push_back(otros);
          }
          auxVec.push_back(selectedPreds[i].second.second);

          mapComps[std::pair<std::string, std::string>(pred, otros)].push_back(auxVec);
        }
      }
      else{
        if((selectedPreds[i].second.second == "True") or (selectedPreds[i].second.second == "False")){
          rule.push_back(std::pair<std::string, std::string> (selectedPreds[i].second.first, selectedPreds[i].second.second));
        }
      }
    }

    // std::cout << "COMPARACIONES ANTES DEL FILTRADO\n\n";
    //
    // for(auto it = mapComps.begin(); it != mapComps.end(); it++){
    //   std::cout << (*it).first.first << " " << (*it).first.second << std::endl;
    //   for(unsigned int i = 0; i < (*it).second.size(); i++){
    //     std::cout << "\t[" << (*it).second[i][0] << "|" << (*it).second[i][1] << "] = " << (*it).second[i][2] << std::endl;
    //   }
    // }
    //
    // std::cout << "\n\n\n\n\n\n";


    bool eq = false, lt = false, gt = false;

    std::map <double, std::vector <bool> > mapBools;
    std::vector<bool> vectBools;
    std::map<std::pair<std::string, std::string>, std::vector <std::vector <std::string> > > mapCompsFiltrado;

    for(auto it = mapComps.begin(); it != mapComps.end(); it++){

      //std::cout << (*it).first.first << " " << (*it).first.second << std::endl;

      if(mapCompsFiltrado.find(std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)) == mapCompsFiltrado.end()){
        auxMat.clear();
        mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)] = auxMat;
      }

      if((*it).first.second == "NUMERO"){
        mapBools.clear();
        for(unsigned int i = 0; i < (*it).second.size(); i++){
          if(mapBools.find(std::stod((*it).second[i][1])) == mapBools.end()){
            vectBools.clear();
            vectBools.push_back(false);
            vectBools.push_back(false);
            vectBools.push_back(false);

            mapBools[std::stoi((*it).second[i][1])] = vectBools;
          }

          if((*it).second[i][0] == "opEQ" and ((*it).second[i][2] == "True")){
            mapBools[std::stoi((*it).second[i][1])][0] = true;
          }

          if((*it).second[i][0] == "opGT" and ((*it).second[i][2] == "True")){
            mapBools[std::stoi((*it).second[i][1])][1] = true;
          }

          if((*it).second[i][0] == "opLT" and ((*it).second[i][2] == "True")){
            mapBools[std::stoi((*it).second[i][1])][2] = true;
          }

        }

        for(auto boolIt = mapBools.begin(); boolIt != mapBools.end(); boolIt++){
          pred = std::to_string((*boolIt).first);
          if((*boolIt).second[0] == true){
            if((*boolIt).second[1] == true){
              if((*boolIt).second[2] == true){
                // 111
                  //std::cout << "\t[- | " + pred + "] = ---" << std::endl;
                  //auxVec.clear();
                  //auxVec.push_back("-");
                  //auxVec.push_back("---");
                  //mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
              }
              else{
                // 110
                  //std::cout << "\t[opGET | " + pred + "] = true" << std::endl;
                  auxVec.clear();
                  auxVec.push_back("opGET");
                  auxVec.push_back(pred);
                  auxVec.push_back("true");
                  mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
              }
            }
            else{
              if((*boolIt).second[2] == true){
                //101
                  //std::cout << "\t[opLET | " + pred + "] = true" << std::endl;
                  auxVec.clear();
                  auxVec.push_back("opLET");
                  auxVec.push_back(pred);
                  auxVec.push_back("true");
                  mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
              }
              else{
                //100
                  //std::cout << "\t[opEQ | " + pred + "] = true" << std::endl;
                  auxVec.clear();
                  auxVec.push_back("opEQ");
                  auxVec.push_back(pred);
                  auxVec.push_back("true");
                  mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
              }
            }
          }
          else{
            if((*boolIt).second[1] == true){
              if((*boolIt).second[2] == true){
                //011
                  //std::cout << "\t[opEQ | " + pred + "] = false" << std::endl;
                  auxVec.clear();
                  auxVec.push_back("opEQ");
                  auxVec.push_back(pred);
                  auxVec.push_back("false");
                  mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
              }
              else{
                //010
                //std::cout << "\t[opGT | " + pred + "] = true" << std::endl;
                auxVec.clear();
                auxVec.push_back("opGT");
                auxVec.push_back(pred);
                auxVec.push_back("true");
                mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
              }
            }
            else{
              if((*boolIt).second[2] == true){
                //001
                //std::cout << "\t[opLT | " + pred + "] = true" << std::endl;
                auxVec.clear();
                auxVec.push_back("opLT");
                auxVec.push_back(pred);
                auxVec.push_back("true");
                mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
              }
              else{
                //000
                //std::cout << "\t[- | " + pred + "] = ---" << std::endl;
                //auxVec.clear();
                //auxVec.push_back("-");
                //auxVec.push_back("---");
                //mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
              }
            }
          }
        }
      }
      else{
        for(unsigned int i = 0; i < (*it).second.size(); i++){
          eq = false;
          lt = false;
          gt = false;

          if((*it).second[i][0] == "opEQ" and ((*it).second[i][2] == "True")){
            eq = true;
          }

          if((*it).second[i][0] == "opLT" and ((*it).second[i][2] == "True")){
            lt = true;
          }

          if((*it).second[i][0] == "opGT" and ((*it).second[i][2] == "True")){
            gt = true;
          }
        }

        //std::cout << (*it).first.first << " " << (*it).first.second << " [" << eq << gt << lt << "]" << std::endl;

        pred = (*it).first.second;

        if(eq == true){
          if(gt == true){
            if(lt == true){
              // 111
                //std::cout << "\t[- | " + pred + "] = ---" << std::endl;
                /*auxVec.clear();
                auxVec.push_back("-");
                auxVec.push_back("---");
                mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);*/
            }else{
              // 110
                //std::cout << "\t[opGET | " + pred + "] = true" << std::endl;
                auxVec.clear();
                auxVec.push_back("opGET");
                auxVec.push_back(pred);
                auxVec.push_back("true");
                mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
            }
          }else{
            if(lt == true){
              //101
                //std::cout << "\t[opLET | " + pred + "] = true" << std::endl;
                auxVec.clear();
                auxVec.push_back("opLET");
                auxVec.push_back(pred);
                auxVec.push_back("true");
                mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
            }else{
              //100
                //std::cout << "\t[opEQ | " + pred + "] = true" << std::endl;
                auxVec.clear();
                auxVec.push_back("opEQ");
                auxVec.push_back(pred);
                auxVec.push_back("true");
                mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
            }
          }
        }
        else{
          if(gt == true){
            if(lt == true){
              //011
                //std::cout << "\t[opEQ | " + pred + "] = false" << std::endl;
                auxVec.clear();
                auxVec.push_back("opEQ");
                auxVec.push_back(pred);
                auxVec.push_back("false");
                mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
            }
            else{
              //010
              //std::cout << "\t[opGT | " + pred + "] = true" << std::endl;
              auxVec.clear();
              auxVec.push_back("opGT");
              auxVec.push_back(pred);
              auxVec.push_back("true");
              mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
            }
          }
          else{
            if(lt == true){
              //001
              //std::cout << "\t[opLT | " + pred + "] = true" << std::endl;
              auxVec.clear();
              auxVec.push_back("opLT");
              auxVec.push_back(pred);
              auxVec.push_back("true");
              mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
            }
            else{
              //000
              //std::cout << "\t[- | " + pred + "] = ---" << std::endl;
              //auxVec.clear();
              //auxVec.push_back("-");
              //auxVec.push_back("---");
              //mapCompsFiltrado[std::pair<std::string, std::string>((*it).first.first,  (*it).first.second)].push_back(auxVec);
            }
          }
        }
      }
    }

    auto auxMapComps = mapCompsFiltrado;
    for(auto it = auxMapComps.begin(); it != auxMapComps.end(); it++){
      if((*it).second.size() == 0){
        mapCompsFiltrado.erase(std::pair<std::string, std::string>((*it).first.first,  (*it).first.second));
      }
    }

    auxMapComps.clear();

    // std::cout << "COMPARACIONES DESPUES DEL FILTRADO\n\n";
    // for(auto it = mapCompsFiltrado.begin(); it != mapCompsFiltrado.end(); it++){
    //   std::cout << (*it).first.first << " " << (*it).first.second << std::endl;
    //   for(unsigned int i = 0; i < (*it).second.size(); i++){
    //     std::cout << "\t[" << (*it).second[i][0] << "|" << (*it).second[i][1] << "] = " << (*it).second[i][2] << std::endl;
    //   }
    // }
    //
    // std::cout << "\n\n\n\n\n\n";

    double inf =std::numeric_limits<double>::min(), sup = std::numeric_limits<double>::max();
    bool infOP = true, supOP = true;

    for(auto it = mapCompsFiltrado.begin(); it != mapCompsFiltrado.end(); it++){
      if((*it).first.second == "NUMERO"){
        inf = std::numeric_limits<double>::min();
        sup = std::numeric_limits<double>::max();
        infOP = true;
        supOP = true;

        for(unsigned int i = 0; i < (*it).second.size(); i++){
          if((*it).second[i][0] == "opGET"){
            //[x,x]
            if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (inf == sup) and (infOP == false) and (supOP == false))){
              //Si y <= x --> [y,inf)
              if(std::stod((*it).second[i][1]) <= inf){
                inf = std::stod((*it).second[i][1]);
                sup = std::numeric_limits<double>::max();
                infOP = false;
                supOP = true;
              }
              //[inf, -inf]
              else{
                sup = std::numeric_limits<double>::min();
                inf = std::numeric_limits<double>::max();
                infOP = false;
                supOP = false;
              }
            }

            //[x,inf)
            if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == false) and (supOP == true))){
              //Si y < x --> [y,inf)
              if(std::stod((*it).second[i][1]) < inf){
                inf = std::stod((*it).second[i][1]);
                infOP = false;
              }
            }

            //(x,inf)
            if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
              //Si y < x --> [y,inf)
              if(std::stod((*it).second[i][1]) < inf){
                inf = std::stod((*it).second[i][1]);
                infOP = false;
              }
            }

            //(-inf,x]
            if(((inf == std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == false))){
              //Si y < x --> [y,x]
              if(std::stod((*it).second[i][1]) < sup){
                inf = std::stod((*it).second[i][1]);
                infOP = false;
              }
              //[inf, -inf]
              else{
                sup = std::numeric_limits<double>::min();
                inf = std::numeric_limits<double>::max();
                infOP = false;
                supOP = false;
              }
            }

            //(-inf,x)
            if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
              //Si y < x --> [y,x)
              if(std::stod((*it).second[i][1]) < inf){
                inf = std::stod((*it).second[i][1]);
                infOP = false;
              }
              //[inf, -inf]
              else{
                sup = std::numeric_limits<double>::min();
                inf = std::numeric_limits<double>::max();
                infOP = false;
                supOP = false;
              }
            }

            //(-inf,inf)
            if(((inf == std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
              inf = std::stod((*it).second[i][1]);
              infOP = false;
            }
          }
          else{
            if((*it).second[i][0] == "opGT"){
              //[x,x]
              if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (inf == sup) and (infOP == false) and (supOP == false))){
                //Si y <= x --> (y,inf)
                if(std::stod((*it).second[i][1]) <= inf){
                  inf = std::stod((*it).second[i][1]);
                  sup = std::numeric_limits<double>::max();
                  infOP = true;
                  supOP = true;
                }
                //[inf, -inf]
                else{
                  sup = std::numeric_limits<double>::min();
                  inf = std::numeric_limits<double>::max();
                  infOP = false;
                  supOP = false;
                }
              }

              //[x,inf)
              if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == false) and (supOP == true))){
                //Si y < x --> (y,inf)
                if(std::stod((*it).second[i][1]) < inf){
                  inf = std::stod((*it).second[i][1]);
                  infOP = true;
                }
              }

              //(x,inf)
              if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                //Si y < x --> (y,inf)
                if(std::stod((*it).second[i][1]) < inf){
                  inf = std::stod((*it).second[i][1]);
                  infOP = true;
                }
              }

              //(-inf,x]
              if(((inf == std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == false))){
                //Si y < x --> (y,x]
                if(std::stod((*it).second[i][1]) < sup){
                  inf = std::stod((*it).second[i][1]);
                  infOP = true;
                }
                //[inf, -inf]
                else{
                  sup = std::numeric_limits<double>::min();
                  inf = std::numeric_limits<double>::max();
                  infOP = false;
                  supOP = false;
                }
              }

              //(-inf,x)
              if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                //Si y < x --> (y,x)
                if(std::stod((*it).second[i][1]) < inf){
                  inf = std::stod((*it).second[i][1]);
                  infOP = true;
                  supOP = true;
                }
                //[inf, -inf]
                else{
                  sup = std::numeric_limits<double>::min();
                  inf = std::numeric_limits<double>::max();
                  infOP = false;
                  supOP = false;
                }
              }

              //(-inf,inf)
              if(((inf == std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                inf = std::stod((*it).second[i][1]);
                infOP = true;
              }
            }
            else{
              if((*it).second[i][0] == "opLET"){
                //[x,x]
                if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (inf == sup) and (infOP == false) and (supOP == false))){
                  //Si y >= x --> (-inf,y]
                  if(std::stod((*it).second[i][1]) >= inf){
                    sup = std::stod((*it).second[i][1]);
                    supOP = false;
                    inf = std::numeric_limits<double>::min();
                    infOP = true;
                  }
                  //[inf, -inf]
                  else{
                    sup = std::numeric_limits<double>::min();
                    inf = std::numeric_limits<double>::max();
                    infOP = false;
                    supOP = false;
                  }
                }

                //[x,inf)
                if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == false) and (supOP == true))){
                  //Si y > x --> [x,y]
                  if(std::stod((*it).second[i][1]) > inf){
                    sup = std::stod((*it).second[i][1]);
                    supOP = false;
                  }
                  else{
                    sup = std::numeric_limits<double>::min();
                    inf = std::numeric_limits<double>::max();
                    infOP = false;
                    supOP = false;
                  }
                }

                //(x,inf)
                if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                  //Si y > x --> (x,y]
                  if(std::stod((*it).second[i][1]) > inf){
                    sup = std::stod((*it).second[i][1]);
                    supOP = false;
                  }
                  else{
                    sup = std::numeric_limits<double>::min();
                    inf = std::numeric_limits<double>::max();
                    infOP = false;
                    supOP = false;
                  }
                }

                //(-inf,x]
                if(((inf == std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == false))){
                  //Si y > x --> (-inf,y]
                  if(std::stod((*it).second[i][1]) > sup){
                    sup = std::stod((*it).second[i][1]);
                    supOP = false;
                  }
                }

                //(-inf,x)
                if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                  //Si y > x --> (-inf,y]
                  if(std::stod((*it).second[i][1]) > sup){
                    sup = std::stod((*it).second[i][1]);
                    supOP = false;
                  }
                }

                //(-inf,inf)
                if(((inf == std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                  sup = std::stod((*it).second[i][1]);
                  supOP = false;
                }

              }
              else{
                if((*it).second[i][0] == "opLT"){
                  //[x,x]
                  if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (inf == sup) and (infOP == false) and (supOP == false))){
                    //Si y > x --> (-inf,y)
                    if(std::stod((*it).second[i][1]) > inf){
                      sup = std::stod((*it).second[i][1]);
                      inf = std::numeric_limits<double>::min();
                      infOP = true;
                      supOP = true;
                    }
                    //[inf, -inf]
                    else{
                      sup = std::numeric_limits<double>::min();
                      inf = std::numeric_limits<double>::max();
                      infOP = false;
                      supOP = false;
                    }
                  }

                  //[x,inf)
                  if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == false) and (supOP == true))){
                    //Si y > x --> [x,y)
                    if(std::stod((*it).second[i][1]) > inf){
                      sup = std::stod((*it).second[i][1]);
                      supOP = true;
                    }
                    else{
                      sup = std::numeric_limits<double>::min();
                      inf = std::numeric_limits<double>::max();
                      infOP = false;
                      supOP = false;
                    }
                  }

                  //(x,inf)
                  if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                    //Si y > x --> (x,y)
                    if(std::stod((*it).second[i][1]) > inf){
                      sup = std::stod((*it).second[i][1]);
                      supOP = true;
                    }
                    else{
                      sup = std::numeric_limits<double>::min();
                      inf = std::numeric_limits<double>::max();
                      infOP = false;
                      supOP = false;
                    }
                  }

                  //(-inf,x]
                  if(((inf == std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == false))){
                    //Si y > x --> (-inf,y)
                    if(std::stod((*it).second[i][1]) > sup){
                      sup = std::stod((*it).second[i][1]);
                      supOP = true;
                    }
                  }

                  //(-inf,x)
                  if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                    //Si y > x --> (-inf,y)
                    if(std::stod((*it).second[i][1]) > sup){
                      sup = std::stod((*it).second[i][1]);
                      supOP = true;
                    }
                  }

                  //(-inf,inf)
                  if(((inf == std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                    sup = std::stod((*it).second[i][1]);
                    supOP = true;
                  }
                }
                else{
                  //[x,x]
                  if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (inf == sup) and (infOP == false) and (supOP == false))){
                    //[-inf,inf]
                    if(std::stod((*it).second[i][1]) > inf){
                      sup = std::numeric_limits<double>::min();
                      inf = std::numeric_limits<double>::max();
                      infOP = false;
                      supOP = false;

                    }
                  }

                  //[x,inf)
                  if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == false) and (supOP == true))){
                    //Si y > x --> [x,inf)
                    if(std::stod((*it).second[i][1]) < inf){
                      sup = std::numeric_limits<double>::min();
                      inf = std::numeric_limits<double>::max();
                      infOP = false;
                      supOP = false;
                    }
                  }

                  //(x,inf)
                  if(((inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                    //Si y > x --> [x,inf)
                    if(std::stod((*it).second[i][1]) < inf){
                      sup = std::numeric_limits<double>::min();
                      inf = std::numeric_limits<double>::max();
                      infOP = false;
                      supOP = false;
                    }
                  }

                  //(-inf,x]
                  if(((inf == std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == false))){
                    //Si y < x --> (-inf,x]
                    if(std::stod((*it).second[i][1]) > sup){
                      sup = std::numeric_limits<double>::min();
                      inf = std::numeric_limits<double>::max();
                      infOP = false;
                      supOP = false;
                    }
                  }

                  //(-inf,x)
                  if(((inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                    //Si y < x --> (-inf,x)
                    if(std::stod((*it).second[i][1]) > sup){
                      sup = std::numeric_limits<double>::min();
                      inf = std::numeric_limits<double>::max();
                      infOP = false;
                      supOP = false;
                    }
                  }

                  //(-inf,inf)
                  if(((inf == std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max()) and (infOP == true) and (supOP == true))){
                    inf = std::stod((*it).second[i][1]);
                    infOP = false;
                    sup = std::stod((*it).second[i][1]);
                    supOP = false;
                  }

                }
              }
            }
          }
        }

        // [a,a]
        if((inf == sup) and (infOP == false) and (supOP == false)){
            rule.push_back(std::pair<std::string, std::string> ( "opEQ - " + (*it).first.first + " " + std::to_string(sup), "True"));
        }

        // [a,b]
        if((inf != sup) and (infOP == false) and (supOP == false) and (inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max())){
          rule.push_back(std::pair<std::string, std::string> ( "opLET - " + (*it).first.first + " " + std::to_string(sup), "True"));
          rule.push_back(std::pair<std::string, std::string> ( "opGET - " + (*it).first.first + " " + std::to_string(inf), "True"));
        }
        // (a,b)
        if((inf != sup) and (infOP == true) and (supOP == true) and (inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max())){
          rule.push_back(std::pair<std::string, std::string> ( "opLT - " + (*it).first.first + " " + std::to_string(sup), "True"));
          rule.push_back(std::pair<std::string, std::string> ( "opGT - " + (*it).first.first + " " + std::to_string(inf), "True"));
        }
        // [a,b)
        if((inf != sup) and (infOP == false) and (supOP == true) and (inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max())){
          rule.push_back(std::pair<std::string, std::string> ( "opLT - " + (*it).first.first + " " + std::to_string(sup), "True"));
          rule.push_back(std::pair<std::string, std::string> ( "opGET - " + (*it).first.first + " " + std::to_string(inf), "True"));
        }
        // (a,b]
        if((inf != sup) and (infOP == true) and (supOP == false) and (inf != std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max())){
          rule.push_back(std::pair<std::string, std::string> ( "opLET - " + (*it).first.first + " " + std::to_string(sup), "True"));
          rule.push_back(std::pair<std::string, std::string> ( "opGT - " + (*it).first.first + " " + std::to_string(inf), "True"));
        }

        // [a,inf)
        if((inf != sup) and (infOP == false) and (supOP == true) and (inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max())){
          rule.push_back(std::pair<std::string, std::string> ( "opGET - " + (*it).first.first + " " + std::to_string(inf), "True"));
        }
        // (a,inf)
        if((inf != sup) and (infOP == true) and (supOP == true) and (inf != std::numeric_limits<double>::min()) and (sup == std::numeric_limits<double>::max())){
          rule.push_back(std::pair<std::string, std::string> ( "opGT - " + (*it).first.first + " " + std::to_string(inf), "True"));
        }

        // (-inf, a]
        if((inf != sup) and (infOP == true) and (supOP == false) and (inf == std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max())){
          rule.push_back(std::pair<std::string, std::string> ( "opLET - " + (*it).first.first + " " + std::to_string(sup), "True"));
        }
        // (-inf, a)
        if((inf != sup) and (infOP == true) and (supOP == true) and (inf == std::numeric_limits<double>::min()) and (sup != std::numeric_limits<double>::max())){
          rule.push_back(std::pair<std::string, std::string> ( "opLT - " + (*it).first.first + " " + std::to_string(sup), "True"));
        }
      }
      else{
        rule.push_back(std::pair<std::string, std::string> ( (*it).second[0][0] + " - " + (*it).first.first + " " +( *it).second[0][1], "True"));
      }
    }


    /*std::cout << "COMPARACIONES DESPUES DEL FILTRADO\n\n";

    for(unsigned int i = 0; i < rule.size(); i++){
      std::cout << rule[i].first << " = " << rule[i].second << std::endl;
    }

    std::cout << "\n\n\n\n\n\n"; */


    std::cout << "\nRegla Preestado calculada:\n";
    for(unsigned int i = 0; i < rule.size(); i++){
      std::cout << rule[i].first << " = " << rule[i].second << std::endl;
    }



    reglasOut -> push_back(std::list < std::pair< std::string, std::string> > (rule.begin(), rule.end()));



    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    (*reglasOut)[0] = clearDeltas((*reglasOut)[0], attribLabels);
    (*reglasOut)[0] = borrarRedundancias((*reglasOut)[0]);
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    std::cout << "\n\nPredicados seleccionados para el postestado:" <<std::endl;
    for(unsigned int i = 0; i < postVec.size(); i++ ){
        std::cout <<"[" << postVec[i].first <<"] " << postVec[i].second.first << " = " << postVec[i].second.second << std::endl;
    }

    rule.clear();
    selectedPreds.clear();
    elemVistos.clear();
    elemVistos.resize(postVec.size(), false);


    for(unsigned int i = 0; i < postVec.size(); i++){
      if(elemVistos[i] == false){
        elemVistos[i] = true;
        conflicto = -1;
        for(unsigned int j = 0; j < postVec.size(); j++ ){
          if(elemVistos[j] == false){
            if(postVec[i].second.first == postVec[j].second.first){
              conflicto = j;
              elemVistos[j] = true;
            }
          }
        }

        if(conflicto != -1){
          w1 = postVec[i].first;
          w2 = postVec[conflicto].first;

          if((w1 > w2) and (((double)w1/(double)(w1+w2)) > 0.05)){
            //i gana -> se incluye en la regla
            selectedPreds.push_back(postVec[i]);
          }
          else{
            if((w2 > w1) and (((double)w2/(double)(w1+w2)) > 0.05)){
              //j gana -> se incluye en la regla
              selectedPreds.push_back(postVec[i]);
            }
            else{
              //Empate a tomar por culo las dos
            }
          }
        }
        else{
          //No hay conflicto -> se incluye en la regla el elemento
          selectedPreds.push_back(postVec[i]);
        }
      }
    }


    mapComps.clear();

    for(unsigned int i = 0; i < selectedPreds.size(); i++ ){
      std::stringstream ss(selectedPreds[i].second.first);
      tokens.clear();
      while (ss >> buf){
        tokens.push_back(buf);
      }

      auxVec.clear();
      if((selectedPreds[i].second.second == "True") or (selectedPreds[i].second.second == "False")){
        if(tokens[0] != "opEQ" and tokens[0] != "opLT" and tokens[0] != "opGT"){
          rule.push_back(std::pair<std::string, std::string> (selectedPreds[i].second.first, selectedPreds[i].second.second));
        }
      }
      else{
          if(tokens[0] == "DELTA"){
            if(isnumber(tokens[tokens.size()-1])){
              if(std::stod(tokens[tokens.size()-1]) != 0.0){
                //rule.push_back(std::pair<std::string, std::string> (selectedPreds[i].second.first, selectedPreds[i].second.second));
                auxVec.clear();
                auxVec.push_back("DELTA");
                auxVec.push_back("-");

                //auxVec.push_back(tokens[tokens.size()-1]);
                auxVec.push_back(std::string(selectedPreds[i].second.second));

                pred = "";
                for (unsigned int tk = 2; tk < tokens.size(); tk++) {
                  pred += tokens[tk];
                  pred += " ";
                }

                mapComps[std::pair<std::string, std::string>(pred, "NUMERO")].push_back(auxVec);
              }
            }
            else{
              rule.push_back(std::pair<std::string, std::string> (selectedPreds[i].second.first, selectedPreds[i].second.second));
            }
          }
          else{
            if((selectedPreds[i].second.second != "True") and (selectedPreds[i].second.second != "False")){
              //rule.push_back(std::pair<std::string, std::string> ("opEQ - " + selectedPreds[i].second.first + selectedPreds[i].second.second, selectedPreds[i].second.second));
              //mapComps[std::pair<std::string, std::string>(selectedPreds[i].second.first, "NUMERO")].push_back("opEQ - " + selectedPreds[i].second.second);

              auxVec.clear();
              auxVec.push_back("opEQ");
              auxVec.push_back("-");
              auxVec.push_back(selectedPreds[i].second.second);

              pred = "";
              for (unsigned int tk = 0; tk < tokens.size() - 1; tk++) {
                pred += tokens[tk];
                pred += " ";
              }
              pred += tokens[tokens.size() - 1];

              mapComps[std::pair<std::string, std::string>(pred, "NUMERO")].push_back(auxVec);
            }
          }
      }
    }


    // for(auto it = mapComps.begin(); it != mapComps.end(); it++){
    //    std::cout << (*it).first.first << " " << (*it).first.second << std::endl;
    //   for(unsigned int i = 0; i < (*it).second.size(); i++){
    //     std::cout << "\t[" << (*it).second[i][0] << "|" << (*it).second[i][1] << "] = " << (*it).second[i][2] << std::endl;
    //   }
    // }


    bool assign = true;
    unsigned int val1 = 0, val2 = 0;
    for(auto it = mapComps.begin(); it != mapComps.end(); it++){
      assign = true;
      for(unsigned int i = 0; i < (*it).second.size(); i++){
        if((*it).second[i][0] == "opEQ"){
          val2 = i;
        }
        else{
          val1 = i;
          assign = false;
        }
      }

      if(assign == false){
//        std::cout << "En " +  (*it).first.first + "hay un ASSIGN\n";
        rule.push_back(std::pair<std::string, std::string> ( "DELTA - " + (*it).first.first, (*it).second[val1][2]));
      }
      else{
//        std::cout << "En " +  (*it).first.first + "NO hay un ASSIGN\n";
        rule.push_back(std::pair<std::string, std::string> ( "opEQ " + (*it).first.first + " " + (*it).second[val2][2], (*it).second[val2][2]));
      }
    }

    std::vector<std::string> tokens2;

    std::string delta, opecuses;
    for(unsigned int i = 0; i < rule.size(); i++){
      //Buscamos los Delta.
      std::stringstream ss(rule[i].first);
      tokens.clear();
      while (ss >> buf){
        tokens.push_back(buf);
      }

      if(tokens[0] == "DELTA"){
        delta = "";
        for (size_t j = 2; tokens[j] != ")" and j < tokens.size(); j++) {
          delta += tokens[j];
        }
        delta += ")";
        //Si hay opEQ que iguale al delta se marca.

        for(unsigned int j = 0; j < rule.size() ; j++){
          std::stringstream ss(rule[j].first);
          tokens2.clear();
          while (ss >> buf){
            tokens2.push_back(buf);
          }


          if(tokens2[0] == "opEQ"){
            opecuses = "";
            for (size_t k = 1; tokens2[k] != ")" and k < tokens2.size(); k++) {
              opecuses += tokens2[k];
            }
            opecuses += ")";

            if(delta == opecuses){
              rule[j].second = "ELIMINAR";
            }

          }
        }
      }
    }



    std::cout << "\nRegla Postestado calculada:\n";
    for(unsigned int i = 0; i < rule.size(); i++){
      std::cout << rule[i].first << " = " << rule[i].second << std::endl;
    }


    //Eliminar los elementos repes de reglasOut
    reglasOut -> push_back(std::list < std::pair< std::string, std::string> > ());

    //Recorremos la regla del postestado
    for(auto antPost = rule.begin(); antPost != rule.end(); antPost++){
      esta = false;
      //Recorremos la regla del preestado
      for(auto antPre = (*reglasOut)[0].begin(); antPre != (*reglasOut)[0].end(); antPre++){
        //Si el elemento esta en el preestado se marca para no incluirlo

        if(((*antPost).first == (*antPre).first) and ((*antPost).second == (*antPre).second)){
          esta = true;
        }

      }

      //Si no esta marcado, se incluye
      if(esta == false and (*antPost).second != "ELIMINAR"){
        (*reglasOut)[1].push_back(std::pair< std::string, std::string> ((*antPost).first, (*antPost).second));
      }
    }

    //reglasOut -> push_back(std::list < std::pair< std::string, std::string> > (rule.begin(), rule.end()));


    std::cout << "#########################################################################" << std::endl;
  }
