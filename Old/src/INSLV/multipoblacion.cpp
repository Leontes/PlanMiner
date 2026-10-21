#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cmath>
#include "INSLV/multipoblacion.h"
#include "INSLV/operaciones.h"
#include "INSLV/example_set.h"

using namespace std;

multipoblacion::multipoblacion(){
  elitismo=0;
  poblacionesB=0;
  poblacionesE=0;
  n_individuos=0;
  modificado=0;
  n_valoracion=0;
  valoracion=0;
  Pb=0;
  Pe=0;
}



multipoblacion::multipoblacion(int pbin, int pent, int preal, int *rangoe,
                               double *rangori, double *rangors, double *mut, double *cruce,
                               int eli, int n, int *tama, int n_val){

  elitismo=eli;
  n_individuos=n;
  poblacionesB=pbin;
  poblacionesE=pent;
  poblacionesR=preal;
  if (pbin>0){
    Pb= new poblacionbinaria*[pbin];
    for (int j=0; j<pbin; j++)
      Pb[j]= new poblacionbinaria(mut[j], cruce[j], eli, n, tama[j]);
  }

  if (pent>0){
    Pe= new poblacionentera*[pent];
    for (int j=0; j<pent; j++)
      Pe[j]= new poblacionentera(rangoe[j], mut[j+pbin], cruce[j+pbin], eli, n, tama[j+pbin]);
  }

  if (preal>0){
    Pr= new poblacionreal*[preal];
    for (int j=0; j<preal; j++)
      Pr[j]= new poblacionreal(rangori[j], rangors[j], mut[j+pbin+pent], cruce[j+pbin+pent], eli, n, tama[j+pbin+pent]);
  }


  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=true;

  n_valoracion=n_val;
  valoracion = new double *[n_individuos];
  for (int i=0; i<n_individuos; i++){
     valoracion[i]= new double[n_valoracion];
     for (int j=0; j<n_valoracion; j++)
       valoracion[i][j]=0;
  }
}






multipoblacion::multipoblacion(const multipoblacion &x){

  elitismo = x.elitismo;
  n_individuos = x.n_individuos;

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=x.modificado[i];

  n_valoracion=x.n_valoracion;
  valoracion= new double*[n_individuos];
  for (int i=0; i<n_individuos; i++){
     valoracion[i]= new double[n_valoracion];
     for (int j=0; j<n_valoracion; j++)
       valoracion[i][j]=x.valoracion[i][j];
  }

  poblacionesB=x.poblacionesB;
  poblacionesE=x.poblacionesE;
  poblacionesR=x.poblacionesR;
  if (poblacionesB>0){
    Pb= new poblacionbinaria*[poblacionesB];
    for (int j=0; j<poblacionesB; j++){
      Pb[j]= new poblacionbinaria;
      Pb[j][0]= x.Pb[j][0];
    }
  }

  if (poblacionesE>0){
    Pe= new poblacionentera*[poblacionesE];
    for (int j=0; j<poblacionesE; j++){
      Pe[j]= new poblacionentera;
      Pe[j][0]=x.Pe[j][0];
    }
  }

  if (poblacionesR>0){
    Pr= new poblacionreal*[poblacionesR];
    for (int j=0; j<poblacionesR; j++){
      Pr[j]= new poblacionreal;
      Pr[j][0]=x.Pr[j][0];
    }
  }

}


multipoblacion &multipoblacion::operator=(const multipoblacion &x){
 if (this !=&x){
  // Eliminar el objeto que recibe la asignacion
  if (n_individuos>0){
    if (modificado!=0)
      delete [] modificado;
    if (valoracion!=0){
      for (int i=0; i<n_valoracion; i++)
        delete [] valoracion[i];
      delete [] valoracion;
    }
  }

  if (poblacionesB>0){
    for (int i=0; i<poblacionesB; i++)
      delete Pb[i];
    delete Pb;
  }

  if (poblacionesE>0){
    for (int i=0; i<poblacionesE; i++)
      delete Pe[i];
    delete Pe;
  }

  if (poblacionesR>0){
    for (int i=0; i<poblacionesR; i++)
      delete Pr[i];
    delete Pr;
  }

  // copiar el objeto

  elitismo = x. elitismo;
  n_individuos = x.n_individuos;

  modificado= new bool[n_individuos];
  for (int i=0; i<n_individuos; i++)
    modificado[i]=x.modificado[i];

  n_valoracion = x.n_valoracion;
  valoracion= new double*[n_individuos];
  for (int i=0; i<n_individuos; i++){
    valoracion[i]=new double[n_valoracion];
    for (int j=0; j<n_valoracion; j++)
      valoracion[i][j]=x.valoracion[i][j];
  }

  poblacionesB=x.poblacionesB;
  poblacionesE=x.poblacionesE;
  poblacionesR=x.poblacionesR;

  if (poblacionesB>0){
    Pb= new poblacionbinaria*[poblacionesB];
    for (int j=0; j<poblacionesB; j++){
      Pb[j]= new poblacionbinaria;
      Pb[j][0]= x.Pb[j][0];
    }
  }

  if (poblacionesE>0){
    Pe= new poblacionentera*[poblacionesE];
    for (int j=0; j<poblacionesE; j++){
      Pe[j]= new poblacionentera;
      Pe[j][0]=x.Pe[j][0];
    }
  }

  if (poblacionesR>0){
    Pr= new poblacionreal*[poblacionesR];
    for (int j=0; j<poblacionesR; j++){
      Pr[j]= new poblacionreal;
      Pr[j][0]=x.Pr[j][0];
    }
  }



 }
  return *this;
}




multipoblacion::~multipoblacion(){
  if (n_individuos>0){
    if (modificado!=0)
      delete [] modificado;
    if (valoracion!=0){
      for (int i=0; i<n_valoracion; i++)
        delete [] valoracion[i];
      delete [] valoracion;
    }
  }

  if (poblacionesB>0){
    for (int i=0; i<poblacionesB; i++)
      delete Pb[i];
    delete [] Pb;
  }

  if (poblacionesE>0){
    for (int i=0; i<poblacionesE; i++)
      delete Pe[i];
    delete []Pe;
  }

  if (poblacionesR>0){
    for (int i=0; i<poblacionesR; i++)
      delete Pr[i];
    delete [] Pr;
  }

  elitismo=0;
  poblacionesB=0;
  poblacionesE=0;
  n_individuos=0;
  modificado=0;
  n_valoracion=0;
  valoracion=0;
  Pb=0;
  Pe=0;
}


