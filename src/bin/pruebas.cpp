#include <iostream>
#include <random>

#include "PtLib.hpp"






int main(int argc, char const *argv[]) {
  Estado resultadoAstar;
  std::vector<Estado> estadosInit;
  std::vector<std::string> problemVars;
  std::vector< std::vector <double> >problemData;
  std::vector <double> problemObj, aux;

  problemData.clear();
  problemVars.clear();
  problemObj.clear();

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0,1000.0);
  double randD = 0.0;

  aux.clear();
  for(unsigned int i = 0; i < 1000; i++){
    aux.push_back(1);
  }
  problemData.push_back(aux);
  problemVars.push_back("Var1");


  aux.clear();
  for(unsigned int i = 0; i < 1000; i++){
    aux.push_back(10* i);
  }
  problemData.push_back(aux);
  problemVars.push_back("Var2");


  aux.clear();
  for(unsigned int i = 0; i < 1000; i++){
    randD = distribution(generator);
    aux.push_back(randD);
  }
  problemData.push_back(aux);
  problemVars.push_back("Var3");

  aux.clear();
  for(unsigned int i = 0; i < 1000; i++){
    randD = distribution(generator);
    aux.push_back(-randD);
  }
  problemData.push_back(aux);
  problemVars.push_back("Var4");


  aux.clear();
  for(unsigned int i = 0; i < 1000; i++){
    randD = distribution(generator);
    aux.push_back(randD - i);
  }
  problemData.push_back(aux);
  problemVars.push_back("Var5");


  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(0);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(10);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(20);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(3000);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(problemData[3][i]);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(problemData[3][i] - problemData[3][i]);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(problemData[3][i] * problemData[2][i]);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";


  problemObj.clear();
  for(unsigned int i = 0; i < 1000; i++){
    problemObj.push_back(50000.0 - (problemData[3][i] / problemData[2][i]));
  }

  std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  estadosInit.clear();
  estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  //resultadoAstar = RSPrueba(estadosInit);
  resultadoAstar = Astar(estadosInit);
  std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";


  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(0.0 - (problemData[3][i] * problemData[2][i]));
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back((problemData[3][i] * problemData[2][i]) * -1.0);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back(problemData[3][i] - problemData[2][i]);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //
  //
  // problemObj.clear();
  // for(unsigned int i = 0; i < 1000; i++){
  //   problemObj.push_back((problemData[3][i] - problemData[2][i]) * problemData[4][i]);
  // }
  //
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n";
  // estadosInit.clear();
  // estadosInit = generarListaInit(&problemVars, &problemData, &problemObj);
  // //resultadoAstar = RSPrueba(estadosInit);
  // resultadoAstar = Astar(estadosInit);
  // std::cout << "\t- Expression found: " << resultadoAstar.getFormula() << " MAPE: "<< resultadoAstar.getH() << "%"<< std::endl;
  // std::cout << "///////////////////////////////////////////////////////////////////////////////////////\n\n\n";
  //


  return 0;
}
