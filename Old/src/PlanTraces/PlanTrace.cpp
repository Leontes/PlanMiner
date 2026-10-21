#include "PlanTraces/PlanTrace.hpp"


void PlanTrace::schematize(/*std::vector < Type *> tipos*/){
  for (unsigned int i = 0; i < TSlinks.size(); i++) {
    TSlinks[i] -> schematize(/*tipos*/);
  }
}
