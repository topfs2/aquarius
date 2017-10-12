#pragma once
#include <string>
#include "entityx/entityx.h"
#include "Geometry.h"

struct NameComponent : entityx::Component<NameComponent> {
    NameComponent(const std::string &name) : name(name) {}

    std::string name;
};

struct Position : entityx::Component<Position> {
    Position(float x = 0.0f, float y = 0.0f, float angle = 0.0f) : x(x), y(y), angle(angle) {}

    float x, y, angle;
};

struct Velocity : entityx::Component<Velocity> {
    Velocity(float x = 0.0f, float y = 0.0f, float angle = 0.0f) : x(x), y(y), angle(angle) {}

    float x, y, angle;
};

struct Layer : entityx::Component<Layer> {
    Layer(const std::string &name) : name(name) {}

    std::string name;
};

struct Sprite : entityx::Component<Sprite> {
    Sprite(const std::string &filename, float width, float height) : filename(filename), width(width), height(height), u0(0), v0(0), u1(1), v1(1) {}
    Sprite(const std::string &filename, float width, float height, float u0, float v0, float u1, float v1) : filename(filename), width(width), height(height), u0(u0), v0(v0), u1(u1), v1(v1) {}

    std::string filename;
    float width;
    float height;
    float u0;
    float v0;
    float u1;
    float v1;
};

namespace physics {
    struct Body : entityx::Component<Body> {
        Body(entityx::ptr<Shape> shape, float density, float friction, bool fixed_rotation = false, bool bullet = false) : shape(shape), density(density), friction(friction), fixed_rotation(fixed_rotation), bullet(bullet), sleeping(false), contacts(0) {}
        virtual ~Body() {}

        const entityx::ptr<Shape> shape;

        const float density;
        const float friction;
        const bool fixed_rotation;
        const bool bullet;

        bool sleeping;
        unsigned int contacts;
    };
}

struct Sound : entityx::Component<Sound> {
    Sound(const std::string &filename, float volume, bool looping = false, bool autoplay = true) : filename(filename), volume(volume), looping(looping), autoplay(autoplay) {}
    Sound(const std::string &filename, bool looping = false, bool autoplay = true) : filename(filename), volume(1.0f), looping(looping), autoplay(autoplay) {}

    std::string filename;
    float volume;
    bool looping;
    bool autoplay;
    bool playing;
};

struct Player : entityx::Component<Player> {
    Player(const std::string &name) : name(name), direction("RIGHT"), state("IDLE"), grounded(false) { }

    std::string name;
    std::string direction;
    std::string state;
    bool grounded;
};

struct Script : entityx::Component<Script> {
    Script(const std::string &filename) : filename(filename), argument("") {}
    Script(const std::string &filename, const std::string &argument) : filename(filename), argument(argument) {}

    const std::string filename;
    const std::string argument;
};
