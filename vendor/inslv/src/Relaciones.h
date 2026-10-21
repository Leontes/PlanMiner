/***************************************************************************
                          Relaciones.h  -  description
                             -------------------
    begin                : jue mar 20 2003
    copyright            : (C) 2003 by Raúl Pérez
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
#ifndef _RELACIONESH_
#define _RELACIONESH_
#include <fstream>
using namespace std;

class Relaciones {
  private:
    int Nrelaciones;
    int** Tabla;
  public:
    Relaciones(){ Nrelaciones=0; Tabla=0;}
    ~Relaciones();
    Relaciones (const char* nomfich);
    int Operator(int rel) const;
    int First(int rel) const;
    int Second(int rel) const;
    int NumRel() const;
};



#endif
