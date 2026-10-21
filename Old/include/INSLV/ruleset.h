/***************************************************************************
                          ruleset.h  -  description
                             -------------------
    begin                : Mon May 6 2002
    copyright            : (C) 2002 by Ral P�ez
    email                : fgr@decsai.ugr.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _RULESETH_
#define _RULESETH_

#include <fstream>
#include <list>
#include <utility>

#include "genetcode.h"
#include "VectorVar.h"
#include "vectordouble.h"
#include "Relaciones.h"
class ruleset {
  private:
    int reservado;
    int n_rule;
    genetcode **rule;
    VectorVar *domain;
    VectorVar *domain_funcion;
    double *peso;
    double *n_mas;
    double *n_menos;
    bool *modificada;
    void Realloc();

  public:
    /**
    @brief Constructor sin argumentos de un objeto de la clase ruleset.
    @post Construye un conjunto de reglas vacio.
    */
    ruleset();
    /**
    @brief Constructor de un objeto de la clase ruleset con un dominio
           asociado.
    @param x (VectorVar) es un objeto que contiene el conjunto de
           variables que describen la sem�tica de las reglas.
    @pre  El objeto x debe estar creado.
    @post Se vincula al objeto creado la sem�tica de las reglas.

    Construye un conjunto de reglas vacio al que se le
          asocia el dominio sobre el que las variables que
          conforman el antecedente y consecuente de las regla
          tomar� sus valores.
    */
    ruleset(const VectorVar &x);


    /**
    @brief Constructor de un objeto de la clase ruleset a partir
           de un fichero que contiene un conjunto de reglas.
    @param nomfich (string) es el nombre del fichero de reglas.

    Construye un objeto ruleset a partir de un fichero que
    contiene un conjunto de reglas.
    */
    //ruleset(string nomfich);

    /**
    @brief Constructor de copia de los objetos ruleset.
    */
    ruleset(const ruleset &x);

    /**
    @brief Redefinici� del operador de asignaci� entre objetos de
           la clase ruleset.
    @param x (ruleset) el objeto que desea ser asignado.
    @return una copia del objeto x.
	  */
    ruleset &operator=(const ruleset &x);



    /**
    @brief Destructor de objetos ruleset.
    */
    ~ruleset();

   /**
   @brief Añade el dominio x a un objeto de la clase ruleset
   */
    void AddDomain(const VectorVar &x);

    /**
    @brief Permite a�dir una nueva regla a un objeto ruleset.
    @param x (genetcode) representa la regla que se desea a�dir. Esta
           regla se expresa mediante un objeto de la clase genetcode.
           Para m� informaci� ver la especificaci� de esta clase.
    */

    void AddDomain_Funcion(const VectorVar &x);

    void Add(const genetcode &x);

    void Add(const genetcode &x, double weight);

    void Add(const genetcode &x, double mas, double menos, bool modif);

    void Add_Weight(int rule, double weight);

    void Add_Weight(int rule, double mas, double menos);


    double Get_Weight(int rule) const;

    void Recalculate_Weight(int i);

    void Recalculate_Weight();

    void Add_N_Mas(int rule, double value);

    double Get_N_Mas(int rule) const;

    void Put_N_Mas(int rule, double value);

    void Put_N_Menos(int rule, double value);

    void Add_N_Menos(int rule, double value);

    double Get_N_Menos(int rule) const;

    double Put_N_Menos(int rule) const;

    void Modificada(int rule, bool value);

    double Get_Modificada(int rule) const;

    void FixModifyToTrue();


    void Anular_N_Mas_y_N_Menos(int i);

    void Anular_N_Mas_y_N_Menos_Todas();


    void Remove();

    genetcode *Extract(int i, double &weight);

    genetcode *Extract(int i, double &mas, double &menos, bool modif);


    void Insert(int i, genetcode *r, double weight);

    void Insert(int i, genetcode *r, double mas, double menos, bool modif);

    genetcode *Get_Code_Rule(int i) const;

    void AddRuleset(ruleset &r);


    /**
      @brief Devuelve el numero de reglas que contiene el conjunto de reglas.
    */
    int N_rule() const;


    /**
      @brief Devuelve el numero de reglas del conjunto de reglas con peso superior a weight.
    */
    int N_rule(double weight) const;

    /**
      @brief Devuelve el numero medio de variables por regla
    */
    double Variables_per_rule() const;
    int Variables_of_rule(int i) const;
    int New_Atributes_of_rule(int i) const;
    int Stable_of_rule(int i) const;
    double *SeeAdaptaciones_of_rule(int i) const;


    int Numero_Ceros_of_rule(int i) const;

    /**
      @brief Devuelve la proporcion de variables usadas en relacion a las variables involucradas
    */
    double Variables_Used() const;


    /**
      @brief Devuelve la proporcion de variables usadas en relacion a las variables involucradas y ademas en frec devuelve las veces que cada variable aparece en la base de reglas.
      @param frec (vector de enteros) cuya reserva debe hacerse antes de llamar a la funcion.
    */
    double Frecuence_each_Variables(int *frec) const;

    int Condiciones_per_RB() const;

    /**
    @brief Devuelve la clase a la que pertenece un ejemplo.
    @param v (vectordouble) es un vector de reales que representa
           un ejemplo que se desea clasificar.
    @return el ordinal de la clase a la que pertenece.
    */
    int InferenceC(const vectordouble &v, const Relaciones &RI) const;

    int InferenceC(const vectordouble &v, const Relaciones &RI, double &grado) const;

    int InferenceCPeso(const vectordouble &v, const Relaciones &RI,
                       double &grado) const;

    /**
    @brief Devuelve la clase a la que pertenece un ejemplo.
    @param v (vectordouble) es un vector de reales que representa
           un ejemplo que se desea clasificar.
    @return el ordinal de la clase a la que pertenece.
    */
    int InferenceC(const vectordouble &v) const;


    /**
    @brief Devuelve la clase a la que pertenece un ejemplo.
    @param v (vectordouble) es un vector de reales que representa
           un ejemplo que se desea clasificar.
    @param grado (real) es el grado de adaptacion con el mejor antecedente
           de las reglas existentes en la base de datos.
    @return el ordinal de la clase a la que pertenece.
    */
    int InferenceC(const vectordouble &v, double &grado) const;

    int InferenceC(const vectordouble &v, double &grado, int &regla_disparada) const;

    int InferenceC(const vectordouble &v, int clase, double &grado_p, double &peso_p, double &grado_n, double &peso_n, int &regla_disparada, int &aislada) const;

    int InferenceC_debug(const vectordouble &v, int clase, double &grado_p, double &peso_p, double &grado_n, double &peso_n, int &regla_disparada, int &aislada) const;

    /**
    @brief Devuelve el valor que resulta de aplicar el ejemplo al
           objeto ruleset.
    @param v (vectordouble) es un vector de reales que representa
           un ejemplo que se desea clasificar.
    @return el valor resultante de la inferencia del ejemplo.
    */
    double InferenceF(const vectordouble &v) const;

    /**
    @brief Devuelve un objeto de tipo VectorVar que contiene el nmero
           de variables que estan relacionados con el conjunto de reglas
           as�como la sem�ica de los valores que puede tomar
           cada una de las variables.
    @return Devuelve un objeto de tipo VectorVar que es una copia de
            la informaci� contenida dentro del objeto ruleset.
    */
    VectorVar ArrayVar() const;

    /**
    @brief Guarda en un archivo la informaci� contenida en un
           objeto ruleset.
    @param nomfich (string) nombre del fichero donde se desea
			     almacenar el conjunto de reglas.
    */
    void Save(string nomfich) const;

    int Class_of_rule (int i) const;


    /**
    @brief Carga desde un archivo un conjunto de reglas.
    @param nomfich (string) nombre del fichero donde se encuentra
			     almacenado un conjunto de reglas.
    */
    void Load(string nomfich);


    void Print(int rule) const;

    string SPrint(int i) const;

    string SPrint_Consequent(int i) const;


    void SaveRuleInterpreted(const char *nomfich) const;

    void SaveRuleInterpreted_append(const char *nomfich) const;


    void Print(int rule, const Relaciones &RI) const;

    string SPrint(int i, const Relaciones &RI) const;

    void SaveRuleInterpreted(const char *nomfich, const Relaciones &RI) const;

    void SaveRuleInterpreted(const char *nomfich, const Relaciones &RI,
                             double training, double test) const;

    void toPDDL(vector < pair <int, list< pair<string, string> > > > * reglas) const;
    list<pair<string, string>  > SplitAntecedent(int i) const;

};
#endif
