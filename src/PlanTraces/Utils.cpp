
#include <iostream>
#include <fstream>
#include <sstream>

#include <queue>




#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "PlanTraces/Utils.hpp"


//Auxiliary functions
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
bool is_number(const std::string& s){
  if(s[0] == '-' and s.size() == 1){
    return false;
  }
  else{
    return(s.find_first_not_of( "-.0123456789" ) == std::string::npos);
  }
}
bool is_var(const std::string& s){
  return std::isalpha(s[0]);
}
int precedencia(std::string str){
    if(str == "^"){
      return 4;
    }
    else{
      if(str == "*" || str == "/"){
        return 3;
      }
      else{
        if(str == "+" || str == "-"){
          return 2;
        }
        else{
          if(str == "("){
            return 1;
          }else{
            return -1;
          }
        }
      }
    }
  }
std::vector<std::string> infixAPrefix(std::vector<std::string> &formInf){

  //Paso 1: Se le da la vuelta a la formula
  std::vector<std::string> formAux, formOut;
  for(unsigned int i = formInf.size(); i > 0; i--){
    if(formInf[i-1] != ""){
      formAux.push_back(formInf[i-1]);
    }
  }

  //Paso 2: Se le da la vuelta a los parentesis
  for (unsigned int  i = 0; i < formAux.size(); i++) {
    if(formAux[i] == ")"){
      formAux[i] = "(";
    }
    else{
      if(formAux[i] == "("){
        formAux[i] = ")";
      }
    }
  }

  //Paso 3: Se calcula la forma postfija
  std::stack<std::string> pilaTokens;
  std::string token;
  for(unsigned int  i = 0; i < formAux.size(); i++){
    //Si es un token
    if(is_number(formAux[i]) or is_var(formAux[i])){
      formOut.push_back(formAux[i]);
    }else{
      //Si es (
      if(formAux[i] == "("){
        pilaTokens.push(formAux[i]);
      }
      else{
        //Si es )
        if(formAux[i] == ")"){
          token = pilaTokens.top();
          pilaTokens.pop();

          //Sacamos tokens hasta encontrar la pareja
          while(token != "("){
            formOut.push_back(token);
            token = pilaTokens.top();
            pilaTokens.pop();
          }
        }
        else{
          //Sacamos operadores de la pila y los metemos la formula en mientras la precedencia sea menor
          while (!pilaTokens.empty() and precedencia(pilaTokens.top()) >= precedencia(formAux[i])) {
            token = pilaTokens.top();
            pilaTokens.pop();
            formOut.push_back(token);
          }
          pilaTokens.push(formAux[i]);
        }
      }
    }
  }

  //Metemos el resto de operadores en la formula
  while (!pilaTokens.empty()){
    formOut.push_back(pilaTokens.top());
    pilaTokens.pop();
  }

  //Paso 4: Se le da la vuelta a la formula en forma postfija
  formAux = formOut;
  formOut.clear();
  for(unsigned int i = formAux.size(); i > 0; i--){
    formOut.push_back(formAux[i-1]);
  }

  return formOut;
}
bool estaenLista(Estado hijo, std::vector<Estado> &listaCerrados){
  for(unsigned int i = 0; i < listaCerrados.size(); i++){
    if(hijo == listaCerrados[i]){
      return true;
    }
  }
  return false;
}
bool vectorInutil(std::vector<double> vec){
  double elem = vec[0];
  for(unsigned int i = 1; i < vec.size(); i++){
    if(vec[i] != -999999999.0){
      if(vec[i] != elem){
        return true;
      }
    }
  }
  return false;
}
bool vectorDeZeros(std::vector<double> vec){
    for(unsigned int i = 0; i < vec.size(); i++){
      if(vec[i] != -999999999.0){
        if(vec[i] != 0.0){
          return true;
        }
      }
    }
    return false;
}
std::vector<std::string> extract_keys(std::map<std::string, bool> const& input_map) {
  std::vector<std::string> retval;
  for (auto const& element : input_map) {
    if(stod(element.first) != -999999999.0 ){
      retval.push_back(element.first);
    }
  }
  return retval;
}
std::string sustituirVars(std::string formula, std::vector <std::pair <std::string, std::string> > pairVars){
  std::string formulaOut;
  std::vector<std::string> tokens;
  std::string buf;

  std::stringstream ss(formula); // Insert the string into a stream

  while (ss >> buf){
    tokens.push_back(buf);
  }

  bool sustituto = false;

  for(unsigned int i = 0; i < tokens.size(); i++){
    sustituto = false;
    for(unsigned int j = 0; j < pairVars.size(); j++){
      if(tokens[i] == pairVars[j].first){
        sustituto = true;
        buf = pairVars[j].second;
      }
    }
    if(sustituto == true){
      formulaOut += buf + " ";
    }else{
      formulaOut += tokens[i] + " ";
    }
  }

  return formulaOut;
}
bool esDiff(std::vector <std::string> listadeDiffs, std::string str){
  for(unsigned int i = 0; i < listadeDiffs.size(); i++){
    if(listadeDiffs[i] == str){
      return true;
    }
  }
  return false;
}
std::string getEquiv(std::string pred, std::vector <std::pair <std::string, std::string> > vectEquiv){
  for (unsigned int i = 0; i < vectEquiv.size(); i++) {
    if("DELTA - " + pred == vectEquiv[i].first){
      return vectEquiv[i].second;
    }
  }
  return pred;
}
bool esPrecondicionable(std::string pred, std::vector < std::pair <std::string, std::vector <double> > > listadeDiffs){
  for(unsigned int i = 0; i < listadeDiffs.size(); i++){
    if (listadeDiffs[i].first == pred){
      return true;
    }
  }
  return false;
}
std::vector <Estado> generarListaInit(std::vector<std::string> *problemVars, std::vector<std::vector<double> > *problemData, std::vector<double> *problemObj){
  std::vector <Estado> out;
  //Constantes
  for(unsigned int i = 0; i < 11; i++){
    out.push_back(Estado(std::to_string(i), problemVars, problemData, problemObj));
  }
  out.push_back(Estado(std::to_string(-1), problemVars, problemData, problemObj));

  //Variables
  for(unsigned int i = 0; i < problemVars -> size(); i++){
    out.push_back(Estado((*problemVars)[i], problemVars, problemData, problemObj));
  }
  return out;

}




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

