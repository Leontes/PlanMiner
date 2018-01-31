#ifndef RULECREATOR_HPP
#define RULECREATOR_HPP

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <queue>
#include "INSLV/domain_t.h"
#include "INSLV/VectorVar.h"
#include "INSLV/vectordouble.h"
#include "INSLV/example_set.h"
#include "INSLV/multipoblacion.h"
#include "INSLV/genetcode.h"
#include "INSLV/ruleset.h"
#include "INSLV/Relaciones.h"
#include "INSLV/operaciones.h"



double Weight_Individual(multipoblacion &G, const VectorVar &V, const example_set &E, VectorVar &F, double ***Tabla, double ***Tabla_Nuevas, int elemento_i,
			 double &n_mas, double &n_menos);

void ReajustarPesoaReglas(multipoblacion &G, const VectorVar &V, example_set &E, ruleset &R, VectorVar &F,
			  double ***Tabla, double ***Tabla_Nuevas);

int GA(multipoblacion &G, const VectorVar &V, example_set &E, ruleset &R, ruleset &R_dis, ruleset &R_mix, double **I, double *agregado,
       double *peso_agregado, int Total_ejemplos, bool discriminante, bool primeraVez, bool con_peso,
       int *ponderacion_por_clase, double ***Tabla, double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla,
       int *errores_para_regla, int *reglas_a_eliminar, int &n_reglas_a_eliminar, bool &Added_Rule, bool &Specific_Rule,
       int tam_rule_not_removed);

//void mGA(multipoblacion *G, int tam, const VectorVar &V, example_set &E);
void fitness (multipoblacion &G, const VectorVar &V, const example_set &E, ruleset &R, bool discriminante, bool con_peso,
              int *ponderacion_por_clase, int nrule, double ***Tabla, double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla,
	      int *errores_para_regla, int tam_rule_not_removed);

void Obtener_Regla_Discriminante(multipoblacion &G, const VectorVar &V, example_set &E, ruleset &R, int elemento_i,
                               bool discriminante, bool reducir_reglas, int* ponderacion_por_clase, double ***Tabla,
                               double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla,
			       int *errores_para_regla, int *reglas_a_eliminar, int &n_reglas_a_eliminar, bool debug,
			       string patr, bool inGA, string initial_rule, int tam_rule_not_removed);


void Obtener_Regla_Discriminante_E(multipoblacion &G, const VectorVar &V, example_set &E, ruleset &R, int elemento_i,
                               bool discriminante, bool reducir_reglas, int* ponderacion_por_clase, double ***Tabla,
                               double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla,
			       int *errores_para_regla, int *reglas_a_eliminar, int &n_reglas_a_eliminar, bool debug, string patr,
			       bool inGA, string initial_rule, double nmas, double nmenos);

string Patron1Regla(ruleset &R_1, const VectorVar &V, int pos);

string Patron2Patrones(string pat1, string pat2);

bool ComprobacionEspGen(string esp, string gen);


string Patron2Reglas(ruleset &R_1, ruleset &R_2, const VectorVar &V, int pos);

void Pinta2Reglas(ruleset &R_1, ruleset &R_2, const VectorVar &V, int pos);



void Obtener_Regla_Descriptiva(multipoblacion &G, const VectorVar &V, example_set &E, ruleset &R, int elemento_i,
                               bool discriminante, bool reducir_reglas, int* ponderacion_por_clase, double ***Tabla,
                               double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla,
			       int *errores_para_regla, int *reglas_a_eliminar, int &n_reglas_a_eliminar, bool debug,
			       string patr, bool inGA, string initial_rule, int tam_rule_not_removed);


void Obtener_Regla_Descriptiva_E(multipoblacion &G, const VectorVar &V, example_set &E, ruleset &R, int elemento_i,
                               bool discriminante, bool reducir_reglas, int* ponderacion_por_clase, double ***Tabla,
                               double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla,
			       int *errores_para_regla, int *reglas_a_eliminar, int &n_reglas_a_eliminar, bool debug, string patr,
			       bool inGA, string initial_rule, double nmas, double nmenos);