void multipoblacion::Swap(int i, int j){

  for (int k=0; k<poblacionesB; k++)
     Pb[k][0].Swap(i,j);

  for (int k=0; k<poblacionesE; k++)
     Pe[k][0].Swap(i,j);

  for (int k=0; k<poblacionesR; k++)
     Pr[k][0].Swap(i,j);

  Swap_bool(modificado[i],modificado[j]);

  double *p=valoracion[i];
  valoracion[i]=valoracion[j];
  valoracion[j]=p;
}


void multipoblacion::Pasar(int i, int j){
    for (int k=0; k<poblacionesB; k++){
       Pb[k][0].Pasar(i,this->Pb[k][0],j);
    }

    for (int k=0; k<poblacionesE; k++)
       Pe[k][0].Pasar(i,this->Pe[k][0],j);

    for (int k=0; k<poblacionesR; k++)
       Pr[k][0].Pasar(i,this->Pr[k][0],j);

}



void multipoblacion::Sort(){
  int k;
  for (int i=0; i<n_individuos-1; i++)
    for (int j=n_individuos-1; j>i; j--){
      k=0;
      while (k<n_valoracion && valoracion[j][k]==valoracion[j-1][k])
        k++;
      if (k<n_valoracion && valoracion[j][k]>valoracion[j-1][k])
        Swap(j,j-1);
    }
}


int multipoblacion::ClaseIndividuo(int ind) const{
  return Pe[0][0].GetValue(ind,0);
}

int multipoblacion::ValorIndividuo(int population, int ind) const{
  return Pe[population][0].GetValue(ind,0);
}

char* multipoblacion::Subpoblacion_Binaria(int subpoblacion, int individuo, int &tama) const{
  char *puntero;
  puntero=Pb[subpoblacion][0].Individuo(individuo,tama);
  return puntero;
}

int* multipoblacion::Subpoblacion_Entera(int subpoblacion, int individuo, int &tama) const{
  int *puntero;
  puntero=Pe[subpoblacion][0].Individuo(individuo,tama);
  return puntero;
}


double* multipoblacion::Subpoblacion_Real(int subpoblacion, int individuo, int &tama) const{
  double *puntero;
  puntero=Pr[subpoblacion][0].Individuo(individuo,tama);
  return puntero;
}





void multipoblacion::Sort(int n_clases){
  int k;
  int *vector= new int[n_clases];
  double *maximo1= new double [n_clases];
  double *minimo1= new double [n_clases];
  double *maximo2= new double [n_clases];
  double *maximo3= new double [n_clases];
  int *posicion= new int [n_clases];

  for (int i=0; i<n_clases; i++)
    vector[i]=0;

  bool cambio=true;
  for (int i=0; i<n_individuos-1 && cambio; i++){
    cambio=false;
    for (int j=n_individuos-1; j>i; j--){
      k=0;
      while (k<n_valoracion && valoracion[j][k]==valoracion[j-1][k])
        k++;
      if (k<n_valoracion && valoracion[j][k]>valoracion[j-1][k]){
        Swap(j,j-1);
        cambio=true;
      }
    }
  }


  // Modifico el orden para permitir que reglas de todas las clases siempre esten presentes en la poblacion
  for (int i=0; i<n_individuos-2; i++){
    if (vector[ClaseIndividuo(i)]==0){
      maximo1[ClaseIndividuo(i)]=valoracion[i][0];
      maximo2[ClaseIndividuo(i)]=valoracion[i][1];
      maximo3[ClaseIndividuo(i)]=valoracion[i][2];
      posicion[ClaseIndividuo(i)]=i;
    }
    else
      minimo1[ClaseIndividuo(i)]=valoracion[i][0];

    vector[ClaseIndividuo(i)]++;
  }

  for (int i=n_individuos-2; i<n_individuos; i++){
    if (vector[ClaseIndividuo(i)]<(1.0*n_individuos/(n_clases+1.0))) {
      int j=n_individuos-3;
      while (vector[ClaseIndividuo(j)]<1.0*n_individuos/(n_clases+1.0))
        j--;

      vector[ClaseIndividuo(i)]++;
      vector[ClaseIndividuo(j)]--;
      Swap(i,j);
    }
  }

  cout <<"[" << vector[0] << "(" << maximo1[0] << "," << minimo1[0] <<")";
  for (int i=1; i<n_clases; i++)
   cout << "," <<vector[i]<< "(" << maximo1[i] << "," << minimo1[i] <<")";
  cout <<"]" << endl;


  delete [] vector;
  delete [] maximo1;
  delete [] minimo1;
  delete [] maximo2;
  delete [] maximo3;
  delete [] posicion;


}

