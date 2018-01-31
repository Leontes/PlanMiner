/***************************************************************************
                          Relaciones.cpp  -  description
                             -------------------
    begin                : jue mar 20 2003
    copyright            : (C) 2003 by Ra�l P�rez
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
#include <fstream>
#include <cstdlib>
#include "INSLV/Relaciones.h"

Relaciones::~Relaciones(){
  if (Nrelaciones>0){
    for (int i=0; i<Nrelaciones; i++)
      delete [] Tabla[i];
    delete [] Tabla;
  }
}


Relaciones::Relaciones(const char* nomfich){

  // Leer el fichero y reservar memoria
  ifstream f(nomfich);
  if (!f){
    cout << "El fichero de relaciones no existe\n";
    exit(1);
  }
  f >> Nrelaciones; //Leo el numero de relaciones
  Nrelaciones++;
  Tabla = new int*[Nrelaciones]; //Reservo memoria
  Tabla [0] = new int[3];
  for (int i=1; i<Nrelaciones; i++){
    Tabla[i] = new int[3];
    for (int j=0; j<3; j++)
      f >> Tabla[i][j];
  }
  f.close();
}


int Relaciones::Operator(int rel) const {
   if (rel<Nrelaciones){
      return Tabla[rel][0];
   }
   else {
      cout << "No existe esa relacion\n";
      return -1;
   }
}


int Relaciones::First(int rel) const {
   if (rel<Nrelaciones){
      return Tabla[rel][1];
   }
   else {
      cout << "No existe esa relacion\n";
      return -1;
   }
}


int Relaciones::Second(int rel) const {
   if (rel<Nrelaciones){
      return Tabla[rel][2];
   }
   else {
      cout << "No existe esa relacion\n";
      return -1;
   }
}


int Relaciones::NumRel() const {
  return Nrelaciones;
}
