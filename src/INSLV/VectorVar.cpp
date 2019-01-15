using namespace std;
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <limits>
#include "INSLV/VectorVar.h"





VectorVar::VectorVar(){
  numero=0;
  lista=0;
  oper=0;
}


void Leer_Variable_Fichero (fstream &fich, string &nom_var, int &antecedente,
                            double &inf, double &sup, int &activa, int &unit, double &c_factor,
                            int &n, double *&va, double *&vb, double *&vc, double *&vd,
			    string *&vname){
  fich >> nom_var >> antecedente >> unit>> c_factor >> inf >> sup >> n;
  cout << endl << nom_var << "," << antecedente << "," << unit << "," <<c_factor <<","<< inf <<"," << sup << "," << n << endl;
  va = new double[n];
  vb = new double[n];
  vc = new double[n];
  vd = new double[n];
  vname = new string[n];
  for (int i=0; i<n; i++)
    fich >> va[i] >> vb[i] >> vc[i] >> vd[i] >> vname[i];


     for (int i=0; i<n; i++)
     	cout << "[" << va[i] << "," << vb[i] <<","<<vc[i]<<","<<vd[i]<<"] " << vname[i] << endl;

}


void Libera_Memoria_Var_aux (double *&va, double *&vb, double *&vc, double *&vd,
                             string *&vname){
   delete []va;
   delete []vb;
   delete []vc;
   delete []vd;
   delete []vname;
}


VectorVar::VectorVar(const char *nom_fich){

   double *va,*vb,*vc,*vd,inf,sup, c_factor;
   string nom_var, *vname;
   int antecedente, activa, n, unit;

   fstream fich(nom_fich,ios::in);
   if (!fich){
     cout << "El fichero no existe\n";
     exit(1);
   }

   fich >> numero;
   lista = new variable_t[numero];
   oper = new combinado[numero];

   for (int j=0; j<numero; j++){
     Leer_Variable_Fichero (fich, nom_var, antecedente, inf, sup, activa, unit, c_factor, n, va, vb, vc, vd, vname);
     lista[j].Asigna(n, nom_var, antecedente, unit, c_factor, inf, sup, va, vb, vc, vd, vname);
     oper[j].op=0;
     // imprimir los datos de las variables

     Libera_Memoria_Var_aux(va,vb,vc,vd,vname);
   }
}


VectorVar::VectorVar(int tamano){
  numero=tamano;
  lista = new variable_t[tamano];
  oper = new combinado[tamano];
}

VectorVar::VectorVar(std::vector < std::vector < double > > * dataset, std::vector< std::pair<std::string, std::string> > attribLabels, std::map < std::string, std::vector <FuzzSet> > attribMap){
  double *va,*vb,*vc,*vd,inf,sup, c_factor;
  string nom_var, *vname;
  int antecedente, activa, n, unit;

  unsigned int cont = 0;
  numero = attribLabels.size();
  lista = new variable_t[attribLabels.size()];
  oper = new combinado[attribLabels.size()];


  for(unsigned int attrib = 0; attrib < attribLabels.size(); attrib++){


    //nom_var nombre de la variable
    nom_var = attribLabels[attrib].first;
    //antecedente 1 si es clase, 0 si no
    if(nom_var == "Class"){
      antecedente = 1;
    }else{
      antecedente = 0;
    }

    //unit poner a 0
    unit = 0;

    //c_factor poner a 0
    c_factor = 0;

    //inf
    inf = attribMap[ attribLabels[attrib].first][0].a;
    //sup
    sup = attribMap[ attribLabels[attrib].first][attribMap[ attribLabels[attrib].first].size()-1].d;

    va = new double[attribMap[ attribLabels[attrib].first].size()];
    vb = new double[attribMap[ attribLabels[attrib].first].size()];
    vc = new double[attribMap[ attribLabels[attrib].first].size()];
    vd = new double[attribMap[ attribLabels[attrib].first].size()];
    vname = new std::string[attribMap[ attribLabels[attrib].first].size()];

    for(unsigned int i = 0; i < attribMap[ attribLabels[attrib].first].size(); i++){
      va[i] = attribMap[ attribLabels[attrib].first][i].a;
      vb[i] = attribMap[ attribLabels[attrib].first][i].b;
      vc[i] = attribMap[ attribLabels[attrib].first][i].c;
      vd[i] = attribMap[ attribLabels[attrib].first][i].d;
      vname[i] = attribMap[ attribLabels[attrib].first][i].label;
    }

    /*
    std::cout << "n: " << (*attrIT).second.size() << "\n";
    std::cout << "nom_var: " << nom_var << "\n";
    std::cout << "antecedente: " << antecedente << "\n";
    std::cout << "unit: " << unit << "\n";
    std::cout << "c_factor: " << c_factor << "\n";
    std::cout << "inf: " << inf << "\n";
    std::cout << "sup: " << sup << "\n";

    for(unsigned int i = 0; i < (*attrIT).second.size(); i++){
      std::cout << va[i] << " "  << vb[i] << " "  << vc[i] << " "  << vd[i] << " "  << vname[i] << "\n";
    }
    std::cout << "\n\n\n";
    */

    lista[cont].Asigna(attribMap[ attribLabels[attrib].first].size(), nom_var, antecedente, unit, c_factor, inf, sup, va, vb, vc, vd, vname, true);
    oper[cont].op=0;
    Libera_Memoria_Var_aux(va,vb,vc,vd,vname);

    cont++;
  }




}


