#include <stdlib.h>
#include "INSLV/operaciones.h"

bool Probabilidad(double x){
  double a=(1.0*rand()/(RAND_MAX*1.0));
  return (a<=x);
}


int Select_Random_Individuo(int n, int menos_este, int eli){
  double nd=n;
  int a=(nd*rand()/(RAND_MAX+1.0));

  while (a==menos_este || a<eli)
    a=(nd*rand()/(RAND_MAX+1.0));

  return a;
}



int PuntoCorte(int n){
  double nd=n;
  return (1.0*n*rand()/(RAND_MAX+1.0));
}

void PuntoCorte2(int n, int &a, int &b){
  double nd=n;
  a=(nd*rand()/(RAND_MAX+1.0));
  b=(nd*rand()/(RAND_MAX+1.0));
  if (a>b){
    int aux=a;
    a=b;
    b=aux;
  }
}




void Swap_int(int &a, int &b){
  int aux=a;
  a=b;
  b=aux;
}


void Swap_double(double &a, double &b){
  double aux=a;
  a=b;
  b=aux;
}

void Swap_bool(bool &a, bool &b){
  bool aux=a;
  a=b;
  b=aux;
}
