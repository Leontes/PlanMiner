#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include "INSLV/domain_t.h"

using namespace std;


// Constructor de clase por defecto
domain_t::domain_t(){
  numero=0;
  rango_inf=rango_sup=0;
  inf_inf=sup_inf=false;
  label=0;
}


// Constructor de clase param�trico
// n n�mero de etiquetas del dominio
// inf l�mite inferior del dominio
// sup limite superior del dominio
// menosinf valor true indica que el limite inferior no esta acotado
// masinf valor true indica que el limite superior no esta acotado
domain_t::domain_t(int n, double inf, double sup, bool menosinf, bool masinf){
  double ancho;

  numero=n;
  rango_inf=inf;
  rango_sup=sup;
  inf_inf=menosinf;
  sup_inf=masinf;
  label = new fuzzy_t[n];

  /* definir las etiquetas del dominio */
  ancho=(sup-inf)/(n-1);
  char aux[80], aux2[10];
  string nom_label;
  for (int i=1; i<n-1; i++){
     sprintf(aux,"L%i",i);
     sprintf(aux2,"/%i",n-1);
     strcat(aux,aux2);
     nom_label=aux;
     label[i].Asigna(rango_inf+(ancho*(i-1)), rango_inf+(ancho*i),
                     rango_inf+(ancho*i), rango_inf+(ancho*(i+1)), nom_label,
		     false, false);

  }
  sprintf(aux,"L0/%i",n-1);
  label[0].Asigna(rango_inf,rango_inf,rango_inf,rango_inf+ancho,aux,menosinf,
 false);
  sprintf(aux,"L%i/%i",n-1,n-1);
  nom_label=aux;

 label[n-1].Asigna(rango_sup-ancho,rango_sup,rango_sup,rango_sup,nom_label,false
, masinf);
}

// Destructor de la clase
domain_t::~domain_t(){
  numero=0;
  delete []label;
  label=0;
}


// Operador de copia
domain_t::domain_t(const domain_t &x){
  numero=x.numero;
  rango_inf=x.rango_inf;
  rango_sup=x.rango_sup;
  inf_inf=x.inf_inf;
  sup_inf=x.sup_inf;
  label = new fuzzy_t[numero];
  for (int i=0; i<numero; i++)
     label[i]=x.label[i];
}


// Operador de asignaci�n
domain_t & domain_t::operator=(const domain_t &x){
  if (label!=0)
    delete [] label;
  numero=x.numero;
  rango_inf=x.rango_inf;
  rango_sup=x.rango_sup;
  inf_inf=x.inf_inf;
  sup_inf=x.sup_inf;
  label = new fuzzy_t[numero];
  for (int i=0; i<numero; i++)
     label[i]=x.label[i];

  return *this;
}

// Operaci�n de asignaci�n a un objeto de la clase
// Crea un dominio uniforme con n etiquetas desde inf a sup cortandonse en 0.5
void domain_t::Asigna(int n, double inf, double sup, bool menosinf, bool masinf){
  double ancho;

  numero=n;
  rango_inf=inf;
  rango_sup=sup;
  inf_inf=menosinf;
  sup_inf=masinf;
  if (label!=0)
    delete label;
  label = new fuzzy_t[n];

  /* definir las etiquetas del dominio */
  ancho=(sup-inf)/(n-1);
  char aux[80], aux2[10];
  string nom_label;
  for (int i=1; i<n-1; i++){
     sprintf(aux,"L%i",i);
     sprintf(aux2,"/%i",n-1);
     strcat(aux,aux2);
     nom_label=aux;
     label[i].Asigna(rango_inf+(ancho*(i-1)), rango_inf+(ancho*i),
                     rango_inf+(ancho*i), rango_inf+(ancho*(i+1)), nom_label,
		     false, false);

  }
  sprintf(aux,"L0/%i",n-1);
  label[0].Asigna(rango_inf,rango_inf,rango_inf,rango_inf+ancho,aux,menosinf,
 false);
  sprintf(aux,"L%i",n-1);
  nom_label=aux;

 label[n-1].Asigna(rango_sup-ancho,rango_sup,rango_sup,rango_sup,nom_label,false
, masinf);
}



