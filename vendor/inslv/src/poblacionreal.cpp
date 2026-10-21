/***************************************************************************
                          poblacionreal.cpp  -  description
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
using namespace std;
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "INSLV/poblacionreal.h"
#include "INSLV/operaciones.h"

poblacionreal::poblacionreal(){
  prob_mutacion=0.0;
  prob_cruce=0.0;
  elitismo=0;
  n_individuos=0;
  tamano=0;
  modificado=0;
  individuos=0;
  rango_i=0;
  rango_s=0;
}



poblacionreal::poblacionreal(double raninf, double ransup, double mut, double cruce, int eli, int n){
  prob_mutacion=mut;
  prob_cruce=cruce;
  elitismo=eli;
  n_individuos=n;
  rango_i=raninf;
  rango_s=ransup;
  tamano= new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=0;

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  individuos = new double*[n_individuos];
  for (int i=0; i<n_individuos; i++)
    individuos[i] = 0;
}




poblacionreal::poblacionreal(double raninf, double ransup, double mut, double cruce, int eli, int n, int *tama){
  prob_mutacion=mut;
  prob_cruce=cruce;
  elitismo=eli;
  n_individuos=n;
  rango_i=raninf;
  rango_s=ransup;
  tamano= new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=tama[i];

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  individuos = new double*[n_individuos];
  for (int i=0; i<n_individuos; i++)
    individuos[i] = new double[tamano[i]];
}



poblacionreal::poblacionreal(double raninf, double ransup, double mut, double cruce, int eli, int n, int tama){
  prob_mutacion=mut;
  prob_cruce=cruce;
  elitismo=eli;
  n_individuos=n;
  rango_i=raninf;
  rango_s=ransup;
  tamano= new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=tama;

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  individuos = new double*[n_individuos];
  for (int i=0; i<n_individuos; i++)
    individuos[i] = new double[tamano[i]];
}



poblacionreal::poblacionreal(const poblacionreal &x){
  prob_mutacion=x.prob_mutacion;
  prob_cruce = x.prob_cruce;
  elitismo = x. elitismo;
  n_individuos = x.n_individuos;
  rango_i=x.rango_i;
  rango_s=x.rango_s;

  tamano = new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=x.tamano[i];


  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=x.modificado[i];


  individuos = new double*[n_individuos];
  for (int i=0; i<n_individuos; i++) {
    if (tamano[i]>0){
      individuos[i] = new double[tamano[i]];
      for (int j=0; j<tamano[i]; j++)
        individuos[i][j]=x.individuos[i][j];
    }
  }

}





poblacionreal &poblacionreal::operator=(const poblacionreal &x){
 if (this!=&x){
  // Eliminar el objeto que recibe la asignacion
  if (n_individuos>0){
    if (tamano!=0)
      delete [] tamano;

    if (modificado!=0)
      delete [] modificado;


    if (individuos!=0){
      for (int i=0; i<n_individuos; i++)
        if (individuos[i]!=0){
          delete [] individuos[i];
	        individuos[i]=0;
        }
      delete [] individuos;
    }
  }

  // copiar el objeto
  prob_mutacion=x.prob_mutacion;
  prob_cruce = x.prob_cruce;
  elitismo = x. elitismo;
  n_individuos = x.n_individuos;
  rango_i=x.rango_i;
  rango_s=x.rango_s;

  tamano = new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=x.tamano[i];

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=x.modificado[i];

  individuos = new double*[n_individuos];
  for (int i=0; i<n_individuos; i++) {
    if (tamano[i]>0){
      individuos[i] = new double[tamano[i]];
      for (int j=0; j<tamano[i]; j++)
        individuos[i][j]=x.individuos[i][j];
    }
  }

 }
 return *this;
}



poblacionreal::~poblacionreal(){
  if (n_individuos>0){
    delete [] tamano;
    delete [] modificado;
    for (int i=0; i<n_individuos; i++)
      if (individuos[i]!=0){
        delete [] individuos[i];
	      individuos[i]=0;
      }
    delete [] individuos;
  }

  n_individuos=0;
}

double* poblacionreal::Individuo(int i, int &tama) const{
  tama = tamano[i];
  return individuos[i];
}


bool poblacionreal::Modificado(int i) const{
 return modificado[i];
}

void poblacionreal::Pasar(int i, const poblacionreal &x, int j){
  if (tamano[i]>0)
    delete[] individuos[i];

  tamano[i]=x.tamano[i];
  individuos[i]= new double[tamano[i]];
  for (int k=0; k<tamano[i]; k++)
    individuos[i][k]=x.individuos[j][k];
}


void poblacionreal::Swap(int i, int j){
  Swap_int(tamano[i],tamano[j]);
  Swap_bool(modificado[i],modificado[j]);
  double *p=individuos[i];
  individuos[i]=individuos[j];
  individuos[j]=p;
}



void poblacionreal::Codigo(int i, double *&v, int &tama) const{
  //if (v!=0)
    //delete [] v;

  tama=tamano[i];
  v=new double[tama];
  for (int j=0;j<tama;j++)
    v[j]=individuos[i][j];
}



void poblacionreal::Pinta(int i) const{
  for (int j=0; j<tamano[i]; j++)
    cout << individuos[i][j] << " ";
  cout << endl;
}


void poblacionreal::PintaBin(int i) const{
  for (int j=0; j<tamano[i]-1; j++)
    if(individuos[i][j]<individuos[i][tamano[i]-1])
      cout << "0";
    else
      cout << "1";
  cout << endl;
}

void poblacionreal::PintaInFile(int i) const{
  ofstream f("slave.log",ios::app);
  for (int j=0; j<tamano[i]; j++)
    f << individuos[i][j] << " ";
  f << endl;
  f.close();
}


void poblacionreal::PintaBinInFile(int i) const{
  ofstream f("slave.log",ios::app);
  for (int j=0; j<tamano[i]-1; j++)
    if(individuos[i][j]<individuos[i][tamano[i]-1])
      f << "0";
    else
      f << "1";
  f << endl;
  f.close();
}



void poblacionreal::PutValue(int indiv, int bit, double value){
  individuos[indiv][bit]=value;
}



void poblacionreal::PoblacionInicialAleatoria(){
  double a;
  for (int i=0; i<n_individuos; i++) {
    for (int j=0; j<tamano[i]-1; j++){
        //a=(1.0*(rango_s-rango_i)*rand())/(RAND_MAX+1.0);
        //individuos[i][j]= rango_i+a;
        individuos[i][j]=0.8;
    }
    individuos[i][tamano[i]-1]=0.5;
  }
}


void poblacionreal::PoblacionInicialAleatoria(double *v_pesos){
  double a;
  for (int i=0; i<n_individuos; i++) {
    for (int j=0; j<tamano[i]; j++){
       //a=(1.0*(rango_s-rango_i)*rand())/(RAND_MAX+1.0);
	//if (a<0.99)
           //individuos[i][j]=v_pesos[i];
	//else
	   individuos[i][j]=v_pesos[j]+(1.0*(0.01)*rand())/(RAND_MAX+1.0)-0.005;
    }
  }
  for (int j=0; j<tamano[0]; j++)
    individuos[0][j]=v_pesos[j];
}


void poblacionreal::PoblacionInicialAleatoria(int n_item){
  for (int i=0; i<n_individuos; i++)
    individuos[i][tamano[i]-1]=n_item;
}


void poblacionreal::PoblacionInicialAleatoria(double **I, int rango){
  double a;
  double max=I[0][0], min=I[0][0];

  for (int j=0; j<tamano[0]-1; j++){
      //a=(1.0*(rango_s-rango_i)*rand())/(RAND_MAX+1.0);
      //individuos[i][j]= rango_i+a;
      if (I[j][0]>max)
        max=I[j][0];
      else if (I[j][0]<min)
             min=I[j][0];
  }

  //cout << "min= " << min << "   max= " << max << endl;


  for (int i=0; i<n_individuos; i++) {
    for (int j=0; j<tamano[i]-1; j++)
      if (min==max)
        individuos[i][j]=((1.0*rand())/(RAND_MAX+1.0));
      else
        individuos[i][j]=I[j][(i%rango)+1];

    // individuos[i][tamano[i]-1]=((1.0*(max-min)*rand())/(RAND_MAX+1.0))+min;
    /*if (min==max)
      individuos[i][tamano[i]-1]=((1.0*rand())/(RAND_MAX+1.0));
    else
      individuos[i][tamano[i]-1]=I[i%(tamano[i]-1)][i%rango];*/

    individuos[i][tamano[i]-1]=(1.0*rand())/(RAND_MAX+1.0);
    //individuos[i][tamano[i]-1]=0;
  }
}