double Bondad (multipoblacion &G, const VectorVar &V, const example_set &E, ruleset &R, double *valor, int N_valor,
               int elemento_i, bool discriminante, bool con_peso, int *ponderacion_por_clase, int nrule,
               double ***Tabla, double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla, int *errores_para_regla, int *reglas_a_eliminar, int &n_reglas_a_eliminar,
	       bool esp_gen, string initial_rule, int tam_rule_not_removed);

double Bondad_E (multipoblacion &G, const VectorVar &V, const example_set &E, ruleset &R, double *valor, int N_valor,
               int elemento_i, bool discriminante, bool con_peso, int *ponderacion_por_clase, int nrule,
               double ***Tabla, double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla, int *errores_para_regla, int *reglas_a_eliminar, int &n_reglas_a_eliminar,
	       bool esp_gen, string initial_rule, double nmas, double nmenos);


double Bondad_Final (multipoblacion &G, const VectorVar &V, const example_set &E, ruleset &R, double *valor, int N_valor,
               int elemento_i, bool discriminante, bool con_peso, int *ponderacion_por_clase, int nrule,
               double ***Tabla, double ***Tabla_Nuevas, VectorVar &F, int *basica_para_regla, int *errores_para_regla, int *reglas_a_eliminar, int &n_reglas_a_eliminar);

example_set RetenerEjemplos(ruleset &R, const VectorVar &V, example_set &E, VectorVar &F);


double Aciertos(const ruleset &R, const VectorVar &V, example_set &E, double *agregado, double *peso_agregado,
                string nomfich, VectorVar &F);

double Aciertos(ruleset &R, const VectorVar &V, example_set &E, VectorVar &F);

double Aciertos(ruleset &R, const VectorVar &V, example_set &E, string nomfich, VectorVar &F);

double Filtrar_Reglas(ruleset &R, const VectorVar &V, example_set &E, VectorVar &F);
double Filtrar_Reglas(ruleset &R, const VectorVar &V, example_set &E, ruleset &R_Descrip, VectorVar &F);
double Filtrar_Reglas_New(ruleset &R, ruleset &R_dis, ruleset &R_mix, int *reglas_a_eliminar,
			  int n_reglas_a_eliminar, bool Specific_Rule);



double Inferencia2BasesReglas(ruleset &R, ruleset &R_Descrip, const VectorVar &V, example_set &E, int *fallosR, int *fallosR_Descrip, int *disparosR, int *disparosR_Descrip);

void Error(const ruleset &R, const example_set &E);

void PintaIndividuo(genetcode GC, const VectorVar &V);

double **ReservaParaMedidasDeInformacion(const VectorVar &V);
void LiberaMedidasDeInformacion(double **&matriz, int n);
void MedidasDeInformacion(const VectorVar &V, const example_set &E, double **I);

double CalcularInformacionCondicionada2variablesRespectoClase (const VectorVar &V, const example_set &E,
        int casos1, int casos2, int casosclase,
        double ***Conjunta123, double **Conjunta13,
        double **Conjunta23, double *simple3, double *I);


void CalcularProbabilidadConjunta3Variables(const VectorVar &V, const example_set &E,
        int var1, int var2, int var3, int n_casos1,
        int n_casos2, int n_casos3, double ***m);

string extract_noun(string file);

bool Existe(string nom_fich);

void Max_Min(const example_set &E, const VectorVar &V, int v1, int v2, int op, double &min, double &max);

void MedidasDeInformacion_operadores(VectorVar &V, example_set &E, VectorVar &F);
void MedidasDeInformacion_operadores_AjustandoDominio(VectorVar &V, example_set &E, VectorVar &F);

void Redefiniendo_Particiones_Variables (VectorVar &V, example_set &E);
void MedidasDeInformacion_operandoresUnarios(VectorVar &V, example_set &E, int nuevo,bool *bina);
void MedidasDeInformacion_operandores2(VectorVar &V, example_set &E, int nuevos, bool *bina);
double CalcularInformacion (int tamaX, int tamaY, double *px, double *py, double **pxy);
double CalcularInformacion (int tamaX, int tamaY, double *px, double *py, double **pxy, double *I);
string Simbolo_Operador(int op);
void MedidasDeInformacion_2variables_cond_consecuente(VectorVar &V, example_set &E);

