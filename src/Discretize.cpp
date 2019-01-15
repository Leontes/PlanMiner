#include "Discretize.hpp"

#include <cstdlib>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>


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

Clusters discretizar(std::vector<double> numeros){
  double CHactual = 0.0;
  unsigned int k = 1;
  Clusters out, mejor;
  mejor.CHIndex = -1.0;

  /*
  double TSS = 0;
  for(unsigned int i = 0; i < numeros.size(); i++){
    TSS += pow(numeros[i] - datasetMed, 2);
  }
  */

  bool mejora = false;
  do{
    mejora = false;
    CHactual = out.CHIndex;
    //std::cout << "Probando con " << k << " clusters..." << std::endl;
    out = kmeans(k, numeros);
    //out = mejorCluster(k, numeros);
    k++;
    /*
    std::cout << "Indice CH: " << out.CHIndex << std::endl;
    std::cout << "Centroides calculados: " << std::endl;
    for(unsigned int i = 0; i < out.clustersCentroids.size(); i++){
      std::cout << "\t*Centroide del cluster" << i << ": " <<  out.clustersCentroids[i] << std::endl;
    }

    std::cout << std::endl;*/

    if(mejor.CHIndex < out.CHIndex or mejor.CHIndex == -1.0){
      mejor.CHIndex = out.CHIndex;
      mejor.clustersCentroids = out.clustersCentroids;
      mejor.clustersElements = out.clustersElements;
      mejora = true;
    }
  }while(mejora == true);

  out.CHIndex = mejor.CHIndex ;
  out.clustersCentroids = mejor.clustersCentroids;
  out.clustersElements = mejor.clustersElements;

  mejor.clustersCentroids.clear();
  mejor.clustersElements.clear();

  for(unsigned int i = 0; i < out.clustersCentroids.size(); i++){
    if(out.clustersElements[i].size() > 0){
        mejor.clustersCentroids.push_back(out.clustersCentroids[i]);
        mejor.clustersElements.push_back(out.clustersElements[i]);
    }
  }




  std::cout << "Clusters seleccionados:\n";
  std::cout << "\tIndice CH: " << mejor.CHIndex << std::endl;
  std::cout << "\tCentroides: " << std::endl;
  for(unsigned int i = 0; i < mejor.clustersCentroids.size(); i++){
    std::cout << "\t\t*Centroide del cluster" << i << ": " <<  mejor.clustersCentroids[i] << " Elementos en el cluster: "<< mejor.clustersElements[i].size() << std::endl;
  }



  double particion = 0.0, corte = 0.0;
  unsigned int indice = 0, contador = 0;

  std::vector <std::pair <std::pair <double, double>, double> > cortes;
  std::vector <std::vector <std::pair <std::pair <double, double>, double> > > cortesCluster;
  std::pair <double, double> pareja;
  double ultimoCorte, med = 0;
  unsigned int maxindex = 0, Bindex = 0, Cindex = 0, ultimoValor = 0;
  bool fin = false;
  Punto A, B, C, D, p, q;

  //std::cout << "\n\n\n----------------------------------------------------------------------------------------------------------------" << std::endl;
  //std::cout << "Clusters seleccionados:" << std::endl;
  for (size_t i = 0; i < mejor.clustersCentroids.size(); i++) {
    //std::cout << "\t*Cluster" << i << ": Centroide: " << mejor.clustersCentroids[i] << ". Numero de elementos en el cluster: " << mejor.clustersElements[i].size() << "." << std::endl;
    std::sort (mejor.clustersElements[i].begin(), mejor.clustersElements[i].end());
    //std::cout << "\t\t-Intervalo del conjunto [" << mejor.clustersElements[i][0] << ", " << mejor.clustersElements[i][mejor.clustersElements[i].size() - 1] << "]" << std::endl;

    //std::cout << "\t\t-Distribucion de los elementos del conjunto" << std::endl;

    particion = mejor.clustersElements[i][mejor.clustersElements[i].size() - 1] - mejor.clustersElements[i][0];
    particion /= 20.0;
    corte = mejor.clustersElements[i][0];
    indice = 0;
    cortes.clear();

      if(mejor.clustersElements[i][0] == mejor.clustersElements[i][mejor.clustersElements[i].size() - 1]){
        contador = mejor.clustersElements[i].size();
        pareja.first = mejor.clustersElements[i][0];
        pareja.second = mejor.clustersElements[i][mejor.clustersElements[i].size() - 1];
        cortes.push_back(std::pair <std::pair <double, double>, unsigned int> (pareja, (unsigned int)100.0*((double)contador/(double)mejor.clustersElements[i].size())));

        /*std::cout << std::setprecision(5) << std::fixed;
        std::cout <<"\t\t\t[" << pareja.first << ", " << pareja.second << "]: \t";
        std::cout << std::setprecision(2) << std::fixed;
        std::cout <<  (100.0*((double)contador/(double)mejor.clustersElements[i].size())) << "%\t";

        for(unsigned int l = 0; l < 100.0*((double)contador/(double)mejor.clustersElements[i].size()); l++ ){
          std::cout << "#";
        }
        std::cout << std::endl; */
      }
      else{
        for(unsigned int j = 0; j < 20; j++){
          corte += particion;
          contador = 0;
          while (mejor.clustersElements[i][indice] < corte and indice < mejor.clustersElements[i].size()){
            indice++;
            contador++;
          }

          pareja.first = (corte - particion);
          pareja.second = corte;
          cortes.push_back(std::pair <std::pair <double, double>, double> (pareja, 100.0*((double)contador/(double)mejor.clustersElements[i].size())));

        /*std::cout << std::setprecision(5) << std::fixed;
        std::cout <<"\t\t\t[" << pareja.first << ", " << pareja.second << "]: \t";
        std::cout << std::setprecision(2) << std::fixed;
        std::cout <<  (100.0*((double)contador/(double)mejor.clustersElements[i].size())) << "%\t";

        for(unsigned int l = 0; l < 100.0*((double)contador/(double)mejor.clustersElements[i].size()); l++ ){
          std::cout << "#";
        }
        std::cout << std::endl;*/
      }
    }

    cortesCluster.push_back(cortes);

    maxindex = 0;
    ultimoValor = 0;
    fin = false;
    while(fin == false){
      if(cortesCluster[i][maxindex].second < ultimoValor){
        fin = true;
      }else{
        ultimoValor = cortesCluster[i][maxindex].second;
        maxindex++;
      }
    }

    maxindex--;
    //std::cout << "Indice del corte con mayor numero de elementos: " << maxindex << std::endl;

    //A.x = cortesCluster[cortesCluster.size()-1][0].first.first;
    A.x = cortesCluster[cortesCluster.size()-1][0].first.first - (particion/500.0);
    A.y = 0;

    //D.x = cortesCluster[cortesCluster.size()-1][cortesCluster[i].size()-1].first.second;
    D.x = cortesCluster[cortesCluster.size()-1][cortesCluster[i].size()-1].first.second + (particion/100.0);
    D.y = 0;

    if(!distintos(A,D)){
      B.x = A.x;
      B.y = A.y;

      C.x = D.x;
      C.y = D.y;
    }else{

      Bindex = maxindex;
      fin = false;
      while(Bindex >= 0 and fin == false){

        B.x = cortesCluster[cortesCluster.size()-1][Bindex].first.first;
        B.y = cortesCluster[cortesCluster.size()-1][Bindex].second;

        //std::cout << "Linea AB:" << "(" << A.x << ", " << A.y << ") - (" << B.x << ", " << B.y << ")" << std::endl;
        fin = true;
        for(unsigned int j = Bindex; j > 0; j--){
          p.x = cortesCluster[cortesCluster.size()-1][j].first.first;
          p.y = 0;
          q.x = cortesCluster[cortesCluster.size()-1][j].first.first;
          q.y = cortesCluster[cortesCluster.size()-1][j].second;
          //std::cout << "Linea pq:" << "(" << p.x << ", " << p.y << ") - (" << q.x << ", " << q.y << ")" << std::endl;
          if(intersectan(A, B, p, q) and distintos(p,q)){
            fin = false;
            Bindex--;
            break;
          }
        }

      }

      Cindex = maxindex;
      fin = false;

      while(Cindex < 20 and fin == false){
        C.x = cortesCluster[cortesCluster.size()-1][Cindex].first.second;
        C.y = cortesCluster[cortesCluster.size()-1][Cindex].second;
        //std::cout << "\n\nLinea CD:" << "(" << C.x << ", " << C.y << ") - (" << D.x << ", " << D.y << ")" << std::endl;
        fin = true;
        for(unsigned int j = Cindex; j < 20; j++){
          p.x = cortesCluster[cortesCluster.size()-1][j].first.first;
          p.y = 0;
          q.x = cortesCluster[cortesCluster.size()-1][j].first.first;
          q.y = cortesCluster[cortesCluster.size()-1][j].second;
          //std::cout << "Linea pq:" << "(" << p.x << ", " << p.y << ") - (" << q.x << ", " << q.y << ")" << std::endl;
          if(intersectan(C, D, p, q) and distintos(p,q)){
            fin = false;
            Cindex++;
            break;
          }
        }
      }
    }

    /*std::cout << "\t\t-Funcion de pertenencia asociada: ";
    std::cout << std::setprecision(5) << std::fixed;
    std::cout << "(" << A.x << ", "<< B.x << ", " << C.x << ", "<< D.x <<")" << std::endl;*/

    mejor.membershipClusters.push_back(funcPert(A.x,B.x,C.x,D.x));

    med = 0;
    for(unsigned int k = 0; k < cortesCluster[cortesCluster.size()-1].size(); k++){
      pareja.first = cortesCluster[cortesCluster.size()-1][k].first.first;
      pareja.second = cortesCluster[cortesCluster.size()-1][k].first.second;
      med += (((pareja.first + pareja.second)/2) * (cortesCluster[cortesCluster.size()-1][k].second / 100.0));

      //std::cout << "Media: " << ((pareja.first + pareja.second)/2.0) << " factor de multiplicación: " << (cortesCluster[cortesCluster.size()-1][k].second / 100.0) << std::endl;
    }

    //std::cout << "\t\t\t*Valor CRISP asociado: " << med << std::endl;


  }
  //std::cout << "----------------------------------------------------------------------------------------------------------------" << std::endl;

  return mejor;
}


std::map < std::string, std::vector <FuzzSet> > discretize(std::vector < std::vector < double > > * dataset, std::vector < std::pair<std::string, std::string> > * attribLabels){

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
            if((*dataset)[i][j] != -999999999.0){
              datosAux.push_back((*dataset)[i][j]);
            }
          }

          clAux = discretizar(datosAux);
          vectAux.clear();

          for(unsigned int j = 0; j < clAux.membershipClusters.size(); j++){
            vectAux.push_back(FuzzSet());
            vectAux[vectAux.size()- 1].a = clAux.membershipClusters[j].A;
            vectAux[vectAux.size()- 1].b = clAux.membershipClusters[j].B;
            vectAux[vectAux.size()- 1].c = clAux.membershipClusters[j].C;
            vectAux[vectAux.size()- 1].d = clAux.membershipClusters[j].D;
            vectAux[vectAux.size()- 1].label = "L_" + std::to_string(j);
          }

          out[(*attribLabels)[i].first] = vectAux;

        }
      }
    }
  }

  return out;

}
