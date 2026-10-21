#include <iostream>
#include <string>
#include <sstream>
#include "INSLV/fuzzy_t.h"

using namespace std;

// Constructor por defecto de la clase
fuzzy_t::fuzzy_t(){
  a=b=c=d=0;
  menosinfinito=masinfinito=false;
  nombre = "Creado, no usado";
}


// Constructor parametrizado de clase
// [a,b,c,d] representan una etiqueta trapezoidal y name es el nombre
// de la etiqueta. Por defecto se asume que los extremos estan acotados.
fuzzy_t::fuzzy_t(double a, double b, double c, double d, string name){
  this->a = a;
  this->b = b;
  this->c = c;
  this->d = d;
  nombre = name;
  menosinfinito=masinfinito=false;
}


// Constructor parametrizado de clase
// [a,b,c,d] representan una etiqueta trapezoidal.
// name es el nombre de la etiqueta
// menos y mas representan si estan acotados o no los extremos.
fuzzy_t::fuzzy_t(double a, double b, double c, double d, string name, bool
 menos, bool mas){
  this->a = a;
  this->b = b;
  this->c = c;
  this->d = d;
  menosinfinito = menos;
  masinfinito = mas;
  nombre = name;
}




// Constructor de copia
fuzzy_t::fuzzy_t(const fuzzy_t &x){
  cout << "constructor de copia\n";
  *this=x;
}




// Operador de asignaci�n por copia
fuzzy_t &fuzzy_t::operator=(const fuzzy_t &x){
  a=x.a;
  b=x.b;
  c=x.c;
  d=x.d;
  menosinfinito=x.menosinfinito;
  masinfinito=x.masinfinito;
  nombre = x.nombre;
  return *this;
}



// Operador de asignaci�n para etiquetas no acotadas por un extremo.
void fuzzy_t::Asigna(double a, double b, double c, double d, string name, bool menos, bool mas){
  this->a = a;
  this->b = b;
  this->c = c;
  this->d = d;
  menosinfinito = menos;
  masinfinito = mas;
  nombre = name;
}

// Operador de asignaci�n para etiquetas acotadas en sus dos extremos.
void fuzzy_t::Asigna(double a, double b, double c, double d, string name){
  this->a = a;
  this->b = b;
  this->c = c;
  this->d = d;
  menosinfinito = false;
  masinfinito = false;
  nombre = name;
}



// Operador que evalua el grado de adaptaci�n de un determinado valor x
// a la etiqueta.
double fuzzy_t::Adaptacion(double x) const {
  if (x==-999999999)
    return 1;
  if ( (menosinfinito && x<c) || (masinfinito && x>b))
    return 1;
  if (x<a)
    return 0;
  else if (x<b)
         return (x-a)/(b-a);
       else  if (x<=c)
                return 1;
             else  if (x<d)
	              return (d-x)/(d-c);
		   else
		      return 0;
}



// Operador que permite poner nombre a la etiqueta
void fuzzy_t::Poner_Nombre(string name){
  nombre = name;
}




// Operador que pone en el dispositivo de salida la definici�n de la etiqueta.
void fuzzy_t::Pinta() const {
   cout << "Nombre: " << nombre << endl;
   if (menosinfinito)
     cout << "[-inf,-inf," << c << "," << d << "]\n";
   else if (masinfinito)
          cout << "[" << a <<","<< b <<",inf,inf]\n";
	else
          cout << "[" << a <<","<< b <<"," << c << "," << d << "]\n";
}

// Operador que pone en el dispositivo de salida la definici�n de la etiqueta.
string fuzzy_t::PintaS() const {
  std::string out;
   if (menosinfinito)
     out += "[-inf,-inf," + to_string(c) + "," + to_string(d) + "] " + "Area: " + to_string(Area());
   else if (masinfinito)
          out += "[" + to_string(a) + "," + to_string(b) + ",inf,inf] " + "Area: " + to_string(Area());
	else
          out += "[" + to_string(a) + "," + to_string(b) + "," + to_string(c) + "," + to_string(d) + "] " + "Area: " + to_string(Area());

  return out;
}


