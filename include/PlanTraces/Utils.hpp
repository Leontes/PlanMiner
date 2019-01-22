#ifndef PTUTILS_HPP
#define PTUTILS_HPP

#include <vector>
#include <map>
#include <list>

#include <string>

#include <cmath>
#include <random>
#include <regex>
#include <stack>

#include "StateLib.hpp"
#include "PlanTraces/PlanTrace.hpp"

enum type {constant, variable, binaryOp};

const std::regex planRE("New plan!!!");
const std::regex taskRE("##Tasks##");
const std::regex stateRE("##States##");

//Auxiliary functions
bool is_number(const std::string& s);
bool is_var(const std::string& s);
int precedencia(std::string str);
std::vector<std::string> infixAPrefix(std::vector<std::string> &formInf);



/** Method ParseGoals
*   @brief Method to parse an string and create a pointer to an Goal object. Unimplemented
*/
void parseGoals();


/** Method ParseTask
*   @brief Method to parse an string and create a pointer to an Task object.
*   @param taskSTR  String to be parsed.
*   @retval Pointer to Task type object
*/
Task * parseTask(std::string taskSTR);

/** Method ParseState
*   @brief Method to parse an string and create a pointer to an State object.
*   @param taskSTR  String to be parsed.
*   @retval Pointer to State type object
*/
State * parseState(std::string stateSTR);

/** Method parse
*   @brief Method to parse a set of plan traces stored in a file.
*   @param filename  String with the plan trace
*   @retval STD vector with pointers to the plan traces stored in the file
*/
std::vector< PlanTrace * > * parse(const char * filename);

/** Method groupTaskStates
*   @brief Method to group actions in a set of plan traces in pairs pre-state/post-state asociated with a action.
*   @param  PTS STD vector with pointers to the plan traces
*   @retval STD MAP with the plan traces' actions and it's pairs associated
*/
std::map < std::string, StatesLists > groupTaskStates(std::vector< PlanTrace * > * PTS);

/** Method extractTypeHierarchy
*   @brief Method to calculate and extract the types of hierarchy of types of the problem
*   @param PTS STD vector with pointers to the plan traces
*   @param mapaPrimitivasParam Tasks' original headers
*   @retval STD vector collection of extracted types
*/
std::vector < Type *> extractTypeHierarchy(std::vector< PlanTrace * > * PTS, std::map <std::string, std::vector <std::pair <std::string, std::string> > > * mapaPrimitivasParam);

/** Method to_table
*   @brief Method to display a collection of pairs pre-state/post-state as a attribute-value matrix.
*   @param  task string with the name of the task associated with the input states
*   @param  states list of pairs pre-state/post-state
*   @param  dataset Output pair of STD matrices of double values
*   @param  attribLabelsVC Output pair of STD vector with the labels of the datasets' columns
*/
void to_table( std::string task, StatesLists states, std::vector < std::vector < std::vector < double > > > * datasets, std::vector < std::vector < std::pair<std::string, std::string> > > * attribLabelsVC);



/*!
  Arithmetic node struct for the Simbolic Regressor. Each nodoArith object represents an arithmetic formula.
*/
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


  /** Method asignarFormula
  *   @brief Method to create a tree structure of nodoArith objects that represent a given formula.
  *   @param  formula Arithmetic expression to be represented
  */
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

  /** Method ejecutar
  *   @brief Method to evaluate the formula defined in the object with a collecion of given values
  *   @param  variables set of input variables
  *   @retval result of the evaluation
  */
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

  /** Method noVars
  *   @brief Method to check if the formula represented in the object has no variables
  *   @retval result of the evaluation
  */
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

  /** Method size
  *   @brief Method to count the number of elements of the arithmetic expression
  *   @retval number of elements
  */
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

  /** Method clearHijos
  *   @brief Method to reset the arithmetic expression of the object
  */
  void clearHijos(){
    for(unsigned int i = 0; i < hijos.size(); i++){
      hijos[i].clearHijos();
    }
    hijos.clear();
  }

  /** Method getNodo
  *   @brief Method to create a copy of a nodoArith object (self or child)
  *   @param i element to be copied
  *   @retval Copy of the object
  */
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

  /** Method printFunct
  *   @brief Method to print the arithmetic expression defined by the object
  *   @retval string with the arithmetic expression to be printed
  */
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

  /** Method operator =
  *   @brief Method to assing nodoArith objects
  *   @param innodo nodoArith to be assigned
  */
  nodoArith & operator = (const nodoArith &innodo){
    if(this!=&innodo){
      tipo = innodo.tipo;
      valor = innodo.valor;
      hijos = innodo.hijos;
    }
    return *this;
  }

  /** Method operator ==
  *   @brief Method to compare nodoArith objects
  *   @param e nodoArith to be compared
  */
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


/*!
  Node for the A* search algorithm.
*/
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


  /** Method evaluar
  *   @brief Method to evaluate the arithmetic expression in the state with the data stored in it
  *   @retval Heuristic value of the state
  */
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

  /** Method getVectorSol
  *   @brief Method to get the arithmetic expression in the state
  *   @retval STD vector with the expression
  */
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

  /** Method getErrorR
  *   @brief Method to get the MAPE value of the arithmetic expression
  *   @retval double MAPE value
  */
  double getErrorR(){
    return errorR;
  }

  /** Method getH
  *   @brief Method to get the heuristic value of the arithmetic expression
  *   @retval double heuristic value
  */
  double getH(){
    return (errorR * (double)formula.size());
  }

  /** Method getFormula
  *   @brief Method to get a printable version of the arithmetic expression
  *   @retval string arithmetic formula
  */
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

  /** Method getFormulaPrefix
  *   @brief Method to get a printable version of the arithmetic expression. Expression in prefix form.
  *   @retval string arithmetic formula
  */
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

  /** Method final
  *   @brief Method to check if the state is final.
  *   @retval boolean value
  */
  bool final(){
    return (errorR < 0.5);
  }

  /** Method timeout
  *   @brief Method to set that the algorithm has been timeout
  */
  void timeout(){timeoutVar = true;}

  /** Method getTO
  *   @brief Method to check if the state is the result of a timed out search
  */
  bool getTO(){return timeoutVar;}

  /** Method generarHijos
  *   @brief Method to generate a vector with a set of child states created from the the objects
  *   @retvat STD vector with the states
  */
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

  /** Method operator <
  *   @brief Method to compare Estado objects
  *   @param innodo Estado to be compared
  */
  bool operator < (const Estado& e) const {
    return (errorR * (double)formula.size()) < (e.errorR * (double)e.formula.size());
  }

  /** Method operator >
  *   @brief Method to compare Estado objects
  *   @param innodo Estado to be compared
  */
  bool operator > (const Estado& e) const {
    return (errorR * (double)formula.size()) > (e.errorR * (double)e.formula.size());
  }


  /** Method operator ==
  *   @brief Method to compare Estado objects
  *   @param innodo Estado to be compared
  */
  bool operator == (const Estado& e) const {
    if(formula.size() != e.formula.size()){
      return false;
    }
    else{
      return nodoArith(formula) == nodoArith(e.formula);
    }
  }

  /** Method operator =
  *   @brief Method to assing Estado objects
  *   @param innodo Estado to be assigned
  */
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

  /** Method to_string
  *   @brief Method to obtain a printable version of the Estado object
  *   @param string with the Estado information
  */
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

#endif
