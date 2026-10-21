/***************************************************************************
                          genetcode.h  -  description
                             -------------------
    begin                : Tue Apr 30 2002
    copyright            : (C) 2002 by Raúl Pérez
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
#ifndef _GENETCODEH_
#define _GENETCODEH_
using namespace std;
#include <fstream>


class genetcode {
  private:
   int binary;
   int *nbinary;
   char **mbinary;
   int integer;
   int *ninteger;
   int **minteger;
   int real;
   int *nreal;
   double **mreal;
  public:
   genetcode();
   genetcode(const genetcode &x);
   ~genetcode();
   genetcode &operator=(const genetcode &x);
   void PutBinary(int bin, int*nbin, char **mbin);
   void PutInteger(int ent, int*nent, int **ment);
   void PutReal(int rea, int*nrea, double **mrea);
   void PutValueBinary(int fila, int columna, char value);
   void PutValueInteger(int fila, int columna, int value);
   void PutValueReal(int fila, int columna, double value);
   int TamaBinary(int fila) const;
   int TamaInteger(int fila) const;
   int TamaReal(int fila) const;
   void GetBinary(int &bin, int*&nbin, char **&mbin);
   void GetInteger(int &ent, int*&nent, int **&ment);
   void GetReal(int &rea, int*&nrea, double **&mrea);
   char GetValueBinary(int fila, int columna) const;
   int GetValueInteger(int fila, int columna) const;
   double GetValueReal(int fila, int columna) const;
   void SaveCode(ofstream &f) const;
   void LoadCode(ifstream &f);
};

#endif
