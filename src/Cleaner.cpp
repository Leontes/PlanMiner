#include "Cleaner.hpp"

#include <map>
#include <string>
#include <iomanip>
#include <sstream>


void cleanDataset(std::vector < std::vector < double > > * dataset, double threshold){
  std::vector < std::vector < double > > aux(*dataset);
  std::map < double, double > frecPre, frecPost;
  unsigned int valsPre = 0, valsPost = 0;
  std::string outPre, outPost;

  std::stringstream oss;


  outPre = "Conditional Probability Class PreState\n";
  outPost = "Conditional Probability Class PostState\n";

int a;
  for(unsigned int i = 0; i < aux.size()-1; i++){
    frecPre.clear();
    valsPre = 0;
    frecPost.clear();
    valsPost = 0;
    //Primer repaso para calcular las frecuencias
    for(unsigned int j = 0; j < aux[i].size(); j++){
      if(aux[i][j] != -999999999.0){
        if(aux[aux.size()-1][j] == 0.0){
          valsPre++;
          if (frecPre.count(aux[i][j])>0){
            frecPre[aux[i][j]] = (frecPre[aux[i][j]] + 1.0);
          }
          else{
            frecPre[aux[i][j]] = 1.0;
          }
        }
        else{
          valsPost++;
          if (frecPost.count(aux[i][j])>0){
            frecPost[aux[i][j]] = (frecPost[aux[i][j]] + 1.0);
          }
          else{
            frecPost[aux[i][j]] = 1.0;
          }
        }
      }
    }

    outPre += "\tPredicate" + std::to_string(i) + " frecuencies:\n";

    for(std::map < double, double >::iterator it = frecPre.begin(); it != frecPre.end(); it++){
      it->second = ((double)it->second/(double)valsPre)*100.0;

      outPre += "\t\tValue: ";
      oss.str("");
      oss << std::setprecision(8) << std::noshowpoint << it->first;
      outPre += oss.str();
      outPre += " ==> ";

      oss.str("");
      oss << std::setprecision(8) << std::noshowpoint << it->second;
      outPre += oss.str();
      outPre += "%\n";
    }

    outPost += "\tPredicate" + std::to_string(i) + " frecuencies:\n";
    for(std::map < double, double >::iterator it = frecPost.begin(); it != frecPost.end(); it++){
      it->second = ((double)it->second/(double)valsPost)*100.0;

      outPost += "\t\tValue: ";
      oss.str("");
      oss << std::setprecision(8) << std::noshowpoint << it->first;
      outPost += oss.str();
      outPost += " ==> ";

      oss.str("");
      oss << std::setprecision(8) << std::noshowpoint << it->second;
      outPost += oss.str();
      outPost += "%\n";
    }

    //Segundo repaso para eliminar el ruido

    for(unsigned int j = 0; j < aux[i].size(); j++){
      if(aux[i][j] != -999999999.0){
        if(aux[aux.size()-1][j] == 0.0){
          if(frecPre[aux[i][j]] < (threshold*100.0) and (frecPre.size() <= 2)){
            aux[i][j] = -999999999.0;
          }
        }
        else{
          if(frecPost[aux[i][j]] < (threshold*100.0) and (frecPost.size() <= 2)){
            aux[i][j] = -999999999.0;
          }
        }
      }
    }
  }

  std::cout << outPre << std::endl;
  std::cout << outPost << std::endl;

  bool delet = true;
  dataset -> clear();
  for (unsigned int i = 0; i < aux.size(); i++){
    dataset -> push_back(std::vector<double>());
  }

  for (unsigned int j = 0; j < aux[0].size(); j++) {
    delet = true;
    for (unsigned int i = 0; i < aux.size()-1; i++) {
      if(aux[i][j] != -999999999.0){
        delet = false;
      }
    }

    if(delet != true){
      for (unsigned int i = 0; i < aux.size(); i++) {
        (*dataset)[i].push_back(aux[i][j]);
      }
    }
  }
}
