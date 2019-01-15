using namespace std;
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include "INSLV/example_set.h"


example_set::example_set(){
  n_example=0;
  n_variable=0;
  data=0;
  covered=0;
  g_positive_covered=0;
  g_negative_covered=0;
  peso_positive=0;
  peso_negative=0;
  rule_used=0;
  rule_aislada=0;
  n_partition=0;
  partition=0;
}

example_set::example_set(int ejemplos, int variables){
  if (ejemplos<0 ||variables<=0)
    cout << "No se admiten valores negativos\n";
  else {
    n_example=ejemplos;
    n_variable=variables;
    n_partition=1;
    covered = new bool[n_example];
    g_positive_covered = new double [n_example];
    g_negative_covered = new double [n_example];
    peso_positive= new double [n_example];
    peso_negative= new double [n_example];
    rule_used= new int [n_example];
    rule_aislada= new int [n_example];
    data = new double*[n_example];
    partition = new int[n_example];
    for (int i=0; i<n_example; i++){
      data[i]= new double[variables];
      covered[i]=false;
      g_positive_covered[i]=0;
      g_negative_covered[i]=0;
      peso_positive[i]=0;
      peso_negative[i]=0;
      rule_used[i]=-1;
      rule_aislada[i]=0;
      partition[i]=0;
    }
  }
}

example_set::example_set(const example_set &x){
  n_example=x.n_example;
  n_variable=x.n_variable;
  n_partition=x.n_partition;
  covered = new bool[n_example];
  g_positive_covered = new double [n_example];
  g_negative_covered = new double [n_example];
  peso_positive= new double [n_example];
  peso_negative= new double [n_example];
  rule_used= new int [n_example];
  rule_aislada= new int [n_example];
  data = new double*[n_example];
  partition = new int[n_example];
  for (int i=0; i<n_example; i++){
    data[i] = new double[n_variable];
    covered[i] = x.covered[i];
    g_positive_covered[i]=x.g_positive_covered[i];
    g_negative_covered[i]=g_negative_covered[i];
    peso_positive[i]=x.peso_positive[i];
    peso_negative[i]=x.peso_negative[i];
    rule_used[i]=x.rule_used[i];
    rule_aislada[i]=x.rule_aislada[i];
    partition[i]=x.partition[i];
  }

  for (int i=0; i<n_example; i++)
    for (int j=0; j<n_variable; j++)
      data[i][j]=x.data[i][j];
}




example_set::example_set(const char *nom_fich){

  fstream fich(nom_fich,ios::in);

  if (!fich){
    cout << "El fichero " << nom_fich << " no existe\n";
  }
  else {

    fich >> n_example;
    fich >> n_variable;

    n_partition=1;
    partition= new int[n_example];
    data = new double*[n_example];
    covered = new bool[n_example];
    g_positive_covered = new double [n_example];
    g_negative_covered = new double [n_example];
    peso_positive= new double [n_example];
    peso_negative= new double [n_example];
    rule_used= new int [n_example];
    rule_aislada= new int [n_example];

    for (int i=0; i<n_example; i++){
      data[i] = new double[n_variable];
      covered[i]=false;
      g_positive_covered[i]=0;
      g_negative_covered[i]=0;
      peso_positive[i]=0;
      peso_negative[i]=0;
      rule_used[i]=-1;
      rule_aislada[i]=0;
      partition[i]=0;
    }

    for (int i=0; i<n_example; i++)
      for (int j=0; j<n_variable; j++)
        fich >> data[i][j];
  }
}


example_set::example_set(std::vector < std::vector < double > > * dataset){

  n_example =(*dataset)[0].size();
  n_variable = dataset -> size();

  n_partition=1;
  partition= new int[n_example];
  data = new double*[n_example];
  covered = new bool[n_example];
  g_positive_covered = new double [n_example];
  g_negative_covered = new double [n_example];
  peso_positive= new double [n_example];
  peso_negative= new double [n_example];
  rule_used= new int [n_example];
  rule_aislada= new int [n_example];

  for (int i=0; i<n_example; i++){
    data[i] = new double[n_variable];
    covered[i]=false;
    g_positive_covered[i]=0;
    g_negative_covered[i]=0;
    peso_positive[i]=0;
    peso_negative[i]=0;
    rule_used[i]=-1;
    rule_aislada[i]=0;
    partition[i]=0;
  }

  for (int i=0; i<n_variable; i++){
    for (int j=0; j<n_example; j++){
      data[j][i] = (*dataset)[i][j];
    }
  }
}





