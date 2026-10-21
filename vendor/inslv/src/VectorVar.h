#ifndef _VECTORVARH_
#define _VECTORVARH_
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include "fuzzy_t.h"
#include "domain_t.h"
#include "variable_t.h"
#include "vectordouble.h"
#include "genetcode.h"
#include "DataSLib.hpp"

struct combinado{
   int op;
   int operando[2];
   double rango[2];
};

class VectorVar {
  private:
    int numero;
    variable_t *lista;
    combinado *oper;

  public:
    VectorVar();
    VectorVar(int tamano);
    VectorVar(const char *nom_fich);
    VectorVar(std::vector < std::vector < double > > * dataset, std::vector< std::pair<std::string, std::string> > attribLabels, std::map < std::string, std::vector <FuzzSet> > attribMap);
    ~VectorVar();
    VectorVar(const VectorVar &x);
    VectorVar &operator=(const VectorVar &x);
    void Asigna(int pos, const variable_t &x);
    void Asigna(int pos, int op, int var1, int var2, double min, double max);
    int N_Antecedente() const;

    void Encode(int &tb, int &te) const ;
    // void Decode(char *bin, int *ent);

    void Add_Variable(variable_t &var);
    void Add_Variable(variable_t &var, int op, int var1, int var2, double min, double max);
    void Add_Variable_Lista(variable_t &var, int op, int var1, int var2, double min, double max);


    void Pinta(int variable) const;
    void Pinta() const;
    std::string PintaS() const;
    std::string PintaS(int variable) const;
    std::string PintaS(int variable, int label) const;
    void PrintVar(int variable) const;
    string SPrintVar(int variable) const;
    void PrintDomain(int variable, int value) const;
    string SPrintDomain(int variable, int value) const;
    bool IsActiva(int variable) const;
    bool IsAntecedente(int variable) const;
    int TotalVariables() const;
    int SizeDomain(int variable) const;

    int Get_Unit(int variable) const;
    double Get_Convert_Factor(int variable) const;
    void Put_Unit(int variable, int Unit);
    void Put_Convert_Factor(int variable, double factor);

    double Adaptacion(double x, int variable) const;
    double Adaptacion(double x, int variable, int dominio) const;
    double Adaptacion(double x, int variable, string dominio) const;
    double Adaptacion(vectordouble x, string regla) const;
    bool Es_Valida(string regla, double *var, double umbral, double &simplicidad) const;
    void SecuenciasDeEtiquetasActivas(string sub, int tam, int &unos, int &ceros, int &n_unos) const;

    double Adaptacion(vectordouble x, string regla, double *var, double umbral) const;
    double Adaptacion_funcion(vectordouble x, string regla, int *lista_funcion, int tama, double adaptacion_previa) const;
    double Adaptacion_funcion_debug(vectordouble x, string regla, int *lista_funcion, int tama, double adaptacion_previa) const;
    double Adaptacion(vectordouble x, string regla, double *var, double umbral, double umbral2) const;
    double Adaptacion(vectordouble x, const genetcode &regla) const;
    void AdaptacionC(vectordouble x, int etiq, double &pos, double &neg) const;

    double Area(int var,int lab) const;
    fuzzy_t FuzzyLabel(int var,int lab) const;
    double CenterLabel(int var,int lab) const;
    bool IsDiscrete(int var) const;
    bool IsInterval(int var) const;
    bool IsFuzzy(int var) const;
    domain_t Domain(int var) const;
    variable_t Variable(int var) const;
    double Inf_Range(int var) const;
    double Sup_Range(int var) const;
    int Consecuente() const;
    void SaveBinaryCode(ofstream &f) const;
    void LoadBinaryCode(ifstream &f);

    vectordouble Convert_Example (vectordouble w) const;
    void Funcion(int pos, int &x1, int &x2, int &op) const;

    double CenterLabel(int i, int j){
      return lista[i].CenterLabel(j);
    }


};

#endif
