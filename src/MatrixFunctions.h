#pragma once 

#include<cmath>
#include<unordered_map>

using Matrix = std::unordered_map<int, std::unordered_map<int, double>>;

Matrix MatrixMultiply(Matrix &matrix1, Matrix &matrix2);

