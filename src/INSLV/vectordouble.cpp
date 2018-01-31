using namespace std;
#include <iostream>
#include <iomanip>
#include "INSLV/vectordouble.h"


vectordouble::vectordouble(){
  reservado=0;
  numero=0;
    data=0;
}


vectordouble::vectordouble(int tamano){
  reservado=tamano;
  numero=0;
  data = new double[tamano];
}


vectordouble::vectordouble(double *x, int tamano){
  reservado=tamano;
  numero=tamano;
  data = new double[tamano];

  for (int i=0; i<tamano; i++)
    data[i]=x[i];
}


vectordouble::vectordouble(const vectordouble &x){
  reservado=x.reservado;
  numero=x.numero;
  data = new double[reservado];

  for (int i=0; i<numero; i++)
    data[i]=x.data[i];
}

vectordouble &vectordouble::operator=(const vectordouble &x){
  if (reservado!=0)
    delete []data;

  reservado=x.reservado;
  numero=x.numero;
  data = new double[reservado];

  for (int i=0; i<numero; i++)
    data[i]=x.data[i];

  return *this;
}

vectordouble::~vectordouble(){
  if (reservado!=0)
    delete [] data;
  reservado=0;
  numero=0;
  data=0;
}

void vectordouble::Realloc(){
  double *x;

  x = new double[reservado*2+1];
  for (int i=0; i<numero; i++)
    x[i]=data[i];

  delete [] data;
  data = x;
  reservado=reservado*2+1;
}



void vectordouble::Append(double x){
  if (numero==reservado)
    Realloc();
  data[numero]=x;
  numero++;
}


void vectordouble::Put(double x, int pos){
  if (pos<0 || pos>numero){
    cout << "No existe esa posicion" << pos << endl;
  }
  else
    data[pos]=x;
}


void vectordouble::Put(double *x, int tamano){
  if (reservado!=0)
    delete []data;

  reservado=tamano;
  numero=tamano;
  data = new double[tamano];

  for (int i=0; i< tamano; i++)
    data[i]=x[i];
}


double* vectordouble::Convert(int &tamano) const {
  double *x;

  x = new double[numero];
  for (int i=0; i<numero; i++)
    x[i]=data[i];

  tamano=numero;
  return x;
}



double vectordouble::At(int pos) const {
  if (pos<0 || pos>numero){
    cout << "No existe esa posicion" << pos << endl;
    return -999999999; /*devuelve un error*/
  }
  else
    return data[pos];
}




void vectordouble::Pinta() const {
  cout << "\n==========================\n";
  for (int i=0; i<numero; i++)
    cout << " " << setw(6) << i;
  cout << endl;

  for (int i=0; i<numero; i++)
    cout << " " << setw(6) << data[i];
  cout << "\n==========================\n";
}



double vectordouble::Max() const {
  double max;

  if (numero>0){
    max=data[0];
    for (int i=1; i<numero; i++)
       if (data[i]>max)
         max=data[i];
    return max;
  }
  else
    return -999999999;
}




double vectordouble::Min() const {
  double min;

  if (numero>0){
    min=data[0];
    for (int i=1; i<numero; i++)
       if (data[i]<min)
         min=data[i];
    return min;
  }
  else
    return -999999999;
}

int vectordouble::find(double x) const {
  int pos=-1;
  for (int i=0; i<numero && pos==-1; i++)
    if (data[i]==x)
      pos=i;

  return pos;
}


int vectordouble::PosMax() const{
  int pos=0;

  if (numero==0)
    return -1;

  for (int i=1; i<numero; i++)
    if (data[i]>data[pos]){
      pos=i;
    }

  return pos;
}



int vectordouble::PosMin() const{
  int pos=0;

  if (numero==0)
    return -1;

  for (int i=1; i<numero; i++)
    if (data[i]<data[pos]){
      pos=i;
    }

  return pos;
}



double vectordouble::Average() const {
  double media=0;

  if (numero>0){
    for (int i=0; i<numero; i++)
      media=media+data[i];

    return media/numero;
  }
  else
    return -999999999;
}


void vectordouble::Sort(){
  double x;

  for (int i=0; i<numero-1; i++)
    for (int j=numero-1;j>i;j--)
      if (data[j-1]>data[j]){
         x=data[j];
	 data[j]=data[j-1];
	 data[j-1]=x;
      }
}