// Operaci�n de asignaci�n a un objeto de la clase
// Crea un dominio uniforme con n etiquetas desde inf a sup cortandonse en 0.5
void domain_t::Asigna_Op_menos(int n, double inf, double sup, bool menosinf, bool masinf){
  double ancho1, ancho2;

  numero=n;
  rango_inf=inf;
  rango_sup=sup;
  inf_inf=menosinf;
  sup_inf=masinf;
  if (label!=0)
    delete label;
  label = new fuzzy_t[n];

  /* definir las etiquetas del dominio */
  int tam_izq, tam_dch;
  if (n%2!=0){
    ancho1=(-inf)/(n/2);
    ancho2=(sup)/(n/2);
    tam_izq=n/2;
    tam_dch=n/2;
  }
  else if (-inf>=sup){
         tam_izq=n/2;
         tam_dch=n/2-1;
         ancho1=(-inf)/(tam_izq-1);
         if (tam_dch-1<=0)
           ancho2=sup;
         else
           ancho2=(sup)/(tam_dch-1);
       }
       else {
         tam_izq=n/2-1;
         tam_dch=n/2;
         ancho2=(sup)/(tam_dch-1);
         if (tam_izq-1<=0)
           ancho1=-inf;
         else
           ancho1=(-inf)/(tam_izq-1);
       }
  char aux[80], aux2[10];
  string nom_label;
  int i;
  // Construyo las etiquetas a la izquierda del cero
  for (i=1; i<tam_izq; i++){
     sprintf(aux,"L%i",i);
     sprintf(aux2,"/%i",n-1);
     strcat(aux,aux2);
     nom_label=aux;
     label[i].Asigna(rango_inf+(ancho1*(i-1)), rango_inf+(ancho1*i),
                     rango_inf+(ancho1*i), rango_inf+(ancho1*(i+1)), nom_label,
		     false, false);

  }
  // Construyo la etiqueta del centro del dominio que corresponde al cero
  sprintf(aux,"L%i/%i",i,n-1);
  nom_label=aux;
  label[i].Asigna(-ancho1, 0, 0, ancho2, nom_label, false, false);

  // Construyo las etiquetas a la derecha del cero.
  for (int j=1; j<tam_dch; j++){
     sprintf(aux,"L%i/%i",i+j,n-1);
     nom_label=aux;
     label[i+j].Asigna(ancho2*(j-1), ancho2*j, ancho2*j, ancho2*(j+1), nom_label, false, false);
  }

  sprintf(aux,"L0/%i",n-1);
  label[0].Asigna(rango_inf,rango_inf,rango_inf,rango_inf+ancho1,aux,menosinf, false);
  sprintf(aux,"L%i/%i",n-1,n-1);
  nom_label=aux;

  label[n-1].Asigna(rango_sup-ancho2,rango_sup,rango_sup,rango_sup,nom_label,false, masinf);
}



