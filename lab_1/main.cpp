#include"function.h"
#include "globals.h"

int main(){
    /*creatMatrixForExperement();
    for(size_t i = 0; i<MATRIX.size(); i+=2){
        for(int j = 1; j<=8; j++){
            experiment(MATRIX[i], MATRIX[i+1], j);
        }
    }*/
    createGraphsFromCSV(BASEPATH+"report/data.csv");
}