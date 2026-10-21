#ifndef _POBLACIONENTERAH_
#define _POBLACIONENTERAH_

class poblacionentera{
  private:
    double prob_mutacion;
    double prob_cruce;
    int elitismo;
    int n_individuos;
    int *tamano;
    bool *modificado;
    double *valoracion;
    int rango;
    int **individuos;
  public:
    poblacionentera();
    poblacionentera(int rango, double mut, double cruce, int eli, int n);
    poblacionentera(int rango, double mut, double cruce, int eli, int n, int tama);
    poblacionentera(int rango, double mut, double cruce, int eli, int n, int *tama);
    poblacionentera(const poblacionentera &x);
    poblacionentera &operator=(const poblacionentera &x);
    ~poblacionentera();
    // Operaciones en la estructura
    int* Individuo(int i, int &tama) const;
    bool Modificado(int i) const;
    void Pasar(int i, const poblacionentera &x, int j);
    void Codigo(int i, int *&v, int &tama) const;
    void Swap(int i, int j);
    void Sort();
    void Pinta(int i) const;
    void PintaFitness(int i) const;
    void PintaInFile(int i) const;
    void PintaFitnessInFile(int i) const;
    void PutValue(int indiv, int bit, int value);
    int GetValue(int individuo, int bit) const;
    // Operaciones geneticas
    void PoblacionInicialAleatoria();
    void PoblacionInicialAleatoria(int consecuente);

    void PoblacionInicial2_Estacionario(int clase);
    void PoblacionInicial2_Estacionario();


    void MutacionUniforme();
    void MutacionUniformeMod(double prob0);
    void CruceUniforme();
    void Cruce2puntos();
    void MutacionUniforme_Estacionario();
    void MutacionUniformeMod_Estacionario(double prob0);
    void MutacionUniformeMod_Estacionario_Especial();

    void CruceUniforme_Estacionario();
    void Cruce2puntos_Estacionario();
    void Cruce2puntos_Estacionario(int indiv1, int indiv2);

};



#endif