// Operaci�n de asignaci�n a un objeto de la clase
// Crea un dominio uniforme con n etiquetas desde inf a sup cortandonse en 0.5
// desp=0, uniforme, desp=1 sensibilidad a la izq, desp=2 sensibilidad en el centro, desp=3 sensibilidad a la dcha
void domain_t::Asigna(int n, double inf, double sup, bool menosinf, bool masinf, int desp){
  double ancho;
  double origen;
  double fin;

  numero=n;
  rango_inf=inf;
  rango_sup=sup;
  inf_inf=menosinf;
  sup_inf=masinf;
  if (label!=0)
    delete label;
  label = new fuzzy_t[n];

  char aux[80];
  string nom_label;

  /* definir las etiquetas del dominio */
  switch (desp){
    // Distribucion Uniforme
    case 0:  ancho=(sup-inf)/(n-1);
                    for (int i=1; i<n-1; i++){
                       sprintf(aux,"L%i/%i",i,n-1);
                       nom_label=aux;
                       label[i].Asigna(rango_inf+(ancho*(i-1)), rango_inf+(ancho*i), rango_inf+(ancho*i), rango_inf+(ancho*(i+1)), nom_label, false, false);
                   }
                   sprintf(aux,"L0/%i",n-1);
                   label[0].Asigna(rango_inf, rango_inf, rango_inf, rango_inf+ancho,aux,menosinf, false);

                   sprintf(aux,"L%i/%i",n-1,n-1);
                   nom_label=aux;
                  label[n-1].Asigna(rango_sup-ancho, rango_sup, rango_sup, rango_sup, nom_label,false, masinf);
		  break;
    // Distribucion a la izquierda
    case 1: ancho =(sup-inf)/(2*(n-1));
                    for (int i=1; i<n-1; i++){
                       sprintf(aux,"L%i/%i",i,n-1);
                       nom_label=aux;
                       label[i].Asigna(rango_inf+(ancho*(i-1)), rango_inf+(ancho*i), rango_inf+(ancho*i), rango_inf+(ancho*(i+1)), nom_label, false, false);
                   }
                   sprintf(aux,"L0/%i",n-1);
                   label[0].Asigna(rango_inf, rango_inf, rango_inf,rango_inf+ancho,aux,menosinf, false);

                   sprintf(aux,"L%i/%i",n-1,n-1);
                   nom_label=aux;
                  label[n-1].Asigna(rango_inf+(ancho*(n-2)),rango_inf+(ancho*(n-1)),rango_sup,rango_sup,nom_label,false, masinf);
		  break;
    // Distribucion centrada
    case 2: ancho =(sup-inf)/(2*(n-1));
                   origen = rango_inf+(sup-inf)/4;
                   fin = rango_inf+3*((sup-inf)/4);
                   for (int i=1; i<n-1; i++){
                       sprintf(aux,"L%i/%i",i,n-1);
                       nom_label=aux;
                       label[i].Asigna(origen+(ancho*(i-1)), origen+(ancho*i), origen+(ancho*i), origen+(ancho*(i+1)), nom_label, false, false);
                   }
                   sprintf(aux,"L0/%i",n-1);
                   label[0].Asigna(rango_inf,rango_inf,origen,origen+ancho,aux,menosinf, false);
                   sprintf(aux,"L%i/%i",n-1,n-1);
                   nom_label=aux;
                  label[n-1].Asigna(fin-ancho,fin,rango_sup,rango_sup,nom_label,false, masinf);
		  break;
    // Distribucion a la derecha
    case 3: ancho =(sup-inf)/(2*(n-1));
                  origen = rango_inf+(sup-inf)/2;
                    for (int i=1; i<n-1; i++){
                       sprintf(aux,"L%i/%i",i,n-1);
                       nom_label=aux;
                       label[i].Asigna(origen+(ancho*(i-1)), origen+(ancho*i), origen+(ancho*i), origen+(ancho*(i+1)), nom_label, false, false);
                   }
                   sprintf(aux,"L0/%i",n-1);
                   label[0].Asigna(rango_inf, rango_inf, origen,origen+ancho,aux,menosinf, false);
                   sprintf(aux,"L%i/%i",n-1,n-1);
                   nom_label=aux;
                  label[n-1].Asigna(rango_sup-ancho,rango_sup, rango_sup, rango_sup,nom_label,false, masinf);
		  break;
  }
}



// Operaci�n de asignaci�n a un objeto de la clase
// Crea un dominio de n etiquetas en [inf,sup]. La definici�n de cada etiqueta
// que compone el dominio viene definida en 4 vectores a,b,c y d, donde
// [a[i],b[i],c[i],d[i]] representa la etiqueta i-esima del dominio.
// Da nombre a cada etiqueta por defecto empezando en Label1.
void domain_t::Asigna(int n, double inf, double sup, double *a, double *b, double *c, double *d){
  string nombre="Label0";
  numero=n;
  rango_inf=inf;
  rango_sup=sup;
  inf_inf=true;
  sup_inf=true;
  if (label!=0)
    delete label;
  label = new fuzzy_t[numero];

  for (int i=0; i<numero; i++){
    nombre[5]++;
    label[i].Asigna(a[i],b[i],c[i],d[i],nombre);
  }
}





// Operaci�n de asignaci�n a un objeto de la clase
// Crea un dominio de n etiquetas en [inf,sup]. La definici�n de cada etiqueta
// que compone el dominio viene definida en 4 vectores a,b,c y d, donde
// [a[i],b[i],c[i],d[i]] representa la etiqueta i-esima del dominio.
// el vector name contiene el nombre de cada etiqueta.
void domain_t::Asigna(int n, double inf, double sup, double *a, double *b, double *c, double *d, string *name, bool singleton){
  numero=n;
  rango_inf=inf;
  rango_sup=sup;
  inf_inf=true;
  sup_inf=true;
  if (label!=0)
    delete label;
  label = new fuzzy_t[numero];


  if(!singleton){
    label[0].Asigna(a[0],b[0],c[0],d[0],name[0],true,false);
    for (int i=1; i<numero-1; i++)
      label[i].Asigna(a[i],b[i],c[i],d[i],name[i]);

    label[numero-1].Asigna(a[numero-1],b[numero-1],c[numero-1],d[numero-1],name[numero-1],false,true);
  }
  else{
    label[0].Asigna(a[0],b[0],c[0],d[0],name[0],false,false);
    for (int i=1; i<numero-1; i++)
      label[i].Asigna(a[i],b[i],c[i],d[i],name[i]);

    label[numero-1].Asigna(a[numero-1],b[numero-1],c[numero-1],d[numero-1],name[numero-1],false,false);
  }
}


