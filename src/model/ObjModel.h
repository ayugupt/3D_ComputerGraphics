#include<vector>
#include "ObjModelStructs.h"
#include<string>

class ObjModel{
    bool isLoaded;
    Vertex center;

public:

    std::vector<Vertex> vertices;
    std::vector<TextureVertex> textureVertices;
    std::vector<VertexNormal> verticesNormal;
    std::vector<Face> faces;

    ObjModel();

    void DeleteModel();

    void LoadObjModel(const char* fileName);
    void LoadObjModel(const std::string &fileName);

    void TranslateCenterToOrigin();
    double GetHighestDistanceAlongAxis();

    void PrintVertices() const;


    //---------------Getters--------------------
    inline bool GetIsLoaded() const{
        return isLoaded;
    }
    //------------------------------------------
};