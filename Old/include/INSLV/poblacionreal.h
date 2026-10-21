/***************************************************************************
                          poblacionreal.h  -  description
                             -------------------
    begin                : Tue Apr 30 2002
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
#ifndef _POBLACIONREALH_
#define _POBLACIONREALH_

class poblacionreal{
  private:
    double prob_mutacion;
    double prob_cruce;
    int elitismo;
    int n_individuos;
    int *tamano;
    bool *modificado;
    double rango_i;
    double rango_s;
    double **individuos;
  public:
    poblacionreal();
    poblacionreal(double raninf, double ransup, double mut, double cruce, int eli, int n);
    poblacionreal(double raninf, double ransup, double mut, double cruce, int eli, int n, int tama);
    poblacionreal(double raninf, double ransup, double mut, double cruce, int eli, int n, int *tama);
    poblacionreal(const poblacionreal &x);
    poblacionreal &operator=(const poblacionreal &x);
    ~poblacionreal();
    // Operaciones en la estructura
    double* Individuo(int i, int &tama) const;
    bool Modificado(int i) const;
    void Pasar(int i, const poblacionreal &x, int j);
    void Codigo(int i, double *&v, int &tama) const;
    void Swap(int i, int j);
    void Pinta(int i) const;
    void PintaBin(int i) const;
    void PintaInFile(int i) const;
    void PintaBinInFile(int i) const;
    void PutValue(int indiv, int bit, double value);
    // Operaciones geneticas
    void PoblacionInicialAleatoria();
    void PoblacionInicialAleatoria(double *v_pesos);    
    void PoblacionInicialAleatoria(int n_item);
    void PoblacionInicialAleatoria(double **I, int rango);
    void PoblacionInicialAleatoria(int vacio, int rango);
    void PoblacionInicialValor(double valor);
    void PoblacionInicialMedidaInformacion();

    void PoblacionInicial(double **I, int clases);
    void PoblacionInicial2_Estacionario();



    void MutacionUniforme();
    void CruceUniforme();
    void Cruce2puntos();
    void MutacionUniforme_Estacionario_Normal();
    void MutacionUniforme_Estacionario();
    void Rotacion(int i);
    void CruceUniforme_Estacionario();
    void Cruce_BLX_Estacionario(double alpha, int indiv1, int indiv2);
    void Cruce2puntos_Estacionario();
    void Cruce2puntos_Estacionario(int indiv1, int indiv2);
    void CruceAND_OR_Estacionario(int indiv1, int indiv2);
    void CruceNAND_NOR_Estacionario(int indiv1, int indiv2);
};



#endif
