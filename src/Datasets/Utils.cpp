#include "Datasets/Utils.hpp"

#include <cstdlib>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>

//Funciones auxiliares
bool enlaLinea(Punto p, Punto q, Punto r){
  if (q.x <= std::max(p.x, r.x) and q.x >= std::min(p.x, r.x) and q.y <= std::max(p.y, r.y) and q.y >= std::min(p.y, r.y)){
    return true;
  }else{
    return false;
  }
}
orientacion getOrientacion(Punto p, Punto q, Punto r){
  int val = ((q.y - p.y) * (r.x - q.x)) - ((q.x - p.x) * (r.y - q.y));

  if (val == 0){
    return colinear;
  }
  else{
    if(val > 0){
      return reloj;
    }
    else{
      return contrareloj;
    }
  }
}
bool intersectan(Punto p1, Punto q1, Punto p2, Punto q2){
  orientacion o1 = getOrientacion(p1, q1, p2);
  orientacion o2 = getOrientacion(p1, q1, q2);
  orientacion o3 = getOrientacion(p2, q2, p1);
  orientacion o4 = getOrientacion(p2, q2, q1);

  if (o1 != o2 && o3 != o4){
    return true;
  }

  if (o1 == 0 && enlaLinea(p1, p2, q1)){
    return true;
  }
  if (o2 == 0 && enlaLinea(p1, q2, q1)){
    return true;
  }

  if (o3 == 0 && enlaLinea(p2, p1, q2)){
    return true;
  }

  if (o4 == 0 && enlaLinea(p2, q1, q2)){
    return true;
  }

  return false;
}
bool distintos(Punto p, Punto q){
  return ((p.x != q.x) or (p.y != q.y));
}
double distancia(double A, double B){
  return abs(A - B);
}
bool parar(std::vector<double> centroidesNuevos, std::vector<double> centroidesViejos){
  for(unsigned int i = 0; i < centroidesNuevos.size(); i++){
    if(centroidesNuevos[i] != centroidesViejos[i]){
      return true;
    }
  }

  return false;
}


//Funciones para kmedias
//Calculo del indice CH
double CH(std::vector<double> centroides, std::vector< std::vector<double> > elementosCluster, double datasetCentroid){
  double outCH = 0;
  double diff, root;
  unsigned int N = 0;

  //Calculamos SSW
  //Tomamos un cluster
  double SSW = 0.0;

  for(size_t k = 0; k < elementosCluster.size(); k++){
    //Tomamos cada elemento del cluster
    for(unsigned int elem = 0; elem < elementosCluster[k].size(); elem++){
      //Calculamos la norma L2 de la diferencia entre el elemento del cluster y el centroide del cluster.Y elevamos al cuadrado
      diff = elementosCluster[k][elem] - centroides[k];
      diff = std::pow(diff,2);
      //Sumamos la diferencia a la varianza media inter-cluster
      SSW += diff;
    }
    N += elementosCluster[k].size();
  }

  //Calculamos SSW
  //Tomamos un cluster
  double SSB = 0;
  for(size_t k = 0; k < elementosCluster.size(); k++){
    //Calculamos la norma L2 de la diferencia entre el centroide del cluster y el centroide del dataset.Y elevamos al cuadrado
    diff = centroides[k] - datasetCentroid;
    diff = std::pow(diff,2);

    //Ponderamos la diferencia por el numero de elementos en el cluster
    SSB = elementosCluster[k].size() * diff;
  }
  if(SSW == 0.0){
    SSW = 1.0;
  }

  outCH = SSB/SSW;
  if(centroides.size() > 1){
    outCH *= (N - centroides.size())/(centroides.size() - 1);
  }else{
    outCH *= (N - centroides.size());
  }

  //std::cout << "SSW: " << SSW << " N: " << N << " SSB: " << SSB << " SSB/SSW: " << SSB/SSW << " centroides.size(): " << centroides.size() << " outCH " << outCH << std::endl;

  return outCH;
}

