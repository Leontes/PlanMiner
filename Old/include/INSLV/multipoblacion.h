#ifndef _MULTIPOBLACIONH_
#define _MULTIPOBLACIONH_
#include "poblacionbinaria.h"
#include "poblacionentera.h"
#include "poblacionreal.h"
#include "genetcode.h"
#include "example_set.h"

class multipoblacion{
  private:
    int elitismo;
    int n_individuos;
    bool *modificado;
    int n_valoracion;
    double **valoracion;
    int poblacionesB;
    int poblacionesE;
    int poblacionesR;
    poblacionbinaria **Pb;
    poblacionentera **Pe;
    poblacionreal **Pr;
  public:
    multipoblacion();
    multipoblacion(int pbin, int pent, int preal, int *rangoe, double *rangori, double *rangors, double *mut, double *cruce, int eli, int n, int *tama, int n_val);
    multipoblacion(const multipoblacion &x);
    multipoblacion &operator=(const multipoblacion &x);
    ~multipoblacion();
    // Operaciones en la estructura
    void Swap(int i, int j);
    void Pasar(int i, int j);

    int ClaseIndividuo(int ind) const;
    int ValorIndividuo(int population, int ind) const;
    char* Subpoblacion_Binaria(int subpoblacion, int individuo, int &tama) const;
    int* Subpoblacion_Entera(int subpoblacion, int individuo, int &tama) const;
    double* Subpoblacion_Real(int subpoblacion, int individuo, int &tama) const;
    void Sort(int n_clases, int* examples_per_class);
    void Sort(int n_clases);
    void Sort();
    void Sort_menos2ultimos();

    double Sort_4L(example_set &E, int n_clases, int* examples_per_class, int**ind_clase, double *agregado, double *peso_agregado, int total_ejemplos_particion);
    void Better(int nclases, int *better, int &nbetter, int **ind_clase) const;
    void Pinta(int i) const;
    void Pinta() const;
    void PintaInFile(int i) const;
    void PintaInFile() const;
    double MediaFitness() const;
    double MediaFitness_Estacionario() const;
    double MediaFitness_Estacionario(double &min_f0, double &max_f1, double &min_f2, double &f2, double &min_f3, double &max_f3) const;
    void PintaFitness(int i) const;
    void PintaFitnessInFile(int i) const;
    void PintaFitness_Estacionario(int i) const;
    void PintaIndividuo(int i) const;
    int N_individuos() const;
    int Elite() const;
    void Codigo(int i, genetcode &code) const;
    void PutCodigo(int i, const genetcode &code);
    bool Modificado(int i) const;
    void PutModificado(int i);
    int N_Valoracion() const;
    void Valoracion(int i, double *dato);
    double ValorFitness(int i) const;
    double ValorFitness(int i, int j) const;

    // Operaciones geneticas
    void PoblacionInicialAleatoria();
    void PoblacionInicialAleatoria(double *v_pesos);
    void PoblacionInicialAleatoria(double **I, int rango);
    void PoblacionInicialAleatoria_4L(double **I, int rango, int n_items);
    void PoblacionInicialAleatoria_4L(int rango, int n_items);

    void PoblacionInicial(double **I, int rango, int n_items, int **sujetos, int n_var, int *n_etiquetas_variable);
    void PoblacionInicial(int rango, int n_items, int **sujetos, int n_var, int *n_etiquetas_variable);
    void PoblacionInicial2_Estacionario(double **I, int rango, int n_items, int **sujetos, int n_var, int *n_etiquetas_variable, int clase);



    void PoblacionInicialAleatoria(int consecuente);
    void MutacionUniforme();
    void MutacionUniformeModEnt(double prob0);
    void CruceUniforme();
    void Cruce2puntos();
    void MutacionUniforme_Estacionario_reales();

    void MutacionUniforme_Estacionario_Especial();

    void MutacionUniforme_Estacionario();
    void MutacionUniformeModEnt_Estacionario(double prob0);
    void CruceUniforme_Estacionario();
    void Cruce2puntos_Estacionario();
    void CruceBasedLogical_Estacionario_reales(int it);
    void CruceBasedLogical_Estacionario(double it);
    //void And();
    //void Or();
    //void Not();
    void Seleccion();
    bool Diferencia(int i, int j, int n, double minimo) const;
    void Cubrimiento(int i, double *conjunto, int n_examples);
    void RecalcularFitness(int i, double *conjunto, int n_examples);

    double Incremento_Acierto(double *agregado, double *peso_agregado, double *adap_individuo, double peso, int n_examples) const;
    void Combinar(double *agregado, double *peso_agregado, double *disparada, double *adap_individuo, double peso, int n_examples, int ind) const;
    double Refitness(double *agregacion, double *peso_agregacion, int ind, int n_examples);


};



#endif