void multipoblacion::Sort(int n_clases, int *n_examples_per_class){
  int k;
  int **vector= new int*[n_clases];
  vector[0]= new int [n_clases*n_individuos];
  for (int i=1; i<n_clases; i++)
    vector[i]=vector[i-1]+n_individuos;

  double *maximo1= new double [n_clases];
  double *minimo1= new double [n_clases];
  int numero_ejemplos;
  //double vaux[3];

  numero_ejemplos=0;
  for (int i=0; i<n_clases; i++){
    vector[i][0]=0;
    numero_ejemplos+=n_examples_per_class[i];
  }

  bool cambio=true;
  for (int i=0; i<n_individuos-1 && cambio; i++){
    cambio=false;
    for (int j=n_individuos-1; j>i; j--){
      k=0;
      while (k<n_valoracion && valoracion[j][k]==valoracion[j-1][k])
        k++;
      if (k<n_valoracion && valoracion[j][k]>valoracion[j-1][k]){
        Swap(j,j-1);
        cambio=true;
      }
    }
  }



  // Modifico el orden para permitir que reglas de todas las clases siempre esten presentes en la poblacion
  double *MediaPobClase = new double[n_clases];

  for (int k=0;k<n_clases;k++){
 	MediaPobClase[k] = 0;
	maximo1[k] = 0;
	minimo1[k] = 0;
  }

  for (int i=0; i<n_individuos-2; i++){
    int k = ClaseIndividuo(i);
    if (vector[k][0]==0){
      maximo1[k]=valoracion[i][0];
      MediaPobClase[k]=valoracion[i][0];
    }
    else {
      minimo1[k]=valoracion[i][0];
      MediaPobClase[k]+=valoracion[i][0];
    }

    vector[k][vector[k][0]+1]=i;
    vector[k][0]++;
  }

  /* Reglas del tamaño minimo de las poblaciones */
  int *PobMinClase = new int[n_clases];
  for (int k=0;k<n_clases;k++){
    if (n_examples_per_class[k]==0)
      PobMinClase[k]=0;
    else if (n_examples_per_class[k]/(1.0*numero_ejemplos)<0.05)
           PobMinClase[k]=n_individuos/(2*n_clases);
         else if (fabs(maximo1[k]-(1.0*MediaPobClase[k]/vector[k][0]))<0.1)
                PobMinClase[k]=n_individuos/(2*n_clases);
              else
                PobMinClase[k]=n_individuos/(1+n_clases);
  }


  /* Purgar las clases que donde maxvalor ==minvalor */

  for (int k=0;k<n_clases;k++){
    if (vector[k][0]>PobMinClase[k] && fabs(maximo1[k]-(1.0*MediaPobClase[k]/vector[k][0]))<0.05 )
      for (int t=PobMinClase[k]; t<=vector[k][0]; t++){
         valoracion[vector[k][t]][0]=-999999999;
         /* Aqui hacer algo para dar diversidad */
         /* Voy a probar con la rotacion de la parte binaria */
         Pb[0][0].Rotacion(vector[k][t]);
         /* Voy a probar con la rotacion de la parte real */
         Pr[0][0].Rotacion(vector[k][t]);

      }
  }

  /* Ordenar la poblacion */
  //bool
  cambio=true;
  for (int i=0; i<n_individuos-1 && cambio; i++){
    cambio=false;
    for (int j=n_individuos-1; j>i; j--){
      k=0;
      while (k<n_valoracion && valoracion[j][k]==valoracion[j-1][k])
        k++;
      if (k<n_valoracion && valoracion[j][k]>valoracion[j-1][k]){
        Swap(j,j-1);
        cambio=true;
      }
    }
  }


  /* Fijar los individuos que seran reemplazados */
  for (int i=n_individuos-2; i<n_individuos; i++){
    k = ClaseIndividuo(i);
    if (vector[k][0]<PobMinClase[k] ){
      int j=n_individuos-3;
      while (vector[ClaseIndividuo(j)][0]<PobMinClase[ClaseIndividuo(j)])
        j--;

      vector[ClaseIndividuo(i)][vector[ClaseIndividuo(i)][0]+1]=vector[ClaseIndividuo(j)][vector[ClaseIndividuo(j)][0]+1];
      vector[ClaseIndividuo(i)][0]++;
      vector[ClaseIndividuo(j)][0]--;
      Swap(i,j);
    }
  }

  /*cout <<"[" << vector[0][0] << "(" << maximo1[0] << "," << minimo1[0] <<")";
  for (int i=1; i<n_clases; i++)
   cout << "," <<vector[i][0]<< "(" << maximo1[i] << "," << minimo1[i] <<")";
  cout <<"]" << endl; */

  delete [] MediaPobClase;
  delete [] PobMinClase;
  delete [] vector[0];
  delete [] vector;
  delete [] maximo1;
  delete [] minimo1;
}


void multipoblacion::Better(int nclases, int *better, int &nbetter, int **ind_clase) const{

  nbetter=1;
  better[0]=0;
  /*for (int i=0; i<nclases; i++){
    if ( ind_clase[i][0]>1 && ind_clase[i][1]!=0 && valoracion[ind_clase[i][1]][0]>0) {
       //if (ValorIndividuo(1,i)>1) {
         better[nbetter]=ind_clase[i][1];
         nbetter++;
       //}
    }
  }*/
}


double Acierto (double *conjunto, int n_examples){
  double aciertos=0;
  for (int i=0; i<n_examples; i++){
    if (conjunto[i]>0)
       aciertos++;
  }
  return aciertos/n_examples;
}

bool Mayor (double *v1, double *v2, int n){
  int k=0;
  while (k<n && v1[k]==v2[k])
        k++;

  if (k==n)
    return false;
  else
    return v1[k]>v2[k];
}

void multipoblacion::Sort_menos2ultimos(){
  int k;
  bool cambio=true;
  for (int i=0; cambio and i<n_individuos-3; i++){
    cambio=false;
    for (int j=n_individuos-3; j>i; j--){
      k=0;
      while (k<n_valoracion && valoracion[j][k]==valoracion[j-1][k])
        k++;
      if (k<n_valoracion && valoracion[j][k]>valoracion[j-1][k]){
        Swap(j,j-1);
        cambio=true;
      }
    }
  }
}



double multipoblacion::Sort_4L(example_set &E, int n_clases, int *n_examples_per_class, int **ind_clase, double *adap_reglas, double *peso_reglas, int n_examples){
  int k;
  int *cl,ncl;
  double acierto=0, acierto_old=0, fallo=0, fallo_old=0;

  // Reservo espacio para guardar infomacion sobre los valores de fitness
  int numero_ejemplos;
  //double vaux[3];

  // Comprobar si los individuos encontrados mejoran al lider de su subpoblacion
  // Si alguno mejora al lider, entonces reordenar la poblacion.
  // Si no mejora, entonces no es necesario reordenar y basta con aplicar el cubrimiento
  // sobre las subpoblaciones.

//  double *adaptacion, *agregacion, *disparada, *peso_agregacion, *mejor_valoracion;
//  agregacion= new double[n_examples];
//  disparada = new double[n_examples];
//  peso_agregacion= new double[n_examples];
//  mejor_valoracion= new double[n_individuos];
//  for (int l=0; l<n_examples; l++){
//    agregacion[l]=adap_reglas[l];
//    peso_agregacion[l]=peso_reglas[l];
//    disparada[l]=-1;
//  }


  // Ordeno la poblacion asumiendo que solo se han modificado los 2 ultimos individuos.
  // Ordenacion por insercion
  for (int i=n_individuos-2; i<n_individuos; i++){
    k=i;
    while (k>0 && Mayor(valoracion[k],valoracion[k-1],n_valoracion)){
       Swap(k,k-1);
       k--;
    }
  }

  // Calculo el numero de ejemplos total
  numero_ejemplos=0;
  for (int i=0; i<n_clases; i++){
    numero_ejemplos+=n_examples_per_class[i];
  }



// ------------------------------------------------------------------------------- */
  // Recalculo de nuevo las supoblaciones despues de la ultima ordenacion
  for (int i=0; i<n_clases; i++)
    ind_clase[i][0]=0;

  for (int i=0; i<n_individuos-2; i++){
    cl=Subpoblacion_Entera(0,i,ncl);
    ind_clase[cl[0]][0]++;
    ind_clase[cl[0]][ind_clase[cl[0]][0]]=i;
  }

  /*cout << "\t(" << ind_clase[0][0];
  for (int i=1; i<n_clases; i++)
    cout << ", " << ind_clase[i][0];
  cout << ")\n";*/



  int num_medio_individuos_por_clase = (n_individuos-2)/(1.0*n_clases)-1;

  int num_min_individuos_por_clase = num_medio_individuos_por_clase ;
  //if (num_min_individuos_por_clase >= num_medio_individuos_por_clase || num_min_individuos_por_clase<0)
    //num_min_individuos_por_clase = 0;

  /* Reglas del tamaño minimo de las poblaciones */
  int *PobMinClase = new int[n_clases];
   for (int k=0;k<n_clases;k++){
     if (n_examples_per_class[k]/(1.0*numero_ejemplos)<0.05)
       PobMinClase[k]=num_min_individuos_por_clase;
     else if (ind_clase[k][0]>0 && fabs(valoracion[ind_clase[k][1]][0]-valoracion[ind_clase[k][ind_clase[k][0]]][0] )<0.1)
            PobMinClase[k]=num_min_individuos_por_clase;
          else
            PobMinClase[k]=num_min_individuos_por_clase;
   }

//  for (int i=0; i<n_clases; i++)
//    PobMinClase[i]=n_individuos/(1+n_clases);

  bool usados_2_ultimos=true;

  /* Fijar los individuos que seran reemplazados */
  for (int i=n_individuos-2; i<n_individuos; i++){
    k = ClaseIndividuo(i);
    if (ind_clase[k][0]<=PobMinClase[k] ){
      usados_2_ultimos=false;
      int j=n_individuos-3;
      while (ind_clase[ClaseIndividuo(j)][0]<=PobMinClase[ClaseIndividuo(j)])
        j--;

      ind_clase[ClaseIndividuo(i)][ind_clase[ClaseIndividuo(i)][0]+1]=ind_clase[ClaseIndividuo(j)][ind_clase[ClaseIndividuo(j)][0]];
      ind_clase[ClaseIndividuo(i)][0]++;
      ind_clase[ClaseIndividuo(j)][0]--;
      Swap(i,j);
    }
  }

  if (!usados_2_ultimos)
    Sort_menos2ultimos();

  delete [] PobMinClase;

}