VectorVar::~VectorVar(){
  if (lista!=0){
    delete []lista;
    delete []oper;
  }
  numero=0;
  lista=0;
  oper=0;
}


VectorVar::VectorVar(const VectorVar &x){
  numero=x.numero;
  lista = new variable_t[numero];
  oper = new combinado[numero];
  for (int i=0; i<numero; i++){
    lista[i]=x.lista[i];
    oper[i]=x.oper[i];
  }
}


VectorVar & VectorVar::operator=(const VectorVar &x){
 if (this!=&x){
  if (lista!=0)
    delete [] lista;

  numero=x.numero;
  lista = new variable_t[numero];
  oper = new combinado[numero];
  for (int i=0; i<numero; i++){
    lista[i]=x.lista[i];
    oper[i]=x.oper[i];
  }
 }

  return *this;
}


void VectorVar::Asigna(int pos, const variable_t &x){
  if (pos<numero){
    lista[pos]=x;
  }
  else
    cout << "La posicion no existe\n";
}

void VectorVar::Asigna(int pos, int op, int var1, int var2, double min, double max){
  if (pos<numero){
    oper[pos].op=op;
    oper[pos].operando[0]=var1;
    oper[pos].operando[1]=var2;
    oper[pos].rango[0]=min;
    oper[pos].rango[1]=max;
  }
  else
    cout << "La posicion no existe\n";
}


int VectorVar::N_Antecedente() const {
  int s=0;
  for (int i=0; i<numero; i++)
    if (lista[i].Activa() && lista[i].Antecedente())
      s++;

  return s;
}


void VectorVar::Encode(int &tb, int &te) const {
  tb=0;
  for (int i=0; i<numero; i++)
    if (lista[i].Activa() && lista[i].Antecedente())
      tb+=lista[i].N_etiquetas();
    else  if (lista[i].Activa() && !lista[i].Antecedente())
            te=lista[i].N_etiquetas();
}


void VectorVar::Pinta(int variable) const{
  if (variable >=0 && variable < numero)
    lista[variable].Pinta();
  else
    cout << "Esa variable no existe\n";
}



void VectorVar::Pinta() const{
  for (int i=0; i<numero; i++){
    std::cout << std::endl;
    lista[i].Pinta();
    std::cout << "----------" << std::endl;
  }
}

std::string VectorVar::PintaS(int variable) const{
  if (variable >=0 && variable < numero)
    return lista[variable].PintaS();
  else
    return "Esa variable no existe\n";
}

std::string VectorVar::PintaS(int variable, int label) const{
  if (variable >=0 && variable < numero)
    return lista[variable].PintaS(label);
  else
    return "Esa variable no existe\n";
}



std::string VectorVar::PintaS() const{
  std::string out;
  for (int i=0; i<numero; i++){
    out += lista[i].PintaS();
  }
  return out;
}


void VectorVar::PrintVar(int variable) const{
  lista[variable].PrintVar();
}

string VectorVar::SPrintVar(int variable) const{
  return lista[variable].SPrintVar();
}

void VectorVar::PrintDomain(int variable, int value) const {
  lista[variable].PrintDomain(value);
}

string VectorVar::SPrintDomain(int variable, int value) const{
  return lista[variable].SPrintDomain(value);
}