void example_set::Realojar(int new_examples){
  int final_n_examples=n_example+new_examples;

  int *aux_partition = new int[final_n_examples];
  double **aux_data = new double*[final_n_examples];
  bool *aux_covered = new bool[final_n_examples];
  double *aux_g_positive_covered = new double[final_n_examples];
  double *aux_g_negative_covered = new double[final_n_examples];
  double *aux_peso_positive = new double[final_n_examples];
  double *aux_peso_negative = new double[final_n_examples];
  int *aux_rule_used = new int[final_n_examples];
  int *aux_rule_aislada = new int[final_n_examples];


  for (int i=0; i<n_example; i++){
    aux_data[i] = data[i];
    aux_covered[i]=covered[i];
    aux_g_positive_covered[i]=g_positive_covered[i];
    aux_g_negative_covered[i]=g_negative_covered[i];
    aux_peso_positive[i]=peso_positive[i];
    aux_peso_negative[i]=peso_negative[i];
    aux_rule_used[i]=rule_used[i];
    aux_rule_aislada[i]=rule_aislada[i];
    aux_partition[i]=partition[i];
  }

  if (n_example!=0){
    delete [] partition;
    delete [] data;
    delete [] covered;
    delete [] g_positive_covered;
    delete [] g_negative_covered;
    delete [] peso_positive;
    delete [] peso_negative;
    delete [] rule_used;
    delete [] rule_aislada;
  }

  partition=aux_partition;
  data = aux_data;
  covered = aux_covered;
  g_positive_covered = aux_g_positive_covered;
  g_negative_covered = aux_g_negative_covered;
  peso_positive = aux_peso_positive;
  peso_negative = aux_peso_negative;
  rule_used = aux_rule_used;
  rule_aislada = aux_rule_aislada;
  n_example=final_n_examples;
}


example_set &example_set::operator=(const example_set &x){
  if (n_example>0 && n_variable>0 && data!=0){
    for (int i=0; i<n_example; i++)
      delete [] data[i];
    delete []data;
    delete []partition;
    delete []covered;
    delete []g_positive_covered;
    delete []g_negative_covered;
    delete []peso_positive;
    delete []peso_negative;
    delete []rule_used;
    delete []rule_aislada;
  }

  n_example=x.n_example;
  n_variable=x.n_variable;
  n_partition=x.n_partition;
  covered = new bool[n_example];
  g_positive_covered = new double [n_example];
  g_negative_covered = new double [n_example];
  peso_positive= new double [n_example];
  peso_negative= new double [n_example];
  rule_used = new int [n_example];
  rule_aislada = new int [n_example];
  partition= new int[n_example];
  data = new double*[n_example];
  for (int i=0; i<n_example; i++){
    data[i] = new double[n_variable];
    partition[i]=x.partition[i];
    covered[i]=x.covered[i];
    g_positive_covered[i]=x.g_positive_covered[i];
    g_negative_covered[i]=x.g_negative_covered[i];
    peso_positive[i]=x.peso_positive[i];
    peso_negative[i]=x.peso_negative[i];
    rule_used[i]=x.rule_used[i];
    rule_aislada[i]=x.rule_aislada[i];
  }

  for (int i=0; i<n_example; i++)
    for (int j=0; j<n_variable; j++)
      data[i][j]=x.data[i][j];

  return *this;
}


example_set::~example_set(){
  if (n_example>0 && n_variable>0 && data!=0){
    for (int i=0; i<n_example; i++)
      delete [] data[i];
    delete [] data;
    delete [] covered;
    delete []g_positive_covered;
    delete []g_negative_covered;
    delete []peso_positive;
    delete []peso_negative;
    delete []rule_used;
    delete []rule_aislada;
    delete [] partition;
  }

}


