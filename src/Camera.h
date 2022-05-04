#pragma once

#include "model/ObjModelStructs.h"
#include "MatrixFunctions.h"

class Camera{
    double m_focal_length{10};
    double height{50};
    double width{100};

    Coords m_global_position;

    Matrix m_transformation_matrix_global_to_local;

public:

    Camera();

    std::pair<double, double> ReturnGlobalRotationXY();
    Matrix GenerateTransformationMatrixGlobalToCameraCoords();

    void SetGlobalPosition(const Coords &coords);

    inline Coords GetGlobalPosition(){
        return m_global_position;
    }

    inline Matrix& GetTransformationMatrixGlobalToCameraCoords(){
        return m_transformation_matrix_global_to_local;
    }

    inline double GetFocalLength() const{
        return m_focal_length;
    }

};  