bool VectorVar::IsActiva(int variable) const{
  return lista[variable].Activa();
}


bool VectorVar::IsAntecedente(int variable) const{
  return lista[variable].Antecedente();
}


int VectorVar::TotalVariables() const{
 return numero;
}

int VectorVar::SizeDomain(int variable) const{
  return lista[variable].SizeDomain();
}


int VectorVar::Get_Unit(int variable) const {
  return lista[variable].Get_Unit();
}


double VectorVar::Get_Convert_Factor(int variable) const {
  return lista[variable].Get_Convert_Factor();
}


void VectorVar::Put_Unit(int variable, int Unit){
  lista[variable].Put_Unit(Unit);
}


void VectorVar::Put_Convert_Factor(int variable, double factor){
  lista[variable].Put_Convert_Factor(factor);
}



double VectorVar::Adaptacion(double x, int variable) const{
  if (variable >=0 && variable < numero)
    return lista[variable].Adaptacion(x);
  else{
    cout << "Esa variable no existe\n";
    exit(1);
  }
}


double VectorVar::Adaptacion(double x, int variable, int dominio) const {
  if (variable >=0 && variable < numero)
    return lista[variable].Adaptacion(x,dominio);
  else {
    cout << "Esa variable no existe\n";
    exit(1);
  }
}



double VectorVar::Adaptacion(double x, int variable, string dominio) const{
  if (variable >=0 && variable < numero)
    return lista[variable].Adaptacion(x,dominio);
  else {
    cout << "Esa variable no existe\n";
    exit(1);
  }

}


double VectorVar::Adaptacion(vectordouble x, string regla) const{
  double max=1,aux;
  string sub;
  int trozo=0,tam;
  for (int i=0; i<numero && max>0; i++){
    if (lista[i].Activa() && lista[i].Antecedente()){
       tam=lista[i].N_etiquetas();
       sub=regla.substr(trozo,tam);
       aux=lista[i].Adaptacion(x.At(i),sub);
       if (aux<max)
         max=aux;
       trozo+=tam;
    }
  }

  return max;
}


double VectorVar::Adaptacion_funcion(vectordouble x, string regla, int *lista_funcion, int tama, double adaptacion_previa) const{
  double max=adaptacion_previa,aux;
  string sub;
  int trozo=0,tam,x1,x2,op;
  double resultado;
  for (int i=0; i<tama && max>0; i++){
    if (lista_funcion[i]!=0){
       tam=lista[lista_funcion[i]-1].N_etiquetas();
       sub=regla.substr(trozo,tam);
       x1 = oper[lista_funcion[i]-1].operando[0];
       x2 = oper[lista_funcion[i]-1].operando[1];
       op = oper[lista_funcion[i]-1].op;
       if (x.At(x1)==-999999999 or x.At(x2)==-999999999)
        resultado= -999999999;
       else if (op==1){
              if (x.At(x1)>=x.At(x2))
                resultado=1;
              else
                resultado=0;
            } else if (op==2)
                      resultado=(x.At(x1))+(x.At(x2));
                   else if (op==3)
                          resultado=x.At(x1)*x.At(x2);
                        else if (op==4)
                               resultado=(x.At(x1))-(x.At(x2));
                             else if (op==5 and x.At(x2)!=0)
                                    resultado = (x.At(x1))/(x.At(x2));
                                  else if (op==5)
                                         return -999999999;
                                       else if (op==19){
					      if ((x.At(x1))==(x.At(x2)))
						resultado=1;
					      else
						resultado=0;
					   }
                                       else if (op==7 or op==8)
                                              resultado=x.At(x1);
                                           else
                                              max=0;

       if (max>0)
         aux=lista[lista_funcion[i]-1].Adaptacion(resultado,sub);

       if (aux<max)
         max=aux;
    }
    //else
        tam=2;

    trozo+=tam;
  }

  return max;
}



