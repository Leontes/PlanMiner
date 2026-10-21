#ifndef _DOMAIN_TH_
#define _DOMAIN_TH_
#include <fstream>
#include <string>
#include "fuzzy_t.h"

class domain_t {
   private:
     int numero;        // Numero de etiquetas
     double rango_inf;  // rango inferior
     double rango_sup; // rango superio
     bool inf_inf;      // determina si el rango inferior es infinito
     bool sup_inf;      // determina si el rango superior es infinito
     fuzzy_t *label;    // vector de etiquetas
   public:
     domain_t();
     domain_t(int n, double inf, double sup, bool menosinf, bool masinf);
     ~domain_t();
     domain_t (const domain_t &x);
     domain_t &operator=(const domain_t &x);
     void Asigna(int n, double inf, double sup, bool menosinf, bool masinf);
     void Asigna_Op_menos(int n, double inf, double sup, bool menosinf, bool masinf);

     void Asigna(int n, double inf, double sup, bool menosinf, bool masinf, int desp);
     void Asigna(int n, double inf, double sup, double *a, double *b, double *c, double *d);
     void Asigna(int n, double inf, double sup, double *a, double *b, double *c, double *d, string *name, bool singleton = false);
     int N_etiquetas() const;
     double Adaptacion(double x) const;
     double Adaptacion(double x, const int etiqueta) const;
     double Adaptacion(double x, string etiquetas) const;


     void Amplia_Delta();
     void Reduce_Delta();


     void Pinta() const;
     std::string PintaS() const;
     std::string PintaS(int lab) const;
     void Pinta(int i) const;
     void Print(int i) const;
     string SPrint(int i) const;
     string SPinta(int i) const;
     int Size() const;
     fuzzy_t FuzzyLabel(int label) const;
     double CenterLabel(int label) const;
     bool IsDiscrete() const;
     bool IsInterval() const;
     bool IsFuzzy() const;
     double Area(int label) const;
     domain_t Domain() const;
     double Inf_Range() const;
     double Sup_Range() const;
     void SaveBinaryCode(ofstream &f) const;
     void LoadBinaryCode(ifstream &f);
};

#endif