void example_set::AddExampleFich(const char *nom_fich, int par, bool order){

  fstream fich(nom_fich,ios::in);
  int new_example;
  int new_var;
  int old_n_example;

  if (!fich){
    cout << "El fichero "<< nom_fich << " no existe\n";
  }
  else {
    fich >> new_example;
    fich >> new_var;
    if (new_var!=n_variable && n_variable!=0){
      cout << "Different number of variables\n";
      exit(1);
    }

    n_variable=new_var;
    old_n_example = n_example;
    Realojar(new_example);

    int tam = n_example - old_n_example + 1;
    int vec[tam];

    if (order) { // Aleatorio completamente
	for (int i=old_n_example; i<n_example; i++){
	    vec[i-old_n_example] = i;
	    //cout << vec[i-old_n_example] << " ";
	}

	std::random_shuffle(vec, vec+tam-1);
    }

    for (int i=old_n_example; i<n_example; i++){
      data[i] = new double[n_variable];
      covered[i]=false;
      g_positive_covered[i]=0;
      g_negative_covered[i]=0;
      peso_positive[i]=0;
      peso_negative[i]=0;
      rule_used[i]=-1;
      rule_aislada[i]=0;
      partition[i]=par;
    }

    for (int i=old_n_example; i<n_example; i++) {
	//	cout << "A " << i-old_n_example << endl;
	//	cout << "B " << vec[i-old_n_example] << endl;
      for (int j=0; j<n_variable; j++)
	if (order) {
	        fich >> data[vec[i-old_n_example]][j];
        }
	else
	        fich >> data[i][j];
}
  }

  fich.close();
  n_partition++;
}

struct Local {
  Local(int p) {col = p;}
  bool operator()(const vector<double>& v1, const vector<double>& v2) {
    return v1[col] < v2[col];
  }

  int col;
};

void example_set::OrderByClass(int order, int VarClase) { // order=0 -> as it is, order=1 -> by class in order
  int old_n_clase = 0;
  vector< vector<double> > vect(n_example);

  switch (order) {
    case 1: { // Ordered by class (in order)
     cout << "Total 1: " << n_example << endl;
     for (int i = 0; i < n_example; i++) {
       vector<double> aux(n_variable);
       for (int j = 0; j < n_variable; j++) {
	 aux[j] = data[i][j];
       }
       vect[i] = aux;
     }

     sort(vect.begin(), vect.end(), Local(VarClase));

     n_clase.push_back(0);
     old_n_clase = 0;
     for (int i = 0; i < n_example; i++) {
       for (int j = 0; j < n_variable; j++) {
	 data[i][j] = vect[i][j];
	 if (j == VarClase) {
	   if (old_n_clase == data[i][j]) {
	     n_clase[data[i][j]]++;
	   }
	   else {
	     n_clase.push_back(1);
	     old_n_clase = data[i][j];
	   }
	 }
       }
     }

     cout << "Clases" << endl;
     for (int i = 0; i < n_clase.size(); i++)
       cout << n_clase[i] << endl;
     /*cout << "CARGA: " << VarClase << endl;
     for (int i = 0; i < n_example; i++) {
       for (int j = 0; j < n_variable; j++)
	 cout << data[i][j] << " ";
       cout << endl;
     }*/
     }
     break;
    case 2: { // Ordered by class (one at a time... 1 2 3)
      cout << "Total 2: " << n_example << endl;
      for (int i = 0; i < n_example; i++) {
	vector<double> aux(n_variable);
	for (int j = 0; j < n_variable; j++) {
	  aux[j] = data[i][j];
	}
	vect[i] = aux;
      }

      sort(vect.begin(), vect.end(), Local(VarClase));

      n_clase.push_back(0);
      old_n_clase = 0;
      for (int i = 0; i < n_example; i++) {
	for (int j = 0; j < n_variable; j++) {
	  data[i][j] = vect[i][j];
	  if (j == VarClase) {
	    if (old_n_clase == data[i][j]) {
	      n_clase[data[i][j]]++;
	    }
	    else {
	      n_clase.push_back(1);
	      old_n_clase = data[i][j];
	    }
	  }
	}
      }

      // Reorder
      vector<int> index(n_clase.size(), 0);
      int k = 0;
      for (int i = 0; i < n_example; i++) {
	for (int j = 0; j < n_variable; j++) {
	  if (k == 0)
	    data[i][j] = vect[index[k]][j];
	  else
	    data[i][j] = vect[index[k] + n_clase[k-1]][j];
	}
	index[k]++;
	do {
	  k++;
	  if (k == index.size())
	    k = 0;
	} while ((index[k] == n_clase[k]) && (i + 1 < n_example));
      }

      cout << "Clases" << endl;
      for (int i = 0; i < n_clase.size(); i++)
	cout << n_clase[i] << endl;
      /*cout << "CARGA: " << VarClase << endl;
      for (int i = 0; i < n_example; i++) {
	for (int j = 0; j < n_variable; j++)
	  cout << data[i][j] << " ";
	cout << endl;
      }*/
    }
    break;
  }
}