double VectorVar::Adaptacion_funcion_debug (vectordouble x, string regla, int *lista_funcion, int tama, double adaptacion_previa) const{
  double max=adaptacion_previa,aux;
  string sub;
  int trozo=0,tam,x1,x2,op;
  double resultado;
  for (int i=0; i<tama && max>0; i++){
    if (lista_funcion[i]!=0){
       lista[lista_funcion[i]-1].Pinta();
       tam=lista[lista_funcion[i]-1].N_etiquetas();
       sub=regla.substr(trozo,tam);
       x1 = oper[lista_funcion[i]-1].operando[0];
       x2 = oper[lista_funcion[i]-1].operando[1];
       op = oper[lista_funcion[i]-1].op;
       cout << "\t\t\t\t\t" << x1 << " op: " << op << " " << x2 << endl;

       if (x.At(x1)==-999999999 or x.At(x2)==-999999999)
        resultado= -999999999;
       else if (op==1){
              if (x.At(x1)>=x.At(x2))
                resultado=1;
              else
                resultado=0;
            } else if (op==2)
                      resultado=(x.At(x1))+(x.At(x2));
                   else if (op==3)
                          resultado=x.At(x1)*x.At(x2);
                        else if (op==4)
                               resultado=(x.At(x1))-(x.At(x2));
                             else if (op==5 and x.At(x2)!=0)
                                    resultado = (x.At(x1))/(x.At(x2));
                                  else if (op==5)
                                         return -999999999;
                                       else if (op==19){
					      if ((x.At(x1))==(x.At(x2)))
						resultado=1;
					      else
						resultado=0;
					   }
                                       else if (op==7 or op==8)
                                              resultado=x.At(x1);
                                           else
                                              max=0;

       if (max>0) {
         aux=lista[lista_funcion[i]-1].Adaptacion(resultado,sub);
         cout << "\t\t\t\t\t\t  ( " << x.At(x1) << " , " << x.At(x2) << "  Resultado: " << resultado << endl;
         cout << "\t\t\t\t\t\t Adaptacion: " << aux << endl;
       }

       if (aux<max){
         max=aux;
         cout << "\t\t\t\t\t\t Adaptacion Acumulada: " << max << endl;
       }
    }
    //else
        tam=2;

    trozo+=tam;
  }

  return max;
}



int NumeroEtiquetasActivas(string sub, int tam){
  int n=0;
  for (int i=0; i<tam; i++)
    if (sub[i]=='1')
      n++;
  return n;
}

void VectorVar::SecuenciasDeEtiquetasActivas(string sub, int tam, int &unos, int &ceros, int &n_unos) const{
  unos=0;
  ceros=0;
  n_unos=0;
  bool last_uno;
  int i=1;
  if (sub[0]=='0'){
    ceros++;
    last_uno=false;
  }
  else{
    unos++;
    last_uno=true;
    n_unos++;
  }
  while (i<tam){
    if (sub[i]=='1')
      n_unos++;
    if (last_uno && sub[i]=='0'){
      last_uno=false;
      ceros++;
    }
    else if (!last_uno && sub[i]=='1'){
           last_uno=true;
           unos++;
    }
    i++;
  }
}

bool VectorVar::Es_Valida(string regla, double *var, double umbral, double &simplicidad) const{
  string sub;
  int trozo=0,tam, unos, ceros, n_unos;
  int i=0;
  simplicidad=0;
  bool valida=true;
  while (i<numero && valida){
    if (lista[i].Activa() && lista[i].Antecedente()){
      tam=lista[i].N_etiquetas();
      if (var[i]>=umbral){
        sub=regla.substr(trozo,tam);
        //unos = NumeroEtiquetasActivas(sub,tam);
        SecuenciasDeEtiquetasActivas(sub,tam,unos,ceros, n_unos);
        valida=(unos!=0);
        if (valida){
          if (unos==1 || ceros==1)
            simplicidad=simplicidad+1;
        }
      }
      trozo+=tam;
    }
    i++;
  }

  return valida;
}

double VectorVar::Adaptacion(vectordouble x, string regla, double *var, double umbral) const{
  double max=1,aux;
  string sub;
  int trozo=0,tam, unos;
  //cout << "Numero de variables: " << numero << endl;
  for (int i=0; i<numero && max>0; i++){
    if (lista[i].Activa() && lista[i].Antecedente()){
      tam=lista[i].N_etiquetas();
      if (var[i]>=umbral){
        sub=regla.substr(trozo,tam);
        unos = NumeroEtiquetasActivas(sub,tam);
        if (unos==0)
          max=0;
        else if (unos<tam){
               aux=lista[i].Adaptacion(x.At(i),sub);
               if (aux<max)
                 max=aux;
             }
      }
      trozo+=tam;
    }
  }

  return max;
}