//Simbolic Regression A*-based algorithm
Estado Astar(std::vector <Estado> & estadosInit){
  std::priority_queue<Estado, std::vector<Estado>, std::greater<Estado> > listaAbiertos, vacia;

  std::vector<Estado> hijos;
  std::vector<Estado> listaCerrados;


  for(unsigned int i = 0; i < estadosInit.size(); i++){
    listaAbiertos.push(estadosInit[i]);
  }

  Estado estadoActual = listaAbiertos.top(), mejorEstado = listaAbiertos.top();
  /*while (!listaAbiertos.empty()) {
     estadoActual = listaAbiertos.top();
     listaAbiertos.pop();
     std::cout << estadoActual.to_string() << std::endl;
  }*/

  bool seguir = true;
  //std::cout << "\n\n--------------- Iniciando Busqueda ---------------\n";
  mejorEstado = listaAbiertos.top();
  if(mejorEstado.final()){
    seguir = false;
  }

  while(seguir and listaAbiertos.size() > 0){
    //Se saca un elemento de lista de abiertos
    estadoActual = listaAbiertos.top();
    listaAbiertos.pop();
    listaCerrados.push_back(estadoActual);

    /*std::cout << "*Estado actual: " << std::endl;
    std::cout << "\t"<< estadoActual.to_string() << std::endl;
    std::cout << "*Nodos abiertos: " << listaAbiertos.size() << std::endl;
    std::cout << "*Nodos vistos: " << listaCerrados.size() << std::endl;
    std::cout << "*Mejora respecto a inicio: " << estadoActual.getH()*100.0/tope << "%\n";*/

    if(estadoActual < mejorEstado){
      mejorEstado = estadoActual;
      //std::cout << "***Mejor solucion actualizada!!***" << std::endl;

      listaAbiertos = vacia;
      listaAbiertos.push(mejorEstado);

    }
    //std::cout << std::endl << std::endl;

    //Se comprueba que sea final
    if(mejorEstado.final() or listaCerrados.size() >= 2000){
    //if(mejorEstado.final() or listaCerrados.size() >= 2000){
      //Si lo es: terminar
      seguir = false;
      if(listaCerrados.size() >= 2000){
        mejorEstado.timeout();
      }
    }else{
      //Si no lo es: Generar hijos
      hijos = estadoActual.generarHijos();
      for (size_t i = 0; i < hijos.size(); i++) {
        //std::cout << hijos[i].to_string() << std::endl;
        //Si no esta en cerrados meter hijo en abiertos
        if(!estaenLista(hijos[i], listaCerrados)){
          listaAbiertos.push(hijos[i]);
        }
      }
    }
    //And repeat...
  }
  return mejorEstado;
}

