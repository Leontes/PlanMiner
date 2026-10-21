#ifndef _FUZZY_TH_
#define _FUZZY_TH_
using namespace std;
#include <string>
#include <fstream>

class fuzzy_t {
   private:
     static constexpr double MISSING=-999999999;
     double a;
     double b;
     double c;
     double d;
     bool menosinfinito; // true = b es menos infinito
     bool masinfinito;   // true = c es m�s infinito
     string nombre;      // nombre de la etiqueta
   public:
     /**
     @brief Constructor por defecto de la clase fuzzy_t.

     Crea un objeto vacio de tipo fuzzy_t
     */
     fuzzy_t();


     /**
     @brief Constructor de la clase fuzzy_t que a�ade los puntos de definici�n de la
            etiqueta y el nombre de la misma.
     @param a (real) indica el extremo inferior del trapecio [Posibilidad(a)=0].
     @param b (real) indica el punto centro inferior del trapecio [Posibilidad(b)=1].
     @param c (real) indica el punto centro superior del trapecio [Posibilidad(c)=1].
     @param d (real) indica el extremo superior del trapecio [Posibilidad(d)=0].
     @param name (string) nombre de la etiqueta trapezoidal definida.

     Crea un objeto de tipo fuzzy_t, con definicion [a,b,c,d,"name"]
     */
     fuzzy_t(double a, double b, double c, double d, string name);


     /**
     @brief Constructor de la clase fuzzy_t que a�ade los puntos de definici�n de la
            etiqueta, el nombre de la misma y si el extremo superior o inferior de la misma
	    es infinito.
     @param a (real) indica el extremo inferior del trapecio [Posibilidad(a)=0].
     @param b (real) indica el punto centro inferior del trapecio [Posibilidad(b)=1].
     @param c (real) indica el punto centro superior del trapecio [Posibilidad(c)=1].
     @param d (real) indica el extremo superior del trapecio [Posibilidad(d)=0].
     @param name (string) nombre de la etiqueta trapezoidal definida.
     @param menos (bool) indica si el extremo inferior de la etiqueta es menos infinito.
            [-inf,-inf,c,d,"name"]
     @param mas (bool) indica si el extremo superior de la etiqueta es m�s infinito.
            [a,b,+inf,+inf,"name"]

     Crea un objeto de tipo fuzzy_t, con definicion [a,b,c,d,"name",menos,mas]
     */
     fuzzy_t(double a, double b, double c, double d, string name, bool menos,
 bool mas);

     /**
     @brief Constructor de cop�a de los objetos fuzzy_t.
     @param x (fuzzy_t) objeto sobre el que se realiza la cop�a.
     */
     fuzzy_t(const fuzzy_t &x);

     /**
     @brief Sobrecarga del operador de asignaci�n que permite realizar la asignaci�n entre
            dos objetos de tipo fuzzy_t.
     @param x (fuzzy_t) objeto del que se desea realizar la cop�a.
     @return la cop�a del objeto fuzzy_t.
     */
     fuzzy_t &operator=(const fuzzy_t &x);

     /**
     @brief Operador que define en un objeto de tipo fuzzy_t una etiqueta.
     @pre El objeto sobre el que se realiza la asignaci�n debe estar creado.
     @post El contenido anterior del objeto se elimina, para tomar los nuevos valores.
     @param a (real) indica el extremo inferior del trapecio [Posibilidad(a)=0].
     @param b (real) indica el punto centro inferior del trapecio [Posibilidad(b)=1].
     @param c (real) indica el punto centro superior del trapecio [Posibilidad(c)=1].
     @param d (real) indica el extremo superior del trapecio [Posibilidad(d)=0].
     @param name (string) nombre de la etiqueta trapezoidal definida.
     @param menos (bool) indica si el extremo inferior de la etiqueta es menos infinito.
            [-inf,-inf,c,d,"name"]
     @param mas (bool) indica si el extremo superior de la etiqueta es m�s infinito.
            [a,b,+inf,+inf,"name"]
     */
     void Asigna(double a, double b, double c, double d, string name,bool menos,
bool mas);


     /**
     @brief Operador que define en un objeto de tipo fuzzy_t una etiqueta asumiendo que
            los extremos de la etiqueta no pueden ser +inf y +inf.
     @pre El objeto sobre el que se realiza la asignaci�n debe estar creado.
     @post El contenido anterior del objeto se elimina, para tomar los nuevos valores.
     @param a (real) indica el extremo inferior del trapecio [Posibilidad(a)=0].
     @param b (real) indica el punto centro inferior del trapecio [Posibilidad(b)=1].
     @param c (real) indica el punto centro superior del trapecio [Posibilidad(c)=1].
     @param d (real) indica el extremo superior del trapecio [Posibilidad(d)=0].
     @param name (string) nombre de la etiqueta trapezoidal definida.
     */
     void Asigna(double a, double b, double c, double d, string name);

     /**
     @brief Dado un valor real x, devuelve el grado de adaptaci�n de dicho valor a la
            etiqueta definida en el objeto fuzzy_t al que se le aplica.
     @pre El objeto debe estar creado.
     @post Elimina el valor anterior del objeto y asigna el nuevo valor.
     @param x (real) valor sobre el que se desea conocer su grado de adaptaci�n.
     @return (real) indicando la Poss(x/L).

     Calcula la posibilidad de que el valor x pertenezca a la etiqueta definida en el
     objeto fuzzy_t.
     */
     double Adaptacion(double x) const;


     /**
     @brief Cambia el identificador de la etiqueta definida en el objetio fuzzy_t.
     @pre El objeto debe estar creado.
     @param name (string) es el nuevo nombre que se le quiere asignar a la etiqueta.
     */
     void Poner_Nombre(string name);

     /**
     @brief Saca por la salida est�ndar la descripci�n de la etiqueta definida en el
            objeto fuzzy_t.


     Saca por la salida est�ndar la descripci�n de la etiqueta definida en el
     objeto fuzzy_t en el formato [a,b,c,d,"name"]. Los puntos a,b,c,d definen el
     trapecio que representa la etiqueta pudiendo ser a y b (-inf) y c y d (+inf).
     */
     void Pinta() const;
     std::string PintaS() const;

     string SPinta() const;

     void Print() const;

     string SPrint() const;

     /**
     @brief Devuelve un objeto de tipo fuzzy_t.
     */
     fuzzy_t FuzzyLabel() const;

     /**
     @brief Devuelve el valor central del n�cleo de la etiqueta difusa.
     */
     double CenterLabel() const;


     /**
     @brief Devuelve si la etiqueta representa un valor discreto.
     */
     bool IsDiscrete() const;


     /**
     @brief Devuelve si la etiqueta representa un intervalo.
     */
     bool IsInterval() const;

     /**
     @brief Devuelve si la etiqueta representa una etiqueta
            difusa trapezoidal que no es un valor discreto ni
            un intervalo.
     */
     bool IsFuzzy() const;

     /**
     @brief Devuelve el area definida por la etiqueta difusa.
     */
     double Area() const;

     void GetDefinicion(double &a, double &b, double &c, double &d, string &nombre) const;

     void PutDefinicion(double a, double b, double c, double d, string nombre);

     void SaveBinaryCode(ofstream &f) const;

     void LoadBinaryCode(ifstream &f);


};

#endif