double VectorVar::Adaptacion(vectordouble x, string regla, double *var, double umbral, double umbral2) const{
  double max=1,aux;
  string sub;
  int trozo=0,tam, unos;

  if (umbral2<0)
    umbral2=-umbral2;

  for (int i=0; i<numero && max>=umbral2 && max>0; i++){
    if (lista[i].Activa() && lista[i].Antecedente()){
      tam=lista[i].N_etiquetas();
      if (var[i]>=umbral){
        sub=regla.substr(trozo,tam);
        unos = NumeroEtiquetasActivas(sub,tam);
        if (unos==0)
          max=0;
        else if (unos<tam){
               aux=lista[i].Adaptacion(x.At(i),sub);
               if (aux<max)
                 max=aux;
             }
      }
      trozo+=tam;
    }
  }

  if (max>=umbral2)
   return max;
  else
   return 0;
}







double VectorVar::Adaptacion(vectordouble x, const genetcode &regla) const{
  return 0;
}



void VectorVar::AdaptacionC(vectordouble x, int etiq, double &pos, double &neg) const{
  double valor,aux;
  int i=0;
  while (lista[i].Antecedente() && i<numero-1)
    i++;

  valor =x.At(i);
  pos=lista[i].Adaptacion(valor,etiq);
  neg=0;
  for (int j=0; j<lista[i].N_etiquetas(); j++)
    if (j!=etiq){
       aux=lista[i].Adaptacion(valor,j);
       if (aux>neg)
         neg=aux;
    }
}


double VectorVar::Area(int var,int lab) const{
  return lista[var].Area(lab);
}

fuzzy_t VectorVar::FuzzyLabel(int var,int lab) const{
  return lista[var].FuzzyLabel(lab);
}

double VectorVar::CenterLabel(int var,int lab) const{
  return lista[var].CenterLabel(lab);
}

bool VectorVar::IsDiscrete(int var) const{
  return lista[var].IsDiscrete();
}

bool VectorVar::IsInterval(int var) const{
  return lista[var].IsInterval();
}

bool VectorVar::IsFuzzy(int var) const{
  return lista[var].IsFuzzy();
}

domain_t VectorVar::Domain(int var) const{
  domain_t aux;
  aux = lista[var].Domain();
  return aux;
}

variable_t VectorVar::Variable(int var) const{
  variable_t aux;
  aux = lista[var].Variable();
  return aux;
}


double VectorVar::Inf_Range(int var) const{
   return lista[var].Inf_Range();
}

double VectorVar::Sup_Range(int var) const{
   return lista[var].Sup_Range();
}


int VectorVar::Consecuente() const {
  int i=0;
  while ((lista[i].Antecedente() || !lista[i].Activa()) && i<numero)
    i++;

  if (i!=numero)
    return i;
  else {
    cout << "No hay variable consecuente\n";
    return -1;
  }
}


void VectorVar::SaveBinaryCode(ofstream &f) const {
  f.write(reinterpret_cast<const char *> (&numero),sizeof(int));
  for (int i=0; i<numero; i++){
     lista[i].SaveBinaryCode(f);
     f.write(reinterpret_cast<const char *> (&(oper[i].op)),sizeof(int));
     f.write(reinterpret_cast<const char *> (&(oper[i].operando[0])),sizeof(int));
     f.write(reinterpret_cast<const char *> (&(oper[i].operando[1])),sizeof(int));
     f.write(reinterpret_cast<const char *> (&(oper[i].rango[0])),sizeof(double));
     f.write(reinterpret_cast<const char *> (&(oper[i].rango[1])),sizeof(double));
  }
}


void VectorVar::LoadBinaryCode(ifstream &f){
  // Eliminar objeto
  if (numero>0){
    delete [] lista;
  }

  f.read(reinterpret_cast<char *> (&numero),sizeof(int));
  lista = new variable_t[numero];
  oper = new combinado[numero];

  for (int i=0; i<numero; i++){
     lista[i].LoadBinaryCode(f);
     f.read(reinterpret_cast<char *> (&(oper[i].op)),sizeof(int));
     f.read(reinterpret_cast<char *> (&(oper[i].operando[0])),sizeof(int));
     f.read(reinterpret_cast<char *> (&(oper[i].operando[1])),sizeof(int));
     f.read(reinterpret_cast<char *> (&(oper[i].rango[0])),sizeof(double));
     f.read(reinterpret_cast<char *> (&(oper[i].rango[1])),sizeof(double));
  }
}