void example_set::AddExample(vectordouble w, int posicion){
  for (int i=0; i<n_variable; i++)
    data[posicion][i]=w.At(i);
}


double Aplicar_Operacion (double x1, double x2, int op){
 if (x1==-999999999 || x2==-999999999)
   return -999999999;
 switch (op){
             //Operacion OR
   case 19: if (x1 == x2)
            return 1;
           else
            return 0;
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

   /*case 5:
	      return (fabs(x1)*fabs(x2))/(fabs(x1)+fabs(x2)+1.0);
	   break;*/

   case 5: if (x2!=0)
             return x1/x2;
           else
              return -999999999;
           break;

   case 4: return x1-x2;
           break;

   case 3: return x1*x2;
           break;
   case 7: return sin(x1);
           break;
   case 8: return cos(x1);
           break;
   case 9: return log(fabs(x1)+1);
           break;
   case 10: return sin(x1)+cos(x1);
           break;
   case 11: return sqrt(fabs(x1));
           break;
   case 12: return 1.0/(fabs(x1)+1);
           break;
   case 13:
          break;
   case 15: return x1;
           break;
 }
 return 0;
}


void example_set::AddNewColumn(int col1, int col2, int op){

  double **m;

  double max, min;
  if (op==6 || op==13){
    min=data[0][col1];
    max=min;
    for (int i=1; i<n_example; i++){
        if (data[i][col1]>max)
	   max = data[i][col1];
	else if (data[i][col1]<min)
                    min = data[i][col1];
    }
  }

  m = new double *[n_example];
  for (int i=0; i<n_example; i++)
    m[i]=new double[n_variable+1];

  for (int i=0; i<n_example; i++){
    for (int j=0; j<n_variable-1; j++)
       m[i][j]=data[i][j];

    m[i][n_variable]=data[i][n_variable-1];
    if (op!=6 && op!=13)
      m[i][n_variable-1]=Aplicar_Operacion(data[i][col1],data[i][col2],op);
   else if (op==6)
                m[i][n_variable-1]=(data[i][col1]-min)/(max-min);
            else
                m[i][n_variable-1]=2*((data[i][col1]-min)/(max-min))-1;

  }

  for (int i=0; i<n_example; i++)
    delete [] data[i];
  delete [] data;

  data=m;
  n_variable++;
}



double example_set::Data(int ejemplo, int variable) const{
  return data[ejemplo][variable];
}


vectordouble example_set::Data(int ejemplo) const{
  vectordouble x;

  x.Put(data[ejemplo],n_variable);
  return x;
}



int example_set::N_Examples() const{
  return n_example;
}

int example_set::N_Partitions() const{
  return n_partition;
}


void example_set::Generate_Partitions(int num_partition){
  if (num_partition<1){
     cout << "Numero ilegal de particiones\n";
     exit(1);
  }


  n_partition=num_partition;

  if (n_partition==1){
    for (int i=0; i<n_example; i++)
      partition[i]=1;
  }
  else {
     for (int i=0; i<n_example; i++)
        partition[i]= (int) ((1.0*rand()*num_partition)/(RAND_MAX-1));
  }
}


bool example_set::Is_Training_Example(int ejemplo, int particion) const{
  return (partition[ejemplo]!=particion);
}


bool example_set::Is_Test_Example(int ejemplo, int particion) const{
  return (partition[ejemplo]==particion);
}