void multipoblacion::Pinta(int i) const{
  for (int j=0; j<poblacionesB; j++)
    Pb[j][0].Pinta(i);

  for (int j=0; j<poblacionesE; j++)
    Pe[j][0].Pinta(i);

  for (int j=0; j<poblacionesR; j++)
    Pr[j][0].Pinta(i);

}

void multipoblacion::Pinta() const{
  for (int i=0; i<n_individuos; i++){
    Pinta(i);
    PintaFitness(i);
  }
}


void multipoblacion::PintaInFile(int i) const{
  for (int j=0; j<poblacionesB; j++)
    Pb[j][0].PintaInFile(i);

  for (int j=0; j<poblacionesE; j++)
    Pe[j][0].PintaInFile(i);

  for (int j=0; j<poblacionesR; j++)
    Pr[j][0].PintaBinInFile(i);
}

void multipoblacion::PintaInFile() const{
  ofstream f;
  for (int i=0; i<n_individuos; i++){
    f.open ("slave.log",ios::app);
    f << "Individuo: " << i << endl;
    f << "----------------------------" << endl;
    f.close();
    PintaInFile(i);
    PintaFitnessInFile(i);
    f.open("slave.log",ios::app);
    f << "----------------------------" << endl;
    f.close();
  }
}



double multipoblacion::MediaFitness() const {
  double media=0;
  for (int i=0; i<n_individuos; i++){
    media += valoracion[i][0];
  }
  return media/n_individuos;
}


double multipoblacion::MediaFitness_Estacionario() const {
  double media=0;
  for (int i=0; i<n_individuos-2; i++){
    media += valoracion[i][0];
  }
  return media/(n_individuos-2);
}



double multipoblacion::MediaFitness_Estacionario(double &min_f0, double &max_f0, double &min_f1, double &max_f1,
                                                 double &min_f2, double &max_f2) const {
  double media=0;
  min_f0=valoracion[0][0];
  max_f0=valoracion[0][0];
  min_f1=valoracion[0][1];
  max_f1=valoracion[0][1];
  min_f2=valoracion[0][2];
  max_f2=valoracion[0][2];

  for (int i=0; i<n_individuos-2; i++){
    media += valoracion[i][0];

    if (valoracion[i][0]>max_f0)
     max_f0=valoracion[i][0];
    else if (valoracion[i][0]<min_f0)
           min_f0=valoracion[i][0];

    if (valoracion[i][1]>max_f1)
     max_f1=valoracion[i][1];
    else if (valoracion[i][1]<min_f1)
           min_f1=valoracion[i][1];

    if (valoracion[i][2]>max_f2)
     max_f2=valoracion[i][2];
    else if (valoracion[i][2]<min_f2)
           min_f2=valoracion[i][2];
  }
  return media/(n_individuos-2);
}





void multipoblacion::PintaFitness(int i) const{
  cout << "Fitnes: ";
  for (int j=0; j<n_valoracion; j++)
    cout << valoracion[i][j] << " ";
  cout << "[" << MediaFitness() << "]";
  cout << endl;
}

void multipoblacion::PintaFitnessInFile(int i) const{
  ofstream f ("slave.log",ios::app);
  f << "Fitnes: ";
  for (int j=0; j<n_valoracion; j++)
    f << valoracion[i][j] << " ";
  f << "[" << MediaFitness() << "]";
  f << endl;
  f.close();
}



void multipoblacion::PintaFitness_Estacionario(int i) const{
  double minf0,minf1,minf2,maxf0,maxf1,maxf2;
  cout << "Fitnes: ";
  for (int j=0; j<n_valoracion; j++)
    cout << valoracion[i][j] << "   ";
  //cout << "[" << MediaFitness_Estacionario(minf0,maxf0,minf1,maxf1,minf2,maxf2) << "]";
  //cout << "(" << minf0 << "," << maxf0 <<") , ("<<minf1 <<","<<maxf1<<") , ("<<minf2 << "," << maxf2 <<") ";
  cout << "Clase => " << ClaseIndividuo(i);
  cout << endl;
}


/*void multipoblacion::PintaIndividuo(int i) const{
  for (int j=0; j<poblacionesR; j++)
    Pr[j][0].PintaBin(i);
  for (int j=0; j<poblacionesE; j++)
    Pe[j][0].Pinta(i);
  for (int j=0; j<poblacionesB; j++)
    Pb[j][0].Pinta(i);
  cout << "Fitnes: ";
  for (int j=0; j<n_valoracion; j++)
    cout << valoracion[i][j] << " ";
  cout << endl <<endl;

}*/


