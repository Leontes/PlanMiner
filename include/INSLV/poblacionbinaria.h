#ifndef _POBLACIONBINARIAH_
#define _POBLACIONBINARIAH_

class poblacionbinaria{
  private:
    double prob_mutacion;
    double prob_cruce;
    int elitismo;
    int n_individuos;
    int *tamano;
    bool *modificado;
    double *valoracion;
    char **individuos;
  public:
    poblacionbinaria();
    poblacionbinaria(double mut, double cruce, int eli, int n);
    poblacionbinaria(double mut, double cruce, int eli, int n, int tama);
    poblacionbinaria(double mut, double cruce, int eli, int n, int *tama);
    poblacionbinaria(const poblacionbinaria &x);
    poblacionbinaria &operator=(const poblacionbinaria &x);
    ~poblacionbinaria();
    // Operaciones en la estructura
    char * Individuo(int i, int &tama) const;
    bool Modificado(int i) const;
    void Pasar(int i,const poblacionbinaria &x, int j);
    void Swap(int i, int j);
    void Sort();
    void Codigo(int i, char *&v, int &tama) const;
    void Pinta(int i) const;
    void PintaFitness(int i) const;
    void PintaInFile(int i) const;
    void PintaFitnessInFile(int i) const;
    void PutValue(int indiv, int bit, char value);
    // Operaciones geneticas
    void PoblacionInicialValor(char valor);
    void PoblacionInicialAleatoria();

    void PoblacionInicial(int **sujeto, int tama, int* tama_dom);
    void PoblacionInicial2_Estacionario(int **sujeto, int tama, int* tama_dom, int clase);



    void MutacionUniforme();
    void CruceUniforme();
    void Cruce2puntos();
    void MutacionUniforme_Estacionario();
    void Rotacion(int indiv);
    void CruceUniforme_Estacionario();
    void Cruce2puntos_Estacionario();
    void Cruce2puntos_Estacionario(int indiv1, int indiv2);
    void CruceAND_OR_Estacionario(int indiv1, int indiv2);
    void CruceNAND_NOR_Estacionario(int indiv1, int indiv2);
    //void And();
    //void Or();
    //void Not();
};



#endif
