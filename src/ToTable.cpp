#include "ToTable.hpp"

#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <cmath>
#include <random>
#include <sstream>



enum type {constant, variable, binaryOp};

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

struct nodoArith {
  type tipo;
  //Numero, variable, +, -, *, /
  std::string valor;
  std::vector<nodoArith> hijos;

  nodoArith(){}
  nodoArith(std::vector <std::string> formula){
    std::stack <nodoArith> pilaNodos;
    nodoArith hijo1, hijo2, aux;
    for(std::vector <std::string>::reverse_iterator it = formula.rbegin(); it != formula.rend(); it++){
      if(is_number(*it) or is_var(*it)){
        aux.valor = *it;
        aux.hijos.clear();
        if(is_number(*it)){
          aux.tipo = constant;
        }
        else{
          aux.tipo = variable;
        }
        pilaNodos.push(aux);
      }
      else{
        hijo1 = pilaNodos.top();
        pilaNodos.pop();
        hijo2 = pilaNodos.top();
        pilaNodos.pop();

        aux.tipo = binaryOp;
        aux.valor = *it;

        aux.hijos.clear();
        aux.hijos.push_back(hijo1);
        aux.hijos.push_back(hijo2);

        pilaNodos.push(aux);
      }
    }

    tipo = pilaNodos.top().tipo;
    valor = pilaNodos.top().valor;
    hijos = pilaNodos.top().hijos;

  }

  ~nodoArith(){}

  void asignarFormula(std::vector <std::string> formula){
    std::stack <nodoArith> pilaNodos;
    nodoArith hijo1, hijo2, aux;
    for(std::vector <std::string>::reverse_iterator it = formula.rbegin(); it != formula.rend(); it++){
      if(is_number(*it) or is_var(*it)){
        aux.valor = *it;
        aux.hijos.clear();
        if(is_number(*it)){
          aux.tipo = constant;
        }
        else{
          aux.tipo = variable;
        }
        pilaNodos.push(aux);
      }
      else{
        hijo1 = pilaNodos.top();
        pilaNodos.pop();
        hijo2 = pilaNodos.top();
        pilaNodos.pop();

        aux.tipo = binaryOp;
        aux.valor = *it;

        aux.hijos.clear();
        aux.hijos.push_back(hijo1);
        aux.hijos.push_back(hijo2);

        pilaNodos.push(aux);
      }
    }

    this -> tipo = pilaNodos.top().tipo;
    this -> valor = pilaNodos.top().valor;
    this -> hijos = pilaNodos.top().hijos;
  }


  double ejecutar(std::map<std::string, double> variables){
    switch (tipo) {
      case constant:
        return std::stod(valor);
        break;
      case variable:
        return variables[valor];
        break;
      case binaryOp:
        if(valor == "+"){
          if(hijos[0].ejecutar(variables) == -999999999.0 or hijos[1].ejecutar(variables) == -999999999.0){
            return -999999999.0;
          }
          else{
            return (hijos[0].ejecutar(variables) + hijos[1].ejecutar(variables));
          }
        }
        if(valor == "-"){
          if(hijos[0].ejecutar(variables) == -999999999.0 or hijos[1].ejecutar(variables) == -999999999.0){
            return -999999999.0;
          }
          else{
            return (hijos[0].ejecutar(variables) - hijos[1].ejecutar(variables));
          }
        }
        if(valor == "*"){
          if(hijos[0].ejecutar(variables) == -999999999.0 or hijos[1].ejecutar(variables) == -999999999.0){
            return -999999999.0;
          }
          else{
            return (hijos[0].ejecutar(variables) * hijos[1].ejecutar(variables));
          }
        }
        if(valor == "/"){

          if(hijos[0].ejecutar(variables) == -999999999.0 or hijos[1].ejecutar(variables) == -999999999.0){
            return -999999999.0;
          }
          else{
            double wd = hijos[1].ejecutar(variables);
            if(wd == 0.0){
              return -999999999.0;
            }
            return (hijos[0].ejecutar(variables) / wd);
          }
        }
        if(valor == "^"){
          if(hijos[0].ejecutar(variables) == -999999999.0 or hijos[1].ejecutar(variables) == -999999999.0){
            return -999999999.0;
          }
          else{
            return pow(hijos[0].ejecutar(variables), hijos[1].ejecutar(variables));
          }
        }
        break;
    }
  }