void VectorVar::Add_Variable(variable_t &var){
  variable_t *p = new variable_t [numero+1];
  combinado *q = new combinado [numero+1];
  p[numero]=lista[numero-1];
  q[numero]=oper[numero-1];
  for (int i=0; i<numero-1; i++){
    p[i]=lista[i];
    q[i]=oper[i];
  }
  p[numero-1] = var;
  q[numero-1].op = 0;
  numero++;
  delete [] lista;
  delete [] oper;
  lista = p;
  oper = q;
}



void VectorVar::Add_Variable(variable_t &var, int op, int var1, int var2, double min, double max){
  variable_t *p = new variable_t [numero+1];
  combinado *q = new combinado [numero+1];
  p[numero]=lista[numero-1];
  q[numero]=oper[numero-1];
  for (int i=0; i<numero-1; i++){
    p[i]=lista[i];
    q[i]=oper[i];
  }
  p[numero-1] = var;
  q[numero-1].op = op;
  q[numero-1].operando[0] = var1;
  q[numero-1].operando[1] = var2;
  q[numero-1].rango[0] = min;
  q[numero-1].rango[1] = max;

  numero++;
  delete [] lista;
  delete [] oper;
  lista = p;
  oper = q;
}

void VectorVar::Add_Variable_Lista(variable_t &var, int op, int var1, int var2, double min, double max){
  variable_t *p = new variable_t [numero+1];
  combinado *q = new combinado [numero+1];
  for (int i=0; i<numero; i++){
    p[i]=lista[i];
    q[i]=oper[i];
  }
  p[numero] = var;
  q[numero].op = op;
  q[numero].operando[0] = var1;
  q[numero].operando[1] = var2;
  q[numero].rango[0] = min;
  q[numero].rango[1] = max;

  numero++;
  delete [] lista;
  delete [] oper;
  lista = p;
  oper = q;
}



double Operacion (vectordouble w, combinado var){
  double a;
  double x1=w.At(var.operando[0]);
  double x2=w.At(var.operando[1]);

 if (x1==-999999999 || x2==-999999999)
   return -999999999;
 switch (var.op){
   case 0:
           return x1;
	   break;
             //Operacion OR
   case 20: if (x1 == 1 || x2 == 1)
            return 1;
           else
            return 0;
           break;
	   //Operacion XOR
    case 21: if ( (x1 == 1 && x2 == 0 ) || (x1 == 0 && x2 == 1 ) )
            return 1;
           else
            return 0;
           break;
    case 22: if ( (x1 == 1 && x2 == 1 ))
            return 1;
           else
            return 0;
           break;

   case 1: if (x1>=x2)
            return 1;
           else
            return 0;
           break;

   case 2: return x1+x2;
           break;

  case 5:  if (x2!=0)
             return (x1/x2);
           else
             return -999999999;
           break;

/*   case 5:
	      return (fabs(x1)*fabs(x2))/(fabs(x1)+fabs(x2)+1.0);
	   break;
*/
   case 4: return x1-x2;
           break;

   case 3: return (x1*x2);
           break;
   case 7: /*a = sin(x1);
                 if (a<-1 || a >1)
		    cout << "sin(" << x1 << ")=" << a << endl;
                  return a;*/
          return x1;
          break;
   case 8:
                 //return cos(x1);
          return x2;
          break;
   case 9: return log(fabs(x1)+1);
          break;
   case 10: return sin(x1)+cos(x1);
           break;
   case 11: return sqrt(fabs(x1));
           break;
   case 12: return 1.0/(fabs(x1)+1);
           break;
   case 6: return (x1-var.rango[0])/(var.rango[1]-var.rango[0]);
           break;
   case 13:return 2*(x1-var.rango[0])/(var.rango[1]-var.rango[0])-1;
           break;
   case 15: return x1;
          break;
 }
 return 0;
}


vectordouble VectorVar::Convert_Example (vectordouble w) const{
   vectordouble z(numero-1);

   for (int i=0; i<numero-1; i++){
       if (oper[i].op==0)
	  z.Put(w.At(i),i);
       else
	  z.Put(Operacion(w,oper[i]),i);
   }
   return z;
}


void VectorVar::Funcion(int pos, int &x1, int &x2, int &op) const{
    x1 = oper[pos].operando[0];
    x2 = oper[pos].operando[1];
    op = oper[pos].op;
}