int example_set::Training_Example(int particion) const{
  int sum=0;
  for (int i=0; i<n_example; i++)
     if (partition[i]!=particion)
       sum++;

  return sum;
}


int example_set::Test_Example(int particion) const{
  int sum=0;
  for (int i=0; i<n_example; i++)
     if (partition[i]==particion)
       sum++;

  return sum;
}


int example_set::Not_Covered_Training_Example (int particion) const{
  int sum=0;
  for (int i=0; i<n_example; i++)
     if (partition[i]!=particion && !covered[i])
       sum++;

  return sum;
}




int example_set::Not_Covered_Examples() const{
  int numero=0;
  for (int i=0; i<n_example; i++){
    //cout << "--> " << i << "\t" << gcovered[i]<< endl;
    if (!covered[i])
      numero++;
  }

  return numero;
}


void example_set::Examples_per_Class(int VarClass, int particion, int nclasses, int *n_examples_in_class) const{
  for (int i=0; i<nclasses; i++)
    n_examples_in_class[i]=0;

// cout << "a:" <<endl;
//
// for (int i = 0; i< n_example; i++) {
// 	for (int i = 0; i< n_variable; i++)
// 		cout << i << ' ';
// 	cout << endl;
// }

  int k;
  for (int i=0; i<n_example; i++)
     if (/*partition[i]!=particion &&*/ (!covered[i])) {
       k = (int) (data[i][VarClass]);
       n_examples_in_class[k]++;
     }

}

int example_set::N_Variables() const{
  return n_variable;
}


void example_set::Remove(int *v, int tama){
  int n=n_example;

  for (int i=0; i<tama; i++)
    if (data[v[i]]!=0) {
      delete [] data[v[i]];
      data[v[i]]=0;
      n--;
    }

  int j=n;
  for (int i=0; i<n; i++) {
    if (data[i]==0){
      while (j<n_example && data[j]==0)
         j++;
      data[i]=data[j];
      data[j]=0;
    }
  }

  n_example=n;

}



void example_set::Mark(int *v, int tama){

  for (int i=0; i<tama; i++){
    covered[v[i]]=true;
    g_positive_covered[v[i]]=1;
  }
}


void example_set::Mark(int *v, int tama, double *grado){

  for (int i=0; i<tama; i++){
    covered[v[i]]=true;
    g_positive_covered[v[i]]=grado[i];
  }
}


void example_set::Mark(int *v, int tama, double *grado, double *peso){

  for (int i=0; i<tama; i++){
    covered[v[i]]=true;
    g_positive_covered[v[i]]=grado[i];
    peso_positive[v[i]]=peso[i];
  }
}



void example_set::UnMark(int *v, int tama){

  for (int i=0; i<tama; i++){
    covered[v[i]]=false;
    g_negative_covered[v[i]]=1;
  }
}



void example_set::UnMark(int *v, int tama, double *grado){

  for (int i=0; i<tama; i++){
    covered[v[i]]=false;
    g_negative_covered[v[i]]=grado[i];
  }
}



void example_set::UnMark(int *v, int tama, double *grado, double *peso){

  for (int i=0; i<tama; i++){
    covered[v[i]]=false;
    g_negative_covered[v[i]]=grado[i];
    peso_negative[v[i]]=peso[i];
  }

}


void example_set::Mark(int *v, int tama, double *grado_p, double *peso_p, double *grado_n, double *peso_n, int *rule, int *r_aislada){

  for (int i=0; i<tama; i++){
    if (v[i]==1)
      covered[i]=true;
    else
      covered[i]=false;

    g_positive_covered[i]=grado_p[i];
    peso_positive[i]=peso_p[i];
    g_negative_covered[i]=grado_n[i];
    peso_negative[i]=peso_n[i];
    rule_used[i]=rule[i];
    rule_aislada[i]=r_aislada[i];
  }
}



void example_set::UnMarkAll(){
  for (int i=0; i<n_example; i++) {
    covered[i]=false;
    g_positive_covered[i]=0;
    g_negative_covered[i]=0;
    peso_positive[i]=0;
    peso_negative[i]=0;
    rule_used[i]=-1;
    rule_aislada[i]=0;
  }
}


bool example_set::Is_Covered(int i) const{
  return covered[i];
}