// Operaci�n que devuelve el n�mero de etiquetas que contiene un dominio
int domain_t::N_etiquetas() const{
  return numero;
}


// Operaci�n que devuelve el n�mero de etiquetas que contiene un dominio
int domain_t::Size() const {
  return numero;
}



// Operaci�n que devuelve el grado de adaptaci�n de un determinado valor x
// a un objeto de tipo dominio.
double domain_t::Adaptacion(double x) const{
  double mayor, nuevo;
  if (numero==0) {
    cout << "El dominio no esta creado\n";
    exit(1);
  }

  mayor = label[0].Adaptacion(x);
  for (int i=1;i<numero and mayor<1; i++){
    nuevo = label[i].Adaptacion(x);
    if (nuevo>mayor)
      mayor=nuevo;
  }
  return mayor;
}


// Operaci�n que devuelve el grado de adaptaci�n de un determinado valor x
// a una determinada etiqueta del dominio referenciada por su posici�n dentro
// del dominio.
double domain_t::Adaptacion(double x, const int etiqueta) const{
  if (numero==0) {
    cout << "El dominio no esta creado\n";
    exit(1);
  }

  if (etiqueta>=numero){
    cout << "Esa etiqueta no esta en el dominio\n";
    exit(1);
  }

  return label[etiqueta].Adaptacion(x);
}



// Operaci�n que evalua el grado de adaptaci�n de un determinado valor x
// a un conjunto de etiquetas del dominio. Las etiquetas se expresan como
// un vector ordenado (cadena de caracteres <string>) de ceros y unos. El cero
// representa ausencia y el uno presencia.
double domain_t::Adaptacion(double x, string etiquetas) const{
  double mayor, nuevo;
  if (numero==0) {
    cout << "El dominio no esta creado\n";
    exit(1);
  }

  mayor=etiquetas.length();
  if (mayor>numero){
    cout << "Esa etiqueta no esta en el dominio\n";
    exit(1);
  }

  mayor=0;
  for (int i=0; i<numero and mayor<1; i++)
    if (etiquetas[i]=='1'){
      if (mayor==0)
        mayor = label[i].Adaptacion(x);
      else if (label[i].Adaptacion(x)>0)
             mayor=1;
    }

  return mayor;
}







void domain_t::Amplia_Delta(){
 string nombre;
 double a,b,c,d;

  label[numero/2].GetDefinicion(a,b,c,d,nombre);
  double crecimiento=(d-c)*0.625;

  label[0].GetDefinicion(a,b,c,d,nombre);
  label[0].PutDefinicion(a,b,c+crecimiento,d+crecimiento,nombre);

  for (int i=1; i<numero; i++){
    label[i].GetDefinicion(a,b,c,d,nombre);
    label[i].PutDefinicion(a+crecimiento,b+crecimiento,c+crecimiento,d+crecimiento,nombre);
  }

  /*label[numero-1].GetDefinicion(a,b,c,d,nombre);
  label[numero-1].PutDefinicion(a+crecimiento,b,c,d+crecimiento,nombre);*/


  rango_sup=rango_sup+crecimiento;
}



/*void domain_t::Amplia_Delta(){
 string nombre;
 double a,b,c,d, centro_anterior;
 const double factor=0.25;

  double crecimiento;

  label[0].GetDefinicion(a,b,c,d,nombre);
  crecimiento=(d-c)*factor;
  label[0].PutDefinicion(a,b,c+crecimiento,d+crecimiento,nombre);

  for (int i=1; i<numero; i++){
    centro_anterior=c+crecimiento;
    label[i].GetDefinicion(a,b,c,d,nombre);
    label[i].PutDefinicion(a+crecimiento,b+crecimiento,c+crecimiento,d+crecimiento,nombre);
  }

  //label[numero-1].GetDefinicion(a,b,c,d,nombre);
  //label[numero-1].PutDefinicion(a+crecimiento,b,c,d+crecimiento,nombre);


  rango_sup=rango_sup+crecimiento;
}
*/



void domain_t::Reduce_Delta(){
 string nombre;
 double a,b,c,d;

  label[numero/2].GetDefinicion(a,b,c,d,nombre);
  double decrecimiento=(d-c)*0.25;

  /*label[0].GetDefinicion(a,b,c,d,nombre);
  label[0].PutDefinicion(a-decrecimiento,b,c,d-decrecimiento,nombre);*/

  for (int i=0; i<numero-1; i++){
    label[i].GetDefinicion(a,b,c,d,nombre);
    label[i].PutDefinicion(a-decrecimiento,b-decrecimiento,c-decrecimiento,d-decrecimiento,nombre);
  }
  label[numero-1].GetDefinicion(a,b,c,d,nombre);
  label[numero-1].PutDefinicion(a-decrecimiento,b-decrecimiento,c,d,nombre);

  rango_inf=rango_inf-decrecimiento;
}