// Operador que pone en el dispositivo de salida la definici�n de la etiqueta.
string fuzzy_t::SPinta() const {
  string cadena;
  ostringstream flujo;
  flujo << nombre << " ";
   if (menosinfinito)
    flujo << "[-inf,-inf," << c << "," << d << "]";
   else if (masinfinito)
          flujo << "[" << a <<","<< b <<",inf,inf]";
	else
          flujo << "[" << a <<","<< b <<"," << c << "," << d << "]";
  cadena = flujo.str();
  return cadena;
}


// Operador que pone en el dispositivo de salida el nombre de la etiqueta
void fuzzy_t::Print() const {
   cout << nombre;
}


// Operador que devuelve en un string el nombre de la etiqueta.
string fuzzy_t::SPrint() const {
  return nombre;
}


// Operador que devuelve un etiqueta (un objeto de tipo <fuzzy_t>
fuzzy_t fuzzy_t::FuzzyLabel() const{
  fuzzy_t aux;
  aux=*this;
  return aux;
}


// Operador que devuelve el valor central de la etiqueta
double fuzzy_t::CenterLabel() const{
  if (menosinfinito)
    return c;
  else if (masinfinito)
          return b;
       else
          return (b+c)/2.0;
}


// Operador que determina si la etiqueta difusa representa un valor crisp
bool fuzzy_t::IsDiscrete() const {
  return (a==d);
}


// Operador que determina si la etiqueta difusa representa un intervalo cl�sico
bool fuzzy_t::IsInterval() const{
  return (a==b && c==d && b!=c);
}


// Operador que determina si la etiqueta difusa es un conjunto difuso.
bool fuzzy_t::IsFuzzy() const{
  return (!IsDiscrete() && !IsInterval());
}



//Operador que determina el area de la etiqueta.
double fuzzy_t::Area() const{
  return ((b-a)/2.0)+((d-c)/2.0)+(c-d);
}



void fuzzy_t::GetDefinicion(double &pa, double &pb, double &pc, double &pd, string &pnombre) const{
  pa =a;
  pb =b;
  pc =c;
  pd =d;
  pnombre = nombre;
}



void fuzzy_t::PutDefinicion(double pa, double pb, double pc, double pd, string pnombre){
  a=pa;
  b=pb;
  c=pc;
  d=pd;
  nombre=pnombre;
}




//Operador que guarda en un fichero binario la definici�n de la etiqueta
void fuzzy_t::SaveBinaryCode(ofstream &f) const{
  f.write(reinterpret_cast<const char *> (&a),sizeof(double));
  f.write(reinterpret_cast<const char *> (&b),sizeof(double));
  f.write(reinterpret_cast<const char *> (&c),sizeof(double));
  f.write(reinterpret_cast<const char *> (&d),sizeof(double));
  f.write(reinterpret_cast<const char *> (&masinfinito), sizeof(bool));
  f.write(reinterpret_cast<const char *> (&menosinfinito), sizeof(bool));
  int n=nombre.length();
  f.write(reinterpret_cast<const char *> (&n),sizeof(int));
  f.write(nombre.c_str(),n);
}


//Operador que asigna a un objeto etiqueta con la informaci�n procedente
// de un fichero binario.
void fuzzy_t::LoadBinaryCode(ifstream &f) {
  char *aux;
  int n;

  f.read(reinterpret_cast<char *> (&a),sizeof(double));
  f.read(reinterpret_cast<char *> (&b),sizeof(double));
  f.read(reinterpret_cast<char *> (&c),sizeof(double));
  f.read(reinterpret_cast<char *> (&d),sizeof(double));
  f.read(reinterpret_cast<char *> (&masinfinito), sizeof(bool));
  f.read(reinterpret_cast<char *> (&menosinfinito), sizeof(bool));
  f.read(reinterpret_cast<char *> (&n),sizeof(int));
  aux=new char[n+1];
  f.read(aux,n);
  aux[n]='\0';
  nombre=aux;
  delete [] aux;
}
