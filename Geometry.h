#pragma once
struct vec2 {
    vec2(float x, float y) : x(x), y(y) { }

    float x;
    float y;
};

struct Shape {
    Shape(const std::string &type) : type(type) { }
    virtual ~Shape() { }

    bool is(const std::string &type) { return type == this->type; }
    const std::string type;
};

struct Circle : Shape {
    Circle(float diameter) : Shape("circle"), diameter(diameter) { }
    virtual ~Circle() { }

    float diameter;
};

struct Rectangle : Shape {
    Rectangle(float width, float height) : Shape("rectangle"), width(width), height(height) { }
    virtual ~Rectangle() { }

    float width;
    float height;
};

struct Polygon : Shape {
    Polygon() : Shape("polygon") { }
    Polygon(std::vector<vec2> vertices) : Shape("polygon"), vertices(vertices) { }
    virtual ~Polygon() { }

    std::vector<vec2> vertices;

    void push_back(vec2 v) { vertices.push_back(v); }
};

struct Line : Shape {
    Line() : Shape("line") { }
    Line(std::vector<vec2> vertices) : Shape("line"), vertices(vertices) { }
    virtual ~Line() { }

    std::vector<vec2> vertices;

    void push_back(vec2 v) { vertices.push_back(v); }
};