void poblacionreal::PoblacionInicialAleatoria(int vacio, int rango){

  for (int i=0; i<n_individuos; i++) {
    for (int j=0; j<tamano[i]-1; j++)
        individuos[i][j]=((1.0*rand())/(RAND_MAX+1.0));

    individuos[i][tamano[i]-1]=(1.0*rand())/(RAND_MAX+1.0);
  }
}



void poblacionreal::PoblacionInicialValor(double valor){
  if (valor<rango_i || valor >rango_s)
    valor=(rango_s-rango_i)/2.0;

  for (int i=0; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++){
        individuos[i][j]= valor;
    }
}



void poblacionreal::PoblacionInicial(double **I, int clases){
  double *max = new double[clases], *min = new double[clases];

  double mayor, menor;
cout << "CL " << tamano[0] - 1 << endl;
  for (int i=0; i<clases; i++){
    mayor=I[0][i+1];
    menor=I[0][i+1];
cout << "I " << I[0][i+1] << endl;
    for (int j=1; j<tamano[0]-1; j++){
      if (I[j][i+1]>mayor)
        mayor=I[j][i+1];
      else if (I[j][i+1]<menor)
             menor=I[j][i+1];

      max[i]=mayor;
      min[i]=menor;
    }
  }

  for (int i=0; i<n_individuos; i++) {
    for (int j=0; j<tamano[i]-1; j++)
      if (min[i%clases]==max[i%clases])
        individuos[i][j]=((1.0*rand())/(RAND_MAX+1.0));
      else
        individuos[i][j]=I[j][(i%clases)+1];

    //individuos[i][tamano[i]-1]=(max[i%clases]-min[i%clases])*(1.0*rand())/(RAND_MAX+1.0);
    individuos[i][tamano[i]-1]=0.2;
  }
}




