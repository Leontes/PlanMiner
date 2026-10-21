#ifndef _VECTORDOUBLEH_
#define _VECTORDOUBLEH_

class vectordouble{
  private:
    static constexpr double MISSING=-999999999;
    int reservado;
    int numero;
    double *data;
  public:
    vectordouble();
    vectordouble(int tamano);
    vectordouble(double *x, int tamano);
    vectordouble(const vectordouble &x);
    vectordouble &operator=(const vectordouble &x);
    ~vectordouble();
    void Realloc();
    void Append(double x);
    void Put(double x, int pos);
    void Put(double *x, int tamano);
    double *Convert(int &tamano) const;
    double At(int pos) const;
    void Pinta() const;
    double Max() const;
    double Min() const;
    int find(double x) const;
    int PosMax() const;
    int PosMin() const;
    double Average() const;
    void Sort();
};

#endif
