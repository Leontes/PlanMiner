/***************************************************************************
                          ruleset.cpp  -  description
                             -------------------
    begin                : Mon May 6 2002
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
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "INSLV/ruleset.h"
#include "INSLV/Relaciones.h"

using namespace std;


// Constructor por defecto de la clase
ruleset::ruleset(){
 reservado=10;
 n_rule=0;
 rule= new genetcode *[reservado];
 peso= new double[reservado];
 domain=0;
 domain_funcion=0;
 n_mas = new double[reservado];
 n_menos= new double[reservado];
 modificada = new bool [reservado];
}

// Constructor a partir de un vector de variables
ruleset::ruleset(const VectorVar &x){
 reservado=10;
 n_rule=0;
 rule = new genetcode *[reservado];
 peso= new double[reservado];
 domain = new VectorVar;
 domain[0]=x;
 n_mas = new double[reservado];
 n_menos= new double[reservado];
 modificada = new bool [reservado];
}


// Constructor de copia
ruleset::ruleset(const ruleset &x){
 reservado=x.reservado;
 n_rule=x.n_rule;
 rule = new genetcode*[reservado];
 peso= new double[reservado];
 n_mas = new double[reservado];
 n_menos= new double[reservado];
 modificada = new bool [reservado];
 for (int i=0; i<n_rule; i++){
    rule[i] = new genetcode;
    rule[i][0] = x.rule[i][0];
    peso[i] = x.peso[i];
    n_mas[i]=x.n_mas[i];
    n_menos[i]=x.n_menos[i];
    modificada[i]=x.modificada[i];
 }
 domain = new VectorVar;
 domain[0] = x.domain[0];
 domain_funcion= new VectorVar;
 domain_funcion[0]= x.domain_funcion[0];
}


// Destructor de la clase
ruleset::~ruleset(){
 for (int i=0; i<n_rule; i++)
   delete rule[i];

 delete [] rule;
 delete [] peso;
 delete [] n_mas;
 delete [] n_menos;
 delete [] modificada;
 if (domain!=0)
    delete domain;
 if (domain_funcion!=0)
    delete domain_funcion;
 reservado=n_rule=0;
}


ruleset& ruleset::operator=(const ruleset& x)
{
  if (this!=&x){
    if (reservado>0){
      for (int i=0; i<n_rule; i++)
	delete rule[i];

      delete [] rule;
      delete [] peso;
      delete [] n_mas;
      delete [] n_menos;
      delete [] modificada;
      if (domain!=0)
	  delete domain;
      if (domain_funcion!=0)
	  delete domain_funcion;
    }

    reservado=x.reservado;
    n_rule=x.n_rule;
    rule = new genetcode*[reservado];
    peso= new double[reservado];
    n_mas = new double[reservado];
    n_menos= new double[reservado];
    modificada = new bool [reservado];
    for (int i=0; i<n_rule; i++){
	rule[i] = new genetcode;
	rule[i][0] = x.rule[i][0];
	peso[i] = x.peso[i];
	n_mas[i]=x.n_mas[i];
	n_menos[i]=x.n_menos[i];
	modificada[i]=x.modificada[i];
    }
    domain = new VectorVar;
    domain[0] = x.domain[0];
    domain_funcion= new VectorVar;
    domain_funcion[0]= x.domain_funcion[0];

    return *this;
  }

}


// Añadir el dominio a un conjunto de reglas
// Precondicion: Usar solo una vez sobre un ruleset por el constructor por defecto
void ruleset::AddDomain(const VectorVar &x){
  domain = new VectorVar;
  domain[0]=x;
}

void ruleset::AddDomain_Funcion(const VectorVar &x){
  domain_funcion = new VectorVar;
  domain_funcion[0]=x;
}

// Operador de reubicaci� de un conjunto de reglas.
void ruleset::Realloc(){
  genetcode **aux = new genetcode*[2*reservado+1];
  double *p = new double[2*reservado+1];
  double *mas = new double[2*reservado+1];
  double *menos = new double[2*reservado+1];
  bool *m = new bool[2*reservado+1];

  for (int i=0; i<n_rule; i++){
    aux[i]=rule[i];
    rule[i]=0;
    p[i]=peso[i];
    mas[i]=n_mas[i];
    menos[i]=n_menos[i];
    m[i]=modificada[i];
  }
  delete [] rule;
  delete [] peso;
  delete [] n_mas;
  delete [] n_menos;
  delete [] modificada;
  rule = aux;
  peso = p;
  n_mas = mas;
  n_menos = menos;
  modificada = m;
  reservado=2*reservado+1;
}

// Aniade al principio del ruleset el conjunto de reglas incluido como argumento.
void ruleset::AddRuleset(ruleset &r){
  int i=0;
  while (n_rule+r.n_rule>=reservado){
    i++;
    //cout << "Realojo " << i << endl;
    Realloc();
  }

  for (int i=n_rule-1; i>=0; i--){
    rule[i+r.n_rule]=rule[i];
    peso[i+r.n_rule]=peso[i];
    n_mas[i+r.n_rule]=n_mas[i];
    n_menos[i+r.n_rule]=n_menos[i];
    modificada[i+r.n_rule]=modificada[i];


    //cout << "muevo la regla " << i << " a la posicion " << i+r.n_rule << endl;
  }
  n_rule=n_rule+r.n_rule;

  for (int i=0; i<r.n_rule; i++){
    rule[i]= new genetcode;
    genetcode *x = r.Get_Code_Rule(i);
    rule[i][0]= x[0];
    peso[i]=r.peso[i];
    n_mas[i]=r.n_mas[i];
    n_menos[i]=r.n_menos[i];
    modificada[i]=r.peso[i];
//cout << "Introducida la regla:\n";
    //cout << SPrint(i) << endl;
    //cout << " coloco la nueva regla " << i << endl;
  }
}

// Operador que a�de una nueva regla al final del conjunto de reglas
void ruleset::Add(const genetcode &x) {
 if (reservado==n_rule){
   Realloc();
 }
 rule[n_rule] = new genetcode;
 rule[n_rule][0] = x;
 peso[n_rule]=0;
 n_mas[n_rule]=0;
 n_menos[n_rule]=0;
 modificada[n_rule]=true;
 n_rule++;
}


// Operador que a�de una nueva regla al final del conjunto de reglas.
// Asocia a la regal el peso "weight".
void ruleset::Add(const genetcode &x, double weight) {
 if (reservado==n_rule){
   Realloc();
 }
 rule[n_rule] = new genetcode;
 rule[n_rule][0] = x;
 peso[n_rule]=weight;
 n_mas[n_rule]=0;
 n_menos[n_rule]=0;
 modificada[n_rule]=true;
 n_rule++;
}


void ruleset::Add(const genetcode &x, double mas, double menos, bool modif) {
 if (reservado==n_rule){
   Realloc();
 }
 rule[n_rule] = new genetcode;
 rule[n_rule][0] = x;
 n_mas[n_rule]=mas;
 n_menos[n_rule]=menos;
 modificada[n_rule]=modif;
 peso[n_rule]=(n_mas[n_rule]+1)/(n_mas[n_rule]+n_menos[n_rule]+1);
 n_rule++;
}


// Operador que asocia el peso "weight" a la regla "rule"-esima del conjunto de reglas.
void ruleset::Add_Weight(int rule, double weight) {
   peso[rule]=weight;
}


void ruleset::Add_Weight(int rule, double mas, double menos) {
  n_mas[rule]+=mas;
  n_menos[rule]+=menos;
  peso[rule]=(n_mas[rule]+1)/(n_mas[rule]+n_menos[rule]+1);
}


void ruleset::Recalculate_Weight(int i){
  peso[i]=(n_mas[i]+1)/(n_mas[i]+n_menos[i]+1);
}

void ruleset::Recalculate_Weight(){
  for (int i=0; i<n_rule; i++)
    Recalculate_Weight(i);
}


double ruleset::Get_Weight(int rule) const{
  return peso[rule];
}


void ruleset::Put_N_Mas(int rule, double value) {
   n_mas[rule]=value;
}

void ruleset::Add_N_Mas(int rule, double value) {
   n_mas[rule]+=value;
}


double ruleset::Get_N_Mas(int rule) const{
  return n_mas[rule];
}

void ruleset::Add_N_Menos(int rule, double value) {
   n_menos[rule]+=value;
}

void ruleset::Put_N_Menos(int rule, double value) {
   n_menos[rule]=value;
}


double ruleset::Get_N_Menos(int rule) const{
  return n_menos[rule];
}


void ruleset::Modificada(int rule, bool value) {
   modificada[rule]=value;
}


double ruleset::Get_Modificada(int rule) const{
  return modificada[rule];
}

void ruleset::FixModifyToTrue(){
  for (int i=0; i<n_rule; i++)
    modificada[i]=true;
}


void ruleset::Anular_N_Mas_y_N_Menos(int i){
  n_menos[i]=0;
  n_mas[i]=0;
}


void ruleset::Anular_N_Mas_y_N_Menos_Todas(){
  for (int i=0; i<n_rule; i++)
    Anular_N_Mas_y_N_Menos(i);
}


// Operador que elimina la ultima regla del conjunto de reglas.
void ruleset::Remove(){
   delete rule[n_rule-1];
   n_rule--;
}


// Extrae una regla del conjunto de reglas
genetcode *ruleset::Extract(int i, double &weight){
  genetcode *aux=0;
  if (i<n_rule){
    aux=rule[i];
    weight=peso[i];
    for (int j=i+1; j<n_rule; j++){
       rule[j-1]=rule[j];
       peso[j-1]=peso[j];
       n_mas[j-1]=n_mas[j];
       n_menos[j-1]=n_menos[j];
       modificada[j-1]=modificada[j];
    }
    n_rule--;
  }
  else
    weight=-1;

  return aux;
}



// Extrae una regla del conjunto de reglas
genetcode *ruleset::Extract(int i, double &mas, double &menos, bool modif){
  genetcode *aux=0;
  if (i<n_rule){
    aux=rule[i];
    mas=n_mas[i];
    menos=n_menos[i];
    modif=modificada[i];

    for (int j=i+1; j<n_rule; j++){
       rule[j-1]=rule[j];
       peso[j-1]=peso[j];
       n_mas[j-1]=n_mas[j];
       n_menos[j-1]=n_menos[j];
       modificada[j-1]=modificada[j];
    }
    n_rule--;
  }
  else{
    mas = menos =-1;
  }

  return aux;
}


// Devuelve el codigo asociado a la regla i
genetcode *ruleset::Get_Code_Rule(int i) const {
  genetcode *aux;

  if (i<n_rule)
   aux=rule[i];
  else
    cout << "Error en Get_Code_Rule: Indice " << i << " de regla fuera de rango\n";

  return aux;
}

void ruleset::Insert(int i, genetcode *r, double weight){
  if (i<=n_rule){
    if (reservado==n_rule)
      Realloc();

    for (int j=n_rule; j>i; j--){
       rule[j]=rule[j-1];
       peso[j]=peso[j-1];
       n_mas[j]=n_mas[j-1];
       n_menos[j]=n_menos[j-1];
       modificada[j]=modificada[j-1];
    }

    rule[i]=r;
    peso[i]=weight;
    n_mas[i]=0;
    n_menos[i]=0;
    modificada[i]=false;
    n_rule++;
  }
}


void ruleset::Insert(int i, genetcode *r, double mas, double menos, bool modif){
  if (i<=n_rule){
    if (reservado==n_rule)
      Realloc();

    for (int j=n_rule; j>i; j--){
       rule[j]=rule[j-1];
       peso[j]=peso[j-1];
       n_mas[j]=n_mas[j-1];
       n_menos[j]=n_menos[j-1];
       modificada[j]=modificada[j-1];
    }

    rule[i]=r;
    n_mas[i]=mas;
    n_menos[i]=menos;
    modificada[i]=modif;
    peso[i] = (n_mas[i]+1)/(n_mas[i]+n_menos[i]+1);
    n_rule++;
  }
}



// Operador que devuelve el numero de reglas que contiene el conjunto de reglas.
int ruleset::N_rule() const {
  return n_rule;
}


// Operador que devuelve el numero de reglas del conjunto de reglas con peso superior o igual a weight.
int ruleset::N_rule(double weight) const {
  int cont=0;
  for (int i=0; i<n_rule; i++)
    if (peso[i]>=weight)
      cont++;
  return cont;
}


// Devuelve el numero medio de variables por regla en la base de reglas.
double ruleset::Variables_per_rule() const{
 int r,*nr;
 double **mr, cont=0;

 for (int i=0; i<n_rule; i++){
   rule[i][0].GetReal(r,nr,mr);
   for (int j=0;j<nr[0]-1;j++)
     if (mr[0][j]>=mr[0][nr[0]-1])
       cont=cont+1;
 }

 return cont/n_rule;
}


int ruleset::Numero_Ceros_of_rule(int i) const{
 int r,*nr,*nn,**nnn,n;
 int b, *nb;
 char **mb;
 double **mr, cont1=0, cont2=0;
 int tama=0, acu=0;

   rule[i][0].GetBinary(b,nb,mb);
   rule[i][0].GetReal(r,nr,mr);

   for (int j=0;j<nr[0]-1;j++){
     int longitud=domain[0].SizeDomain(j);
     if (mr[0][j]>=mr[0][nr[0]-1]){
       for (int s=0; s<longitud; s++)
         if (mb[0][tama+s]=='0')
	   acu++;
     }
     tama+=longitud;
   }

 return acu;
}



int ruleset::Variables_of_rule(int i) const{
 int r,*nr,*nn,**nnn,n;
 double **mr, cont1=0, cont2=0;

   rule[i][0].GetReal(r,nr,mr);
   for (int j=0;j<nr[0]-1;j++)
     if (mr[0][j]>=mr[0][nr[0]-1])
       cont1=cont1+1;

   rule[i][0].GetInteger(n,nn,nnn);
   for (int j=0; j<nn[1]; j++)
     if (nnn[1][j]!=0)
       cont2++;

 return cont1+cont2;
}



int ruleset::New_Atributes_of_rule(int i) const{
 int r,*nr,*nn,**nnn,n;
 double **mr, cont1=0, cont2=0;

 rule[i][0].GetInteger(n,nn,nnn);
 for (int j=0; j<nn[1]; j++)
   if (nnn[1][j]!=0)
     cont2++;

 return cont2;
}




bool Comprensible (char *s, int tama)
{
    int sec_unos=0, sec_ceros=0, n_ceros=0;
    bool act_unos = (s[0]=='1');

    if (act_unos)
        sec_unos=1;
    else
    {
        sec_ceros=1;
        n_ceros=1;
    }

    for (int i=1; i<tama; i++)
    {
        if (!act_unos && s[i]=='1')
        {
            act_unos=true;
            sec_unos++;
        }
        else if (!act_unos && s[i]=='0')
        {
            n_ceros++;
        }
        else if (act_unos && s[i]=='0')
        {
            sec_ceros++;
            n_ceros++;
            act_unos=false;
        }
    }

    return ( (sec_unos==1) || (n_ceros==1) );
}






int ruleset::Stable_of_rule(int i) const{
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla, regla_funcion;
 int var1=0, var2=0;
 int n_ant;

 // extraer la componente binaria
 rule[i][0].GetBinary(b,nb,mb);
 char *s= new char[nb[0]+1];
 for (int j=0; j<nb[0]; j++)
   s[j]=mb[0][j];
 s[nb[0]]='\0';
 regla=s;
 delete []s;

 s= new char[nb[1]+1];
 for (int j=0; j<nb[1]; j++)
   s[j]=mb[1][j];
 s[nb[1]]='\0';
 regla_funcion=s;
 delete []s;


 rule[i][0].GetInteger(n,nn,nnn);
 rule[i][0].GetReal(r,nr,mr);
 n_ant = domain[0].N_Antecedente();
 int z=0;
 int j=0;
 int donde=0;
 int n_casos=0;
 while (j<n_ant){
   if (domain[0].IsActiva(j) && domain[0].IsAntecedente(j)){
     n_casos=domain[0].SizeDomain(j);
     if (mr[0][j]>=mr[0][nr[0]-1]){
       if (Comprensible(&regla[donde],n_casos) )
         var1++;
     }

   }
   donde+=n_casos;
   j++;
 }

 //Parte de funciones
 donde=0;
 for (int i=0; i<nn[1]; i++){
     if (nnn[1][i]!=0){
        n_casos=domain_funcion[0].SizeDomain(nnn[1][i]-1);
        if (Comprensible(&regla_funcion[donde],n_casos))
          var2++;
     }
     n_casos=5;
     donde+=n_casos;
 }

 return var1+var2;
}




double *ruleset::SeeAdaptaciones_of_rule(int i) const{
  double *ad;
  int r, *nr;
  double **mr;

  rule[i][0].GetReal(r,nr,mr);
  ad = mr[1];
  return ad;
}



//Devuelve la proporcion de variables usadas en las reglas en relacion al global de variables involucradas

double ruleset::Variables_Used() const{
 int r,*nr,b,*nb;
 char **mb;
 double **mr, cont=0;

 rule[0][0].GetReal(r,nr,mr);
 int *used= new int[nr[0]];

 for (int j=0; j<nr[0]-1; j++)
   used[j]=0;

 int donde,z, n_casos;
 char *s;
 for (int i=0; i<n_rule; i++){
   rule[i][0].GetReal(r,nr,mr);
   rule[i][0].GetBinary(b,nb,mb);
   donde=0;
   s= new char[nb[0]+1];
   for (int j=0; j<nb[0]; j++)
     s[j]=mb[0][j];
   s[nb[0]]='\0';

   for (int j=0;j<nr[0]-1;j++){
     n_casos=domain[0].SizeDomain(j);
     for(z=0; z<n_casos && s[donde+z]=='1';z++);

     if (mr[0][j]>=mr[0][nr[0]-1] && z!=n_casos)
       used[j]++;
     donde+=n_casos;
   }

   delete []s;
 }

 for (int j=0; j<nr[0]-1; j++)
   if (used[j]>0)
     cont=cont+1;

 delete [] used;

 return cont;
}



//Devuelve la proporcion de variables usadas en las reglas en relacion al global de variables involucradas
// y ademas en frec devuelve el numero de veces que aparece cada variable en la base de reglas

double ruleset::Frecuence_each_Variables(int *frec) const {
 int r,*nr,b,*nb;
 char **mb;
 double **mr, cont=0;

 rule[0][0].GetReal(r,nr,mr);

 for (int j=0; j<nr[0]-1; j++)
   frec[j]=0;

 int donde,z, n_casos;
 char *s;
 for (int i=0; i<n_rule; i++){
   rule[i][0].GetReal(r,nr,mr);
   rule[i][0].GetBinary(b,nb,mb);
   donde=0;
   s= new char[nb[0]+1];
   for (int j=0; j<nb[0]; j++)
     s[j]=mb[0][j];
   s[nb[0]]='\0';

   for (int j=0;j<nr[0]-1;j++){
     n_casos=domain[0].SizeDomain(j);
     for(z=0; z<n_casos && s[donde+z]=='1';z++);

     if (mr[0][j]>=mr[0][nr[0]-1] && z!=n_casos)
       frec[j]++;
     donde+=n_casos;
   }

   delete []s;
 }

 for (int j=0; j<nr[0]-1; j++)
   if (frec[j]>0)
     cont=cont+1;



 return cont/(nr[0]-1);
}


int ruleset::Condiciones_per_RB() const{
 int r,*nr,b,*nb;
 char **mb;
 double **mr, cont=0;

 rule[0][0].GetReal(r,nr,mr);

 int donde,z, n_casos, medida=0;
 char *s;
 for (int i=0; i<n_rule; i++){
   rule[i][0].GetReal(r,nr,mr);
   rule[i][0].GetBinary(b,nb,mb);
   donde=0;
   s= new char[nb[0]+1];
   for (int j=0; j<nb[0]; j++)
     s[j]=mb[0][j];
   s[nb[0]]='\0';

   for (int j=0;j<nr[0]-1;j++){
     n_casos=domain[0].SizeDomain(j);
     for(z=0; z<n_casos && s[donde+z]=='1';z++);

     if (mr[0][j]>=mr[0][nr[0]-1] && z!=n_casos)
       medida++;
     donde+=n_casos;
   }

   delete []s;
 }


 return medida;

}

// Operador que devuelve el grado de satisfacci� de un ejemplo (v) con el conjunto de reglas.
//
int ruleset::InferenceC(const vectordouble &v) const{
 double max=0,aux;
 int re=-1;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla, regla_funcion;
 int var1, var2;
 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;

    s= new char[nb[1]+1];
    for (int j=0; j<nb[1]; j++)
      s[j]=mb[1][j];
    s[nb[1]]='\0';
    regla_funcion=s;
    delete []s;

    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);
    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]-1]);
    aux=domain_funcion[0].Adaptacion_funcion(v,regla_funcion, nnn[1], nn[1], aux);

    //Aplicar las relaciones

    //aux=domain[0].Adaptacion(v,regla);

    aux = aux*peso[i];

    if (aux>max){
     max=aux;
     re=i;
    }
    else if (re!=-1 && aux>0 && aux==max && peso[i]>peso[re]) {
           max=aux;
           re=i;
         }
 }
 if (re!=-1) {
   rule[re][0].GetInteger(n,nn,nnn);
   return nnn[0][0];
 }
 else
   return -1;
}


// Operador que devuelve el ordinal de la regla con mejor adaptacion sobre el ejemplo (v) con el conjunto de reglas y
// el grado de adaptacion de esa regla
//
int ruleset::InferenceC(const vectordouble &v, double &grado) const{
 double max=0,aux;
 int re=-1;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla, regla_funcion;
 int var1, var2;
 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;

    s= new char[nb[1]+1];
    for (int j=0; j<nb[1]; j++)
      s[j]=mb[1][j];
    s[nb[1]]='\0';
    regla_funcion=s;
    delete []s;
    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);
    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]-1]);
    aux=domain_funcion[0].Adaptacion_funcion(v,regla_funcion, nnn[1], nn[1], aux);

    //Aplicar las relaciones

    //aux=domain[0].Adaptacion(v,regla);

    aux = aux*peso[i];



    if (aux>max){
     max=aux;
     re=i;
     grado = aux;
    }
    else if (re!=-1 && aux>0 && aux==max && peso[i]>peso[re]) {
           max=aux;
           re=i;
           grado = aux;
         }
 }
 if (re!=-1) {
   rule[re][0].GetInteger(n,nn,nnn);
   return nnn[0][0];
 }
 else
   return -1;
}



// Operador que devuelve el ordinal de la regla con mejor adaptacion sobre el ejemplo (v) con el conjunto de reglas y
// el grado de adaptacion de esa regla
//
int ruleset::InferenceC(const vectordouble &v, double &grado, int &regla_disparada) const{
 double max=0,aux;
 int re=-1;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla, regla_funcion;
 int var1, var2;


 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;

    s= new char[nb[1]+1];
    for (int j=0; j<nb[1]; j++)
      s[j]=mb[1][j];
    s[nb[1]]='\0';
    regla_funcion=s;
    delete []s;

    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);
    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]-1]);
    aux=domain_funcion[0].Adaptacion_funcion(v,regla_funcion, nnn[1], nn[1], aux);

    //Aplicar las relaciones

    //aux=domain[0].Adaptacion(v,regla);

    aux = aux*peso[i];



    if (aux>max){
     max=aux;
     re=i;
     grado = aux;
    }
    else if (re!=-1 && aux>0 && aux==max && peso[i]>peso[re]) {
           max=aux;
           re=i;
           grado = aux;
         }
 }

 regla_disparada=re;

 if (re!=-1) {
   rule[re][0].GetInteger(n,nn,nnn);
   return nnn[0][0];
 }
 else
   return -1;
}






int ruleset::InferenceC(const vectordouble &v, int clase, double &grado_p, double &peso_p, double &grado_n, double &peso_n, int &regla_disparada, int &aislada) const{
 double max=0,aux, grado=0;
 int re=-1;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla, regla_funcion;
 int var1, var2;
 double *adaptacion_particular = new double[n_rule];
 bool clasifica_bien=false;
 int de_clase=0, de_noclase=0, indice_pos=n_rule, indice_neg=n_rule;

 grado_p=0;
 peso_p=0;
 grado_n=0;
 peso_n=0;

 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;

    s= new char[nb[1]+1];
    for (int j=0; j<nb[1]; j++)
      s[j]=mb[1][j];
    s[nb[1]]='\0';
    regla_funcion=s;
    delete []s;

    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);

    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]-1]);
    aux=domain_funcion[0].Adaptacion_funcion(v,regla_funcion, nnn[1], nn[1], aux);

    //Aplicar las relaciones

    //aux=domain[0].Adaptacion(v,regla);

    aux = aux*peso[i];

    if (nnn[0][0]==clase)
      adaptacion_particular[i]=aux;
    else
      adaptacion_particular[i]=-aux;



    if (aux>0) {
      if (nnn[0][0]==clase){
        if (aux>grado_p or (aux==grado_p and peso[i]>peso_p)){
          grado_p=aux;
          peso_p=peso[i];
          indice_pos=i;
        }
      }
      else {
        if (aux>grado_n or (aux==grado_n and peso[i]>peso_n)){
          grado_n=aux;
          peso_n=peso[i];
          indice_neg=i;
        }
      }
    }


    if (aux>max){
     max=aux;
     re=i;
     grado = aux;
     if (nnn[0][0]==clase)
       clasifica_bien=true;
     else
       clasifica_bien=false;
    }
    else if (re!=-1 && aux>0 && aux==max && peso[i]>peso[re]) {
           max=aux;
           re=i;
           grado = aux;
           if (nnn[0][0]==clase)
             clasifica_bien=true;
           else
            clasifica_bien=false;
         }
 }

 // Numero de reglas de la clase que clasifican correctamente el ejemplo antes de una regla que lo clasifical mal o no lo clasifica.

 if (clasifica_bien){
   de_clase=0;
   for (int i=0; i<n_rule;i++)
     if (adaptacion_particular[i]>0 and
        ((adaptacion_particular[i]>grado_n) or
        (adaptacion_particular[i]==grado_n and peso[i]>peso_n) or
        (adaptacion_particular[i]==grado_n and peso[i]==peso_n and i<indice_neg)) )
       de_clase++;
   if (de_clase==0){
      cout << "Error!!!!  Clase " << clase << "  Grado Pos: " << grado_p << " Grado Neg: "<< grado_n << "  Regla: " << re <<endl;
      for (int t=0; t<n_rule; t++)
        cout <<"\t"<<adaptacion_particular[t];
      cout << endl;
   }

   aislada=de_clase;
 }
 else {
   if (grado_p==0)
     de_noclase=-1000;
   else {
     de_noclase=0;
     for (int i=0; i<n_rule; i++)
       if (adaptacion_particular[i]<0 and
          ((-adaptacion_particular[i]>grado_p) or
          (-adaptacion_particular[i]==grado_p and peso[i]>peso_p) or
          (-adaptacion_particular[i]==grado_p and peso[i]==peso_p and i<indice_pos) ))
         de_noclase--;
   }

   aislada=de_noclase;
 }



 delete []adaptacion_particular;

 regla_disparada=re;

 if (re!=-1) {
   rule[re][0].GetInteger(n,nn,nnn);
   return nnn[0][0];
 }
 else
   return -1;
}





int ruleset::InferenceC_debug(const vectordouble &v, int clase, double &grado_p, double &peso_p, double &grado_n, double &peso_n, int &regla_disparada, int &aislada) const{
 double max=0,aux, grado=0;
 int re=-1;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla, regla_funcion;
 int var1, var2;
 double *adaptacion_particular = new double[n_rule];
 bool clasifica_bien=false;
 int de_clase=0, de_noclase=0, indice_pos=n_rule, indice_neg=n_rule;

 grado_p=0;
 peso_p=0;
 grado_n=0;
 peso_n=0;

 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;

    s= new char[nb[1]+1];
    for (int j=0; j<nb[1]; j++)
      s[j]=mb[1][j];
    s[nb[1]]='\0';
    regla_funcion=s;
    delete []s;

    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);
    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]-1]);
    cout << "Regla " << i << endl;
    cout << "\tAdaptacion con la parte de variables: " << aux << endl;
    aux=domain_funcion[0].Adaptacion_funcion_debug(v,regla_funcion, nnn[1], nn[1], aux);
    cout << "\tAdaptacion con la parte de funciones: " << aux << endl;

    //Aplicar las relaciones

    //aux=domain[0].Adaptacion(v,regla);

    aux = aux*peso[i];

    if (nnn[0][0]==clase)
      adaptacion_particular[i]=aux;
    else
      adaptacion_particular[i]=-aux;



    if (aux>0) {
      if (nnn[0][0]==clase){
        if (aux>grado_p or (aux==grado_p and peso[i]>peso_p)){
          grado_p=aux;
          peso_p=peso[i];
          indice_pos=i;
        }
      }
      else {
        if (aux>grado_n or (aux==grado_n and peso[i]>peso_n)){
          grado_n=aux;
          peso_n=peso[i];
          indice_neg=i;
        }
      }
    }


    if (aux>max){
     max=aux;
     re=i;
     grado = aux;
     if (nnn[0][0]==clase)
       clasifica_bien=true;
     else
       clasifica_bien=false;
    }
    else if (re!=-1 && aux>0 && aux==max && peso[i]>peso[re]) {
           max=aux;
           re=i;
           grado = aux;
           if (nnn[0][0]==clase)
             clasifica_bien=true;
           else
            clasifica_bien=false;
         }
 }

 // Numero de reglas de la clase que clasifican correctamente el ejemplo antes de una regla que lo clasifical mal o no lo clasifica.

 if (clasifica_bien){
   de_clase=0;
   for (int i=0; i<n_rule;i++)
     if (adaptacion_particular[i]>0 and
        ((adaptacion_particular[i]>grado_n) or
        (adaptacion_particular[i]==grado_n and peso[i]>peso_n) or
        (adaptacion_particular[i]==grado_n and peso[i]==peso_n and i<indice_neg)) )
       de_clase++;
   if (de_clase==0){
      cout << "Error!!!!  Clase " << clase << "  Grado Pos: " << grado_p << " Grado Neg: "<< grado_n << "  Regla: " << re <<endl;
      for (int t=0; t<n_rule; t++)
        cout <<"\t"<<adaptacion_particular[t];
      cout << endl;
   }

   aislada=de_clase;
 }
 else {
   if (grado_p==0)
     de_noclase=-1000;
   else {
     de_noclase=0;
     for (int i=0; i<n_rule; i++)
       if (adaptacion_particular[i]<0 and
          ((-adaptacion_particular[i]>grado_p) or
          (-adaptacion_particular[i]==grado_p and peso[i]>peso_p) or
          (-adaptacion_particular[i]==grado_p and peso[i]==peso_p and i<indice_pos) ))
         de_noclase--;
   }

   aislada=de_noclase;
 }



 delete []adaptacion_particular;

 regla_disparada=re;

 if (re!=-1) {
   rule[re][0].GetInteger(n,nn,nnn);
   return nnn[0][0];
 }
 else
   return -1;
}









// Operador que devuelve el grado de satisfacci� de un ejemplo (v) con el conjunto de reglas.
//
int ruleset::InferenceC(const vectordouble &v, const Relaciones &RI) const{
 double max=0,aux;
 int re=-1;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla;
 int var1, var2;
 double rango;
 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;
    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);
    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]-1]);

    //Aplicar las relaciones

    for (int p=0; p<3 && aux!=0; p++)
       if (nnn[1][p]!=0){
         var1=RI.First(nnn[1][p]);
         var2=RI.Second(nnn[1][p]);
         switch (RI.Operator(nnn[1][p])){
           case 1: if (v.At(var1)>=v.At(var2))
                     aux=0;
                   break;
           case 2: rango = (*domain).Sup_Range(var1)-(*domain).Inf_Range(var1);
                   if (fabs((v.At(var1)-v.At(var2))/rango)>0.5)
                     aux=0;
                   else
                     aux = aux * pow(1-fabs((v.At(var1)-v.At(var2))/rango),2);
                   break;
           case 3:  if (v.At(var1)>=v.At(var2))
                           aux=0;
                       else {
                                rango = (*domain).Sup_Range(var1)-(*domain).Inf_Range(var1);
                                if (fabs((v.At(var1)-v.At(var2))/rango)>0.5)
                                     aux=0;
                                else
                                     aux = aux * pow(1-fabs((v.At(var1)-v.At(var2))/rango),2);
                                }
                   break;
         }
       }

    //aux=domain[0].Adaptacion(v,regla);
    if(aux>max){
     max=aux;
     re=i;
    }
 }
 if (re!=-1) {
   rule[re][0].GetInteger(n,nn,nnn);
   return nnn[0][0];
 }
 else
   return -1;
}



int ruleset::InferenceC(const vectordouble &v, const Relaciones &RI, double &grado) const{
 double max=0,aux;
 int re=-1;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla;
 int var1, var2;
 double rango;
 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;
    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);
    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]-1]);

    //Aplicar las relaciones

    for (int p=0; p<3 && aux!=0; p++)
       if (nnn[1][p]!=0){
         var1=RI.First(nnn[1][p]);
         var2=RI.Second(nnn[1][p]);
         switch (RI.Operator(nnn[1][p])){
           case 1: if (v.At(var1)>=v.At(var2))
                     aux=0;
                   break;
           case 2: rango = (*domain).Sup_Range(var1)-(*domain).Inf_Range(var1);
                   if (fabs((v.At(var1)-v.At(var2))/rango)>0.5)
                     aux=0;
                   else
                     aux = aux * pow(1-fabs((v.At(var1)-v.At(var2))/rango),2);
                   break;
           case 3:  if (v.At(var1)>=v.At(var2))
                           aux=0;
                       else {
                                rango = (*domain).Sup_Range(var1)-(*domain).Inf_Range(var1);
                                if (fabs((v.At(var1)-v.At(var2))/rango)>0.5)
                                     aux=0;
                                else
                                     aux = aux * pow(1-fabs((v.At(var1)-v.At(var2))/rango),2);
                                }
                   break;
         }
       }

    //aux=domain[0].Adaptacion(v,regla);
    if(aux>max){
     max=aux;
     re=i;
    }
 }
 if (re!=-1) {
   rule[re][0].GetInteger(n,nn,nnn);
   grado = max;
   return nnn[0][0];
 }
 else{
   grado = 0;
   return -1;
 }
}




int ruleset::InferenceCPeso(const vectordouble &v, const Relaciones &RI,
                            double &grado) const{
 double max=0,aux;
 int re=-1;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla;
 int var1, var2;
 double rango;
 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;
    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);
    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]-1]);

    //Aplicar las relaciones

    for (int p=0; p<3 && aux!=0; p++)
       if (nnn[1][p]!=0){
         var1=RI.First(nnn[1][p]);
         var2=RI.Second(nnn[1][p]);
         switch (RI.Operator(nnn[1][p])){
           case 1: if (v.At(var1)>=v.At(var2))
                     aux=0;
                   break;
           case 2: rango = (*domain).Sup_Range(var1)-(*domain).Inf_Range(var1);
                   if (fabs((v.At(var1)-v.At(var2))/rango)>0.5)
                     aux=0;
                   else
                     aux = aux * pow(1-fabs((v.At(var1)-v.At(var2))/rango),2);
                   break;
           case 3:  if (v.At(var1)>=v.At(var2))
                           aux=0;
                       else {
                                rango = (*domain).Sup_Range(var1)-(*domain).Inf_Range(var1);
                                if (fabs((v.At(var1)-v.At(var2))/rango)>0.5)
                                     aux=0;
                                else
                                     aux = aux * pow(1-fabs((v.At(var1)-v.At(var2))/rango),2);
                                }
                   break;
         }
       }

    //aux=domain[0].Adaptacion(v,regla);

    aux = aux * peso[i];

    if(aux>max){
     max=aux;
     re=i;
    }
 }
 if (re!=-1) {
   rule[re][0].GetInteger(n,nn,nnn);
   grado = max;
   return nnn[0][0];
 }
 else{
   grado = 0;
   return -1;
 }
}






double ruleset::InferenceF(const vectordouble &v) const{
 double aux,salida=0,neg,pos,center;
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla;
 int conse=domain[0].Consecuente();
 for (int i=0; i<n_rule; i++){
    rule[i][0].GetBinary(b,nb,mb);
    char *s= new char[nb[0]+1];
    for (int j=0; j<nb[0]; j++)
      s[j]=mb[0][j];
    s[nb[0]]='\0';
    regla=s;
    delete []s;
    rule[i][0].GetInteger(n,nn,nnn);
    rule[i][0].GetReal(r,nr,mr);
    aux=domain[0].Adaptacion(v,regla,mr[0],mr[0][nr[0]]);
    if(aux>0){
     domain[0].AdaptacionC(v,nnn[0][0],pos,neg);
     center=domain[0].CenterLabel(conse,nnn[0][0]);
     salida+=center*pos;
    }
 }
   return salida;
}


VectorVar ruleset::ArrayVar() const{
  return (*domain);
}



int ruleset::Class_of_rule (int i) const{
 int n,*nn,**nnn;
 rule[i][0].GetInteger(n,nn,nnn);
 return nnn[0][0];
}


void ruleset::Save(string nomfich) const {
  ofstream f;

  f.open(nomfich.c_str());
  if (!f){
     cout << "No hay espacio en el disco\n";
     return;
  }

  cout <<"Salvando las reglas ..."<< endl;
  f << "RULESET\n";
  cout <<"n_rule: " << n_rule << endl;
  f.write(reinterpret_cast<const char *>(&n_rule),sizeof(int));

  for (int i=0; i<n_rule; i++){
    cout <<"\tSalvando codigo regla " << i << endl;
    rule[i][0].SaveCode(f);
    cout <<"\tSalvando peso (" << peso[i] <<") regla " << i << endl;
    f.write(reinterpret_cast<const char *>(&(peso[i])),sizeof(double));
  }
  // salvar el vector de variables con los dominios

  cout <<"Salvando dominio... " << endl;

  domain[0].SaveBinaryCode(f);

  cout <<"Terminado proceso de salvado." << endl;

  f.close();
}


void ruleset::Load(string nomfich){
  ifstream f;
  string cabecera;

  // Eliminar el valor de objeto actual
 if (reservado>0){
   for (int i=0; i<n_rule; i++)
     delete rule[i];

   delete [] rule;
   delete [] peso;
 }

 if (domain!=0)
  delete domain;

 // Crear el nuevo objeto a partir del fichero
  f.open(nomfich.c_str());
  if (!f){
     cout << "No existe el fichero\n";
     return;
  }

  getline(f,cabecera);
  if (cabecera!="RULESET"){
    cout << "El fichero no es del tipo especificado\n";
    return;
  }

  f.read(reinterpret_cast<char *>(&n_rule),sizeof(int));

  reservado=n_rule;
  rule = new genetcode *[reservado];
  peso = new double[reservado];

  for (int i=0; i<n_rule; i++){
    rule[i]= new genetcode;
    rule[i][0].LoadCode(f);
    f.read(reinterpret_cast<char *>(&(peso[i])),sizeof(double));
  }
  // salvar el vector de variables con los dominios

  domain = new VectorVar;
  domain[0].LoadBinaryCode(f);

  f.close();
}






void ruleset::Print(int i) const {
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla;
 int var1, var2;
 int n_ant;

 // extraer la componente binaria
 rule[i][0].GetBinary(b,nb,mb);
 char *s= new char[nb[0]+1];
 for (int j=0; j<nb[0]; j++)
   s[j]=mb[0][j];
 s[nb[0]]='\0';
 regla=s;
 delete []s;

 rule[i][0].GetInteger(n,nn,nnn);
 rule[i][0].GetReal(r,nr,mr);
 n_ant = domain[0].N_Antecedente();
 int z;
 int j=0;
 int donde=0;
 int n_casos=0;
 cout << "IF\n";
 while (j<n_ant){
   if (domain[0].IsActiva(j) && domain[0].IsAntecedente(j)){
     n_casos=domain[0].SizeDomain(j);
     for(z=0; z<n_casos && regla[donde+z]=='1';z++);
     if (mr[0][j]>=mr[0][nr[0]-1] && z!=n_casos){
       cout <<"\t";
       domain[0].PrintVar(j);
       cout <<" = {";
       for(int t=donde; t<donde+n_casos;t++){
         if (regla[t]=='1'){
           cout << " ";
           domain[0].PrintDomain(j,t-donde);
         }
       }
       cout << "}\n";
     }

   }
   donde+=n_casos;
   j++;
 }

 //Conclusion de la regla
 cout << "THEN ";
 domain[0].PrintVar(domain[0].Consecuente());
 cout << " IS ";
 domain[0].PrintDomain(domain[0].Consecuente(),nnn[0][0]);
 cout << "   W " << peso[i] << endl;
}










void ruleset::Print(int i, const Relaciones &RI) const {
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla;
 int var1, var2;
 int n_ant;

 // extraer la componente binaria
 rule[i][0].GetBinary(b,nb,mb);
 char *s= new char[nb[0]+1];
 for (int j=0; j<nb[0]; j++)
   s[j]=mb[0][j];
 s[nb[0]]='\0';
 regla=s;
 delete []s;

 rule[i][0].GetInteger(n,nn,nnn);
 rule[i][0].GetReal(r,nr,mr);
 n_ant = domain[0].N_Antecedente();
 int z;
 int j=0;
 int donde=0;
 int n_casos=0;
 cout << "IF\n";
 while (j<n_ant){
   if (domain[0].IsActiva(j) && domain[0].IsAntecedente(j)){
     n_casos=domain[0].SizeDomain(j);
     for(z=0; z<n_casos && regla[donde+z]=='1';z++);
     if (mr[0][j]>=mr[0][nr[0]-1] && z!=n_casos){
       cout <<"\t";
       domain[0].PrintVar(j);
       cout <<" = {";
       for(int t=donde; t<donde+n_casos;t++){
         if (regla[t]=='1'){
           cout << " ";
           domain[0].PrintDomain(j,t-donde);
         }
       }
       cout << "}\n";
     }

   }
   donde+=n_casos;
   j++;
 }

 //Relaciones

 for (int p=0; p<3; p++)
   if (nnn[1][p]!=0){
     cout << "\t";
     var1=RI.First(nnn[1][p]);
     var2=RI.Second(nnn[1][p]);
     switch (RI.Operator(nnn[1][p])){
       case 1: domain[0].PrintVar(var1);
               cout <<" < ";
               domain[0].PrintVar(var2);
               cout << endl;
               break;
       case 2: domain[0].PrintVar(var1);
               cout <<" =E ";
               domain[0].PrintVar(var2);
               cout << endl;
               break;
       case 3: domain[0].PrintVar(var1);
               cout <<" <=E ";
               domain[0].PrintVar(var2);
               cout << endl;
               break;
         }
       }

 //Conclusion de la regla
 cout << "THEN ";
 domain[0].PrintVar(domain[0].Consecuente());
 cout << " IS ";
 domain[0].PrintDomain(domain[0].Consecuente(),nnn[0][0]);
 cout << "   W " << peso[i] << endl;
}





string ruleset::SPrint_Consequent(int i) const{
 int n,*nn,**nnn;

 rule[i][0].GetInteger(n,nn,nnn);
 return domain[0].SPrintDomain(domain[0].Consecuente(),nnn[0][0]);
}



string ruleset::SPrint(int i) const{
 string buffer="";
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla, regla_funcion;
 int var1, var2;
 int n_ant;

 // extraer la componente binaria
 rule[i][0].GetBinary(b,nb,mb);
 char *s= new char[nb[0]+1];
 for (int j=0; j<nb[0]; j++)
   s[j]=mb[0][j];
 s[nb[0]]='\0';
 regla=s;
 delete []s;

 s= new char[nb[1]+1];
 for (int j=0; j<nb[1]; j++)
   s[j]=mb[1][j];
 s[nb[1]]='\0';
 regla_funcion=s;
 delete []s;


 rule[i][0].GetInteger(n,nn,nnn);
 rule[i][0].GetReal(r,nr,mr);
 n_ant = domain[0].N_Antecedente();
 int z=0;
 int j=0;
 int donde=0;
 int n_casos=0;
 buffer+= "IF\n";
 while (j<n_ant){

   if (domain[0].IsActiva(j) && domain[0].IsAntecedente(j)){
     n_casos=domain[0].SizeDomain(j);
     for(z=0; z<n_casos && regla[donde+z]=='1';z++);
     if (mr[0][j]>=mr[0][nr[0]-1] && z!=n_casos){
       buffer+="\t";
       buffer+=domain[0].SPrintVar(j);
       buffer+=" = {";
       for(int t=donde; t<donde+n_casos;t++){
         if (regla[t]=='1'){
           buffer+= " ";
	   buffer+=domain[0].SPrintDomain(j,t-donde);
         }
        }
       buffer+= "}\n";
     }

   }
   donde+=n_casos;
   j++;
 }

 //Parte de funciones
 donde=0;
 for (int j=0; j<nn[1]; j++){
     if (nnn[1][j]!=0){
        n_casos=domain_funcion[0].SizeDomain(nnn[1][j]-1);
        for(z=0; z<n_casos && regla_funcion[donde+z]=='1';z++);
        buffer+="\t";
        buffer+=domain_funcion[0].SPrintVar(nnn[1][j]-1);
        buffer+=" = {";
        for(int t=donde; t<donde+n_casos;t++){
            if (regla_funcion[t]=='1'){
                buffer+= " ";
                buffer+=domain_funcion[0].SPrintDomain(nnn[1][j]-1,t-donde);
            }
        }
       buffer+= "}\n";
     }
     n_casos=2;
     donde+=n_casos;
}





 //Conclusion de la regla
 buffer+="THEN ";
 buffer+=domain[0].SPrintVar(domain[0].Consecuente());
 buffer+=" IS ";
 buffer+=domain[0].SPrintDomain(domain[0].Consecuente(),nnn[0][0]);
 buffer+="   W ";
 ostringstream ss;
 ss << peso[i];
 buffer+= ss.str();
 buffer+=" and N+= ";
 ostringstream ss2;
 ss2 << n_mas[i];
 buffer+= ss2.str();
 buffer+=" and N-= ";
 ostringstream ss3;
 ss3 << n_menos[i];
 buffer+= ss3.str();
 buffer+="\n";

 return buffer;

}



string ruleset::SPrint(int i,const Relaciones &RI) const{
 string buffer="";
 int b,*nb,n,*nn,**nnn,r,*nr;
 char **mb;
 double **mr;
 string regla;
 int var1, var2;
 int n_ant;

 // extraer la componente binaria
 rule[i][0].GetBinary(b,nb,mb);
 char *s= new char[nb[0]+1];
 for (int j=0; j<nb[0]; j++)
   s[j]=mb[0][j];
 s[nb[0]]='\0';
 regla=s;
 delete []s;

 rule[i][0].GetInteger(n,nn,nnn);
 rule[i][0].GetReal(r,nr,mr);
 n_ant = domain[0].N_Antecedente();
 int z;
 int j=0;
 int donde=0;
 int n_casos=0;
 buffer+= "IF\n";
 while (j<n_ant){
   if (domain[0].IsActiva(j) && domain[0].IsAntecedente(j)){
     n_casos=domain[0].SizeDomain(j);
     for(z=0; z<n_casos && regla[donde+z]=='1';z++);
     if (mr[0][j]>=mr[0][nr[0]-1] && z!=n_casos){
       buffer+="\t";
       buffer+=domain[0].SPrintVar(j);
       buffer+=" = {";
       for(int t=donde; t<donde+n_casos;t++){
         if (regla[t]=='1'){
           buffer+= " ";
           buffer+=domain[0].SPrintDomain(j,t-donde);
         }
       }
       buffer+= "}\n";
     }

   }
   donde+=n_casos;
   j++;
 }

 //Relaciones

 for (int p=0; p<3; p++)
   if (nnn[1][p]!=0){
     buffer+="\t";
     var1=RI.First(nnn[1][p]);
     var2=RI.Second(nnn[1][p]);
     switch (RI.Operator(nnn[1][p])){
       case 1: buffer+=domain[0].SPrintVar(var1);
               buffer+=" < ";
               buffer+=domain[0].SPrintVar(var2);
               buffer+="\n";
               break;
       case 2: buffer+=domain[0].SPrintVar(var1);
               buffer+=" =E ";
               buffer+=domain[0].SPrintVar(var2);
               buffer+="\n";
               break;
       case 3: buffer+=domain[0].SPrintVar(var1);
               buffer+=" <=E ";
               buffer+=domain[0].SPrintVar(var2);
               buffer+="\n";
               break;
         }
       }

 //Conclusion de la regla
 buffer+="THEN ";
 buffer+=domain[0].SPrintVar(domain[0].Consecuente());
 buffer+=" IS ";
 buffer+=domain[0].SPrintDomain(domain[0].Consecuente(),nnn[0][0]);
 buffer+="   W ";
 ostringstream ss;
 ss << peso[i];
 buffer+= ss.str();
 buffer+="\n";

 return buffer;

}

void ruleset::SaveRuleInterpreted(const char *nomfich) const {
  ofstream f(nomfich);
  string regla;
  cout << "Numero de reglas =" << n_rule << endl;
  for (int i=0; i<n_rule; i++){
    regla=SPrint(i);
    f << regla.c_str() << endl;
  }

  f.close();
}

void ruleset::SaveRuleInterpreted_append(const char *nomfich) const {
  ofstream f(nomfich,ios::app);
  string regla;
  cout << "Numero de reglas =" << n_rule << endl;
  for (int i=0; i<n_rule; i++){
    regla=SPrint(i);
    f << regla.c_str() << endl;
  }

  f.close();
}


void ruleset::SaveRuleInterpreted(const char *nomfich, const Relaciones &RI) const {
  ofstream f(nomfich);
  string regla;
  for (int i=0; i<n_rule; i++){
    regla=SPrint(i,RI);
    f << regla.c_str() << endl;
  }

  f.close();
}

void ruleset::SaveRuleInterpreted(const char *nomfich, const Relaciones &RI,
                                  double training, double test) const {
  ofstream f(nomfich);
  string regla;
  for (int i=0; i<n_rule; i++){
    regla=SPrint(i,RI);
    f << regla.c_str() << endl;
  }
  f << "Porcentaje Training: " << training << endl;
  f << "Porcentaje Test: " << test << endl;
  f.close();
}

list<pair<string, string>  > ruleset::SplitAntecedent(int i) const{
  list< pair<string, string> > listaPred;

  int b,*nb,n,*nn,**nnn,r,*nr;
  char **mb;
  double **mr;
  string regla, regla_funcion;
  int var1, var2;
  int n_ant;

  // extraer la componente binaria
  rule[i][0].GetBinary(b,nb,mb);
  char *s= new char[nb[0]+1];
  for (int j=0; j<nb[0]; j++){
    s[j]=mb[0][j];
  }
  s[nb[0]]='\0';
  regla=s;
  delete []s;

  s= new char[nb[1]+1];
  for (int j=0; j<nb[1]; j++){
    s[j]=mb[1][j];
  }
  s[nb[1]]='\0';
  regla_funcion=s;
  delete []s;


  rule[i][0].GetInteger(n,nn,nnn);
  rule[i][0].GetReal(r,nr,mr);
  n_ant = domain[0].N_Antecedente();
  int z=0;
  int j=0;
  int donde=0;
  int n_casos=0;
  std::string cadena;
  while (j<n_ant){
    if (domain[0].IsActiva(j) && domain[0].IsAntecedente(j)){
      n_casos=domain[0].SizeDomain(j);
      for(z=0; z<n_casos && regla[donde+z]=='1';z++);
        if (mr[0][j]>=mr[0][nr[0]-1] && z!=n_casos){
          if( n_casos <= 2 ){
            for(int t=donde; t<donde+n_casos;t++){
              if (regla[t]=='1'){
                cadena = domain[0].SPrintDomain(j,t-donde);
                if(cadena == "True" or cadena == "False"){
                  listaPred.push_back(pair<string,string>(domain[0].SPrintVar(j), cadena));
                }
                else{
                  listaPred.push_back(pair<string,string>(domain[0].SPrintVar(j), to_string( (int) domain[0].CenterLabel(j, t-donde) ) ));
                }
              }
            }
          }
        }
      }
      donde+=n_casos;
      j++;
    }
  return listaPred;
}




void ruleset::toPDDL(vector < pair <int, list< pair<string, string> > > > * reglas) const {
  for (int i=0; i<n_rule; i++){
    if("PostState"== domain[0].SPrintDomain(domain[0].Consecuente(), Class_of_rule(i))){
      (*reglas).push_back(pair <int, list< pair<string, string> > > (n_mas[i], SplitAntecedent(i)));
      //(*reglas)[1] = SplitAntecedent(i);
    }else{
      (*reglas).push_back(pair <int, list< pair<string, string> > > ( 0 - n_mas[i], SplitAntecedent(i)));
      //(*reglas)[0] = SplitAntecedent(i);
    }
  }
}
