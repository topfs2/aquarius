#pragma once
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include "entityx/entityx.h"
#include "events.h"

#include "RenderProgram.h"
#include "Texture.h"

class CSDLOpenGLSystem : public entityx::System<CSDLOpenGLSystem>, public entityx::Receiver<CSDLOpenGLSystem>
{
public:
    CSDLOpenGLSystem();
    ~CSDLOpenGLSystem();

    void configure(entityx::ptr<entityx::EventManager> events);
    void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt);

    void receive(const CreateLayerEvent &e);
    void receive(const UpdateObserverEvent &e);

private:
    struct LayerOption {
        LayerOption(const std::string &name, float x, float y, float z) : name(name), x(x), y(y), z(z) { }

        std::string name;
        float x;
        float y;
        float z;
    };

    typedef std::map<std::string, CTexturePtr> TextureMap;
    typedef std::vector<LayerOption> LayerVector;

    entityx::Entity m_observer;

    CRenderProgram m_textureShader;
    CRenderProgram m_colorShader;
    TextureMap m_textures;

    LayerVector m_layers;
};
