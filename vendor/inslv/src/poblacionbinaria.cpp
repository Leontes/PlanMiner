using namespace std;
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "INSLV/poblacionbinaria.h"
#include "INSLV/operaciones.h"



poblacionbinaria::poblacionbinaria(){
  prob_mutacion=0.0;
  prob_cruce=0.0;
  elitismo=0;
  n_individuos=0;
  tamano=0;
  modificado=0;
  valoracion=0;
  individuos=0;
}



poblacionbinaria::poblacionbinaria(double mut, double cruce, int eli, int n){
  prob_mutacion=mut;
  prob_cruce=cruce;
  elitismo=eli;
  n_individuos=n;
  tamano= new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=0;

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  valoracion= new double[n_individuos];

  individuos = new char*[n_individuos];
  for (int i=0; i<n_individuos; i++)
    individuos[i] = 0;
}



poblacionbinaria::poblacionbinaria(double mut, double cruce, int eli, int n, int tama){
  prob_mutacion=mut;
  prob_cruce=cruce;
  elitismo=eli;
  n_individuos=n;
  tamano= new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=tama;

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  valoracion= new double[n_individuos];

  individuos = new char*[n_individuos];
  for (int i=0; i<n_individuos; i++)
    individuos[i] = new char[tamano[i]];
}



poblacionbinaria::poblacionbinaria(double mut, double cruce, int eli, int n, int *tama){
  prob_mutacion=mut;
  prob_cruce=cruce;
  elitismo=eli;
  n_individuos=n;
  tamano= new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=tama[i];

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  valoracion= new double[n_individuos];

  individuos = new char*[n_individuos];
  for (int i=0; i<n_individuos; i++)
    individuos[i] = new char[tamano[i]];
}



poblacionbinaria::poblacionbinaria(const poblacionbinaria &x){
  prob_mutacion=x.prob_mutacion;
  prob_cruce = x.prob_cruce;
  elitismo = x. elitismo;
  n_individuos = x.n_individuos;

  tamano = new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=x.tamano[i];


  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=x.modificado[i];

  valoracion= new double[n_individuos];
  for (int i=0; i<n_individuos; i++)
    valoracion[i]=x.valoracion[i];

  individuos = new char*[n_individuos];
  for (int i=0; i<n_individuos; i++) {
    if (tamano[i]>0){
      individuos[i] = new char[tamano[i]];
      for (int j=0; j<tamano[i]; j++)
        individuos[i][j]=x.individuos[i][j];
    }
  }

}


poblacionbinaria &poblacionbinaria::operator=(const poblacionbinaria &x){
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

  tamano = new int[n_individuos];
  for (int i=0; i<n_individuos; i++)
    tamano[i]=x.tamano[i];

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=x.modificado[i];

  valoracion= new double[n_individuos];
  for (int i=0; i<n_individuos; i++)
    valoracion[i]=x.valoracion[i];

  individuos = new char*[n_individuos];
  for (int i=0; i<n_individuos; i++) {
    if (tamano[i]>0){
      individuos[i] = new char[tamano[i]];
      for (int j=0; j<tamano[i]; j++)
        individuos[i][j]=x.individuos[i][j];
    }
  }

  return *this;
}