void multipoblacion::PintaIndividuo(int i) const{
  cout << "Nivel de Valor:     "; Pb[0][0].Pinta(i);
  cout << "Nivel de Variable:  "; Pr[0][0].Pinta(i);
  cout << "Nivel de Funcion:   "; Pe[1][0].Pinta(i);
  cout << "Nivel de VFuncion:  "; Pb[1][0].Pinta(i);
  cout << "Consecuente:        "; Pe[0][0].Pinta(i);
  cout << "Fitnes: ";
  for (int j=0; j<n_valoracion; j++)
    cout << valoracion[i][j] << " ";
  cout << endl <<endl;

}



int multipoblacion::N_individuos() const {
  return n_individuos;
}

int multipoblacion::Elite() const{
  return elitismo;
}

int multipoblacion::N_Valoracion() const{
  return n_valoracion;
}

void multipoblacion::Codigo(int i, genetcode &code) const{
 // Paso a code la parte binaria
  char **bin=new char*[poblacionesB];
  for (int j=0; j<poblacionesB; j++)
     bin[j]=0;
  int *tb=new int[poblacionesB];
  for (int j=0;j<poblacionesB; j++)
    Pb[j][0].Codigo(i,bin[j],tb[j]);

  code.PutBinary(poblacionesB,tb,bin);

  delete [] tb;
  for (int j=0; j<poblacionesB; j++)
    delete [] bin[j];
  delete [] bin;

  // Paso a code la parte entera
  int **ent=new int*[poblacionesE];
  for (int j=0; j<poblacionesE; j++)
   ent[j]=0;
  int *te=new int[poblacionesE];
  for (int j=0;j<poblacionesE; j++)
    Pe[j][0].Codigo(i,ent[j],te[j]);

  code.PutInteger(poblacionesE,te,ent);

  delete [] te;
  for (int j=0; j<poblacionesE; j++)
    delete [] ent[j];
  delete [] ent;

  // Paso a code la parte real
  double **rea=new double*[poblacionesR];
  for (int j=0; j<poblacionesR; j++)
    rea[j]=0;
  int *tr=new int[poblacionesR];
  for (int j=0;j<poblacionesR; j++)
    Pr[j][0].Codigo(i,rea[j],tr[j]);

  code.PutReal(poblacionesR,tr,rea);

  delete [] tr;
  for (int j=0; j<poblacionesR; j++)
    delete [] rea[j];
  delete [] rea;

}



void multipoblacion::PutCodigo(int i, const genetcode &code){
 // Paso a code la parte binaria
  for (int j=0; j<poblacionesB; j++)
    for (int k=0; k<code.TamaBinary(j); k++)
       Pb[j][0].PutValue(i,k,code.GetValueBinary(j,k));


  // Paso a code la parte entera
  for (int j=0; j<poblacionesE; j++)
    for (int k=0; k<code.TamaInteger(j); k++)
       Pe[j][0].PutValue(i,k,code.GetValueInteger(j,k));


  // Paso a code la parte real
  for (int j=0; j<poblacionesR; j++)
    for (int k=0; k<code.TamaReal(j); k++)
       Pr[j][0].PutValue(i,k,code.GetValueReal(j,k));

}




bool multipoblacion::Modificado(int i) const {
  return modificado[i];
}

void multipoblacion::PutModificado(int i){
  modificado[i]=true;
}

void multipoblacion::Valoracion(int i, double *valor){
  for (int j=0; j<n_valoracion; j++)
    valoracion[i][j]=valor[j];
  modificado[i]=false;
}


void multipoblacion::PoblacionInicialAleatoria(){
  for (int j=0; j<poblacionesB; j++)
    Pb[0][0].PoblacionInicialAleatoria();

  for (int j=0; j<poblacionesE; j++)
    Pe[j][0].PoblacionInicialAleatoria();

  for (int j=0; j<poblacionesR; j++)
    Pr[j][0].PoblacionInicialAleatoria();

  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;

}


void multipoblacion::PoblacionInicialAleatoria(int consecuente){
  for (int j=0; j<poblacionesB; j++)
    Pb[j][0].PoblacionInicialAleatoria();

  for (int j=0; j<poblacionesE; j++)
    Pe[j][0].PoblacionInicialAleatoria(consecuente);

  for (int j=0; j<poblacionesR; j++)
    Pr[j][0].PoblacionInicialAleatoria();

  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;

}



void multipoblacion::PoblacionInicialAleatoria(double *v_pesos){
  for (int j=0; j<poblacionesB; j++)
    Pb[0][0].PoblacionInicialAleatoria();

  for (int j=0; j<poblacionesE; j++)
    Pe[j][0].PoblacionInicialAleatoria();

  for (int j=0; j<poblacionesR; j++)
    Pr[j][0].PoblacionInicialAleatoria(v_pesos);

  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;

}



void multipoblacion::PoblacionInicialAleatoria(double **I, int rango){

  Pb[0][0].PoblacionInicialAleatoria();

  Pe[0][0].PoblacionInicialAleatoria();

  Pr[0][0].PoblacionInicialAleatoria(I,rango);
  for (int j=1; j<poblacionesR; j++)
    Pr[j][0].PoblacionInicialAleatoria();

  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;

}

void multipoblacion::PoblacionInicialAleatoria_4L(double **I, int rango, int n_items){

  // Nivel de Valor
  Pb[0][0].PoblacionInicialAleatoria();
  Pb[1][0].PoblacionInicialAleatoria();
  // Consecuente
  Pe[0][0].PoblacionInicialAleatoria();
  // Dominancia
  Pe[1][0].PoblacionInicialAleatoria(0);

  // Nivel de Variable
  Pr[0][0].PoblacionInicialAleatoria(I,rango);
  // Conjunto de ejemplos cubiertos
  Pr[1][0].PoblacionInicialAleatoria(n_items);
  Pr[2][0].PoblacionInicialAleatoria(0,1);


  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;

}

void multipoblacion::PoblacionInicialAleatoria_4L(int rango, int n_items){

  // Nivel de Valor
  Pb[0][0].PoblacionInicialAleatoria();

  // Consecuente
  Pe[0][0].PoblacionInicialAleatoria();
  // Dominancia
  Pe[1][0].PoblacionInicialAleatoria(0);

  // Nivel de Variable
  Pr[0][0].PoblacionInicialAleatoria(0,rango);
  // Conjunto de ejemplos cubiertos
  Pr[1][0].PoblacionInicialAleatoria(n_items);

  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;

}




