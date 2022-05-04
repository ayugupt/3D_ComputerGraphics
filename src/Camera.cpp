#include "Camera.h"
#include<cmath>

Camera::Camera(){
    m_transformation_matrix_global_to_local =  GenerateTransformationMatrixGlobalToCameraCoords();
}

std::pair<double, double> Camera::ReturnGlobalRotationXY(){
    Coords directionVector{-1*m_global_position.x, -1*m_global_position.y, -1*m_global_position.z};
    
    double angleY = directionVector.z!=0?atan(directionVector.x/directionVector.z):(directionVector.x<0?-1*M_PI/2:M_PI/2);
    double tempZ = directionVector.x*sin(angleY) + directionVector.z*cos(angleY);

    // std::cout<<tempZ<<std::endl;

    if(tempZ > 0){
        angleY += M_PI;
        tempZ = directionVector.x*sin(angleY) + directionVector.z*cos(angleY);
    }
    // std::cout<<tempZ<<std::endl;


    double angleX = tempZ!=0?atan(-1*directionVector.y/tempZ):(directionVector.y<0?-1*M_PI/2:M_PI/2);
    tempZ = -1*sin(angleX)*directionVector.y +cos(angleX)*tempZ;

    //std::cout<<tempZ<<std::endl;


    if(tempZ > 0){
        angleX += M_PI;
    }

    return std::pair<double, double>{angleX, angleY};
}

Matrix Camera::GenerateTransformationMatrixGlobalToCameraCoords(){
    Matrix translationMatrix{{0, {{0, 1}, {1, 0}, {2, 0}, {3, -1*m_global_position.x}}},
                             {1, {{0, 0}, {1, 1}, {2, 0}, {3, -1*m_global_position.y}}},
                             {2, {{0, 0}, {1, 0}, {2, 1}, {3, -1*m_global_position.z}}},
                             {3, {{0, 0}, {1, 0}, {2, 0}, {3, 1}}},
                                };

    auto angles = ReturnGlobalRotationXY();

    auto sinAngleX = sin(angles.first);
    auto cosAngleX = cos(angles.first);
    auto sinAngleY = sin(angles.second);
    auto cosAngleY = cos(angles.second);

    Matrix rotationMatrixY{ 
                            {0, { {0, cosAngleY}, {1, 0}, {2, -1*sinAngleY}, {3, 0} } }, 
                            {1, { {0, 0}, {1, 1}, {2, 0}, {3, 0} } },
                            {2, { {0, sinAngleY}, {1, 0}, {2, cosAngleY}, {3, 0} } }, 
                            {3, { {0, 0}, {1, 0}, {2, 0}, {3, 1} } }, 
                          };

    Matrix rotationMatrixX{ 
                            {0, { {0, 1}, {1, 0}, {2, 0}, {3, 0} } }, 
                            {1, { {0, 0}, {1, cosAngleX}, {2, sinAngleX}, {3, 0} } },
                            {2, { {0, 0}, {1, -1*sinAngleX}, {2, cosAngleX}, {3, 0} } }, 
                            {3, { {0, 0}, {1, 0}, {2, 0}, {3, 1} } }, 
                          };

    Matrix transformationMatrix = MatrixMultiply(rotationMatrixX, rotationMatrixY);
    transformationMatrix = MatrixMultiply(transformationMatrix, translationMatrix);

    return transformationMatrix;
}

void Camera::SetGlobalPosition(const Coords &coords){
    m_global_position = coords;
    m_transformation_matrix_global_to_local =  GenerateTransformationMatrixGlobalToCameraCoords();

}