std::vector<double> SI(std::vector<double> centroides, std::vector< std::vector<double> > elementosCluster, double datasetCentroid){
  std::vector<double> SIvect;
  double a = 0.0, b = 0.0, SIaux = 0.0, SIClust = 0.0, aux = 0.0;
  for(unsigned int i = 0; i < elementosCluster.size(); i++){
    if(elementosCluster[i].size() < 2){
      SIClust = 0.0;
      SIvect.push_back(SIClust);
    }
    else{
      SIClust = 0.0;
      for(unsigned int j = 0; j < elementosCluster[i].size(); j++){
        a = 0.0;
        //a
        for(unsigned int k = 0; k < elementosCluster[i].size(); k++){
          if(j != k){
            a+= pow(elementosCluster[i][j] - elementosCluster[i][k], 2);
          }
         }
         a /= (elementosCluster[i].size() - 1.0);
         //b
         b = 999999999.9;
         for(unsigned int k = 0; k < elementosCluster.size(); k++){
           if(i != k and elementosCluster[k].size() > 0){
             aux = 0.0;
             for(unsigned int l = 0; l < elementosCluster[k].size(); l++){
               aux +=  pow(elementosCluster[i][j] - elementosCluster[k][l], 2);
             }
             aux /= elementosCluster[k].size();
             if (aux < b){
               b = aux;
             }
           }
         }

         if(b != 0){
           SIaux = b - a;
           if (b > a){
             SIaux /= b;
           }
           else{
             SIaux /= a;
           }
         }
         else{
           SIaux = -1.0;
         }
         SIClust += SIaux;
       }

       SIClust /=  elementosCluster[i].size();
       SIvect.push_back(SIClust);
     }
   }
   return SIvect;
 }

//Algoritmo de kmedias
Clusters kmeans(unsigned int k, std::vector<double> numeros){
  std::vector<double> centroidesNuevos(k, 0.0), centroidesViejos(k, 0.0);
  //std::srand(unsigned(clock()));

  unsigned int indice = 0, tamParticion = numeros.size()/k;

  double datasetMed = 0;
  for(unsigned int i = 0; i < numeros.size(); i++){
    datasetMed += numeros[i];
  }
  datasetMed /= numeros.size();

  for (size_t i = 0; i < centroidesNuevos.size(); i++) {
    //indice = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/numeros.size()));
    indice = (tamParticion*i) + (tamParticion/2);

    centroidesNuevos[i] = numeros[indice];
  }

  std::vector< std::vector<double> > elementosCluster(k);

  double distanciaMin = std::numeric_limits<double>::max(), dist = 0.0;
  unsigned int cluster = 0;

  do{

    for(size_t i = 0; i < elementosCluster.size(); i++){
      elementosCluster[i].clear();
    }

    centroidesViejos = centroidesNuevos;
    for (size_t i = 0; i < numeros.size(); i++) {
      distanciaMin = std::numeric_limits<double>::max();
      dist = 0.0;
      cluster = 0;

      for (size_t k = 0; k < centroidesNuevos.size(); k++) {
        dist = std::pow(numeros[i] - centroidesNuevos[k], 2);
        if(dist < distanciaMin){
          cluster = k;
          distanciaMin = dist;
        }
      }
      elementosCluster[cluster].push_back(numeros[i]);
    }

    for (size_t i = 0; i < elementosCluster.size(); i++){
      centroidesNuevos[i] = 0;
      for (size_t j = 0; j < elementosCluster[i].size(); j++){
        centroidesNuevos[i] += elementosCluster[i][j];
      }
      if(elementosCluster[i].size() > 0){
        centroidesNuevos[i] /= elementosCluster[i].size();
      }
      else{
        indice = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/numeros.size()));
        centroidesNuevos[i] = numeros[indice];
      }
    }
  }while(parar(centroidesNuevos, centroidesViejos));

  Clusters out;

  out.clustersElements = elementosCluster;
  out.clustersCentroids = centroidesNuevos;
  out.CHIndex = CH(centroidesNuevos, elementosCluster, datasetMed);

  return out;
}
//Funcion de seleccion del mejor cluster -> Lanza K-medias 10 veces con un K dado y selecciona el mejor de las 10 runs
Clusters mejorCluster(unsigned int k, std::vector<double> numeros){
  Clusters mejor = kmeans(k, numeros), otro;
  for (unsigned int i = 0; i < 9; i++) {
    otro = kmeans(k, numeros);
    if(otro.CHIndex > mejor.CHIndex){
      mejor = otro;
    }
  }

  return mejor;
}

bool contiene(std::vector <double> vect, double val){
  for(unsigned int i = 0; i < vect.size(); i++) {
    if(val == vect[i]){
      return true;
    }
  }
  return false;
}

