#include "ObjModel.h"
#include<iostream>
#include<fstream>
#include"ObjModelStructs.h"
#include<vector>
#include<list>
#include<sstream>

ObjModel::ObjModel():isLoaded{false}{}

void ObjModel::DeleteModel(){
    center = Vertex{};
    vertices.clear();
    textureVertices.clear();
    verticesNormal.clear();
    faces.clear();
}

void ObjModel::LoadObjModel(const char *fileName){
    std::ifstream readFile{fileName};
    std::string str;

    if(!readFile){
        throw "File couldn't be opened";
    }

    DeleteModel();


    while(std::getline(readFile, str)){
        std::vector<std::string> keys;
        std::string key;

        std::stringstream strStream;
        strStream<<str;

        while(std::getline(strStream, key, ' ')){
            if(key.length() > 0){
                keys.push_back(key);
            }
        }


        if(keys[0].length() == 1 && keys[0][0] == 'v'){
            double x = atof(keys[1].c_str());
            double y = atof(keys[2].c_str());
            double z = atof(keys[3].c_str());

            center.coords.x += x;
            center.coords.y += y;
            center.coords.z += z;
            
            vertices.push_back(Vertex(x, y, z));
        }else if(keys[0].length() == 1 && keys[0][0] == 'f'){
            Face face;
            for(int i = 1; i < keys.size(); i++){
                int lastIndex = 0, part=1;
                for(int j = 0; j < keys[i].length(); j++){
                    if(keys[i][j] == '/'){
                        std::string indexSubstr = keys[i].substr(lastIndex, j-lastIndex); 
                        if(indexSubstr.length() > 0){
                            int index = atoi(indexSubstr.c_str())-1;
                            if(part == 1){
                                face.vertices.push_back(Vertex(vertices[index]));
                            }else if(part == 2){
                                face.textureVertices.push_back(TextureVertex(textureVertices[index]));
                            }
                        }
                        part++;
                        lastIndex = j+1;
                    }else{
                        if(j == keys[i].length()-1){
                            std::string indexSubstr = keys[i].substr(lastIndex, j+1-lastIndex);
                            if(indexSubstr.length() > 0){
                                int index = atoi(indexSubstr.c_str())-1;
                                if(part == 1){
                                    face.vertices.push_back(Vertex(vertices[index]));
                                }else if(part == 2){
                                    face.textureVertices.push_back(TextureVertex(textureVertices[index]));
                                }else{
                                    face.verticesNormal.push_back(VertexNormal(verticesNormal[index]));
                                }
                            } 
                        }
                    }
                }
            }
            faces.push_back(face);
        }else if(keys[0].length() > 1 && keys[0][0] == 'v' && keys[0][1] == 't'){
            if(keys.size() == 2){
                textureVertices.push_back(TextureVertex(atof(keys[1].c_str())));
            }else if(keys.size() == 3){
                textureVertices.push_back(TextureVertex(atof(keys[1].c_str()), atof(keys[2].c_str())));
            }else if(keys.size() == 4){
                textureVertices.push_back(TextureVertex(atof(keys[1].c_str()), atof(keys[2].c_str()), atof(keys[3].c_str())));
            }
        }else if(keys[0].length() > 1 && keys[0][0] == 'v' && keys[0][1] == 'n'){
            verticesNormal.push_back(VertexNormal(atof(keys[1].c_str()), atof(keys[2].c_str()), atof(keys[3].c_str())));
        }else if(keys[0].length() > 1 && keys[0][0] == 'v' && keys[0][1] == 'p'){
            // if(keys.size() == 2){
            //     textureVertices.push_back(TextureVertex(atof(keys[1].c_str())));
            // }else if(keys.size() == 3){
            //     textureVertices.push_back(TextureVertex(atof(keys[1].c_str()), atof(keys[2].c_str())));
            // }else if(keys.size() == 4){
            //     textureVertices.push_back(TextureVertex(atof(keys[1].c_str()), atof(keys[2].c_str()), atof(keys[3].c_str())));
            // }
        }
    }

    center.coords.x /= vertices.size();
    center.coords.y /= vertices.size();
    center.coords.z /= vertices.size();

    isLoaded = true;
    readFile.close();
}

void ObjModel::PrintVertices()const{

    for(auto &vertex:vertices){
        std::cout<<vertex.coords.x<<" "<<vertex.coords.y<<" "<<vertex.coords.z<<std::endl;
    }
}


void ObjModel::TranslateCenterToOrigin(){
    if(center.coords.x == 0 && center.coords.y == 0 && center.coords.z == 0){
        return;
    }

    for(auto &vertex:vertices){
        vertex.coords.x -= center.coords.x;
        vertex.coords.y -= center.coords.y;
        vertex.coords.z -= center.coords.z;
    }

    for(auto &face:faces){
        for(auto &vertex:face.vertices){
            vertex.coords.x -= center.coords.x;
            vertex.coords.y -= center.coords.y;
            vertex.coords.z -= center.coords.z;
        }
    }

    center = Vertex{};
}

double ObjModel::GetHighestDistanceAlongAxis(){
    double highest = 0;

    for(auto &vertex:vertices){
        if(highest < abs(vertex.coords.x-center.coords.x)){
            highest = abs(vertex.coords.x-center.coords.x);
        }
        if(highest < abs(vertex.coords.y-center.coords.y)){
            highest = abs(vertex.coords.y-center.coords.y);
        }
        if(highest < abs(vertex.coords.z-center.coords.z)){
            highest = abs(vertex.coords.z-center.coords.z);
        }
    }

    return highest;
}