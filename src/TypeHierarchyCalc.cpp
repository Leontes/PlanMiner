#include "TypeHierarchyCalc.hpp"

#include <sstream>
#include <utility>


template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> extract_keys(std::map <std::string, std::vector <std::pair <std::string, std::string> > > const& input_map) {
  std::vector<std::string> retval;
  for (auto const& element : input_map) {
    retval.push_back(element.first);
  }
  return retval;
}

std::string solveConflict(std::string A, std::string B, std::vector < Type * > *vtipos){
  std::string out;

  Type * auxA , * auxB, * pivote;
  for (unsigned int i = 0; i < vtipos -> size(); i++) {
    if(A == (*vtipos)[i] -> name){
      auxA = (*vtipos)[i];
    }

    if(B == (*vtipos)[i] -> name){
      auxB = (*vtipos)[i];
      pivote = auxB;
    }

  }
  while (auxA != 0){
    auxB = pivote;
    while (auxB != 0){
      if(auxA -> name == auxB -> name){
        return auxA -> name;
      }
      auxB = auxB -> parent;
    }
    auxA = auxA -> parent;
  }
  return out;
}



std::vector < Type *> extractTypeHierarchy(std::vector< PlanTrace * > * PTS, std::map <std::string, std::vector <std::pair <std::string, std::string> > > * mapaPrimitivasParam){
  std::vector < Type * > vectorAux;
  std::string tName;
  std::vector<Parameter*> parametros;



  std::map <std::string, std::vector <std::pair <std::string, std::string> > > mapaPrimitivasParamAUX;

  //Tomamos 1 plan
  for(unsigned int i = 0; i < PTS -> size(); i++){
    //Tomamos un TS
    for (unsigned int j = 0; j < (*PTS)[i] -> getNTS(); j++) {
      //Tomamos el nombre de la tarea y sus parametros
      tName = (*PTS)[i] -> getTS(j) -> getTask() -> get_Tname();
      parametros = (*PTS)[i] -> getTS(j) -> getTask() -> getParameters();
      //Creamos un mapa con todos los tipos distintos para los
      for(unsigned int k = 0; k < parametros.size(); k++){
        mapaPrimitivasParamAUX[tName].push_back(std::pair <std::string, std::string> (parametros[k] -> getName(), parametros[k] -> getTypes()[0]) );
      }
    }
  }


  //Limpiamos de elementos repetidos
  std::vector<std::string> keys = extract_keys(mapaPrimitivasParamAUX);
  bool encontrado = false;
  for(unsigned int i = 0; i < keys.size(); i++){
    for(unsigned int j = 0; j < mapaPrimitivasParamAUX[keys[i]].size(); j++){
      encontrado = false;
      for(unsigned int k = 0; k < (*mapaPrimitivasParam)[keys[i]].size(); k++){
        if((mapaPrimitivasParamAUX[keys[i]][j].first == (*mapaPrimitivasParam)[keys[i]][k].first)
              and (mapaPrimitivasParamAUX[keys[i]][j].second == (*mapaPrimitivasParam)[keys[i]][k].second)){
          encontrado = true;
        }
      }
      if(encontrado == false){
        (*mapaPrimitivasParam)[keys[i]].push_back(std::pair <std::string, std::string> (mapaPrimitivasParamAUX[keys[i]][j].first, mapaPrimitivasParamAUX[keys[i]][j].second));
      }

    }
  }

  std::vector <std::string> tiposDistintos;
  for(unsigned int i = 0; i < keys.size(); i++){
    for(unsigned int j = 0; j < (*mapaPrimitivasParam)[keys[i]].size(); j++){
      encontrado = false;
      for(unsigned int k = 0; k < tiposDistintos.size(); k++){
        if(tiposDistintos[k] ==  (*mapaPrimitivasParam)[keys[i]][j].second){
          encontrado = true;
        }
      }

      if(encontrado == false){
        tiposDistintos.push_back((*mapaPrimitivasParam)[keys[i]][j].second);
      }
    }
  }



  //Encontramos los conflictos entre parametros con el mismo nombre en la misma tarea evitando los conflictos repetidos
  std::vector <std::pair <std::string, std::string> > conflictos;

  for(unsigned int i = 0; i < keys.size(); i++){
    for(unsigned int j = 0; j < (*mapaPrimitivasParam)[keys[i]].size(); j++){
      for(unsigned int k = j; k < (*mapaPrimitivasParam)[keys[i]].size(); k++){
        if(((*mapaPrimitivasParam)[keys[i]][j].first == (*mapaPrimitivasParam)[keys[i]][k].first) and ((*mapaPrimitivasParam)[keys[i]][j].second != (*mapaPrimitivasParam)[keys[i]][k].second)){
          encontrado = false;
          for(unsigned int l = 0; l < conflictos.size(); l++){
            if( ((conflictos[l].first ==  (*mapaPrimitivasParam)[keys[i]][j].second) and (conflictos[l].second == (*mapaPrimitivasParam)[keys[i]][k].second)) or ((conflictos[l].second ==  (*mapaPrimitivasParam)[keys[i]][j].second) and (conflictos[l].first == (*mapaPrimitivasParam)[keys[i]][k].second)) ){
              encontrado = true;
            }
          }
          if(encontrado == false){
            conflictos.push_back(std::pair <std::string, std::string> ((*mapaPrimitivasParam)[keys[i]][j].second, (*mapaPrimitivasParam)[keys[i]][k].second));
          }
        }
      }
    }
  }



  std::cout << "Types found:\n";
  for(unsigned int i = 0; i < tiposDistintos.size(); i++){
    std::cout << tiposDistintos[i] << " " << std::endl;
    vectorAux.push_back(new Type(tiposDistintos[i]));
  }
  std::cout << std::endl;


  for(unsigned int i = 0; i < conflictos.size(); i++){
    vectorAux.push_back(new Type("PARENTTYPE" + std::to_string(i)));
    for(unsigned int j = 0; j < vectorAux.size(); j++){
      if(conflictos[i].first == vectorAux[j] -> name){
        vectorAux[j] -> parent = vectorAux[vectorAux.size()-1];
      }
      if(conflictos[i].second == vectorAux[j] -> name){
        vectorAux[j] -> parent = vectorAux[vectorAux.size()-1];
      }
    }
  }





  //Recalculamos las cabeceras con los conflictos entre parametros resueltos
  mapaPrimitivasParamAUX = *mapaPrimitivasParam;
  mapaPrimitivasParam -> clear();

  for(unsigned int i = 0; i < conflictos.size(); i++){
    std::cout << "Conflict found between the type: " << conflictos[i].first << " and " << conflictos[i].second << std::endl;
    std::cout << "\t- New Parent type created: " << solveConflict(conflictos[i].first, conflictos[i].second, &vectorAux) << std::endl;
  }



  bool parametroConflictivo = false, parametroYaIncluido = false;
  std::string resolucion;
  for(unsigned int i = 0; i < keys.size(); i++){
    for(unsigned int j = 0; j < mapaPrimitivasParamAUX[keys[i]].size(); j++){
      parametroConflictivo = false;
      for(unsigned int k = j; k < mapaPrimitivasParamAUX[keys[i]].size(); k++){
        if((mapaPrimitivasParamAUX[keys[i]][j].first == mapaPrimitivasParamAUX[keys[i]][k].first) and (mapaPrimitivasParamAUX[keys[i]][j].second != mapaPrimitivasParamAUX[keys[i]][k].second)){
          parametroConflictivo = true;
          resolucion = solveConflict(mapaPrimitivasParamAUX[keys[i]][j].second, mapaPrimitivasParamAUX[keys[i]][k].second, &vectorAux);
        }
      }

      parametroYaIncluido = false;
      for(unsigned int k = 0; k < (*mapaPrimitivasParam)[keys[i]].size(); k++){
        if(mapaPrimitivasParamAUX[keys[i]][j].first == (*mapaPrimitivasParam)[keys[i]][k].first){
          parametroYaIncluido = true;
        }
      }


      if(parametroConflictivo == true){

        if(parametroYaIncluido == false){
          (*mapaPrimitivasParam)[keys[i]].push_back(std::pair <std::string, std::string> (mapaPrimitivasParamAUX[keys[i]][j].first, resolucion));
        }
      }else{
        if(parametroYaIncluido == false){
          (*mapaPrimitivasParam)[keys[i]].push_back(std::pair <std::string, std::string> (mapaPrimitivasParamAUX[keys[i]][j].first, mapaPrimitivasParamAUX[keys[i]][j].second));
        }
      }

    }


  }



  std::cout << "New tasks headers created: \n";
  for(unsigned int i = 0; i < keys.size(); i++){
    std::cout << keys[i];
    for(unsigned int j = 0; j < (*mapaPrimitivasParam)[keys[i]].size(); j++){
      std::cout << " " << (*mapaPrimitivasParam)[keys[i]][j].first  << " - " << (*mapaPrimitivasParam)[keys[i]][j].second;
    }
    std::cout << std::endl;
  }



  return vectorAux;
}