double example_set::Grade_Is_Covered(int i) const{
  if (g_positive_covered[i]>g_negative_covered[i] || (g_positive_covered[i]==g_negative_covered[i] and peso_positive[i]>peso_negative[i]) )
    return g_positive_covered[i];
  else
    return -g_negative_covered[i];
}


int example_set::Class_of_example(int VarClase, int i) const {
 int k;

 k = (int) (data[i][VarClase]);
 return k;
}

double example_set::Grade_Is_Positive_Covered(int i) const{
    return g_positive_covered[i];
}

double example_set::Grade_Is_Negative_Covered(int i) const{
    return g_negative_covered[i];
}

double example_set::Peso_Is_Positive_Covered(int i) const{
    return peso_positive[i];
}

double example_set::Peso_Is_Negative_Covered(int i) const{
    return peso_negative[i];
}


int example_set::Rule_Used(int i) const{
    return rule_used[i];
}

int example_set::Rule_Aislada(int i) const{
    return rule_aislada[i];
}


/*example_set example_set::Extract_Training_Set(int particion, int order) const{
  int sum=0;
  for (int i=0; i<n_example; i++)
    if (partition[i]!=particion)
     sum++;

  cout << "ejemplos: " << sum << "  variables: " << n_variable << endl;
  example_set E1(sum,  n_variable);


  int l=0;
  for (int i=0; i<n_example; i++){
     if (partition[i]!=particion){
       for (int j=0; j<n_variable; j++)
         E1.data[l][j]=data[i][j];
       l++;
     }
  }

  return E1;
}*/

example_set example_set::Extract_Training_Set(int particion, int VarClase, int order) const {
  int sum=0;
  for (int i=0; i<n_example; i++)
    if (partition[i]!=particion)
      sum++;

  cout << "ejemplos: " << sum << "  variables: " << n_variable << endl;
  example_set E1 = example_set(sum,  n_variable);

  cout << "Training E" << endl;

  int l=0;
  for (int i=0; i<n_example; i++){
    if (partition[i]!=particion) {
      for (int j=0; j<n_variable; j++) {
	E1.data[l][j]=data[i][j];
	//cout << E1.data[l][j] << " ";
      }
      //cout << endl;
      l++;
    }
  }

  switch (order) {
    case 1: E1.OrderByClass(1, VarClase); break;
    case 2: E1.OrderByClass(2, VarClase); break;
    default: break;
  }


  /*cout << "Training E ordered" << endl;
  for (int i=0; i< E1.N_Examples(); i++) {
    for (int j=0; j<n_variable; j++)
      cout << E1.data[i][j] << " ";
    cout << endl;
  }
  cout << "end" << endl;*/

  return E1;
}

example_set example_set::Extract_Test_Set(int particion, int VarClase) const{
  int sum=0;
  for (int i=0; i<n_example; i++)
    if (partition[i]==particion)
      sum++;

  cout << "ejemplos: " << sum << "  variables: " << n_variable << endl;
  example_set E1 = example_set(sum,  n_variable);

  int l=0;
  for (int i=0; i<n_example; i++){
    if (partition[i]==particion){
      for (int j=0; j<n_variable; j++)
	E1.data[l][j]=data[i][j];
      l++;
    }
  }

  return E1;
}


example_set example_set::SubSet(int tama) const {
  example_set E1(tama, n_variable);
  for (int i=0; i<tama; i++)
    for (int j=0; j<n_variable; j++)
      E1.data[i][j]=data[i][j];

  return E1;
}


example_set example_set::SubSet(int ini, int fin) const {
  example_set E1(fin-ini+1, n_variable);
  for (int i=0; i<fin-ini+1; i++)
    for (int j=0; j<n_variable; j++)
      E1.data[i][j]=data[i+ini][j];

  return E1;
}



example_set example_set::Union(const example_set &A){
  example_set resultado(n_example+A.n_example,n_variable);


  for (int i=0; i<n_example; i++)
    for (int j=0; j<n_variable; j++)
      resultado.data[i][j]=data[i][j];

  for (int i=0; i<A.n_example; i++)
    for (int j=0; j<n_variable; j++)
      resultado.data[i+n_example][j]=A.data[i][j];

  return resultado;
}