void multipoblacion::PoblacionInicial(int rango, int n_items, int **sujetos, int n_var, int *n_etiquetas_variable){
  // Nivel de Valor
  Pb[0][0].PoblacionInicial(sujetos, n_var, n_etiquetas_variable);
  Pb[1][0].PoblacionInicialAleatoria();

  // Consecuente
  Pe[0][0].PoblacionInicialAleatoria();
  // Dominancia
  Pe[1][0].PoblacionInicialAleatoria(0);

  // Nivel de Variable
  Pr[0][0].PoblacionInicialAleatoria(0,rango);
  //Conjunto de ejemplos cubiertos
  Pr[1][0].PoblacionInicialAleatoria(n_items);

  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;
}


void multipoblacion::PoblacionInicial(double **I, int rango, int n_items, int **sujetos, int n_var, int *n_etiquetas_variable){
  // Nivel de Valor
  Pb[0][0].PoblacionInicial(sujetos, n_var, n_etiquetas_variable);
  Pb[1][0].PoblacionInicialAleatoria();

  // Consecuente
  Pe[0][0].PoblacionInicialAleatoria();
  // Dominancia
  Pe[1][0].PoblacionInicialAleatoria(0);

  // Nivel de Variable
  Pr[0][0].PoblacionInicial(I,rango);
  //Conjunto de ejemplos cubiertos
  Pr[1][0].PoblacionInicialAleatoria(n_items);

  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;
}






void multipoblacion::PoblacionInicial2_Estacionario(double **I, int rango, int n_items, int **sujetos, int n_var, int *n_etiquetas_variable, int clase){
  // Nivel de Valor
  Pb[0][0].PoblacionInicial2_Estacionario(sujetos, n_var, n_etiquetas_variable, clase);

  // Consecuente
  Pe[0][0].PoblacionInicial2_Estacionario(clase);
  // Dominancia
  Pe[1][0].PoblacionInicial2_Estacionario();

  // Nivel de Variable
  Pr[0][0].PoblacionInicial2_Estacionario();
  //Conjunto de ejemplos cubiertos
  //Pr[1][0].PoblacionInicialAleatoria(n_items);

  for (int j=0; j<n_individuos; j++)
    modificado[j]=true;

  /*for (int j=0; j<n_individuos; j++)
    for (int k=0; k<n_valoracion; k++)
      valoracion[j][k]=0;*/
}








void multipoblacion::MutacionUniforme(){

  for (int j=0; j<poblacionesB; j++){
    Pb[j][0].MutacionUniforme();
    for (int i=0; i<n_individuos; i++)
      if (Pb[j][0].Modificado(i))
        modificado[i]=true;
  }

  for (int j=0; j<poblacionesE; j++){
    Pe[j][0].MutacionUniforme();
    for (int i=0; i<n_individuos; i++)
      if (Pe[j][0].Modificado(i))
        modificado[i]=true;
  }

  for (int j=0; j<poblacionesR; j++){
    Pr[j][0].MutacionUniforme();
    for (int i=0; i<n_individuos; i++)
      if (Pr[j][0].Modificado(i))
        modificado[i]=true;
  }

}



void multipoblacion::MutacionUniformeModEnt(double prob0){

  for (int j=0; j<poblacionesB; j++){
    Pb[j][0].MutacionUniforme();
    for (int i=0; i<n_individuos; i++)
      if (Pb[j][0].Modificado(i))
        modificado[i]=true;
  }

  for (int j=0; j<poblacionesE; j++){
    if (j<1)
      Pe[j][0].MutacionUniforme();
    else
      Pe[j][0].MutacionUniformeMod(prob0);
    for (int i=0; i<n_individuos; i++)
      if (Pe[j][0].Modificado(i))
        modificado[i]=true;
  }

  for (int j=0; j<poblacionesR; j++){
    Pr[j][0].MutacionUniforme();
    for (int i=0; i<n_individuos; i++)
      if (Pr[j][0].Modificado(i))
        modificado[i]=true;
  }

}





void multipoblacion::CruceUniforme(){

  for (int j=0; j<poblacionesB; j++){
    Pb[j][0].CruceUniforme();
    for (int i=0; i<n_individuos; i++)
      if (Pb[j][0].Modificado(i))
        modificado[i]=true;
  }
  for (int j=0; j<poblacionesE; j++){
    Pe[j][0].CruceUniforme();
    for (int i=0; i<n_individuos; i++)
      if (Pe[j][0].Modificado(i))
        modificado[i]=true;
  }


  for (int j=0; j<poblacionesR; j++){
    Pr[j][0].CruceUniforme();
    for (int i=0; i<n_individuos; i++)
      if (Pr[j][0].Modificado(i))
        modificado[i]=true;
  }


}


void multipoblacion::Cruce2puntos(){
  for (int j=0; j<poblacionesB; j++){
    Pb[j][0].Cruce2puntos();
    for (int i=0; i<n_individuos; i++)
      if (Pb[j][0].Modificado(i))
        modificado[i]=true;
  }

  for (int j=0; j<poblacionesE; j++){
    Pe[j][0].Cruce2puntos();
    for (int i=0; i<n_individuos; i++)
      if (Pe[j][0].Modificado(i))
        modificado[i]=true;
  }

  for (int j=0; j<poblacionesR; j++){
    Pr[j][0].Cruce2puntos();
    for (int i=0; i<n_individuos; i++)
      if (Pr[j][0].Modificado(i))
        modificado[i]=true;
  }


}




void multipoblacion::MutacionUniforme_Estacionario_Especial(){
  if (Probabilidad(0.5)){
    Pe[1][0].MutacionUniformeMod_Estacionario_Especial();
  }
  else if (Probabilidad(0.5)){
          Pr[0][0].MutacionUniforme_Estacionario();
       }
}






void multipoblacion::MutacionUniforme_Estacionario(){

bool modificado_alguno=false;

 while (!modificado_alguno){

  if (Probabilidad(0.5)) {
    Pb[0][0].MutacionUniforme_Estacionario();
    modificado_alguno=true;
  }

  if (Probabilidad(0.5)){
    Pb[1][0].MutacionUniforme_Estacionario();
    modificado_alguno=true;
  }

  if (Probabilidad(0.5)){
    Pe[0][0].MutacionUniforme_Estacionario();
    modificado_alguno=true;
  }

  if (Probabilidad(0.8)){
    Pe[1][0].MutacionUniformeMod_Estacionario(0.2);
    modificado_alguno=true;
  }


  if (Probabilidad(0.8)){
    Pr[0][0].MutacionUniforme_Estacionario();
    modificado_alguno=true;
  }

  //Pr[2][0].MutacionUniforme_Estacionario_Normal();
 }

}



