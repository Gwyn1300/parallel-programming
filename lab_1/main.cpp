#include"function.h"
#include "globals.h"

int main(){
    creatMatrixForExperement();
    for(size_t i = 0; i<MATRIX.size(); i+=2){
       experiment(MATRIX[i], MATRIX[i+1]);
    }
    createGraphsFromCSV(BASEPATH+"report/data.csv");
}