void poblacionreal::PoblacionInicial2_Estacionario(){
  for (int j=0; j<tamano[0]; j++){
    individuos[0][j]=0.5;
    individuos[n_individuos-2][j]=0.5;
    individuos[n_individuos-1][j]=0.5;
  }
  individuos[0][tamano[0]-1]=0.5;
  individuos[n_individuos-2][tamano[n_individuos-2]-1]=0.5;
  individuos[n_individuos-1][tamano[n_individuos-1]-1]=0.5;
}








void poblacionreal::MutacionUniforme(){
  double aux;
  for (int i=elitismo; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
      if (Probabilidad(prob_mutacion)){
        do{
          aux=(1.0*(rango_s-rango_i)*rand())/(RAND_MAX+1.0);
        }while (aux==individuos[i][j]);
	individuos[i][j]=aux;
	modificado[i]=true;
      }
}


void poblacionreal::CruceUniforme(){
  int a,p1;
  double aux;
  for (int i=elitismo; i<n_individuos; i++)
    if (Probabilidad(prob_cruce)){
      a=Select_Random_Individuo(n_individuos,i, elitismo);
      p1=PuntoCorte(tamano[i]);
      modificado[i]=true;
      modificado[a]=true;
      for (int j=0; j<p1; j++){
         aux=individuos[i][j];
	       individuos[i][j]=individuos[a][j];
	       individuos[a][j]=aux;
      }
    }
}


void poblacionreal::Cruce2puntos(){
  int a,p1,p2;
  double aux;
  for (int i=elitismo; i<n_individuos; i++)
    if (Probabilidad(prob_cruce)){
      a=Select_Random_Individuo(n_individuos,i,elitismo);
      PuntoCorte2(tamano[i],p1,p2);
      modificado[i]=true;
      modificado[a]=true;
      for (int j=p1; j<p2; j++){
         aux=individuos[i][j];
	       individuos[i][j]=individuos[a][j];
	       individuos[a][j]=aux;
      }
    }
}


 void poblacionreal::MutacionUniforme_Estacionario_Normal(){
  double aux;
  for (int i=n_individuos-2; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
      if (Probabilidad(prob_mutacion)){
        individuos[i][j]=(1.0*rand())/(RAND_MAX+1.0);
	if (individuos[i][j]<0)
	   individuos[i][j]=0;
	else if (individuos[i][j]>1)
	             individuos[i][j]=1;
	modificado[i]=true;
      }
}


void poblacionreal::MutacionUniforme_Estacionario(){
  double aux;
  int pos;
  for (int i=n_individuos-2; i<n_individuos; i++){
    for (int k=0; k<tamano[i]-1; k++){
      //pos = (int) (1.0*tamano[i]*rand()/(RAND_MAX+1.0));
      if (Probabilidad(prob_mutacion)){
         //individuos[i][k]= 1.0*rand()/(RAND_MAX+1.0);
         //individuos[i][k] = 1.0 - individuos[i][k];
         if (individuos[i][k]<individuos[i][tamano[i]-1])
            //individuos[i][k]=(1.0*(1-individuos[i][tamano[i]-1])*rand())/(RAND_MAX+1.0)+individuos[i][tamano[i]-1];
            individuos[i][k]=individuos[i][tamano[i]-1]+0.01;
         else
            //individuos[i][k]=(1.0*(individuos[i][tamano[i]-1])*rand())/(RAND_MAX+1.0);
            individuos[i][k]=individuos[i][tamano[i]-1]-0.01;

         if (individuos[i][k]>1)
           individuos[i][k]=1;
         else if (individuos[i][k]<0)
                individuos[i][k]=0;
      }

      //individuos[i][pos]=(1.0*rand())/(RAND_MAX+1.0);
    }
    modificado[i]=true;
  }
}


void poblacionreal::Rotacion(int i){
  int p;
  double *copia= new double[tamano[i]];

  // Hago una copia del comosoma
  for (int j=0; j<tamano[i]; j++)
    copia[j]=individuos[i][j];
  // Selecciono un punto en el cromosoma
  p=PuntoCorte(tamano[i]);

  // Modifico el cromosoma con la rotacion
  for (int j=0; j<tamano[i]; j++)
    individuos[i][j]=copia[(j+p)%tamano[i]];

  delete [] copia;
}




void poblacionreal::CruceUniforme_Estacionario(){
  int a,b,p1;
  double aux;
  for (int i=n_individuos-2; i<n_individuos; i++){
      a=Select_Random_Individuo(n_individuos,i, 0);
      b=Select_Random_Individuo(n_individuos,i, 0);
      p1=PuntoCorte(tamano[i]);
      modificado[i]=true;
      for (int j=0; j<p1; j++)
	       individuos[i][j]=individuos[a][j];

      for (int j=p1; j<tamano[i]; j++)
         individuos[i][j]=individuos[b][j];
  }
}


void poblacionreal::Cruce_BLX_Estacionario(double alpha, int indiv1, int indiv2){
double incremento, r_inf, r_sup;

  for (int j=0; j<tamano[0];j++){
     if (individuos[indiv1][j]<individuos[indiv2][j]){
       r_inf =individuos[indiv1][j];
       r_sup =individuos[indiv2][j];
     }
     else{
       r_inf =individuos[indiv2][j];
       r_sup =individuos[indiv1][j];
     }
     incremento = (r_sup-r_inf)*alpha;

     r_inf=r_inf-incremento;
    if (r_inf<0)
       r_inf=0;
    r_sup=r_sup+incremento;
    if (r_sup>1)
       r_sup=1;

      //individuos[n_individuos-2][j]=(1.0*(r_sup-r_inf)*rand())/(RAND_MAX+1.0)+r_inf;
      individuos[n_individuos-1][j]=(1.0*(r_sup-r_inf)*rand())/(RAND_MAX+1.0)+r_inf;
  }
  //modificado[n_individuos-2]=true;
  modificado[n_individuos-1]=true;
}


void poblacionreal::Cruce2puntos_Estacionario(){
  int a,b,p1,p2;
  double aux;
  for (int i=n_individuos-2; i<n_individuos; i++){
      a=Select_Random_Individuo(n_individuos,i,0);
      b=Select_Random_Individuo(n_individuos,i,0);
      PuntoCorte2(tamano[i],p1,p2);
      modificado[i]=true;
      for (int j=0; j<p1; j++)
	       individuos[i][j]=individuos[a][j];

      for (int j=p1; j<p2; j++)
	       individuos[i][j]=individuos[b][j];

      for (int j=p2; j<tamano[i]; j++)
	       individuos[i][j]=individuos[a][j];
  }
}

void poblacionreal::Cruce2puntos_Estacionario(int indiv1, int indiv2){
  int a,b,p1,p2;
  double aux;
  int i=n_individuos-2;
  //for (int i=n_individuos-2; i<n_individuos; i++){
      //a=Select_Random_Individuo(n_individuos,i,0);
      //b=Select_Random_Individuo(n_individuos,i,0);
      PuntoCorte2(tamano[indiv1],p1,p2);
      modificado[indiv1]=true;
      modificado[indiv2]=true;

      for (int j=p1; j<p2; j++){
        aux=individuos[indiv2][j];
        individuos[indiv2][j]=individuos[indiv1][j];
        individuos[indiv1][j]=aux;
      }
  //}
}


void poblacionreal::CruceAND_OR_Estacionario(int indiv1, int indiv2){
  int a,b,p1,p2;
  double aux;
  int i=n_individuos-2;
  //for (int i=n_individuos-2; i<n_individuos; i++){
      //a=Select_Random_Individuo(n_individuos,i,0);
      //b=Select_Random_Individuo(n_individuos,i,0);
      PuntoCorte2(tamano[i]-1,p1,p2);
      modificado[i]=true;
      modificado[i+1]=true;
      for (int j=p1; j<=p2; j++){
	if (individuos[indiv1][j]< individuos[indiv2][j]){
	       individuos[i][j]=individuos[indiv2][j];
	       individuos[i+1][j]=individuos[indiv1][j];
        }
        else {
	       individuos[i][j]=individuos[indiv1][j];
	       individuos[i+1][j]=individuos[indiv2][j];
        }
      }
  //}
}


void poblacionreal::CruceNAND_NOR_Estacionario(int indiv1, int indiv2){
  int a,b,p1,p2;
  double aux;
  int i=n_individuos-2;
  //for (int i=n_individuos-2; i<n_individuos; i++){
      //a=Select_Random_Individuo(n_individuos,i,0);
      //b=Select_Random_Individuo(n_individuos,i,0);
      PuntoCorte2(tamano[i]-1,p1,p2);
      modificado[i]=true;
      modificado[i+1]=true;
      for (int j=p1; j<=p2; j++){
	if (individuos[indiv1][j]< individuos[indiv2][j]){
	       individuos[i][j]=1-individuos[indiv2][j];
	       individuos[i+1][j]=1-individuos[indiv1][j];
        }
        else {
	       individuos[i][j]=1-individuos[indiv1][j];
	       individuos[i+1][j]=1-individuos[indiv2][j];
        }
      }
  //}
}
