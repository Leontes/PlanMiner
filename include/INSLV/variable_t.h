#ifndef _VARIABLE_TH_
#define _VARIABLE_TH_

#include <string>
#include <fstream>
#include "domain_t.h"
#include "fuzzy_t.h"
class variable_t{
   private:
     string nombre;     // Nombre de la variable
     domain_t *dominio; // Dominio de la variable
     bool activa;
     bool antecedente;
     int unit;
     double convert_factor;
   public:
     variable_t();
     variable_t(string &p_nombre, bool p_activa, bool p_antecedente, domain_t &dom);
     variable_t(string &p_nombre, bool p_activa, bool p_antecedente, domain_t &dom, int Unit, double ConvFactor);
     variable_t(const variable_t &x);
     variable_t &operator=(const variable_t &x);
     ~variable_t();
     void Asigna(int n, double inf, double sup, bool menosinf, bool masinf, string name);
     void Asigna(int n, string varname, int status, double inf, double sup, double *a,  double *b, double *c, double *d, string *name);
     void Asigna(int n, string varname, int status, int Unit, double c_factor, double inf, double sup, double *a,  double *b, double *c, double *d, string *name, bool singleton = false);
     double Adaptacion(double x) const;
     double Adaptacion(double x, int etiqueta) const;
     double Adaptacion(double x, string etiqueta) const;
     void Pinta() const;
     string PintaS() const;
     string PintaS(int label) const;
     void PrintVar() const;
     string SPrintVar() const;
     void PrintDomain(int value) const;
     string SPrintDomain(int value) const;
     int SizeDomain() const;
     bool Activa() const;
     bool Antecedente() const;
     int N_etiquetas() const;
     string Name() const { return nombre;}

     int Get_Unit() const;
     double Get_Convert_Factor() const;
     void Put_Unit(int unidad);
     void Put_Convert_Factor(double factor);



     double Area(int lab) const;
     fuzzy_t FuzzyLabel(int lab) const;
     double CenterLabel(int lab) const;
     bool IsDiscrete() const;
     bool IsInterval() const;
     bool IsFuzzy() const;
     domain_t Domain() const;
     variable_t Variable() const;
     double Inf_Range() const;
     double Sup_Range() const;
     void SaveBinaryCode(ofstream &f) const;
     void LoadBinaryCode(ifstream &f);

     double CenterLabel(int i){
       return dominio -> CenterLabel(i);
     }

};


#endif