  bool noVars(){
    switch (tipo) {
      case constant:
        return true;
        break;
      case variable:
        return false;
        break;
      case binaryOp:
          return (hijos[0].noVars() and hijos[1].noVars());
        break;
    }
  }

  unsigned int size(){
    switch (tipo) {
      case constant:
      case variable:
        return 1;
        break;
      case binaryOp:
        return 1 + hijos[0].size() + hijos[1].size() ;
        break;

    }
  }

  void clearHijos(){
    for(unsigned int i = 0; i < hijos.size(); i++){
      hijos[i].clearHijos();
    }
    hijos.clear();
  }

  nodoArith * getNodo(unsigned int *i){
    nodoArith * aux;
    if(*i == 0){
      return this;
    }
    else{
      for(unsigned hijo = 0; hijo < hijos.size(); hijo++){
        if(*i == 0){
          return this;
        }
        else{
          (*i)--;
          aux = hijos[hijo].getNodo(i);
          if(aux != nullptr){
            return aux;
          }
        }
      }
      return nullptr;
    }
  }

  std::string printFunct(){
    std::string out = "";
    if(tipo != constant){
      out += "(" + valor;
    }
    else{
      out += valor;
    }

    for(unsigned hijo = 0; hijo < hijos.size(); hijo++){
      out += " ";
      out += hijos[hijo].printFunct();
    }

      if(tipo != constant){
      out += ")";
    }


    return out;
  }

  nodoArith& operator = (const nodoArith &innodo){
    //Comprueba que no se esté intentanod igualar un objeto a sí mismo
    if(this!=&innodo){
      tipo = innodo.tipo;
      valor = innodo.valor;
      hijos = innodo.hijos;
    }
    return *this;
  }

  bool operator == (const nodoArith& e) const {

    switch (tipo) {
      case constant:
        if (e.tipo != constant or valor != e.valor){
          return false;
        }else{
          return true;
        }
        break;
      case variable:
        if (e.tipo != variable or valor != e.valor){
          return false;
        }else{
          return true;
        }
        break;
      case binaryOp:
        if(e.tipo == binaryOp and valor == e.valor){
          if(valor == "+" or valor == "*"){
            return((hijos[0] == e.hijos[0] and hijos[1] == e.hijos[1]) or (hijos[0] == e.hijos[1] and hijos[1] == e.hijos[0]));
          }
          else{
            return(hijos[0] == e.hijos[0] and hijos[1] == e.hijos[1]);
          }
        }
        else{
          return false;
        }
        break;
    }
  }
};


struct Estado{
  std::vector <std::string> formula;
  double errorR = 0.0;
  std::vector<std::string> * vars;
  std::vector<std::vector<double> > * data;
  std::vector<double> * objetivo;
  bool timeoutVar = false;

  Estado(){}

  Estado(std::vector<std::string> iFormula, std::vector<std::string> *iVars, std::vector<std::vector<double> > *iData, std::vector<double> *iObjetivo, bool iUn = false){

    formula = iFormula;
    vars = iVars;
    data = iData;
    objetivo = iObjetivo;

    errorR = evaluar();

  }


  Estado(std::vector<std::string> *iVars, std::vector<std::vector<double> > *iData, std::vector<double> *iObjetivo){

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);
    std::uniform_int_distribution<> disVar(1, iVars -> size() - 1);
    std::uniform_int_distribution<> disOB(1, 4);

