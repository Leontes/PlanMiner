/***************************************************************************
                          genetcode.cpp  -  description
                             -------------------
    begin                : Tue Apr 30 2002
    copyright            : (C) 2002 by Ral P�ez
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

#include "INSLV/genetcode.h"

genetcode::genetcode(){
  binary=integer=real=0;
  nbinary=ninteger=nreal=0;
  mbinary=0;
  minteger=0;
  mreal=0;
}

genetcode::genetcode(const genetcode &x){
  binary=x.binary;
  nbinary=new int[binary];
  for (int i=0; i<binary; i++)
    nbinary[i]=x.nbinary[i];
  mbinary= new char*[binary];
  for (int i=0; i<binary; i++){
    mbinary[i]= new char[nbinary[i]];
    for (int j=0; j<nbinary[i]; j++)
      mbinary[i][j]=x.mbinary[i][j];
  }

  integer=x.integer;
  ninteger=new int[integer];
  for (int i=0; i<integer; i++)
    ninteger[i]=x.ninteger[i];
  minteger= new int*[integer];
  for (int i=0; i<integer; i++){
    minteger[i]= new int[ninteger[i]];
    for (int j=0; j<ninteger[i]; j++)
      minteger[i][j]=x.minteger[i][j];
  }

  real=x.real;
  nreal=new int[real];
  for (int i=0; i<real; i++)
    nreal[i]=x.nreal[i];
  mreal= new double*[real];
  for (int i=0; i<real; i++){
    mreal[i]= new double[nreal[i]];
    for (int j=0; j<nreal[i]; j++)
      mreal[i][j]=x.mreal[i][j];
  }

}


genetcode::~genetcode(){
  if (binary>0){
    for (int i=0; i<binary; i++)
      delete [] mbinary[i];
    delete [] mbinary;
    delete [] nbinary;
  }

  if (integer>0){
    for (int i=0; i<integer; i++)
      delete [] minteger[i];
    delete [] minteger;
    delete [] ninteger;
  }

  if (real>0){
    for (int i=0; i<real; i++)
      delete [] mreal[i];
    delete [] mreal;
    delete [] nreal;
  }

}


genetcode &genetcode::operator=(const genetcode &x){
  if (this!=&x){
    PutBinary(x.binary,x.nbinary,x.mbinary);
    PutInteger(x.integer,x.ninteger,x.minteger);
    PutReal(x.real,x.nreal,x.mreal);
  }
  return *this;
}

void genetcode::PutBinary(int bin, int*nbin, char **mbin){
  if (binary>0){
    for (int i=0; i<binary; i++)
      delete [] mbinary[i];
    delete [] mbinary;
    delete [] nbinary;
  }

  binary=bin;
  nbinary=new int[binary];
  for (int i=0; i<binary; i++)
    nbinary[i]=nbin[i];
  mbinary=new char *[binary];
  for (int i=0; i<binary; i++){
    mbinary[i]=new char[nbinary[i]];
    for (int j=0; j<nbinary[i]; j++)
      mbinary[i][j]=mbin[i][j];
  }

}

void genetcode::PutInteger(int ent, int*nent, int **ment){
  if (integer>0){
    for (int i=0; i<integer; i++)
      delete [] minteger[i];
    delete [] minteger;
    delete [] ninteger;
  }

  integer=ent;
  ninteger=new int[integer];
  for (int i=0; i<integer; i++)
    ninteger[i]=nent[i];
  minteger=new int*[integer];
  for (int i=0; i<integer; i++){
    minteger[i]=new int[ninteger[i]];
    for (int j=0; j<ninteger[i]; j++)
      minteger[i][j]=ment[i][j];
  }
}

void genetcode::PutReal(int rea, int*nrea, double **mrea){

  if (real>0){
    for (int i=0; i<real; i++)
      delete [] mreal[i];
    delete [] mreal;
    delete [] nreal;
  }

  real=rea;
  nreal=new int[real];
  for (int i=0; i<real; i++)
    nreal[i]=nrea[i];
  mreal=new double*[real];
  for (int i=0; i<real; i++){
    mreal[i]=new double[nreal[i]];
    for (int j=0; j<nreal[i]; j++)
      mreal[i][j]=mrea[i][j];
  }
}


void genetcode::PutValueBinary(int fila, int columna, char value){
  mbinary[fila][columna]=value;
}

void genetcode::PutValueInteger(int fila, int columna, int value){
  minteger[fila][columna]=value;
}

void genetcode::PutValueReal(int fila, int columna, double value){
  mreal[fila][columna]=value;
}


int genetcode::TamaBinary(int fila) const{
  return nbinary[fila];
}

int genetcode::TamaInteger(int fila) const{
  return ninteger[fila];
}

int genetcode::TamaReal(int fila) const{
  return nreal[fila];
}


void genetcode::GetBinary(int &bin, int*&nbin, char **&mbin){
  bin=binary;
  nbin=nbinary;
  mbin=mbinary;
}

void genetcode::GetInteger(int &ent, int*&nent, int **&ment) {
  ent=integer;
  nent=ninteger;
  ment=minteger;
}

void genetcode::GetReal(int &rea, int*&nrea, double **&mrea){
  rea=real;
  nrea=nreal;
  mrea=mreal;
}


char genetcode::GetValueBinary(int fila, int columna) const{
  return mbinary[fila][columna];
}


int genetcode::GetValueInteger(int fila, int columna) const{
  return minteger[fila][columna];
}

double genetcode::GetValueReal(int fila, int columna) const{
  return mreal[fila][columna];
}



void genetcode::SaveCode(ofstream &f) const{

    f.write(reinterpret_cast<const char *>(&binary),sizeof(int));
    for (int j=0; j<binary; j++){
      f.write(reinterpret_cast<const char *>(&(nbinary[j])),sizeof(int));
      f.write(reinterpret_cast<const char *>(mbinary[j]),sizeof(char)*nbinary[j]);
    }
    f.write(reinterpret_cast<const char *>(&integer),sizeof(int));
    for (int j=0; j<integer; j++){
      f.write(reinterpret_cast<const char *>(&(ninteger[j])),sizeof(int));
      f.write(reinterpret_cast<const char *> (minteger[j]),sizeof(int)*ninteger[j]);
    }
    f.write(reinterpret_cast<const char *>(&real),sizeof(int));
    for (int j=0; j<real; j++){
      f.write(reinterpret_cast<const char *>(&(nreal[j])),sizeof(int));
      f.write(reinterpret_cast<const char *>(mreal[j]),sizeof(double)*nreal[j]);
    }
}


void genetcode::LoadCode(ifstream &f){

    //Eliminar la informacion del objeto
  if (binary>0){
    for (int i=0; i<binary; i++)
      delete [] mbinary[i];
    delete [] mbinary;
    delete [] nbinary;
  }

  if (integer>0){
    for (int i=0; i<integer; i++)
      delete [] minteger[i];
    delete [] minteger;
    delete [] ninteger;
  }

  if (real>0){
    for (int i=0; i<real; i++)
      delete [] mreal[i];
    delete [] mreal;
    delete [] nreal;
  }

  // Crear el objeto a partir del fichero
    f.read(reinterpret_cast<char *>(&binary),sizeof(int));
    nbinary = new int[binary];
    mbinary = new char *[binary];
    for (int j=0; j<binary; j++){
      f.read(reinterpret_cast<char *>(&(nbinary[j])),sizeof(int));
      mbinary[j] = new char[nbinary[j]];
      f.read(reinterpret_cast<char *> (mbinary[j]),sizeof(char)*nbinary[j]);
    }
    f.read(reinterpret_cast<char *> (&integer),sizeof(int));
    ninteger = new int[integer];
    minteger = new int*[integer];
    for (int j=0; j<integer; j++){
      f.read(reinterpret_cast<char *>(&(ninteger[j])),sizeof(int));
      minteger[j] = new int[ninteger[j]];
      f.read(reinterpret_cast<char *> (minteger[j]),sizeof(int)*ninteger[j]);
    }
    f.read(reinterpret_cast<char *> (&real),sizeof(int));
    nreal = new int[real];
    mreal = new double *[real];
    for (int j=0; j<real; j++){
      f.read(reinterpret_cast<char *>(&(nreal[j])),sizeof(int));
      mreal[j]= new double[nreal[j]];
      f.read(reinterpret_cast<char *> (mreal[j]),sizeof(double)*nreal[j]);
    }

}
