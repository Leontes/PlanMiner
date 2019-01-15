#ifndef DISCRETIZE_HPP
#define DISCRETIZE_HPP

#include <string>
#include <vector>
#include <map>


struct FuzzSet{
  double a;
  double b;
  double c;
  double d;

  std::string label;
};

struct funcPert{
  double A, B, C, D;

  funcPert(double a, double b, double c, double d){
    A = a;
    B = b;
    C = c;
    D = d;
  }
};

struct Clusters {
  double CHIndex;
  std::vector<double> clustersCentroids;
  std::vector< std::vector<double> > clustersElements;
  std::vector<funcPert> membershipClusters;
};

struct Punto {
    double x;
    double y;
};

enum orientacion {colinear, reloj, contrareloj};


std::map < std::string, std::vector <FuzzSet> > discretize(std::vector < std::vector < double > > * dataset, std::vector < std::pair<std::string, std::string> > * attribLabels);



#endif
