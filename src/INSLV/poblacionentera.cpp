using namespace std;
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "INSLV/poblacionentera.h"
#include "INSLV/operaciones.h"




poblacionentera::poblacionentera(){
  prob_mutacion=0.0;
  prob_cruce=0.0;
  elitismo=0;
  n_individuos=0;
  tamano=0;
  modificado=0;
  valoracion=0;
  individuos=0;
  rango=0;
}



poblacionentera::poblacionentera(int rang, double mut, double cruce, int eli, int n){
  prob_mutacion=mut;
  prob_cruce=cruce;
  elitismo=eli;
  n_individuos=n;
  rango=rang;
  tamano= new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=0;

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  valoracion= new double[n_individuos];

  individuos = new int*[n_individuos];
  for (int i=0; i<n_individuos; i++)
    individuos[i] = 0;
}



poblacionentera::poblacionentera(int rang, double mut, double cruce, int eli, int n, int tama){
  prob_mutacion=mut;
  prob_cruce=cruce;
  elitismo=eli;
  n_individuos=n;
  rango=rang;
  tamano= new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=tama;

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  valoracion= new double[n_individuos];

  individuos = new int*[n_individuos];
  for (int i=0; i<n_individuos; i++)
    individuos[i] = new int[tamano[i]];
}



poblacionentera::poblacionentera(const poblacionentera &x){
  prob_mutacion=x.prob_mutacion;
  prob_cruce = x.prob_cruce;
  elitismo = x. elitismo;
  n_individuos = x.n_individuos;
  rango=x.rango;

  tamano = new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=x.tamano[i];


  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=x.modificado[i];

  valoracion= new double[n_individuos];
  for (int i=0; i<n_individuos; i++)
    valoracion[i]=x.valoracion[i];

  individuos = new int*[n_individuos];
  for (int i=0; i<n_individuos; i++) {
    if (tamano[i]>0){
      individuos[i] = new int[tamano[i]];
      for (int j=0; j<tamano[i]; j++)
        individuos[i][j]=x.individuos[i][j];
    }
  }

}