void to_table( std::string task, StatesLists states, std::vector < std::vector < std::vector < double > > > * datasets, std::vector < std::vector < std::pair<std::string, std::string> > > * attribLabelsVC){

  std::vector < std::vector < double > > * dataset = new std::vector < std::vector < double > >();
  std::vector < std::pair<std::string, std::string> > * attribLabels = new std::vector < std::pair<std::string, std::string> >();

  std::cout << "\nAction: " << task << std::endl;

  unsigned int nStates = states.first.size() + states.second.size();
  unsigned int index = 0;

  //Recorremos los preestados y sacamos su información en forma de tabla
  for (std::list<State*>::iterator it = states.first.begin(); it != states.first.end(); it++) {
    //std::cout << "Pre: "  << index << std::endl;
      (*it) -> to_table(dataset, attribLabels, nStates, &index);
  }

  //Recorremos los postestados y sacamos su información en forma de tabla
  for (std::list<State*>::iterator it = states.second.begin(); it != states.second.end(); it++) {
    //std::cout << "Post: "  << index << std::endl;
      (*it) -> to_table(dataset, attribLabels, nStates, &index);
  }

  //Finalizamos incluyendo la info de las etiquetas de las clases
  std::vector<double> classVec(nStates, 1.0);
  for(unsigned int i = 0; i < states.first.size(); i++){
    classVec[i] = 0.0;
  }
  dataset -> push_back(classVec);
  attribLabels -> push_back(std::pair <std::string, std::string> ("Class", "Class"));




  //Separamos el dataset en dos datasets: Uno con la información logica y otro con la númerica
  std::vector < std::vector < double > > datasetCP = *dataset;
  std::vector < std::pair<std::string, std::string> > attribLabelsCP = *attribLabels;
  std::vector < double > auxDVect;


  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //Dataset con los predicados logicos
  datasetCP.clear();
  attribLabelsCP.clear();
  for(unsigned int attr = 0; attr < attribLabels -> size(); attr++){
    if((*attribLabels)[attr].second != "Numerical" and (*attribLabels)[attr].second != "Class"){
      auxDVect.clear();
      for(unsigned int i = 0; i < nStates; i++){
        auxDVect.push_back((*dataset)[attr][i]);
      }
      datasetCP.push_back(auxDVect);
      attribLabelsCP.push_back((*attribLabels)[attr]);
    }
  }

  datasetCP.push_back(classVec);
  attribLabelsCP.push_back(std::pair <std::string, std::string> ("Class", "Class"));

  datasets -> push_back(datasetCP);
  attribLabelsVC -> push_back(attribLabelsCP);


  //Dataset con los predicados numericos
  datasetCP.clear();
  attribLabelsCP.clear();
  for(unsigned int attr = 0; attr < attribLabels -> size(); attr++){
    if((*attribLabels)[attr].second == "Numerical" and (*attribLabels)[attr].second != "Class"){
      auxDVect.clear();
      for(unsigned int i = 0; i < nStates; i++){
        auxDVect.push_back((*dataset)[attr][i]);
      }
      datasetCP.push_back(auxDVect);
      attribLabelsCP.push_back((*attribLabels)[attr]);
    }
  }
  (*dataset) = datasetCP;
  (*attribLabels) = attribLabelsCP;
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




  //Tomamos el dataset con la info numérica y le metemos mano
  //1º Calcular los diffs
  std::vector<double> nAttrib;
  bool tieneAlgo = false;
  std::vector < std::pair <std::string, std::vector <double> > > listadeDiffs;

  //Recorremos los atributos numericos
  for(unsigned int attr = 0;attr < attribLabels->size(); attr++){
    if((*attribLabels)[attr].second == "Numerical"){
      nAttrib.clear();
      tieneAlgo = false;

      //Recorremos todas las filas del postestado
      for(unsigned int i = 0; i < states.first.size(); i++){
        //Y calculamos la diferencia entre el pre y el post
        if((*dataset)[attr][i] != -999999999.0 and (*dataset)[attr][i + states.first.size()] != -999999999.0){
          nAttrib.push_back((*dataset)[attr][i + states.first.size()] - (*dataset)[attr][i]);
          tieneAlgo = true;
        }else{
          nAttrib.push_back(-999999999.0);
        }
      }

      //Si solo hemos encontrado MVs obviamos el atributo
      if(tieneAlgo == true){
        //Y si no es un vector lleno de ceros tambien...
        if(vectorDeZeros(nAttrib) == true){
          listadeDiffs.push_back(std::pair <std::string, std::vector <double> > ((*attribLabels)[attr].first, nAttrib));
        }
      }
    }
  }

  Estado resultadoAstar;
  std::vector<Estado> estadosInit;
  std::vector<std::string> problemVars;
  std::vector< std::vector <double> >problemData;
  std::vector <double> aux;
  std::vector <std::pair <std::string, std::string> > pairVars, vectEquiv;


  // for (size_t i = 0; i < listadeDiffs.size(); i++) {
  //   std::cout << listadeDiffs[i].first << std::endl << "\t";
  //   for (size_t j = 0; j < listadeDiffs[i].second.size(); j++) {
  //     std::cout << listadeDiffs[i].second[j]<<" ";
  //   }
  //   std::cout << std::endl;
  // }


  //Ajustamos formulas para las diferencias
  for (unsigned int i = 0; i < listadeDiffs.size(); i++) {
    //Reseteamos el dataset para el regresor simbolico
    problemData.clear();
    problemVars.clear();
    pairVars.clear();

    std::cout << "Adjusting numeric expression for: DELTA - " << listadeDiffs[i].first << std::endl;

    //Buscamos todos los atributos numericos
    for(unsigned int attr = 0;attr < attribLabels->size(); attr++){
      //Ignorando aquel de el que surgió
      if((*attribLabels)[attr].first != (listadeDiffs[i].first) and ((*attribLabels)[attr].second == "Numerical")){
        aux.clear();

        //Y los vamos metiendo en el dataset para el regresor
        for(unsigned int i = 0; i < states.first.size(); i++){
          aux.push_back((*dataset)[attr][i + states.first.size()]);
        }

        problemData.push_back(aux);
        problemVars.push_back("Var" + std::to_string(attr));
        pairVars.push_back(std::pair <std::string, std::string> ("Var" + std::to_string(attr), ((*attribLabels)[attr].first)));

      }
    }

    //Reseteamos la info de inicio del A*
    estadosInit.clear();
    estadosInit = generarListaInit(&problemVars, &problemData, &listadeDiffs[i].second);

    //Y lanzamos el regresor
    resultadoAstar = Astar(estadosInit);


    //Si el algoritmo ha terminado correctamente tras encontrar una solucion suficientemente buena, se acepta. Si no, se manda a tomar por culo el delta
    if(resultadoAstar.getTO() == false){
      //std::cout << resultadoAstar.to_string() << std::endl;
      vectEquiv.push_back(std::pair <std::string, std::string> ("DELTA - " + listadeDiffs[i].first, sustituirVars(resultadoAstar.getFormulaPrefix(), pairVars)));
      std::cout << "\t - Expression found: " << sustituirVars(resultadoAstar.getFormula(), pairVars) << " MAPE: "<< resultadoAstar.getErrorR() << "%"<< std::endl;
    }else{
      vectEquiv.push_back(std::pair <std::string, std::string> ("N/A", "N/A"));
      std::cout << "\t - [TIMEOUT] Expression NOT found. Best formula with MAPE: " << resultadoAstar.getErrorR() << "%"<< std::endl;
    }
  }


  /*std::vector <double> vecObj;
  //Para cada atributo numerico buscamos una posible formula que lo represente
  for(unsigned int attr = 0;attr < attribLabels->size(); attr++){
    if((*attribLabels)[attr].second == "Numerical"){
      vecObj.clear();
      for(unsigned int i = 0; i < nStates; i++){
        vecObj.push_back((*dataset)[attr][i]);
      }

      //Si el atributo no varía en todo el dataset no hace falta ajustarle nada
      if(vectorInutil(vecObj) == true){
        problemData.clear();
        problemVars.clear();
        pairVars.clear();
        //Buscamos todos los atributos numericos
        for(unsigned int attr2 = 0;attr2 < attribLabels->size(); attr2++){
          if((attr != attr2) and ((*attribLabels)[attr2].second == "Numerical")){
            aux.clear();

            //Y los vamos metiendo en el dataset para el regresor
            for(unsigned int i = 0; i < nStates; i++){
              aux.push_back((*dataset)[attr2][i]);
            }

            problemData.push_back(aux);
            problemVars.push_back("Var" + std::to_string(attr2));
            pairVars.push_back(std::pair <std::string, std::string> ("Var" + std::to_string(attr2), ((*attribLabels)[attr2].first)));
          }
        }

        estadosInit.clear();
        estadosInit = generarListaInit(&problemVars, &problemData, &vecObj);
        resultadoAstar = Astar(estadosInit);

        if(resultadoAstar.getTO() == false){
          //std::cout << resultadoAstar.to_string() << std::endl;
          vectEquiv.push_back(std::pair <std::string, std::string> ((*attribLabels)[attr].first, sustituirVars(resultadoAstar.getFormulaPrefix(), pairVars)));
        }
      }
    }
  }*/


  // for(unsigned int i = 0; i < vectEquiv.size(); i++){
  //   std::cout << vectEquiv[i].first << " = " << vectEquiv[i].second << std::endl;
  // }


  //Rellenamos los huecos que faltan en los vectores de diferencias para cuadrar el vector antes de meterlo en el dataset
  for (unsigned int elem = 0; elem < listadeDiffs.size(); elem++){
    aux.clear();
    for(unsigned int i = 0; i < listadeDiffs[elem].second.size(); i++){
      //aux.push_back(-999999999.0);
      aux.push_back(listadeDiffs[elem].second[i]);
    }
    for(unsigned int i = 0; i < listadeDiffs[elem].second.size(); i++){
      aux.push_back(listadeDiffs[elem].second[i]);
    }
    listadeDiffs[elem].second = aux;
  }



  //Inclusion de la relaciones booleanas en el dataset
  //Seleccionamos un atributo
  for(unsigned int attr = 0; attr < attribLabels->size(); attr++){
    //Si es numerico...

    if((*attribLabels)[attr].second == "Numerical"){
      if(esPrecondicionable((*attribLabels)[attr].first, listadeDiffs)){

        //Seleccionamos otro atributo numerico
        for(unsigned int attr2 = attr; attr2 < attribLabels->size(); attr2++){
          if((attr != attr2) and ((*attribLabels)[attr2].second == "Numerical")){
            //==
            nAttrib.clear();
            tieneAlgo = false;
            for(unsigned int i = 0; i < nStates; i++){
              if(((*dataset)[attr][i] != -999999999.0) and ((*dataset)[attr2][i] != -999999999.0)){
                if((*dataset)[attr][i] == (*dataset)[attr2][i]){
                  nAttrib.push_back(1.0);
                }else{
                  nAttrib.push_back(0.0);
                }
                tieneAlgo = true;
              }
              else{
                nAttrib.push_back(-999999999.0);
              }
            }

            if((tieneAlgo == true) and (vectorInutil(nAttrib) == true)){
              attribLabelsCP.push_back(std::pair<std::string, std::string>("opEQ - " + (*attribLabels)[attr].first + "  " + (*attribLabels)[attr2].first, "Logical"));
              datasetCP.push_back(nAttrib);
            }

            //<
            nAttrib.clear();
            tieneAlgo = false;
            for(unsigned int i = 0; i < nStates; i++){
              if((*dataset)[attr][i] != -999999999.0 and (*dataset)[attr2][i] != -999999999.0){
                if((*dataset)[attr][i] < (*dataset)[attr2][i]){
                  nAttrib.push_back(1.0);
                }else{
                  nAttrib.push_back(0.0);
                }
                tieneAlgo = true;
              }
              else{
                nAttrib.push_back(-999999999.0);
              }
            }

            if((tieneAlgo == true) and (vectorInutil(nAttrib) == true)){
              attribLabelsCP.push_back(std::pair<std::string, std::string>("opLT - " + (*attribLabels)[attr].first + "  " + (*attribLabels)[attr2].first, "Logical"));
              datasetCP.push_back(nAttrib);
            }

            //>
            nAttrib.clear();
            tieneAlgo = false;
            for(unsigned int i = 0; i < nStates; i++){
              if((*dataset)[attr][i] != -999999999.0 and (*dataset)[attr2][i] != -999999999.0){
                if((*dataset)[attr][i] > (*dataset)[attr2][i]){
                  nAttrib.push_back(1.0);
                }else{
                  nAttrib.push_back(0.0);
                }
                tieneAlgo = true;
              }
              else{
                nAttrib.push_back(-999999999.0);
              }
            }

            if((tieneAlgo == true) and (vectorInutil(nAttrib) == true)){
              attribLabelsCP.push_back(std::pair<std::string, std::string>("opGT - " + (*attribLabels)[attr].first + "  " + (*attribLabels)[attr2].first, "Logical"));
              datasetCP.push_back(nAttrib);
            }
          }
        }


        //Repetimos para los diffs
        for(unsigned int attr2 = 0; attr2 < listadeDiffs.size(); attr2++){
          if((*attribLabels)[attr].first != listadeDiffs[attr2].first){
            //==
            nAttrib.clear();
            tieneAlgo = false;
            //El for hay que partirlo en dos trozos para que asigne correctamente la info a los pre y post estados
            for(unsigned int i = 0; i < (listadeDiffs[attr2].second.size()/2); i++){
              if((*dataset)[attr][i] != -999999999.0 and listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)] != -999999999.0){
                //if((*dataset)[attr][i] == listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)]){
                if((*dataset)[attr][i] == -(listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)])){
                  nAttrib.push_back(1.0);
                }else{
                  nAttrib.push_back(0.0);
                }
                tieneAlgo = true;
              }
              else{
                nAttrib.push_back(-999999999.0);
              }
            }
            for(unsigned int i = 0; i < (listadeDiffs[attr2].second.size()/2); i++){
                //nAttrib.push_back(-999999999.0);

                if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] != -999999999.0 and listadeDiffs[attr2].second[i] != -999999999.0){
                  //if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] == listadeDiffs[attr2].second[i]){
                  if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] == -(listadeDiffs[attr2].second[i])){
                    nAttrib.push_back(1.0);
                  }else{
                    nAttrib.push_back(0.0);
                  }
                  tieneAlgo = true;
                }
                else{
                  nAttrib.push_back(-999999999.0);
                }

            }

            if((tieneAlgo == true)){
              attribLabelsCP.push_back(std::pair<std::string, std::string>("opEQ - " + (*attribLabels)[attr].first + "  " +  getEquiv(listadeDiffs[attr2].first, vectEquiv), "Logical"));
              datasetCP.push_back(nAttrib);
            }


            //<
            nAttrib.clear();
            tieneAlgo = false;
            for(unsigned int i = 0; i < (listadeDiffs[attr2].second.size()/2); i++){
              if((*dataset)[attr][i] != -999999999.0 and listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)] != -999999999.0){
                //if((*dataset)[attr][i] < listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)]){
                if((*dataset)[attr][i] < -(listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)])){
                  nAttrib.push_back(1.0);
                }else{
                  nAttrib.push_back(0.0);
                }
                tieneAlgo = true;
              }
              else{
                nAttrib.push_back(-999999999.0);
              }
            }
            for(unsigned int i = 0; i < (listadeDiffs[attr2].second.size()/2); i++){
              //nAttrib.push_back(-999999999.0);

              if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] != -999999999.0 and listadeDiffs[attr2].second[i] != -999999999.0){
                //if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] < listadeDiffs[attr2].second[i]){
                if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] < -(listadeDiffs[attr2].second[i])){
                  nAttrib.push_back(1.0);
                }else{
                  nAttrib.push_back(0.0);
                }
                tieneAlgo = true;
              }
              else{
                nAttrib.push_back(-999999999.0);
              }
            }

            if((tieneAlgo == true)){
              attribLabelsCP.push_back(std::pair<std::string, std::string>("opLT - " + (*attribLabels)[attr].first + "  " + getEquiv(listadeDiffs[attr2].first, vectEquiv), "Logical"));
              datasetCP.push_back(nAttrib);
            }


            //>
            nAttrib.clear();
            tieneAlgo = false;
            for(unsigned int i = 0; i < (listadeDiffs[attr2].second.size()/2); i++){
              if((*dataset)[attr][i] != -999999999.0 and listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)] != -999999999.0){
                //if((*dataset)[attr][i] > listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)]){
                if((*dataset)[attr][i] > -(listadeDiffs[attr2].second[i + (listadeDiffs[attr2].second.size()/2)])){
                  nAttrib.push_back(1.0);
                }else{
                  nAttrib.push_back(0.0);
                }
                tieneAlgo = true;
              }
              else{
                nAttrib.push_back(-999999999.0);
              }
            }
            for(unsigned int i = 0; i < (listadeDiffs[attr2].second.size()/2); i++){
              //nAttrib.push_back(-999999999.0);

              if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] != -999999999.0 and listadeDiffs[attr2].second[i] != -999999999.0){
                //if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] > listadeDiffs[attr2].second[i]){
                if((*dataset)[attr][i + (listadeDiffs[attr2].second.size()/2)] > -(listadeDiffs[attr2].second[i])){
                  nAttrib.push_back(1.0);
                }else{
                  nAttrib.push_back(0.0);
                }
                tieneAlgo = true;
              }
              else{
                nAttrib.push_back(-999999999.0);
              }
            }

            if((tieneAlgo == true)){
              attribLabelsCP.push_back(std::pair<std::string, std::string>("opGT - " + (*attribLabels)[attr].first + "  " + getEquiv(listadeDiffs[attr2].first, vectEquiv), "Logical"));
              datasetCP.push_back(nAttrib);
            }


          }
        }
      }
    }
   //Si es logico no se hace nada de nada.
    else{
      /*for(unsigned int attr2 = attr; attr2 < attribLabels->size(); attr2++){
        if(attr != attr2 and (*attribLabels)[attr2].second == "Logical"){
          //==
          nAttrib.clear();
          tieneAlgo = false;
          for(unsigned int i = 0; i < nStates; i++){
            if((*dataset)[attr][i] != -999999999.0 and (*dataset)[attr2][i] != -999999999.0){
              if((*dataset)[attr][i] == (*dataset)[attr2][i]){
                nAttrib.push_back(1.0);
              }else{
                nAttrib.push_back(0.0);
              }
              tieneAlgo = true;
            }
            else{
              nAttrib.push_back(-999999999.0);
            }
          }

          if((tieneAlgo == true) and (vectorInutil(nAttrib) == true)){
            attribLabelsCP.push_back(std::pair<std::string, std::string>("opEQ - " + (*attribLabels)[attr].first + "  " + (*attribLabels)[attr2].first, "Logical"));
            datasetCP.push_back(nAttrib);
          }
        }
      }*/
    }
  }


  //Finalmente insertamos los diffs en el dataset
  for(unsigned int i = 0; i < listadeDiffs.size(); i++){
    nAttrib.clear();
    for(unsigned int j = 0; j < listadeDiffs[i].second.size()/2; j++){
      nAttrib.push_back(0.0);
    }


    for(unsigned int j = 0; j < listadeDiffs[i].second.size()/2; j++){
      if(listadeDiffs[i].second[j] == -999999999.0){
        nAttrib.push_back(-999999999.0);
      }
      else{
        if(listadeDiffs[i].second[j] > 0.0){
          nAttrib.push_back(1.0);
        }
        else{
          nAttrib.push_back(-1.0);
        }
      }

    }

    datasetCP.push_back(nAttrib);
    if(vectEquiv[i].first != "N/A"){
      attribLabelsCP.push_back(std::pair <std::string, std::string> (vectEquiv[i].first + " " + vectEquiv[i].second, "Delta"));
    }
  }


  //Y finiquitamos todo metiendo las etiquetas de las clases en el dataset
  datasetCP.push_back(classVec);
  attribLabelsCP.push_back(std::pair <std::string, std::string> ("Class", "Class"));
  (*dataset) = datasetCP;
  (*attribLabels) = attribLabelsCP;

  datasets -> push_back(*dataset);
  attribLabelsVC -> push_back(*attribLabels);

  delete dataset;
  delete attribLabels;

}
