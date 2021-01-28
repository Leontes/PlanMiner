
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <queue>
#include <set>
#include <cmath>


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
          if(!regex_match(rBuffer,taskRE) and rBuffer != "" and !regex_match(rBuffer,planRE) and !regex_match(rBuffer,stateRE)){
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
              (*statesMap[rBuffer.substr(0, contador)]).setTimeStamp(rBuffer.substr(1, contador-2));
            }
          }
      }

      std::string numPre = "", numPos = "";
      unsigned int indice = 0;

      for(unsigned int i = 0; i < taskVec.size(); i++){
        //std::cout << taskVec[i] << std::endl;
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

bool incluido(std::pair <std::string, std::string> ParamsAux, std::vector <std::pair <std::string, std::string> > listaParamsFinal){
  for (size_t i = 0; i < listaParamsFinal.size(); i++) {
    if(ParamsAux.first == listaParamsFinal[i].first and ParamsAux.second == listaParamsFinal[i].second){
      return true;
    }
  }
  return false;
}

std::vector < Type *> extractTypeHierarchy(std::vector< PlanTrace * > * PTS, std::map <std::string, std::vector <std::pair <std::string, std::string> > > * mapaPrimitivasParam,
  std::map <std::string, std::vector <std::pair <std::string, std::string> > > * mapaPredicadosParam){
  std::vector < Type * > vectorAux;
  std::string tName;
  std::vector<Parameter*> parametros;
  std::map <std::string, std::vector <std::pair <std::string, std::string> > > mapaPrimitivasParamAUX;
  std::map <std::string, std::vector <std::pair <std::string, std::string> > > mapaPredicadosParamAUX;
  std::vector < std::vector <std::string> > outMat;
  //Tomamos 1 plan
  for(unsigned int i = 0; i < PTS -> size(); i++){
    //Tomamos un TS
    for (unsigned int j = 0; j < (*PTS)[i] -> getNTS(); j++) {
      //Tomamos el nombre de la tarea y sus parametros
      tName = (*PTS)[i] -> getTS(j) -> getTask() -> get_Tname();
      parametros = (*PTS)[i] -> getTS(j) -> getTask() -> getParameters();
      //Creamos un mapa con todos los tipos distintos para los
      for(unsigned int k = 0; k < parametros.size(); k++){
        mapaPrimitivasParamAUX[tName].push_back(std::pair <std::string, std::string> (parametros[k] -> getName(), parametros[k] -> getTypes()[0]));
      }


      outMat = (*PTS)[i] -> getTS(j) -> getPreS() -> getTokens();
      for (size_t m = 0; m < outMat.size(); m++) {
        for (size_t n = 1; n < outMat[m].size(); n+=3) {
          mapaPredicadosParamAUX[outMat[m][0]].push_back(std::pair <std::string, std::string> (outMat[m][n], outMat[m][n+2]));
        }
      }
<<<<<<< HEAD
=======

>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
      outMat = (*PTS)[i] -> getTS(j) -> getPostS() -> getTokens();
      for (size_t m = 0; m < outMat.size(); m++) {
        for (size_t n = 1; n < outMat[m].size(); n+=3) {
          mapaPredicadosParamAUX[outMat[m][0]].push_back(std::pair <std::string, std::string> (outMat[m][n], outMat[m][n+2]));
        }
      }
    }
  }
<<<<<<< HEAD
=======

>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
  std::vector <std::pair <std::string, std::string> > listaParamsAux, listaParamsFinal;
  for(auto it = mapaPrimitivasParamAUX.begin(); it != mapaPrimitivasParamAUX.end(); it++){
    listaParamsAux = (*it).second;
    for(unsigned int i = 0; i < listaParamsAux.size(); i++){
        if (!incluido(listaParamsAux[i], listaParamsFinal)){
          listaParamsFinal.push_back(listaParamsAux[i]);
        }
    }
<<<<<<< HEAD
    mapaPrimitivasParamAUX[(*it).first] = listaParamsFinal;
    listaParamsFinal.clear();
  }

=======

    mapaPrimitivasParamAUX[(*it).first] = listaParamsFinal;
    listaParamsFinal.clear();
  }

>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
  for(auto it = mapaPredicadosParamAUX.begin(); it != mapaPredicadosParamAUX.end(); it++){
    listaParamsAux = (*it).second;
    for(unsigned int i = 0; i < listaParamsAux.size(); i++){
        if (!incluido(listaParamsAux[i], listaParamsFinal)){
          listaParamsFinal.push_back(listaParamsAux[i]);
        }
    }
<<<<<<< HEAD
=======

>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
    mapaPredicadosParamAUX[(*it).first] = listaParamsFinal;
    listaParamsFinal.clear();
  }

  // for(auto it = mapaPrimitivasParamAUX.begin(); it != mapaPrimitivasParamAUX.end(); it++){
  //   std::cout << "Tarea: " << (*it).first << std::endl << "[ ";
  //   for(unsigned int i = 0; i < (*it).second.size(); i++){
  //     std::cout << (*it).second[i].first << " - " << (*it).second[i].second << " ";
  //   }
  //   std::cout << "]" << std::endl;
  // }

  // for(auto it = mapaPredicadosParamAUX.begin(); it != mapaPredicadosParamAUX.end(); it++){
  //   std::cout << "Predicado: " << (*it).first << std::endl << "[ ";
  //   for(unsigned int i = 0; i < (*it).second.size(); i++){
  //     std::cout << (*it).second[i].first << " - " << (*it).second[i].second << " ";
  //   }
  //   std::cout << "]" << std::endl;
  // }

  //Encontramos los conflictos entre parametros con el mismo nombre en la misma tarea/predicado evitando los conflictos repetidos
  std::vector <std::pair <std::string, std::string> > conflictos;
  bool tipoencontrado = false;

  for(auto it = mapaPrimitivasParamAUX.begin(); it != mapaPrimitivasParamAUX.end(); it++){
    for(unsigned int i = 0; i < (*it).second.size(); i++){
      tipoencontrado = false;
      for (size_t j = 0; j < vectorAux.size(); j++) {
        if((*it).second[i].second == vectorAux[j] -> name){
          tipoencontrado = true;
        }
      }
      if(tipoencontrado == false){
        vectorAux.push_back(new Type((*it).second[i].second));
      }
      for(unsigned int j = i; j < (*it).second.size(); j++){
        if((*it).second[i].first == (*it).second[j].first and (*it).second[i].second != (*it).second[j].second){
          conflictos.push_back(std::pair <std::string, std::string> ((*it).second[i].second, (*it).second[j].second));
<<<<<<< HEAD
=======
        }
      }
    }
  }

  for(auto it = mapaPredicadosParamAUX.begin(); it != mapaPredicadosParamAUX.end(); it++){
    for(unsigned int i = 0; i < (*it).second.size(); i++){
      tipoencontrado = false;
      for (size_t j = 0; j < vectorAux.size(); j++) {
        if((*it).second[i].second == vectorAux[j] -> name){
          tipoencontrado = true;
        }
      }
      if(tipoencontrado == false){
        vectorAux.push_back(new Type((*it).second[i].second));
      }
      for(unsigned int j = i; j < (*it).second.size(); j++){
        if((*it).second[i].first == (*it).second[j].first and (*it).second[i].second != (*it).second[j].second){
          conflictos.push_back(std::pair <std::string, std::string> ((*it).second[i].second, (*it).second[j].second));
>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
        }
      }
    }
  }
<<<<<<< HEAD
  for(auto it = mapaPredicadosParamAUX.begin(); it != mapaPredicadosParamAUX.end(); it++){
    for(unsigned int i = 0; i < (*it).second.size(); i++){
      tipoencontrado = false;
      for (size_t j = 0; j < vectorAux.size(); j++) {
        if((*it).second[i].second == vectorAux[j] -> name){
          tipoencontrado = true;
        }
      }
      if(tipoencontrado == false){
        vectorAux.push_back(new Type((*it).second[i].second));
      }
      for(unsigned int j = i; j < (*it).second.size(); j++){
        if((*it).second[i].first == (*it).second[j].first and (*it).second[i].second != (*it).second[j].second){
          conflictos.push_back(std::pair <std::string, std::string> ((*it).second[i].second, (*it).second[j].second));
        }
      }
    }
  }
=======

>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
  std::cout << "Types found in PTs:\n";
  for(unsigned int i = 0; i < vectorAux.size(); i++){
    std::cout << vectorAux[i] -> name << " " << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Conflicts between types found: \n";
  for (size_t i = 0; i < conflictos.size(); i++) {
    std::cout << "\t" << conflictos[i].first << " < - > " << conflictos[i].second << std::endl;
  }

  int objetosencontrados = 0;
  Type * tipo1, * tipo2;
  for(unsigned int i = 0; i < conflictos.size(); i++){
    objetosencontrados = 0;
    for(unsigned int j = 0; (j < vectorAux.size()) and (objetosencontrados < 2); j++){
      if(conflictos[i].first == vectorAux[j] -> name){
        tipo1 = vectorAux[j];
        objetosencontrados++;
      }
      if(conflictos[i].second == vectorAux[j] -> name){
        tipo2 = vectorAux[j];
        objetosencontrados++;
      }
    }

    if((tipo1 -> parent == 0) and (tipo2 -> parent == 0)){
      vectorAux.push_back(new Type("PARENTTYPE" + std::to_string(i)));
      tipo1 -> parent = vectorAux[vectorAux.size()-1];
      tipo2 -> parent = vectorAux[vectorAux.size()-1];
    }
    else{
      if((tipo1 -> parent != 0) and (tipo2 -> parent != 0) and (tipo1 -> parent != tipo2 -> parent)){
        conflictos.push_back(std::pair <std::string, std::string> ((tipo1 -> parent) -> name, (tipo2 -> parent) -> name));
      }
      else{
        if(tipo1 -> parent != 0){
          tipo2 -> parent = tipo1 -> parent;
        }
        if(tipo2 -> parent != 0){
          tipo1 -> parent = tipo2 -> parent;
        }
      }
    }
  }

  std::cout << "\nNew tipes calculated:\n";
  for(unsigned int i = 0; i < vectorAux.size(); i++){
    std::cout << vectorAux[i] -> name;
    if(vectorAux[i] -> parent != 0){
       std::cout << " - " << vectorAux[i] -> parent -> name << std::endl;
    }
    else{
       std::cout << " - OBJECT" << std::endl;
    }
  }
  std::cout << std::endl;



  //Recalculamos las cabeceras con los conflictos entre parametros resueltos
  mapaPrimitivasParam -> clear();
  std::vector <std::string> keys;
  for(auto it = mapaPrimitivasParamAUX.begin(); it != mapaPrimitivasParamAUX.end(); it++){
    keys.push_back((*it).first);
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

  mapaPredicadosParam -> clear();
  std::vector <std::string> keysPred;
  for(auto it = mapaPredicadosParamAUX.begin(); it != mapaPredicadosParamAUX.end(); it++){
    keysPred.push_back((*it).first);
  }

  parametroConflictivo = false;
  parametroYaIncluido = false;

  for(unsigned int i = 0; i < keysPred.size(); i++){
    for(unsigned int j = 0; j < mapaPredicadosParamAUX[keysPred[i]].size(); j++){
      parametroConflictivo = false;
      for(unsigned int k = j; k < mapaPredicadosParamAUX[keysPred[i]].size(); k++){
        if((mapaPredicadosParamAUX[keysPred[i]][j].first == mapaPredicadosParamAUX[keysPred[i]][k].first) and (mapaPredicadosParamAUX[keysPred[i]][j].second != mapaPredicadosParamAUX[keysPred[i]][k].second)){
          parametroConflictivo = true;
          resolucion = solveConflict(mapaPredicadosParamAUX[keysPred[i]][j].second, mapaPredicadosParamAUX[keysPred[i]][k].second, &vectorAux);
        }
      }

      parametroYaIncluido = false;
      for(unsigned int k = 0; k < (*mapaPredicadosParam)[keysPred[i]].size(); k++){
        if(mapaPredicadosParamAUX[keysPred[i]][j].first == (*mapaPredicadosParam)[keysPred[i]][k].first){
          parametroYaIncluido = true;
        }
      }
      if(parametroConflictivo == true){

        if(parametroYaIncluido == false){
          (*mapaPredicadosParam)[keysPred[i]].push_back(std::pair <std::string, std::string> (mapaPredicadosParamAUX[keysPred[i]][j].first, resolucion));
        }
      }else{
        if(parametroYaIncluido == false){
          (*mapaPredicadosParam)[keysPred[i]].push_back(std::pair <std::string, std::string> (mapaPredicadosParamAUX[keysPred[i]][j].first, mapaPredicadosParamAUX[keysPred[i]][j].second));
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

  std::cout << "\n\nNew predicates headers created: \n";
  for(unsigned int i = 0; i < keysPred.size(); i++){
    std::cout << keysPred[i];
    for(unsigned int j = 0; j < (*mapaPredicadosParam)[keysPred[i]].size(); j++){
      std::cout << " " << (*mapaPredicadosParam)[keysPred[i]][j].first  << " - " << (*mapaPredicadosParam)[keysPred[i]][j].second;
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
  return output;

}
<<<<<<< HEAD



=======



>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
bool noesta(std::set<std::string> & conjuntoElem, Estado actual){
  std::vector<std::string> form = actual.getVectFormula();
  std::string clave = "", otraClave = "";
  bool esta = false;
  if(form.size() == 1.0){
    clave = form[0];
    esta = (conjuntoElem.find(clave) == conjuntoElem.end());
  }
  else{
    std::string operandoAct1 = form[0];
    for(unsigned int i = 1; i < form.size()-4; i++){
      operandoAct1 += " " + form[i];
    }

    std::string operadorAct = form[form.size()-3];
    std::string operandoAct2 = form[form.size()-2];

    clave += operandoAct1 + operadorAct + operandoAct2;
    otraClave += operandoAct2 + operadorAct + operandoAct1;

    if(operadorAct == "+" or operadorAct == "*"){
      esta = ((conjuntoElem.find(clave) == conjuntoElem.end()) and (conjuntoElem.find(otraClave) == conjuntoElem.end()));
<<<<<<< HEAD

    }
    else{
      esta = ((conjuntoElem.find(clave) == conjuntoElem.end()));
    }
  }

=======

    }
    else{
      esta = ((conjuntoElem.find(clave) == conjuntoElem.end()));
    }
  }

>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
  return esta;
}


//Simbolic Regression A*-based algorithm
Estado Astar(std::vector <Estado> & estadosInit){
  std::vector<Estado> hijos, vectorHijos;
  std::priority_queue<Estado, std::vector<Estado>, std::greater<Estado> > listaAbiertos;
  std::set<std::string> conjuntoAbiertos, conjuntoCerrados;

  time_t start, finish;
  time(&start);

  for(unsigned int i = 0; i < estadosInit.size(); i++){
    listaAbiertos.push(estadosInit[i]);
  }


  Estado estadoActual = listaAbiertos.top(), mejorEstado = listaAbiertos.top();

  bool seguir = true;
  if(mejorEstado.final()){
    seguir = false;
  }
  //double gScoreIncierto = 0.0;
  //unsigned int nEstados = 1;
  //unsigned int index;

  while(seguir and listaAbiertos.size() > 0){
    estadoActual = listaAbiertos.top();
    listaAbiertos.pop();
    conjuntoAbiertos.insert(estadoActual.getFormula());
    conjuntoCerrados.insert(estadoActual.getFormula());

    if (estadoActual.final()){
      seguir = false;
      mejorEstado = estadoActual;
    }

    //std::cout << "Numero de estados explorados: " << nEstados << std::endl;
    //std::cout << estadoActual.to_string() << std::endl;

    hijos = estadoActual.generarHijos();
    vectorHijos.clear();
    for(unsigned int i = 0; i < hijos.size(); i++){
      if(noesta(conjuntoCerrados, hijos[i])){
        //tentative_gScore := gScore[current] + dist_between(current, neighbor)
        //gScoreIncierto = estadoActual.getC() + (hijos[i].getC() - estadoActual.getC());

        if(noesta(conjuntoAbiertos, hijos[i])){
          listaAbiertos.push(hijos[i]);
        }
      }
    }

    time(&finish);
<<<<<<< HEAD
    if(difftime(finish, start) >= 300.0){
=======
    if(difftime(finish, start) >= 3.0){
>>>>>>> 6e8344f7d2eef24cc20ab12fd5ea3d617b5d3d71
      seguir = false;
    }


    //std::cout << "Tiempo transcurrido: " << difftime(finish, start) << std::endl;
    //nEstados++;
  }

  time(&finish);
  std::cout << "\t- Time elapsed: " << difftime(finish, start) << "s\n";

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
  std::vector <double> aux, goal;
  std::vector <std::pair <std::string, std::string> > pairVars, vectEquiv;


  // for (size_t i = 0; i < listadeDiffs.size(); i++) {
  //   std::cout << listadeDiffs[i].first << std::endl << "\t";
  //   for (size_t j = 0; j < listadeDiffs[i].second.size(); j++) {
  //     std::cout << listadeDiffs[i].second[j]<<" ";
  //   }
  //   std::cout << std::endl;
  // }


  //Ajustamos formulas para las diferencias
  for (unsigned int i = 0; i < listadeDiffs.size(); i++){
    //Reseteamos el dataset para el regresor simbolico
    problemData.clear();
    problemVars.clear();
    pairVars.clear();
    goal.clear();

    std::cout << "Adjusting numeric expression for: DELTA - " << listadeDiffs[i].first << std::endl;
    for(unsigned int j = 0; j < listadeDiffs[i].second.size(); j++){
      goal.push_back(abs(listadeDiffs[i].second[j]));
      //goal.push_back(listadeDiffs[i].second[j]);
    }

    //Buscamos todos los atributos numericos
    for(unsigned int attr = 0;attr < attribLabels->size(); attr++){
      //Ignorando aquel de el que surgió
      if((*attribLabels)[attr].first != (listadeDiffs[i].first) and ((*attribLabels)[attr].second == "Numerical") and (*attribLabels)[attr].first != "TIMESTAMP"){
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
    estadosInit = generarListaInit(&problemVars, &problemData, &goal);

    //Y lanzamos el regresor
    resultadoAstar = Astar(estadosInit);


    //Si el algoritmo ha terminado correctamente tras encontrar una solucion suficientemente buena, se acepta. Si no, se manda a tomar por culo el delta
    if(resultadoAstar.getTO() == false){
      //std::cout << resultadoAstar.to_string() << std::endl;
      vectEquiv.push_back(std::pair <std::string, std::string> ("DELTA - " + listadeDiffs[i].first, sustituirVars(resultadoAstar.getFormulaArbol(), pairVars)));
      std::cout << "\t - Expression found: " << sustituirVars(resultadoAstar.getFormula(), pairVars) << " MAPE: "<< resultadoAstar.getH()*10.0 << "%"<< std::endl;
    }else{
      vectEquiv.push_back(std::pair <std::string, std::string> ("N/A", "N/A"));
      std::cout << "\t - [TIMEOUT] Expression NOT found. Best formula with MAPE: " << resultadoAstar.getH() << "%"<< std::endl;
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
          if((attr != attr2) and ((*attribLabels)[attr2].second == "Numerical") and ((*attribLabels)[attr2].first != "TIMESTAMP")){
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
          if(((*attribLabels)[attr].first != listadeDiffs[attr2].first) and (listadeDiffs[attr2].first != "TIMESTAMP") ){
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

            if((tieneAlgo == true) and (vectorInutil(nAttrib) == true)){
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

            if((tieneAlgo == true) and (vectorInutil(nAttrib) == true)){
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

            if((tieneAlgo == true) and (vectorInutil(nAttrib) == true)){
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
