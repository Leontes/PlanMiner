#ifndef DSUTILS_HPP
#define DSUTILS_HPP

#include <string>
#include <vector>
#include <map>


/*!
  Struct to code fuzzy sets
*/
struct FuzzSet{
  double a;
  double b;
  double c;
  double d;

  std::string label;
};

/*!
  Struct to code membership function
*/
struct funcPert{
  double A, B, C, D;

  funcPert(double a, double b, double c, double d){
    A = a;
    B = b;
    C = c;
    D = d;
  }
};

/*!
  Struct to code kmeans clusters
*/
struct Clusters {
  double CHIndex;
  std::vector<double> clustersCentroids;
  std::vector< std::vector<double> > clustersElements;
  std::vector<funcPert> membershipClusters;
};

/*!
  Struct to code a point
*/
struct Punto {
    double x;
    double y;
};

enum orientacion {colinear, reloj, contrareloj};



/** Method discretize
*   @brief Method to discretize the attributes of a given dataset.
*   @param  dataset Output pair of STD matrices of double values
*   @param  attribLabels Output pair of STD vector with the labels of the datasets' columns
*   @retval STD map with every attribute and its discrete sets calculated
*/
std::map < std::string, std::vector <FuzzSet> > discretize(std::vector < std::vector < double > > * dataset, std::vector < std::pair<std::string, std::string> > * attribLabels);

/** Method cleanDataset
*   @brief Method to clean a dataset from noise and incompleteness
*   @param  dataset Output pair of STD matrices of double values
*   @param  threshold double to discern between noise and infrecuent values
*/
void cleanDataset(std::vector < std::vector < double > > * dataset, double threshold = 0.01);

#endif