std::map < std::string, std::vector <FuzzSet> > labeler(std::vector < std::vector < double > > * dataset, std::vector < std::pair<std::string, std::string> > * attribLabels){

  std::map < std::string, std::vector <FuzzSet> > out;
  std::vector <FuzzSet> vectAux;
  std::vector <double> datosAux;
  Clusters clAux;
  bool posFlag, negFlag;

  for (unsigned int i = 0; i < attribLabels -> size(); i++) {
    if((*attribLabels)[i].second  == "Logical"){
      vectAux.clear();
      vectAux.push_back(FuzzSet());
      vectAux[0].a = 0.0;
      vectAux[0].b = 0.0;
      vectAux[0].c = 0.0;
      vectAux[0].d = 0.0;
      vectAux[0].label = "False";

      vectAux.push_back(FuzzSet());
      vectAux[1].a = 1.0;
      vectAux[1].b = 1.0;
      vectAux[1].c = 1.0;
      vectAux[1].d = 1.0;
      vectAux[1].label = "True";

      out[(*attribLabels)[i].first] = vectAux;
    }
    else{
      if((*attribLabels)[i].second  == "Delta"){
        vectAux.clear();
        vectAux.push_back(FuzzSet());
        vectAux[0].a = 0.0;
        vectAux[0].b = 0.0;
        vectAux[0].c = 0.0;
        vectAux[0].d = 0.0;
        vectAux[0].label = "NA";

        posFlag = false;
        negFlag = false;
        for(unsigned int j = 0; j <(*dataset)[i].size(); j++){
          //std::cout << (*dataset)[i][j] << std::endl;
          if((*dataset)[i][j] == 1.0){
            posFlag = true;
          }
          if((*dataset)[i][j] == -1.0){
            negFlag = true;
          }
        }

        if(posFlag == true){
          vectAux.push_back(FuzzSet());
          vectAux[vectAux.size()-1].a = 1.0;
          vectAux[vectAux.size()-1].b = 1.0;
          vectAux[vectAux.size()-1].c = 1.0;
          vectAux[vectAux.size()-1].d = 1.0;
          vectAux[vectAux.size()-1].label = "POS";
        }

        if(negFlag == true){
          vectAux.push_back(FuzzSet());
          vectAux[vectAux.size()-1].a = -1.0;
          vectAux[vectAux.size()-1].b = -1.0;
          vectAux[vectAux.size()-1].c = -1.0;
          vectAux[vectAux.size()-1].d = -1.0;
          vectAux[vectAux.size()-1].label = "NEG";
        }

        out[(*attribLabels)[i].first] = vectAux;
      }
      else{
        if((*attribLabels)[i].second  == "Class"){
          vectAux.clear();
          vectAux.push_back(FuzzSet());
          vectAux[0].a = 0.0;
          vectAux[0].b = 0.0;
          vectAux[0].c = 0.0;
          vectAux[0].d = 0.0;
          vectAux[0].label = "PreState";

          vectAux.push_back(FuzzSet());
          vectAux[1].a = 1.0;
          vectAux[1].b = 1.0;
          vectAux[1].c = 1.0;
          vectAux[1].d = 1.0;
          vectAux[1].label = "PostState";

          out[(*attribLabels)[i].first] = vectAux;
        }
        else{
          datosAux.clear();

          for(unsigned int j = 0; j <(*dataset)[i].size(); j++){
            if(contiene(datosAux,(*dataset)[i][j]) == false){
              datosAux.push_back((*dataset)[i][j]);
            }
          }


          vectAux.clear();

          for(unsigned int j = 0; j < datosAux.size(); j++){
            vectAux.push_back(FuzzSet());
            vectAux[vectAux.size()- 1].a = datosAux[j];
            vectAux[vectAux.size()- 1].b = datosAux[j];
            vectAux[vectAux.size()- 1].c = datosAux[j];
            vectAux[vectAux.size()- 1].d = datosAux[j];
            vectAux[vectAux.size()- 1].label = "L_" + std::to_string(j);
          }

          out[(*attribLabels)[i].first] = vectAux;

        }
      }
    }
  }

  return out;

}
void lowFreq(std::vector < std::vector < double > > * dataset, double threshold){
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
      if(aux[i][j] == 0 or aux[i][j] == 1){
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

//Hiearchycal Recursive Clustering
Clusters HRC (std::vector<double> numeros, std::string nivel){
    double umbral = 0.05, alpha = 0.5, beta = 0.5;
    Clusters otro, mejorL;
    mejorL = kmeans(2, numeros);
    for (unsigned int i = 0; i < 4; i++) {
        otro = kmeans(2, numeros);
        if(otro.CHIndex > mejorL.CHIndex){
            mejorL = otro;
        }
    }
    double med = 0.0, varianza = 0.0;

    Clusters nuevosClust, outClusts;
    for(unsigned int i = 0; i < mejorL.SIndexClusters.size(); i++){
        std::cout << "\n\n-----------------------\nCluster " << nivel << "." << i << std::endl;
        med = 0.0;
        for (size_t j = 0; j < mejorL.clustersElements[i].size(); j++) {
          med += mejorL.clustersElements[i][j];
        }
        med /= mejorL.clustersElements[i].size();
        std::cout << "\tMedia del dataset antes de dividirlo: "<< med << std::endl;

        varianza = 0.0;
        for (size_t j = 0; j < mejorL.clustersElements[i].size(); j++) {
          varianza += (med - mejorL.clustersElements[i][j]) * (med - mejorL.clustersElements[i][j]);
        }

        std::cout << "\tVarianza del cluster: "<< varianza << std::endl;
        varianza = std::sqrt(varianza);
        std::cout << "\tDesviacion estandar del cluster: "<< varianza << std::endl;
        varianza  /= numeros.size();
        std::cout << "\tDesviacion estandar normalizada del cluster: "<< varianza << std::endl;
        std::cout << "\tIndice de silueta del cluster: " << mejorL.SIndexClusters[i] << std::endl;

        //Ajustar el indice de silueta
        double silAjustado = -1.0 * mejorL.SIndexClusters[i];
        silAjustado += 1.0;
        std::cout << "\tIndice de silueta ajustado del cluster: " << silAjustado << std::endl;
        double evalFunct = alpha * silAjustado + beta * varianza;
        std::cout << "\tValor de evaluación del cluster: " << evalFunct << "\n-----------------------" << std::endl;

        //Caso Recursivo
        if(evalFunct > umbral){
            if(mejorL.clustersElements[i].size() > 1){
                nuevosClust = HRC(mejorL.clustersElements[i], nivel + "." + std::to_string(i));
                for(unsigned int j = 0; j < nuevosClust.SIndexClusters.size(); j++){
                    outClusts.SIndexClusters.push_back(nuevosClust.SIndexClusters[j]);
                    outClusts.clustersCentroids.push_back(nuevosClust.clustersCentroids[j]);
                    outClusts.clustersElements.push_back(nuevosClust.clustersElements[j]);
                }
            }
        }

        //Caso Base
        else{
              outClusts.SIndexClusters.push_back(mejorL.SIndexClusters[i]);
              outClusts.clustersCentroids.push_back(mejorL.clustersCentroids[i]);
              outClusts.clustersElements.push_back(mejorL.clustersElements[i]);
        }
    }

    //Recalculamos CH y SI de los clusters.
    outClusts.CHIndex = CH(outClusts.clustersCentroids, outClusts.clustersElements, med);
    outClusts.SIndexClusters = SI(outClusts.clustersCentroids, outClusts.clustersElements, med);
    outClusts.SIndex = 0;
    for(unsigned int i = 0; i < outClusts.SIndexClusters.size(); i++){
    outClusts.SIndex += outClusts.SIndexClusters[i];
    }
    outClusts.SIndex /= outClusts.SIndexClusters.size();


    return outClusts;
}


void discretizar(std::vector < std::vector < double > > * dataset){
  Clusters mejor;
  std::vector<double> numeros;
  for (unsigned int i = 0; i < dataset -> size(); i++){
    numeros = (*dataset)[i];
    std::cout << "\n************************************************\n";
    mejor = HRC(numeros, "X");
    std::cout << "\n\n#######################\nIndice CH: " << mejor.CHIndex << std::endl;
    std::cout << "Indice Silueta: " << mejor.SIndex << std::endl;

    std::cout << "\n\n\n----------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Clusters seleccionados:" << std::endl;
    for (size_t i = 0; i < mejor.clustersCentroids.size(); i++){
      std::cout << "\t*Cluster" << i << ": Centroide: " << mejor.clustersCentroids[i] << ". Numero de elementos en el cluster: " << mejor.clustersElements[i].size() << "." << std::endl;
      std::sort (mejor.clustersElements[i].begin(), mejor.clustersElements[i].end());
      std::cout << "\t\t-Intervalo del conjunto [" << mejor.clustersElements[i][0] << ", " << mejor.clustersElements[i][mejor.clustersElements[i].size() - 1] << "]" << std::endl;
    }
    std::cout << "----------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\n************************************************\n";

    //Si el numero de clusters es 0 los datos se dejan tal cual
    //Si no
      // Si un numero pertenece a un cluster se cambia por su centroide
      // Si no pertenece se cambia por un Valor Perdido
    bool sustituido = false;
    if(mejor.clustersCentroids.size() > 0){
      for (unsigned int j = 0; j < (*dataset)[i].size(); j++) {
        sustituido = false;
        for (unsigned int k = 0; k < mejor.clustersCentroids.size(); k++) {
          if(((*dataset)[i][j] > mejor.clustersElements[k][0]) and ((*dataset)[i][j] < mejor.clustersElements[i][mejor.clustersElements[i].size()-1])){
            (*dataset)[i][j] = mejor.clustersCentroids[k];
            sustituido = true;
          }
        }

        if(sustituido == false){
          (*dataset)[i][j] = -999999999.0;
        }
      }
    }
  }




}
