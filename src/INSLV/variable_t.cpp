using namespace std;
#include <iostream>
#include <string>
#include <cstdlib>
#include "INSLV/variable_t.h"

using namespace std;

// Constructor por defecto de la clase
variable_t::variable_t(){
  dominio=0;
  nombre="Sin asignar";
  activa=false;
  antecedente=true;
  unit=0;
  convert_factor=1;
}


// Constructor con parametros
variable_t::variable_t(string &p_nombre, bool p_activa, bool p_antecedente, domain_t &dom){
  nombre = p_nombre;
  activa = p_activa;
  antecedente = p_antecedente;
  dominio = new domain_t;
  dominio[0]=dom;
  unit=0;
  convert_factor=1;
}


// Constructor con parametros 2
variable_t::variable_t(string &p_nombre, bool p_activa, bool p_antecedente, domain_t &dom, int Unit, double ConvFactor){
  nombre = p_nombre;
  activa = p_activa;
  antecedente = p_antecedente;
  dominio = new domain_t;
  dominio[0]=dom;
  unit=Unit;
  convert_factor=ConvFactor;
}



// Constructor de copia
variable_t::variable_t(const variable_t &x){
  nombre=x.nombre;
  activa=x.activa;
  antecedente=x.antecedente;
  unit = x.unit;
  convert_factor = x.convert_factor;
  if (x.dominio==0)
    dominio=0;
  else {
    dominio = new domain_t;
    dominio=x.dominio;
  }
}


// Operador de asignaci�n por copia
variable_t & variable_t::operator=(const variable_t &x){
  if (dominio!=0)
    delete dominio;

  dominio = new domain_t;
  nombre=x.nombre;
  activa=x.activa;
  antecedente=x.antecedente;
  unit = x.unit;
  convert_factor = x.convert_factor;
  *dominio=*(x.dominio);
  return *this;
}


// Destructor de la clase
variable_t::~variable_t(){
  delete dominio;
  dominio=0;
  nombre="Eliminado";
}



// Operador de asignaci�n param�trico de la clase.
// Define una variable de forma autom�tica, definiendo un domino uniformemente
// distribuido con n etiquetas sobre el rango [inf,sup], pudiendo indicar especificar
// si los extremos del dominio no estan acotados. name es el nombre de la variable.
// Por defecto asume que es una variable antecedente y que se considera en el proceso
// de aprendizaje.
void variable_t::Asigna(int n, double inf, double sup, bool menosinf, bool masinf, string name){
  nombre = name;
  activa=true;
  antecedente=true;
  unit=0;
  convert_factor=1;
  if (dominio!=0)
    delete dominio;

  dominio = new domain_t;
  (*dominio).Asigna(n,inf,sup,menosinf,masinf);
}





// Operador de asignaci�n param�trico de la clase.
// Define una variable en base a un dominio que viene expresado como una secuencia
// ordenada de etiquetas ( [a[i],b[i],c[i],d[i]] expresa la definici�n de la etiqueta i)
// inf y sup indican los limites inferior y superior respectivamente.
// varname es el nombre de la variable y name es un vector con los nombres de las etiquetas.
// status indica como considerar a la variable (-1 variable antecedente y no activa,
// 0 variable antecedente y activa y 1 variable consecuente y activa).

void variable_t::Asigna(int n, string varname, int status, double inf,
double sup, double  *a, double *b, double *c, double *d, string *name){
  nombre=varname;
  if (status==-1){
    activa=false;
    antecedente=true;
  }
  else if (status==0) {
          activa=true;
	  antecedente=true;
       }
       else {
         activa=true;
	 antecedente=false;
       }
  if (dominio!=0)
    delete dominio;

  dominio = new domain_t;
  (*dominio).Asigna(n,inf,sup,a,b,c,d,name);
}


void variable_t::Asigna(int n, string varname, int status, int Unit, double c_factor, double inf,
double sup, double  *a, double *b, double *c, double *d, string *name, bool singleton){
  nombre=varname;
  if (status==-1){
    activa=false;
    antecedente=true;
    unit=0;
    convert_factor=1;
  }
  else if (status==0) {
          activa=true;
	  antecedente=true;
          unit = Unit;
          convert_factor = c_factor;
       }
       else {
          activa=true;
      	  antecedente=false;
          unit = Unit;
          convert_factor = c_factor;
       }
  if (dominio!=0)
    delete dominio;

  dominio = new domain_t;
  (*dominio).Asigna(n,inf,sup,a,b,c,d,name,singleton);
}



// Operador que devuelve el grado de adaptaci�n de un determinado valor x
// a la variable.
double variable_t::Adaptacion(double x) const {
  if (dominio==0){
    cout << "Variable sin dominio asociado\n";
    exit(1);
  }
  return (*dominio).Adaptacion(x);
}


// Operador que devuelve el grado de adaptaci�n de un determinado valor x
// a la etiqueta "etiqueta" de la variable.
double variable_t::Adaptacion(double x, int etiqueta) const {
  if (dominio==0){
    cout << "Variable sin dominio asociado\n";
    exit(1);
  }
  return (*dominio).Adaptacion(x,etiqueta);
}



// Operador que devuelve el grado de adaptaci�n de un determinado valor x
// a un grupo de etiquetas "etiquetas" de la variable. El par�metro etiquetas
// viene expresado como un vector ordenado (cadena caracteres string) de ceros
// y unos, representando ausencia y presencia respectivamente.
double variable_t::Adaptacion(double x, string etiquetas) const {
  if (dominio==0){
    cout << "Variable sin dominio asociado\n";
    exit(1);
  }
  return (*dominio).Adaptacion(x,etiquetas);
}



