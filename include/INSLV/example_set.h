#ifndef _EXAMPLE_SETH_
#define _EXAMPLE_SETH_
#include "vectordouble.h"
#include <vector>
#include <algorithm>

class example_set {
  private:
    static constexpr double MISSING = -999999999;
    int n_example;
    int n_variable;
    double **data;
    bool *covered;
    double *g_positive_covered;
    double *peso_positive;
    double *g_negative_covered;
    double *peso_negative;
    int *rule_used;
    int *rule_aislada;
    int n_partition;
    int *partition;
    void Realojar(int new_examples);
    std::vector<int> n_clase;
  public:
    example_set();
    example_set(int ejemplos, int variables);
    example_set(const example_set &x);
    example_set(const char *nom_fich);
    example_set(std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels);
    example_set &operator=(const example_set &x);
    ~example_set();
    void AddExampleFich(const char *nom_fich, int par, bool order = false); // order=F -> as it is, order=T -> random
    void AddNewColumn(int col1, int col2, int op);
    double Data(int ejemplo, int variable) const;
    vectordouble Data(int ejemplo) const;
    void AddExample(vectordouble w, int posicion);
    int N_Examples() const;
    int N_Partitions() const;
    void Generate_Partitions(int num_partition);
    bool Is_Training_Example(int ejemplo, int particion) const;
    bool Is_Test_Example(int ejemplo, int particion) const;
    int Training_Example(int particion) const;
    int Test_Example(int particion) const;
    int Not_Covered_Training_Example (int particion) const;
    int Not_Covered_Examples() const;
    void Examples_per_Class(int VarClass,int particion, int nclasses, int *n_example_in_class) const;
    int N_Variables() const;
    void Remove(int *v, int tama);
    void Mark(int *v, int tama);
    void UnMark(int *v, int tama);
    void Mark(int *v, int tama, double *grado);
    void Mark(int *v, int tama, double *grado, double *peso);
    void Mark(int *v, int tama, double *grado_p, double *peso_p, double *grado_n, double *peso_n, int *rule, int *r_aislada);


    void UnMark(int *v, int tama, double *grado);
    void UnMark(int *v, int tama, double *grado, double *peso);
    void UnMarkAll();
    bool Is_Covered(int i) const;
    double Grade_Is_Covered (int i) const;
    double Grade_Is_Positive_Covered(int i) const;
    double Grade_Is_Negative_Covered(int i) const;
    double Peso_Is_Positive_Covered(int i) const;
    double Peso_Is_Negative_Covered(int i) const;
    int Class_of_example(int VarClase, int i) const;

    int Rule_Used(int i) const;
    int Rule_Aislada(int i) const;

    example_set Extract_Training_Set(int particion, int VarClase, int order = 0) const;
    example_set Extract_Test_Set(int particion, int VarClase) const;

    example_set SubSet(int tama) const;
    example_set SubSet(int ini,int fin) const;

    example_set Union(const example_set &A);

    void OrderByClass(int order, int VarClase = 0);
};


#endif