poblacionbinaria::~poblacionbinaria(){
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


char * poblacionbinaria::Individuo(int i, int &tama) const{
  tama = tamano[i];
  return individuos[i];
}


bool poblacionbinaria::Modificado(int i) const{
 return modificado[i];
}

void poblacionbinaria::Pasar(int i, const poblacionbinaria &x, int j){

  if (tamano[i]>0)
    delete[] individuos[i];

  tamano[i]=x.tamano[j];
  individuos[i]= new char[tamano[i]];
  for (int k=0; k<tamano[i]; k++)
    individuos[i][k]=x.individuos[j][k];
}


void poblacionbinaria::Swap(int i, int j){
  Swap_int(tamano[i],tamano[j]);
  Swap_bool(modificado[i],modificado[j]);
  Swap_double(valoracion[i],valoracion[j]);
  char *p=individuos[i];
  individuos[i]=individuos[j];
  individuos[j]=p;
}


void poblacionbinaria::Codigo(int i, char *&v, int &tama) const{

  //if (v!=0)
    //delete [] v;

  tama=tamano[i];
  v=new char[tama];
  for (int j=0;j<tama;j++)
    v[j]=individuos[i][j];
}

void poblacionbinaria::Sort(){
  for (int i=0; i<n_individuos-1; i++)
    for (int j=n_individuos-1; j>i; j--)
      if (valoracion[j]>valoracion[j-1])
        Swap(j,j-1);
}


void poblacionbinaria::Pinta(int i) const{
  for (int j=0; j<tamano[i]; j++)
    cout << individuos[i][j];
  cout << endl;
}


void poblacionbinaria::PintaFitness(int i) const{
  cout << "Fitnes: " << valoracion[i] << endl;
}


void poblacionbinaria::PintaInFile(int i) const{
  ofstream f ("slave.log",ios::app);
  for (int j=0; j<tamano[i]; j++)
    f << individuos[i][j];
  f << endl;
  f.close();
}


void poblacionbinaria::PintaFitnessInFile(int i) const{
  ofstream f ("slave.log",ios::app);
  f << "Fitnes: " << valoracion[i] << endl;
  f.close();
}



void poblacionbinaria::PutValue(int indiv, int bit, char value){
  individuos[indiv][bit]=value;
}



void poblacionbinaria::PoblacionInicialAleatoria(){
  for (int i=0; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
        individuos[i][j]= (Probabilidad(0.9))?'1':'0';
        //individuos[i][j]= '1';
}


void poblacionbinaria::PoblacionInicialValor(char valor){
  for (int i=0; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
        individuos[i][j]= valor;
}



void poblacionbinaria::PoblacionInicial(int **sujeto, int tama, int* tama_dom){
  int j;
  for (int i=0; i<n_individuos; i++){
    j=0;
    for (int k=0; k<tama; k++){
      if (sujeto[i][k]==-1){
        for (int l=0; l<tama_dom[k]; l++){
          individuos[i][j]= (Probabilidad(0.9))?'1':'0';
          j++;
        }
      }
      else {
        for (int l=0; l<tama_dom[k]; l++)
          individuos[i][j+l]='0';
        individuos[i][j+sujeto[i][k]]='1';
        j+=tama_dom[k];
      }
    }
  }
}




// Aniade 3 individuos de la clase extraida en la iteracion anterior. Posiciones 0, n_individuos-2 y n_individuos-1.
void poblacionbinaria::PoblacionInicial2_Estacionario(int **sujeto, int tama, int* tama_dom, int clase){
  int j,i;


  j=0;
  int sig=0;
  for (int k=0; k<tama; k++){
    if (sujeto[0][k]==-1){
      for (int l=0; l<tama_dom[k]; l++){
        individuos[0][j]= (Probabilidad(0.9))?'1':'0';
        j++;
      }
    }
    else {
      for (int l=0; l<tama_dom[k]; l++)
        individuos[0][j+l]='0';
      individuos[0][j+sujeto[0][k]]='1';
      j+=tama_dom[k];
    }
  }


  sig++;
  for (int i=n_individuos-2; i<n_individuos; i++){
    j=0;
    for (int k=0; k<tama; k++){
      if (sujeto[sig][k]==-1){
        for (int l=0; l<tama_dom[k]; l++){
          individuos[i][j]= (Probabilidad(0.9))?'1':'0';
          j++;
        }
      }
      else {
        for (int l=0; l<tama_dom[k]; l++)
          individuos[i][j+l]='0';
        individuos[i][j+sujeto[sig][k]]='1';
        j+=tama_dom[k];
      }
    }
    sig++;
  }
}








void poblacionbinaria::MutacionUniforme(){
  for (int i=elitismo; i<n_individuos; i++)
    for (int j=0; j<tamano[i]; j++)
      if (Probabilidad(prob_mutacion)){
         if (individuos[i][j]=='1')
	    individuos[i][j]='0';
	 else
	    individuos[i][j]='1';
  	modificado[i]=true;
      }
}


void poblacionbinaria::MutacionUniforme_Estacionario(){
  for (int i=n_individuos-2; i<n_individuos; i++){
    for (int j=0; j<tamano[i]; j++){
      if (Probabilidad(prob_mutacion)){
         //cout << "Mutado: " << i << "(" << j <<")"<<endl;
         if (individuos[i][j]=='1')
            individuos[i][j]='0';
        else
            individuos[i][j]='1';
        modificado[i]=true;
      }
    }
  }

}

void poblacionbinaria::Rotacion(int i){
  int p;
  char *copia= new char[tamano[i]+1];

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



void poblacionbinaria::CruceUniforme(){
  int a,p1;
  char aux;
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


void poblacionbinaria::CruceUniforme_Estacionario(){
  int a,b,p1;
  char aux;
  for (int i=n_individuos-2; i<n_individuos; i++){
      a=Select_Random_Individuo(n_individuos,i, 0);
      b=Select_Random_Individuo(n_individuos,i, 0);
      p1=PuntoCorte(tamano[i]);
      modificado[i]=true;
      for (int j=0; j<p1; j++){
	       individuos[i][j]=individuos[a][j];
      }
      for (int j=p1; j<tamano[i]; j++){
	       individuos[i][j]=individuos[b][j];
      }
  }
}




void poblacionbinaria::Cruce2puntos(){
  int a,p1,p2;
  char aux;
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


void poblacionbinaria::Cruce2puntos_Estacionario(){
  int a,b,p1,p2;
  char aux;
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


void poblacionbinaria::Cruce2puntos_Estacionario(int indiv1, int indiv2){
  int a,b,p1,p2;
  char aux;
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

void poblacionbinaria::CruceAND_OR_Estacionario(int indiv1, int indiv2){
  int a,b,p1,p2;
  char aux;
  int i=n_individuos-2;

  PuntoCorte2(tamano[i],p1,p2);
  modificado[i]=true;
  modificado[i+1]=true;
  for (int j=p1; j<p2; j++){
    if (individuos[indiv1][j] =='1' && individuos[indiv2][j] =='1')
       individuos[i][j]='1';
    else
       individuos[i][j]='0';

    if (individuos[indiv1][j] =='1' || individuos[indiv2][j] =='1')
       individuos[i+1][j]='1';
    else
       individuos[i+1][j]='0';
  }
}

void poblacionbinaria::CruceNAND_NOR_Estacionario(int indiv1, int indiv2){
  int a,b,p1,p2;
  char aux;
  int i=n_individuos-2;
      modificado[i]=true;
      modificado[i+1]=true;
      for (int j=0; j<tamano[i]; j++){
        if (individuos[indiv1][j] =='1' && individuos[indiv2][j] =='1')
          individuos[i][j]='0';
        else
          individuos[i][j]='1';

        if (individuos[indiv1][j] =='1' || individuos[indiv2][j] =='1')
          individuos[i+1][j]='0';
        else
          individuos[i+1][j]='1';
      }
}