// Operador que pone en el dispositivo de salida la definici�n de la variable
void variable_t::Pinta() const{
  if (dominio==0){
    cout << "Variable sin dominio asociado\n";
    exit(1);
  }
  cout << "Variable: " << nombre << endl;
  cout << "========================\n";
  (*dominio).Pinta();
}

// Operador que pone en el dispositivo de salida la definici�n de la variable
std::string variable_t::PintaS() const{
  if (dominio==0){
    return "Variable sin dominio asociado\n";
  }
  return (*dominio).PintaS();
}

// Operador que pone en el dispositivo de salida la definici�n de la variable
std::string variable_t::PintaS(int label) const{
  if (dominio==0){
    return "Variable sin dominio asociado\n";
  }
  return (*dominio).PintaS(label);
}


// Operador que pone en el dispositivo de salida el nombre de la variable
void variable_t::PrintVar() const{
  cout << nombre;
}

// Operador que devuelve en un string el nombre de la variable.
string variable_t::SPrintVar() const{
  return nombre;
}


// Operador que pone en el dispositivo de salida el nombre de la etiqueta "value"
// de la variable.
void variable_t::PrintDomain(int value) const{
  (*dominio).Print(value);
}


// Operador que devuelve un string con el nombre de la etiqueta "value" de la variable.
string variable_t::SPrintDomain(int value) const{
  return (*dominio).SPrint(value);
}


// Operador que devuelve el numero de etiquetas que componen el dominio de la variable.
int variable_t::SizeDomain() const{
  return (*dominio).Size();
}

// Operador que devuelve si la etiqueta es considerada en el proceso de aprendizaje.
bool variable_t::Activa()const{
  return activa;
}


//Operador que devuelve si es un variable antecedente en la regla.
bool variable_t::Antecedente() const{
  return antecedente;
}


// Operador que devuelve el n�mero de etiquetas que componen el dominio de la variable.
int variable_t::N_etiquetas() const{
  return (*dominio).N_etiquetas();
}


int variable_t::Get_Unit() const{
  return unit;
}

double variable_t::Get_Convert_Factor() const {
  return convert_factor;
}

void variable_t::Put_Unit(int unidad){
  unit = unidad;
}

void variable_t::Put_Convert_Factor(double factor){
  convert_factor=factor;
}



// Operador que devuelve un objeto <fuzzy_t> con la definici�n de la etiqueta
// i-esima del dominio de la variable.
fuzzy_t variable_t::FuzzyLabel(int i) const{
  fuzzy_t aux;
  aux = (*dominio).FuzzyLabel(i);
  return aux;
}


// Operador que devuelve el valor central de la etiqueta i-esima del dominio de la variable.
double variable_t::CenterLabel(int i) const{
  return (*dominio).CenterLabel(i);
}


// Operador que determina si el dominio asociado a la variable esta compuesto exclusivamente
// por valores crisp.
bool variable_t::IsDiscrete() const{
  return (*dominio).IsDiscrete();
}



// Operador que determina si el dominio asociado a la variable esta compuesto exclusivamente
// por intervalos.

bool variable_t::IsInterval() const{
  return (*dominio).IsInterval();
}



// Operador que determina si el dominio asociado a la variable esta compuesto exclusivamente
// por conjuntos difusos. Ninguno es un interval o un valor crisp.
bool variable_t::IsFuzzy() const{
  return (*dominio).IsInterval();
}



// Operador que devuelve el area de la etiqueta l-esima del dominio de la variable.
double variable_t::Area(int l) const{
  return (*dominio).Area(l);
}


// Operador que devuelve un objeto <domain_t> con la definici�n del dominio de la variable
domain_t variable_t::Domain() const{
  domain_t aux;
  aux=(*dominio);
  return aux;
}


// Devuelve una copia de la variable
variable_t variable_t::Variable() const{
  variable_t aux;
  aux=*this;
  return aux;
}



// Devuelve el valor inferior del intervalo de definici�n del dominio de la variable.
double variable_t::Inf_Range() const{
  return (*dominio).Inf_Range();
}

// Devuelve el valor superior del intervalo de definici�n del dominio de la variable.
double variable_t::Sup_Range() const{
  return (*dominio).Sup_Range();
}


// Operador que guarda en un fichero binario la definici�n de la variable.
void variable_t::SaveBinaryCode(ofstream &f) const{
  int n=nombre.length()+1;
  f.write(reinterpret_cast<const char *> (&n),sizeof(int));
  f.write(nombre.c_str(),n);
  cout <<"\t\tSalvada Variable: " << nombre.c_str() << " de tamano " << n <<endl;
  f.write(reinterpret_cast<const char *> (&activa),sizeof(bool));
  f.write(reinterpret_cast<const char *> (&antecedente),sizeof(bool));
  f.write(reinterpret_cast<const char *> (&unit),sizeof(int));
  f.write(reinterpret_cast<const char *> (&convert_factor),sizeof(double));
  dominio[0].SaveBinaryCode(f);
}


// Operador que recupera y asigna a partir de un fichero binario la definici�n de una variable
void variable_t::LoadBinaryCode(ifstream &f){
  // Eliminar la informacion del objeto
  if (dominio!=0)
    delete dominio;
  // Asignar los nuevos valores al objeto
  int n;
  char *aux;
  f.read(reinterpret_cast<char *> (&n),sizeof(int));
  aux=new char[n];
  f.read(aux,n);
  nombre=aux;
  delete [] aux;
  f.read(reinterpret_cast<char *> (&activa),sizeof(bool));
  f.read(reinterpret_cast<char *> (&antecedente),sizeof(bool));
  f.read(reinterpret_cast<char *> (&unit),sizeof(int));
  f.read(reinterpret_cast<char *> (&convert_factor),sizeof(double));
  dominio = new domain_t;
  dominio[0].LoadBinaryCode(f);
}
