#pragma once
#include<iostream>
#include<list>

struct Coords{
    double x{};
    double y{};
    double z{};

    Coords(double x=0, double y=0, double z=0):x{x}, y{y}, z{z}{}
    Coords(const Coords &coords)=default;
    Coords &operator=(const Coords&obj){
        x = obj.x;
        y = obj.y;
        z = obj.z;

        return *this;
    }
};  

struct Vertex{
    Coords coords{};
    double w{};

    Vertex(double x, double y, double z, double w=1):coords{x, y, z}, w{w}{}
    Vertex(const Coords &coords, double w=1):coords{coords}, w{w}{}
    Vertex(const Vertex &vertex)=default;
    Vertex()=default;

    Vertex &operator=(const Vertex&obj){
        coords = obj.coords;
        w = obj.w;
        return *this;
    }
};

struct TextureVertex{
    double u{};
    double v{};
    double w{};

    TextureVertex(double u=0, double v=0, double w=0): u{u}, v{v}, w{w}{}
    TextureVertex(const TextureVertex& textureVertex)=default;

    TextureVertex &operator=(const TextureVertex& obj){
        u = obj.u;
        v = obj.v;
        w = obj.w;
        return *this;
    }
};  

struct VertexNormal{
    Coords coords{};

    VertexNormal(double x=0, double y=0, double z=0): coords{x, y, z}{}
    VertexNormal(const Coords &coords): coords{coords}{}
    VertexNormal(const VertexNormal& vertexNormal)=default;

    VertexNormal &operator=(const VertexNormal &obj){
        coords = obj.coords;
        return *this;
    }
};

struct Face{
    std::list<Vertex> vertices;
    std::list<TextureVertex> textureVertices;
    std::list<VertexNormal> verticesNormal;
};