bool Terminado_Aprendizaje(multipoblacion &G, example_set &E, bool &ultimo_modo);
void Obtener_Aciertos_y_Errores_Reglas_Aprendidas(example_set &E, int *basica_para_regla, int *errores_para_regla, int nrule);

double Aciertos_Lista_Reglas_Eliminables(ruleset &R, const VectorVar &V, example_set &E, VectorVar &F, int*lista, int &tama, int *l_aciertos, int &tama_l);




//---- Funciones para manejar la tabla con las adaptaciones de los ejemplos a las etiquetas de las variables
double ***Crear_Tabla_Adaptaciones(const VectorVar &V, const example_set &E);
double ***Crear_Tabla_Adaptaciones_Nuevas(const VectorVar &V, const example_set &E);
void Destruir_Tabla_Adaptaciones(const VectorVar &V, const example_set &E, double ***&Tabla);
double T_Adaptacion_Variable_Etiqueta(const VectorVar &V, double ***Tabla, int ejemplo,
                                      int variable, int etiqueta);
double T_Adaptacion_Variable(const VectorVar &V, double ***Tabla, int ejemplo, int variable, string labels);
double T_Adaptacion_Antecedente(const VectorVar &V, double ***Tabla, int ejemplo, string regla,
                                double *nivel_variable, double umbral);
double T_Adaptacion_Antecedente_Nuevas(const VectorVar &V, double ***Tabla, int ejemplo, string regla,
                                       int *nivel_variable_funciones, int tama_nivel_variable_funciones, double matching_previo);
void T_AdaptacionC(const VectorVar &V, double ***Tabla, int ejemplo, int etiq, double &pos, double &neg);
int BetterEtiqueta(const VectorVar &V,double ***Tabla, int variable, int ejemplo);



double Evento1_Especificar(multipoblacion &G, const VectorVar &V, example_set &E_Par, ruleset &R, ruleset &R_dis, ruleset &R_mix, int *ponderacion_por_clase, VectorVar &F,
       double ***Tabla, double ***Tabla_Nuevas, int regla, double *multi_valor);

double Evento1_Generalizar(multipoblacion &G, const VectorVar &V, example_set &E_Par, ruleset &R, ruleset &R_dis, ruleset &R_mix, int *ponderacion_por_clase, VectorVar &F,
       double ***Tabla, double ***Tabla_Nuevas, int regla, double *multi_valor);

void PintaDiferenciaPatrones(const string &cad1, const string &cad2);
int Comparar2patrones(const string &cad1, const string &cad2);
int Comparar2patronesPinta(const string &cad1, const string &cad2);
bool AnalizarConjuntosDeReglas(const VectorVar &V, ruleset &R, ruleset &R_Descrip, ruleset &R_mix, int nuevas);
void UnionReglasDescriptivas(ruleset &R1, int r1, ruleset &R2, int r2);
void IntersecionReglasDiscriminantes(ruleset &R1, int r1, ruleset &R2, int r2);

string Interseccion(const string &cad1, const string &cad2);
string Union(const string &cad1, const string &cad2);
int Grados_Libertad(string cad);
int DistanciaHamming(const string &s1, const string &s2);
int DiferenciaDistanciaHamming(const string &s1, const string &s2, int pos_inicial);


void ActualizarPeso(multipoblacion &G, const VectorVar &V, example_set &E, ruleset &R, VectorVar &F,
       double ***Tabla, double ***Tabla_Nuevas);


bool vectoresIguales_E(double *v1, double *v2, int tama);
bool vectoresIguales(double *v1, double *v2, int tama);



int inslv(std::string pDomainName, std::string task, std::string sPath, std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels, vector < pair <int, list< pair<string, string> > > > * reglas);



#endif