void multipoblacion::MutacionUniforme_Estacionario_reales(){

  Pr[0][0].MutacionUniforme_Estacionario_Normal();

}



void multipoblacion::MutacionUniformeModEnt_Estacionario(double prob0){

  for (int j=0; j<poblacionesB; j++){
    Pb[j][0].MutacionUniforme_Estacionario();
    for (int i=0; i<n_individuos; i++)
      if (Pb[j][0].Modificado(i))
        modificado[i]=true;
  }

  for (int j=0; j<poblacionesE; j++){
    if (j>0)
      Pe[j][0].MutacionUniformeMod_Estacionario(prob0);
    else
      Pe[j][0].MutacionUniforme_Estacionario();
    for (int i=0; i<n_individuos; i++)
      if (Pe[j][0].Modificado(i))
        modificado[i]=true;
  }

  for (int j=0; j<poblacionesR; j++){
    Pr[j][0].MutacionUniforme_Estacionario();
    for (int i=0; i<n_individuos; i++)
      if (Pr[j][0].Modificado(i))
        modificado[i]=true;
  }

}







void multipoblacion::CruceUniforme_Estacionario(){

  for (int j=0; j<poblacionesB; j++){
    Pb[j][0].CruceUniforme_Estacionario();
    for (int i=0; i<n_individuos; i++)
      if (Pb[j][0].Modificado(i))
        modificado[i]=true;
  }
  for (int j=0; j<poblacionesE; j++){
    Pe[j][0].CruceUniforme_Estacionario();
    for (int i=0; i<n_individuos; i++)
      if (Pe[j][0].Modificado(i))
        modificado[i]=true;
  }


  for (int j=0; j<poblacionesR; j++){
    Pr[j][0].CruceUniforme_Estacionario();
    for (int i=0; i<n_individuos; i++)
      if (Pr[j][0].Modificado(i))
        modificado[i]=true;
  }


}


void multipoblacion::Cruce2puntos_Estacionario(){
int a=Select_Random_Individuo(n_individuos-2,-1,0);
int b=Select_Random_Individuo(n_individuos-2,a,0);

  for (int j=0; j<poblacionesB; j++)
    Pb[j][0].Cruce2puntos_Estacionario(a,b);


  for (int j=0; j<poblacionesE; j++)
    Pe[j][0].Cruce2puntos_Estacionario(a,b);


  for (int j=0; j<poblacionesR; j++)
    Pr[j][0].Cruce2puntos_Estacionario(a,b);

  modificado[a]=true;
  modificado[b]=true;

}


void multipoblacion::CruceBasedLogical_Estacionario(double it){
int a,b;
  if (Probabilidad(1-it)){
    a=Select_Random_Individuo(n_individuos-2,-1,0);
    b=Select_Random_Individuo(n_individuos-2,a,0);
  } else {
    a=0;
    b=Select_Random_Individuo(n_individuos-2,a,0);
  }

  Pasar(n_individuos-2,a);
  Pasar(n_individuos-1,b);

//cout <<"Cruce de : " << a << " y b: " << b << endl;

  if (Probabilidad(1.0)){
  // Con 0.5 se aplica un cruce sobre 2 puntos.
   modificado[n_individuos-2]=false;
   while (!modificado[n_individuos-2]){
    if (Probabilidad(0.25)){
      Pb[0][0].Cruce2puntos_Estacionario(n_individuos-2,n_individuos-1);
      modificado[n_individuos-2]=true;
      modificado[n_individuos-1]=true;
    }
    if (Probabilidad(0.25)){
      Pb[1][0].Cruce2puntos_Estacionario(n_individuos-2,n_individuos-1);
      modificado[n_individuos-2]=true;
      modificado[n_individuos-1]=true;
    }
    if (Probabilidad(0.25)){
      Pe[1][0].Cruce2puntos_Estacionario(n_individuos-2,n_individuos-1);
      modificado[n_individuos-2]=true;
      modificado[n_individuos-1]=true;
    }
    if (Probabilidad(0.25)){
      Pr[0][0].Cruce2puntos_Estacionario(n_individuos-2,n_individuos-1);
      modificado[n_individuos-2]=true;
      modificado[n_individuos-1]=true;
    }
   }
  }
  else if (ClaseIndividuo(a)==ClaseIndividuo(b)){
      // Si son de la misma clase se aplica el operador AND_OR
        Pb[0][0].CruceAND_OR_Estacionario(a,b);

        Pe[0][0].Cruce2puntos_Estacionario(a,b);

        Pr[0][0].CruceAND_OR_Estacionario(a,b);

      }
       else {
       // Si son de clases distintas se aplica el operador NAND_NOR
          Pb[0][0].CruceNAND_NOR_Estacionario(a,b);

          Pe[0][0].Cruce2puntos_Estacionario(a,b);

          Pr[0][0].CruceNAND_NOR_Estacionario(a,b);

       }

  modificado[n_individuos-2]=true;
  modificado[n_individuos-1]=true;
}



void multipoblacion::CruceBasedLogical_Estacionario_reales(int it){
int a,b;
  if (Probabilidad(0.9)){
    a=Select_Random_Individuo(n_individuos-2,-1,0);
    b=Select_Random_Individuo(n_individuos-2,a,0);
 } else {
    a=0;
    b=Select_Random_Individuo(n_individuos-2,a,0);
 }

 Pr[0][0].Cruce_BLX_Estacionario(0.25,a,b);

 modificado[n_individuos-2]=true;
 modificado[n_individuos-1]=true;
}



void multipoblacion::Seleccion(){
multipoblacion Paux=*this;
double a;
int j=0;


  // Por defecto, los elite se quedan en la poblacion actual

  // Pasar el resto
  for (int i=elitismo; i<n_individuos; i++){
    a=rand()%10+1;
    while (a>0){
      a=a-(1.0/(j+1))*0.5;
      j++;
      if (j==n_individuos)
        j=0;
    }

    for (int k=0; k<poblacionesB; k++){
       Pb[k][0].Pasar(i,Paux.Pb[k][0],j);
    }

    for (int k=0; k<poblacionesE; k++)
       Pe[k][0].Pasar(i,Paux.Pe[k][0],j);

    for (int k=0; k<poblacionesR; k++)
       Pr[k][0].Pasar(i,Paux.Pr[k][0],j);

    for (int k=0; k<n_valoracion; k++)
      valoracion[i][k]=Paux.valoracion[j][k];
      modificado[i]=Paux.modificado[j];
  }

}




double multipoblacion::ValorFitness(int i) const{
  return valoracion[i][0];
}