poblacionentera &poblacionentera::operator=(const poblacionentera &x){
  // Eliminar el objeto que recibe la asignacion
  if (n_individuos>0){
    if (tamano!=0)
      delete [] tamano;

    if (modificado!=0)
      delete [] modificado;


    if (valoracion!=0)
      delete [] valoracion;


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
  rango=x.rango;

  tamano = new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=x.tamano[i];

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=x.modificado[i];

  valoracion= new double[n_individuos];
  for (int i=0; i<n_individuos; i++)
    valoracion[i]=x.valoracion[i];

  individuos = new int*[n_individuos];
  for (int i=0; i<n_individuos; i++) {
    if (tamano[i]>0){
      individuos[i] = new int[tamano[i]];
      for (int j=0; j<tamano[i]; j++)
        individuos[i][j]=x.individuos[i][j];
    }
  }

  return *this;
}




poblacionentera::~poblacionentera(){
  if (n_individuos>0){
    delete [] tamano;
    delete [] modificado;
    delete [] valoracion;
    for (int i=0; i<n_individuos; i++)
      if (individuos[i]!=0){
        delete [] individuos[i];
	individuos[i]=0;
      }
    delete [] individuos;
  }

  n_individuos=0;
}

int * poblacionentera::Individuo(int i, int &tama) const{
  tama = tamano[i];
  return individuos[i];
}


bool poblacionentera::Modificado(int i) const{
 return modificado[i];
}

void poblacionentera::Pasar(int i, const poblacionentera &x, int j){
  if (tamano[i]>0)
    delete[] individuos[i];

  tamano[i]=x.tamano[i];
  individuos[i]= new int[tamano[i]];
  for (int k=0; k<tamano[i]; k++)
    individuos[i][k]=x.individuos[j][k];
}


void poblacionentera::Swap(int i, int j){
  Swap_int(tamano[i],tamano[j]);
  Swap_bool(modificado[i],modificado[j]);
  Swap_double(valoracion[i],valoracion[j]);
  int *p=individuos[i];
  individuos[i]=individuos[j];
  individuos[j]=p;
}



void poblacionentera::Codigo(int i, int *&v, int &tama) const{
  //if (v!=0)
    //delete [] v;

  tama=tamano[i];
  v=new int[tama];
  for (int j=0;j<tama;j++)
    v[j]=individuos[i][j];
}



void poblacionentera::Sort(){
  for (int i=0; i<n_individuos-1; i++)
    for (int j=n_individuos-1; j>i; j--)
      if (valoracion[j]>valoracion[j-1])
        Swap(j,j-1);
}


void poblacionentera::Pinta(int i) const{
  for (int j=0; j<tamano[i]; j++)
    cout << individuos[i][j] << " ";
  cout << endl;
}


void poblacionentera::PintaFitness(int i) const{
  cout << "Fitnes: " << valoracion[i] << endl;
}

void poblacionentera::PintaInFile(int i) const{
  ofstream f("slave.log",ios::app);
  for (int j=0; j<tamano[i]; j++)
    f << individuos[i][j] << " ";
  f << endl;
  f.close();
}


void poblacionentera::PintaFitnessInFile(int i) const{
  ofstream f("slave.log",ios::app);

  f << "Fitnes: " << valoracion[i] << endl;
  f.close();
}


void poblacionentera::PutValue(int indiv, int bit, int value){
  individuos[indiv][bit]=value;
}

int poblacionentera::GetValue(int indiv, int bit) const{
  return individuos[indiv][bit];
}

void poblacionentera::PoblacionInicialAleatoria(){
  for (int i=0; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
        individuos[i][j]= i%rango;
}

void poblacionentera::PoblacionInicialAleatoria(int consecuente){
  for (int i=0; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
        individuos[i][j]= consecuente;
}



void poblacionentera::PoblacionInicial2_Estacionario(int clase){
  individuos[0][0]=clase;
  individuos[n_individuos-2][0]=clase;
  individuos[n_individuos-1][0]=clase;
}

void poblacionentera::PoblacionInicial2_Estacionario(){
  for (int i=0; i<tamano[0];i++){
    individuos[0][i]=0;
    individuos[n_individuos-2][i]=0;
    individuos[n_individuos-1][i]=0;
  }
}



void poblacionentera::MutacionUniforme(){
  int aux;
  for (int i=elitismo; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
      if (Probabilidad(prob_mutacion)){
        do{
          aux=rand()%rango;
        }while (aux==individuos[i][j]);
	individuos[i][j]=aux;
	modificado[i]=true;
      }
}

void poblacionentera::MutacionUniformeMod(double prob0){
  int aux;
  for (int i=elitismo; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
      if (Probabilidad(prob_mutacion)){
        if (Probabilidad(prob0))
            aux=0;
        else
            do{
                aux=rand()%rango;
            }while (aux==individuos[i][j]);

	individuos[i][j]=aux;
	modificado[i]=true;
      }
}


void poblacionentera::CruceUniforme(){
  int a,p1;
  int aux;
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


void poblacionentera::Cruce2puntos(){
  int a,p1,p2;
  int aux;
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


void poblacionentera::MutacionUniforme_Estacionario(){
  int aux;
  for (int i=n_individuos-2; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
      if (Probabilidad(prob_mutacion)){
        do{
          aux=rand()%rango;
        }while (aux==individuos[i][j]);

	individuos[i][j]=aux;
	modificado[i]=true;
      }
}


void poblacionentera::MutacionUniformeMod_Estacionario(double prob0){
  int aux,j;
  if (rango>1){
    for (int i=n_individuos-2; i<n_individuos; i++){
        if (Probabilidad(prob0)){
            j=rand()%tamano[i];
            if (individuos[i][j]!=0){
                individuos[i][j]=0;
            }
            else{
                aux=rand()%rango;
                individuos[i][j]=aux;
            }
            modificado[i]=true;
        }
    }
  }
}



void poblacionentera::MutacionUniformeMod_Estacionario_Especial(){
  int aux,j;
  if (rango>1){
    for (int i=n_individuos-2; i<n_individuos; i++){
      int intentos=0;
      j=rand()%tamano[i];
      while (intentos<5 and individuos[i][j]==0){
         intentos++;
         j=rand()%tamano[i];
      }
      if (intentos<5){
            if (individuos[i][j]!=0){
                individuos[i][j]=0;
            }
            else{
                aux=rand()%rango;
                individuos[i][j]=aux;
            }
            modificado[i]=true;
        }
    }
  }
}



void poblacionentera::CruceUniforme_Estacionario(){
  int a,b,p1;
  int aux;
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


void poblacionentera::Cruce2puntos_Estacionario(){
  int a,b,p1,p2;
  int aux;
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


/* Solo vale cuando la poblacion entera tiene un unico individuo */
void poblacionentera::Cruce2puntos_Estacionario(int indiv1, int indiv2){
  int a,b,p1,p2;
  int aux;
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
}
