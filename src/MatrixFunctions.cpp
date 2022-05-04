#include "MatrixFunctions.h" 
#include<iostream>

Matrix MatrixMultiply(Matrix &matrix1, Matrix &matrix2){
    if(matrix1.size() > 0 && matrix2.size() > 0){
        if(matrix1[0].size() != matrix2.size()){
            throw "Cannot multiply";
        }
    }else{
        throw "Empty matrix";
    }

    Matrix result;

    for(int i = 0; i < matrix1.size(); i++){
        for(int j = 0; j < matrix2[0].size(); j++){
            double sum = 0;
            for(int k = 0; k < matrix2.size(); k++){
                sum += matrix1[i][k]*matrix2[k][j];
            }
            result[i][j] = sum;
        }
    }

    return result;
}