double multipoblacion::ValorFitness(int i, int j) const{
  return valoracion[i][j];
}



bool multipoblacion::Diferencia(int i, int j, int n, double minimo) const{
  double tot=0;
  int tpi, tpj;
  double *ci, *cj;

  ci = Subpoblacion_Real(1,i,tpi);
  cj = Subpoblacion_Real(1,j,tpj);

  for (int z=0; z<n && tot<=minimo; z++)
    if (ci[z]>0 && cj[z]==0)
      tot++;

  return (tot>minimo);
}

void multipoblacion::Cubrimiento(int i, double *conjunto, int n_examples){
  int n, m;
  double aux, positivos=0, negativos=0;
  double *regla=Subpoblacion_Real(1,i,n);
  int *dom = Subpoblacion_Entera(1,i,m);
  double val_old=valoracion[i][0], pos_old = valoracion[i][3], neg_old=valoracion[i][4];

  for (int j=0; j<n_examples; j++){
    aux=regla[j]*valoracion[i][2];
    if (regla[j]>0 && conjunto[j]==0 ){
      conjunto[j]=aux;
      positivos=positivos+regla[j];
    }
    else if (regla[j]<0 && aux<conjunto[j]){
           conjunto[j]=aux;
           negativos=negativos-regla[j];
         }
  }

  if (positivos>0){
    dom[0]=1;
    valoracion[i][0]=(positivos+negativos)*valoracion[i][2];
  }
  else
    dom[0]=0;
}



void multipoblacion::RecalcularFitness(int i, double *conjunto, int n_examples){
  int n,m;
  double positivos=0;
  double negativos=0;
  double peso = valoracion[i][2];
  double aux;
  double *regla=Subpoblacion_Real(1,i,n);
  int *dom = Subpoblacion_Entera(1,i,m);


  for (int j=0; j<n_examples; j++){
    aux=regla[j]*peso;
    if (conjunto[j]>0 && conjunto[j]==aux)
      positivos=positivos+regla[j];
    else if (conjunto[j]<0 && conjunto[j]==aux)
           negativos=negativos-regla[j];
  }
  if (positivos>0){
    dom[0]=1;
    valoracion[i][0]=(positivos-negativos)*peso;
  }
  else
    dom[0]=0;
}


double multipoblacion::Incremento_Acierto(double *agregado, double *peso_agregado, double *adap_individuo, double peso, int n_examples) const{
  double aciertos=0;
  double fallos=0;
  double acierto_old=0;
  double fallo_old=0;


  for (int i=0; i<n_examples; i++){
    if (agregado[i]>0){
      acierto_old++;
      if (adap_individuo[i]<0 && (-(adap_individuo[i]*peso)>agregado[i] || (-(adap_individuo[i]*peso)==agregado[i] && peso>peso_agregado[i]))){
        fallos++;
      }
    }
    else {
           fallo_old++;
           if ( adap_individuo[i]>0 && ((adap_individuo[i]*peso)>-agregado[i] || ((adap_individuo[i]*peso)==-agregado[i]) && peso>peso_agregado[i])) {
             aciertos++;
           }
    }
  }

  return acierto_old-fallos+aciertos;
}


double Maximum(double a, double b){
  if (a>b)
   return a;
  else
   return b;
}

double Valor_Absoluto(double x){
  if (x<0)
   return -x;
  else
   return x;
}


void multipoblacion::Combinar(double *agregado, double *peso_agregado, double *disparada, double *adap_individuo, double peso, int n_examples, int ind) const{

  double v_abs1, v_abs2;
  //bool *ha_lanzado = new bool[ind+1];

  // Esto es nuevo para eliminar de la elite las reglas que no aportan
  //for (int i=0; i<=ind; i++)
  //  ha_lanzado[i]=false;

  for (int i=0; i<n_examples; i++){
    v_abs1 = Valor_Absoluto(adap_individuo[i]*peso);
    v_abs2 = Valor_Absoluto(agregado[i]);
    if (v_abs1>v_abs2 || (v_abs1==v_abs2 && peso>peso_agregado[i])){
      agregado[i]=adap_individuo[i]*peso;
      peso_agregado[i]=peso;
      disparada[i]=ind;
    }
    //if (disparada[i]!=-1)
    //  ha_lanzado[(int)disparada[i]]=true;
  }

//   int *don, ncl;
//   for (int i=0; i<=ind; i++){
//     if (!ha_lanzado[i]){
//       don=Subpoblacion_Entera(1,i,ncl);
//       don[0]=0;
//     }
//   }




/*    if (adap_individuo[i]>0 && agregado[i]>=0){
      if (adap_individuo[i]*peso > agregado[i]){
        disparada[i]=ind;
        agregado[i]=adap_individuo[i]*peso;
      }
    }
    else if (adap_individuo[i]>0 && agregado[i]<0 && adap_individuo[i]*peso>-agregado[i]){
           agregado[i]=adap_individuo[i]*peso;
           disparada[i]=ind;
         }
         else if (adap_individuo[i]<0 && agregado[i]>0 && -adap_individuo[i]*peso>agregado[i]){
                agregado[i]=adap_individuo[i]*peso;
                disparada[i]=ind;
              }
              else if (adap_individuo[i]<0 && agregado[i]<=0){
                     if (-adap_individuo[i]*peso>-agregado[i]){
                       disparada[i]=ind;
                       agregado[i]=adap_individuo[i]*peso;
                     }
                   }
  }*/
}



double multipoblacion::Refitness(double *agregacion, double *peso_agregacion, int ind, int n_examples){
  double aciertos=0, errores=0;
  double v_abs1, v_abs2;
  int tam;
  double *adaptacion = Subpoblacion_Real(1,ind,tam);
  for (int i=0; i<n_examples; i++){
    v_abs1 = Valor_Absoluto(adaptacion[i]*valoracion[ind][2]);
    v_abs2 = Valor_Absoluto(agregacion[i]);
    if (agregacion[i]>0){ // Esta cubierto
       if (adaptacion[i]<0 && (v_abs1 > v_abs2 || (v_abs1==v_abs2 && valoracion[ind][2]>peso_agregacion[i])))
         errores=errores+v_abs1;
    }
    else { // No esta cubierto
       if (adaptacion[i]>0 && v_abs1 > v_abs2)
         aciertos=aciertos+v_abs1;
       else if (adaptacion[i]<0 && v_abs1 > v_abs2)
              errores=errores+v_abs1;
    }

  }

  if (aciertos==0 && errores==0)
    return -n_examples;
  else
    return aciertos-errores;
}