    if(dis(gen) <= 5){
      //Constante
      formula.push_back(std::to_string(dis(gen)));
    }else{
      //Variable
      formula.push_back((*iVars)[disVar(gen)]);
    }


    unsigned int opBin = disOB(gen);

    if(opBin == 1){
      formula.push_back("+");
    }
    else{
      if(opBin == 2){
        formula.push_back("-");
      }
      else{
        if(opBin == 3){
          formula.push_back("*");
        }
        else{
          formula.push_back("/");
        }
      }
    }


    if(dis(gen) <= 5){
      //Constante
      formula.push_back(std::to_string(dis(gen)));
    }else{
      //Variable
      formula.push_back((*iVars)[disVar(gen)]);
    }

    //formula.push_back(inputForm.substr(start, end - start));

    vars = iVars;
    data = iData;
    objetivo = iObjetivo;

    errorR = evaluar();

  }


  Estado(std::string inputForm, std::vector<std::string> *iVars, std::vector<std::vector<double> > *iData, std::vector<double> *iObjetivo){
    std::string delim = " ";
    auto start = 0U;
    auto end = inputForm.find(delim);

    while (end != std::string::npos){
        formula.push_back(inputForm.substr(start, end - start));
        start = end + delim.length();
        end = inputForm.find(delim, start);
    }
    formula.push_back(inputForm.substr(start, end - start));

    vars = iVars;
    data = iData;
    objetivo = iObjetivo;

    errorR = evaluar();
  }


  double evaluar(){

    std::vector<std::string> infFor = infixAPrefix(formula);
    //nodoArith formulaEvaluable(infFor);
    nodoArith formulaEvaluable;
    formulaEvaluable.asignarFormula(infFor);


    double error = 0.0, val = 0.0;
    unsigned int cont = 0;
    std::map<std::string, double> variables;
    for(unsigned int i = 0; i < objetivo -> size(); i++){

      variables.clear();
      for(unsigned int j = 0; j < data -> size(); j++){
        variables[(*vars)[j]] = (*data)[j][i];
      }
      //MSE
      //error += pow((*objetivo)[i] - formulaEvaluable.ejecutar(variables), 2);

      //MAE
      val = formulaEvaluable.ejecutar(variables);
      if(val != -999999999.0 and (*objetivo)[i] != -999999999.0){


        if((*objetivo)[i] >= 0.0 and (*objetivo)[i] <1.0){
          error += (std::abs((*objetivo)[i] - val));
        }else{
          error += (std::abs(((*objetivo)[i] - val)/(*objetivo)[i]));
        }
        cont++;
      }

    }


    error /= cont;
    error *= 100.0;
    return error;
  }




  double evaluarPrint(){

    nodoArith formulaEvaluable(formula);


    double error = 0.0, val = 0.0;
    unsigned int cont = 0;
    std::map<std::string, double> variables;
    for(unsigned int i = 0; i < objetivo -> size(); i++){

      variables.clear();
      for(unsigned int j = 0; j < data -> size(); j++){
        variables[(*vars)[j]] = (*data)[j][i];
      }
      //MSE
      //error += pow((*objetivo)[i] - formulaEvaluable.ejecutar(variables), 2);

      //MAE
      val = formulaEvaluable.ejecutar(variables);
      if(val != -999999999.0 and (*objetivo)[i] != -999999999.0){


        if((*objetivo)[i] >= 0.0 and (*objetivo)[i] <1.0){
          error += (std::abs((*objetivo)[i] - val));
        }else{
          error += (std::abs(((*objetivo)[i] - val)/(*objetivo)[i]));
        }
        cont++;
      }

    }


    error /= cont;
    error *= 100.0;

    std::cout << "MAPE: " << error << std::endl << std::endl << std::endl;
    return error;
  }


  std::vector<double> getVectorSol(){

        std::vector<std::string> infFor = infixAPrefix(formula);
        //nodoArith formulaEvaluable(infFor);
        nodoArith formulaEvaluable;
        formulaEvaluable.asignarFormula(infFor);


        std::vector<double> out;
        std::map<std::string, double> variables;
        for(unsigned int i = 0; i < objetivo -> size(); i++){
          variables.clear();
          for(unsigned int j = 0; j < data -> size(); j++){
            variables[(*vars)[j]] = (*data)[j][i];
          }
          out.push_back(formulaEvaluable.ejecutar(variables));
        }



        return out;
  }

  double getErrorR(){
    return errorR;
  }


  double getH(){
    return (errorR * (double)formula.size());
  }

  std::string getFormula(){
    std::vector<std::string> infFor = infixAPrefix(formula);
    //nodoArith formulaEvaluable(infFor);
    nodoArith formulaEvaluable;
    formulaEvaluable.asignarFormula(infFor);

    if(formulaEvaluable.noVars()){
      std::map<std::string, double> variables;
      return(std::to_string(formulaEvaluable.ejecutar(variables)));
    }else{
      std::string out = "";
      for (unsigned int i = 0; i < formula.size(); i++) {
        out += formula[i] + " ";
      }

      return out;
    }
  }

  std::string getFormulaPrefix(){
    std::vector<std::string> infFor = infixAPrefix(formula);
    //nodoArith formulaEvaluable(infFor);
    nodoArith formulaEvaluable;
    formulaEvaluable.asignarFormula(infFor);

    if(formulaEvaluable.noVars()){
      std::map<std::string, double> variables;
      return(std::to_string(formulaEvaluable.ejecutar(variables)));
    }else{
      std::string out = "";
      std::vector<std::string> formPre = infixAPrefix(formula);
      for (unsigned int i = 0; i < formPre.size(); i++) {
        out += formPre[i] + " ";
      }

      return out;
    }



  }

  bool final(double tope){
    if(errorR == 0.0){
      return true;
    }
    else{
      return ((errorR * 100.0)/tope) < 0.05;
    }
  }


  bool final(){
    return (errorR < 0.5);
  }

  void timeout(){timeoutVar = true;}

  bool getTO(){return timeoutVar;}

  std::vector<Estado> generarHijos(){

      std::vector <std::string> nForm;

      std::vector<std::string> opBin;
      opBin.push_back("+");
      opBin.push_back("*");
      opBin.push_back("-");
      opBin.push_back("/");

      std::vector<std::vector<std::string> > nuevasFormulas;
      std::vector<Estado> hijos;
      unsigned int tope = 0;

      for(unsigned int oB = 0; oB < opBin.size(); oB++){
        //Formulas que empiezan por el +, *, - y /
        //Constantes
        for(size_t i = 0; i < 11; i++) {
          nForm = formula;
          if((opBin[oB] != "*" or opBin[oB] != "/") and (i != 0)){
            nForm.push_back(opBin[oB]);
            nForm.push_back(std::to_string(i));
            nuevasFormulas.push_back(nForm);
          }
        }

        nForm = formula;
        nForm.push_back(opBin[oB]);
        nForm.push_back(std::to_string(-1));
        nuevasFormulas.push_back(nForm);

        tope = nuevasFormulas.size();
        //Variables
        for(size_t i = 0; i < (vars -> size()); i++){
          nForm = formula;
          nForm.push_back(opBin[oB]);
          nForm.push_back((*vars)[i]);
          nuevasFormulas.push_back(nForm);
        }
        //tope = nuevasFormulas.size();


        /*
        //Relaciones Simples
        //Suma y multiplicacion
        for(size_t i = 0; i < (vars -> size()); i++){
          for(size_t j = i+1; j < (vars -> size()); j++){
            nuevasFormulas.push_back(formula);
            nuevasFormulas[nuevasFormulas.size()-1].push_back(opBin[oB]);
            nuevasFormulas[nuevasFormulas.size()-1].push_back((*vars)[i]);
            nuevasFormulas[nuevasFormulas.size()-1].push_back("+");
            nuevasFormulas[nuevasFormulas.size()-1].push_back((*vars)[j]);

            nuevasFormulas.push_back(formula);
            nuevasFormulas[nuevasFormulas.size()-1].push_back(opBin[oB]);
            nuevasFormulas[nuevasFormulas.size()-1].push_back((*vars)[i]);
            nuevasFormulas[nuevasFormulas.size()-1].push_back("*");
            nuevasFormulas[nuevasFormulas.size()-1].push_back((*vars)[j]);
          }
        }
        tope = nuevasFormulas.size();

        //Resta y division
        for(size_t i = 0; i < (vars -> size()); i++){
          for(size_t j = 0; j < (vars -> size()); j++){
            if(i != j){
              nuevasFormulas.push_back(formula);
              nuevasFormulas[nuevasFormulas.size()-1].push_back(opBin[oB]);
              nuevasFormulas[nuevasFormulas.size()-1].push_back((*vars)[i]);
              nuevasFormulas[nuevasFormulas.size()-1].push_back("-");
              nuevasFormulas[nuevasFormulas.size()-1].push_back((*vars)[j]);

              nuevasFormulas.push_back(formula);
              nuevasFormulas[nuevasFormulas.size()-1].push_back(opBin[oB]);
              nuevasFormulas[nuevasFormulas.size()-1].push_back((*vars)[i]);
              nuevasFormulas[nuevasFormulas.size()-1].push_back("/");
              nuevasFormulas[nuevasFormulas.size()-1].push_back((*vars)[j]);
            }
          }
        }
        tope = nuevasFormulas.size();
        */
      }


      std::string formHijo;
      for(unsigned int i = 0; i < nuevasFormulas.size(); i++){
        formHijo = "";
        for(unsigned int j = 0; j < nuevasFormulas[i].size(); j++){
          formHijo += nuevasFormulas[i][j] + " ";
        }
        //std::cout << formHijo << std::endl;
        if(formHijo.size() > 0){
          hijos.push_back(Estado(formHijo, vars, data, objetivo));
        }
      }


      return hijos;

    }


  //Version A
  bool operator < (const Estado& e) const {
    return (errorR * (double)formula.size()) < (e.errorR * (double)e.formula.size());
  }

  bool operator > (const Estado& e) const {
    return (errorR * (double)formula.size()) > (e.errorR * (double)e.formula.size());
  }



  bool operator == (const Estado& e) const {
    if(formula.size() != e.formula.size()){
      return false;
    }
    else{
      return nodoArith(formula) == nodoArith(e.formula);
    }
  }

  Estado& operator = (const Estado &inEstado){
    //Comprueba que no se esté intentando igualar un objeto a sí mismo
    if(this!=&inEstado){
      formula = inEstado.formula;
      errorR = inEstado.errorR;
      vars = inEstado.vars;
      data = inEstado.data;
      objetivo = inEstado.objetivo;
      timeoutVar = inEstado.timeoutVar;
    }
    return *this;
  }

  std::string to_string(){
    std::string out = "";

    out += "\n++Formula Infija: ";
    for (unsigned int i = 0; i < formula.size(); i++) {
      out += formula[i] + " ";
    }

    out += "\n++Formula Prefija: ";
    std::vector<std::string> formPre = infixAPrefix(formula);
    for (unsigned int i = 0; i < formPre.size(); i++) {
      out += formPre[i] + " ";
    }

    out += "\n++Formula Arbol: " + nodoArith(infixAPrefix(formula)).printFunct() + "\n";

    out += "\n\t- errorR: " + std::to_string(errorR);
    out += "\n\t- Valor H: " + std::to_string(errorR * (double)formula.size());
   if(errorR == 0.0){
     out += "\n\t- Resultado exacto";
   }
   else{
     out += "\n\t- Resultado NO exacto";
   }

    return  out;
  }

};


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
