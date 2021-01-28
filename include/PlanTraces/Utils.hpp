#ifndef PTUTILS_HPP
#define PTUTILS_HPP

#include <vector>
#include <map>
#include <list>

#include <string>

#include <math.h>
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
std::vector < Type *> extractTypeHierarchy(std::vector< PlanTrace * > * PTS, std::map <std::string, std::vector <std::pair <std::string, std::string> > > * mapaPrimitivasParam,
  std::map <std::string, std::vector <std::pair <std::string, std::string> > > * mapaPredicadosParam);

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
  double ejecutar(std::map<std::string, double> & variables){
    switch (tipo) {
      case constant:
        return std::stod(valor);
        break;
      case variable:
        return variables[valor];
        break;
      case binaryOp:
        double val1 = hijos[0].ejecutar(variables);
        double val2 = hijos[1].ejecutar(variables);
        if(valor == "+"){
          if(val1 == -999999999.0 or val2 == -999999999.0){
            return -999999999.0;
          }
          else{
            return (val1 + val2);
          }
        }
        if(valor == "-"){
          if(val1 == -999999999.0 or val2 == -999999999.0){
            return -999999999.0;
          }
          else{
            return (val1 - val2);
          }
        }
        if(valor == "*"){
          if(val1 == -999999999.0 or val2 == -999999999.0){
            return -999999999.0;
          }
          else{
            return (val1 * val2);
          }
        }
        if(valor == "/"){

          if(val1 == -999999999.0 or val2 == -999999999.0){
            return -999999999.0;
          }
          else{
            if(val2 == 0.0){
              return -999999999.0;
            }
            else{
              return (val1 / val2);
            }
          }
        }
        if(valor == "^"){
          if(val1 == -999999999.0 or val2 == -999999999.0){
            return -999999999.0;
          }
          else{
            return pow(val1, val2);
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
    if(tipo != constant and tipo != variable){
      out += "( " + valor;
    }
    else{
      out += valor;
    }

    for(unsigned hijo = 0; hijo < hijos.size(); hijo++){
      out += " ";
      out += hijos[hijo].printFunct();
    }

      if(tipo != constant and tipo != variable){
      out += " )";
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

  void assign(std::string inputForm, std::vector<std::string> *iVars, std::vector<std::vector<double> > *iData, std::vector<double> *iObjetivo){
    std::string delim = " ";
    auto start = 0U;
    auto end = inputForm.find(delim);

    formula.clear();
    while (end != std::string::npos){
        formula.push_back(inputForm.substr(start, end - start));
        start = end + delim.length();
        end = inputForm.find(delim, start);
    }
    formula.push_back(inputForm.substr(start, end - start));

    vars = iVars;
    data = iData;
    objetivo = iObjetivo;

    errorR = evaluar(0.05);

    // std::vector<std::vector<std::string> > nuevasFormulas = generaFormulas(formula);
    //
    // std::vector<std::string> oldFormula = formula;
    // errorR = 1000000.0;
    //
    // double errorHijo = 0.0;
    // for(unsigned int i = 0; i < nuevasFormulas.size(); i++){
    //   formula = nuevasFormulas[i];
    //   errorHijo = evaluar(0.01);
    //   if(errorHijo < errorR){
    //     errorR = errorHijo;
    //   }
    // }
    // formula = oldFormula;
  }


  /** Method evaluar
  *   @brief Method to evaluate the arithmetic expression in the state with the data stored in it
  *   @retval Heuristic value of the state
  */
  double evaluar(double percent = 1.0){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, objetivo -> size());

    std::vector<std::string> infFor = infixAPrefix(formula);
    //nodoArith formulaEvaluable(infFor);
    nodoArith formulaEvaluable;
    formulaEvaluable.asignarFormula(infFor);

    double error = 0.0, val = 0.0;
    unsigned int cont = 0;
    std::map<std::string, double> variables;
    unsigned int ndatos = percent * objetivo -> size(), indice;
    for(unsigned int i = 0; i < ndatos; i++){
      if(percent == 1.0){
        indice = i;
      }
      else{
        indice = dis(gen);
      }

      variables.clear();
      for(unsigned int j = 0; j < data -> size(); j++){
        variables[(*vars)[j]] = (*data)[j][indice];
      }
      //MSE
      //error += pow((*objetivo)[i] - formulaEvaluable.ejecutar(variables), 2);

      //MAPE
      val = formulaEvaluable.ejecutar(variables);

      if((isnan(val) == false) and (val != -999999999.0) and ((*objetivo)[indice] != 999999999.0)){
        if((*objetivo)[indice] >-1.0 and (*objetivo)[indice] <1.0){
          error += (std::abs((*objetivo)[indice] - val));
        }else{
          error += (std::abs(((*objetivo)[indice] - val)/(*objetivo)[indice]));
        }
        cont++;
      }
    }

    if(cont != 0.0){
      error /= cont;
      error *= 100.0;
    }
    else{
      error = 99999999.0;
    }
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
  double getH() const{
    return errorR/10.0;
  }

  /** Method getC
  *   @brief Method to get the cost value of the arithmetic expression
  *   @retval double cost value
  */
  double getC() const {
    return 2.0 * ((double)formula.size());
  }

  /** Method getF
  *   @brief Method to get the value of the arithmetic expression
  *   @retval double cost value
  */
  double getF() const {
    return getH() + getC();
  }

  /** Method getVectFormula
  *   @brief Method to get the arithmetic expression
  *   @retval std::vector<std::string> arithmetic formula
  */
  std::vector<std::string> getVectFormula(){
    return formula;
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
      for (unsigned int i = 0; i < (formula.size()-1); i++) {
        out += formula[i] + " ";
      }

      out += formula[formula.size()-1];
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
      for (unsigned int i = 0; i < (formPre.size()-1); i++) {
        out += formPre[i] + " ";
      }
      out += formPre[formPre.size()-1];

      return out;
    }
  }

  std::string getFormulaArbol(){
    return nodoArith(infixAPrefix(formula)).printFunct();
  }

  /** Method final
  *   @brief Method to check if the state is final.
  *   @retval boolean value
  */
  bool final(){
    return (evaluar() < 2.0);
  }

  /** Method timeout
  *   @brief Method to set that the algorithm has been timeout
  */
  void timeout(){timeoutVar = true;}

  /** Method getTO
  *   @brief Method to check if the state is the result of a timed out search
  */
  bool getTO(){return timeoutVar;}


  std::vector<std::vector<std::string> > generaFormulas(std::vector <std::string> & formula){
    std::vector <std::string> nForm;
    std::vector<std::string> opBin;
    opBin.push_back("+");
    opBin.push_back("*");
    opBin.push_back("-");
    opBin.push_back("/");

    std::vector<std::vector<std::string> > nuevasFormulas;
    for(unsigned int oB = 0; oB < opBin.size(); oB++){
      //Formulas que empiezan por el +, *, - y /
      //Constantes
      for(size_t i = 0; i < 11; i++){
        if(formula.size() > 1){
          nForm = std::vector <std::string>(&formula[0], &formula[formula.size()-1]);
        }else{
          nForm = formula;
        }

        if(((opBin[oB] == "*") and (i > 1))
              or ((opBin[oB] == "+" or opBin[oB] == "-") and (i > 0))
                or ((opBin[oB] == "/") and (i > 1))){
          nForm.push_back(opBin[oB]);
          nForm.push_back(std::to_string(i));
          nuevasFormulas.push_back(nForm);
        }
      }

      //Variables
      for(size_t i = 0; i < (vars -> size()); i++){
        if(formula.size() > 1){
          nForm = std::vector <std::string>(&formula[0], &formula[formula.size()-1]);
        }else{
          nForm = formula;
        }
        nForm.push_back(opBin[oB]);
        nForm.push_back((*vars)[i]);
        nuevasFormulas.push_back(nForm);
      }
    }

    return nuevasFormulas;
  }


  /** Method generarHijos
  *   @brief Method to generate a vector with a set of child states created from the the objects
  *   @retvat STD vector with the states
  */
  std::vector<Estado> generarHijos(){
    std::vector<std::vector<std::string> > nuevasFormulas = generaFormulas(formula);
    std::vector<Estado> hijos;
    std::string formHijo;
    Estado hijo;
    for(unsigned int i = 0; i < nuevasFormulas.size(); i++){
      formHijo = "";
      for(unsigned int j = 0; j < nuevasFormulas[i].size(); j++){
        formHijo += nuevasFormulas[i][j] + " ";
      }
      //std::cout << formHijo << std::endl;
      if(formHijo.size() > 0){
        hijo.assign(formHijo, vars, data, objetivo);
        hijos.push_back(hijo);
      }
    }
    return hijos;
  }


  bool podar(){
    std::vector<std::vector<std::string> > formulasHijos = generaFormulas(formula);
    std::vector<std::vector<std::string> > formulasHijosHijos, aux;
    for(unsigned int i = 0; i < formulasHijos.size(); i++){
      aux = generaFormulas(formulasHijos[i]);
      formulasHijosHijos.insert(formulasHijosHijos.end(), aux.begin(), aux.end());
    }
    Estado estadohijo;
    double min = 10000000.0, dAux;
    std::string formHijo;
    for(unsigned int i = 0; i < formulasHijosHijos.size(); i++){
      formHijo = "";
      for(unsigned int j = 0; j < formulasHijosHijos[i].size(); j++){
        formHijo += formulasHijosHijos[i][j] + " ";
      }

      estadohijo.assign(formHijo, vars, data, objetivo);
      dAux = estadohijo.getH();
      if(dAux < min){
        min = dAux;
      }
    }

    std::cout << "\n-------\n";
    std::cout << getH() << std::endl;
    std::cout << min << std::endl;
    std::cout << "-------\n\n";

    return ((getH() - min) < 0.005);
  }

  /** Method operator <
  *   @brief Method to compare Estado objects
  *   @param innodo Estado to be compared
  */
  bool operator < (const Estado& e) const {
    double h1 = getF();
    double h2 = e.getF();
    return h1 < h2;
  }

  /** Method operator >
  *   @brief Method to compare Estado objects
  *   @param innodo Estado to be compared
  */
  bool operator > (const Estado& e) const {
    // if(errorR == e.errorR){
    //   return ((double)formula.size() > (double)e.formula.size());
    // }else{
    //   return (errorR > e.errorR);
    // }


    double h1 = getF();
    double h2 = e.getF();

    return h1 > h2;

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
  std::string to_string (){
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

    out += "\n\t- H(x): " + std::to_string(getH());
    out += "\n\t- F(x): " + std::to_string(getF());
    if (errorR == 0.0){
      out += "\n\t- Resultado exacto";
    }
    else{
      out += "\n\t- Resultado NO exacto";
    }

    return  out;
  }

};

std::vector <Estado> generarListaInit(std::vector<std::string> *problemVars, std::vector<std::vector<double> > *problemData, std::vector<double> *problemObj);
Estado Astar(std::vector <Estado> & estadosInit);

Estado RSPrueba(std::vector <Estado> & estadosInit);

void to_table( std::string task, StatesLists states, std::vector < std::vector < std::vector < double > > > * datasets, std::vector < std::vector < std::pair<std::string, std::string> > > * attribLabelsVC);
void discoverInfo(std::vector < std::vector < double > > * dataset, std::vector < std::pair<std::string, std::string> > * attribLabels);


#endif