//Operaci�n que pone en el dispositivo de salida la definici�n del dominio
void domain_t::Pinta() const{
  for (int i=0; i<numero; i++)
    label[i].Pinta();
}


//Operaci�n que pone en el dispositivo de salida la definici�n del dominio
std::string domain_t::PintaS() const{
  std::string out;
  for (int i=0; i<numero; i++)
    out += label[i].PintaS();
  return out;
}

//Operaci�n que pone en el dispositivo de salida la definici�n del dominio
std::string domain_t::PintaS(int lab) const{
  return label[lab].PintaS();
}


//Operaci�n que pone en el dispositivo de salida la etiqueta i-esima del dominio
void domain_t::Pinta(int i) const{
  label[i].Pinta();
}


void domain_t::Print(int i) const {
  label[i].Print();
}

string domain_t::SPrint(int i) const {
  return label[i].SPrint();
}

// Operaci�n que devuelve la etiqueta difusa i-esima del dominio
fuzzy_t domain_t::FuzzyLabel(int i) const{
  fuzzy_t aux;
  aux = label[i].FuzzyLabel();
  return aux;
}


// Operaci�n que devuelve el punto central de la etiqueta i-esima del dominio
double domain_t::CenterLabel(int i) const{
  return label[i].CenterLabel();
}


// Operaci�n que determina si el objeto <domain_t> esta compuentos por etiquetas
// que todas son dominios difusos.
bool domain_t::IsDiscrete() const{
  int i=0;
  while (i<numero && label[i].IsDiscrete())
    i++;

  return (i==numero);
}

// Operaci�n que determina si el objeto <domain_t> esta compuesto por etiquetas
// que todas son intervalos
bool domain_t::IsInterval() const{
  int i=0;
  while (i<numero && label[i].IsInterval())
    i++;

  return (i==numero);
}


// Operaci�n que determina si el objeto <domain_t> esta compuesto por etiquetas
// que todas son valores crisp.
bool domain_t::IsFuzzy() const{
  int i=0;
  while (i<numero && label[i].IsFuzzy())
    i++;

  return (i==numero);
}


// Operaci�n que devuelve el area de la etiqueta l-esima del dominio
double domain_t::Area(int l) const{
  return label[l].Area();
}


// Operaci�n que devuelve un objeto de tipo <domain_t> del dominio
domain_t domain_t::Domain() const{
  domain_t aux;
  aux=*this;
  return aux;
}


// Operaci�n que devuelve el valor inferior de todas las etiquetas que
// definen el dominio.
double domain_t::Inf_Range() const{
  return rango_inf;
}


// Operaci�n que devuelve el valor superior de todas las etiquetas que
// definen el dominio.
double domain_t::Sup_Range() const{
  return rango_sup;
}



// Operaci�n que guarda en un fichero binario la definici�n del dominio
void domain_t::SaveBinaryCode(ofstream &f) const{
  f.write(reinterpret_cast<const char *> (&numero),sizeof(int));
  f.write(reinterpret_cast<const char *> (&rango_inf), sizeof(double));
  f.write(reinterpret_cast<const char *> (&rango_sup), sizeof(double));
  f.write(reinterpret_cast<const char *> (&inf_inf), sizeof(bool));
  f.write(reinterpret_cast<const char *> (&sup_inf), sizeof(bool));
  for (int i=0; i<numero; i++)
    label[i].SaveBinaryCode(f);
}



// Operaci�n que recupera y asigna a un objeto <domain_t> la definici�n
//de un dominio a partir de la informaci�n extraida de un fichero binario.
void domain_t::LoadBinaryCode(ifstream &f) {
  //Eliminar la informaci�n del objeto
  if (numero>0)
    delete [] label;
  //Poner la nueva informaci�n
  f.read(reinterpret_cast< char *> (&numero),sizeof(int));
  f.read(reinterpret_cast< char *> (&rango_inf), sizeof(double));
  f.read(reinterpret_cast< char *> (&rango_sup), sizeof(double));
  f.read(reinterpret_cast< char *> (&inf_inf), sizeof(bool));
  f.read(reinterpret_cast< char *> (&sup_inf), sizeof(bool));
  label = new fuzzy_t[numero];
  for (int i=0; i<numero; i++)
    label[i].LoadBinaryCode(f);
}
