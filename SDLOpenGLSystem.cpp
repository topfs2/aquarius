#include <GL/glew.h>
#include "SDLOpenGLSystem.h"
#include "components.h"
#include "settings.h"
#include "Log.h"

using namespace logging;

//#define DEBUG_PHYSICS

CSDLOpenGLSystem::CSDLOpenGLSystem() : m_textureShader("simple_texture"), m_colorShader("simple_colour") {
    m_layers.push_back(LayerOption("default", 0.0f, 0.0f, 0.0f));
}

CSDLOpenGLSystem::~CSDLOpenGLSystem() {
}

void CSDLOpenGLSystem::configure(entityx::ptr<entityx::EventManager> events) {
    events->subscribe<CreateLayerEvent>(*this);
    events->subscribe<UpdateObserverEvent>(*this);
}

void CSDLOpenGLSystem::update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {
    float ar = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

    float w = 32.0f;
    float h = w / ar;

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;

    glShadeModel( GL_SMOOTH );

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//    glClearColor(109.0f / 255.0f, 169.0f / 255.0f, 101.0f / 255.0f, 0.0f);
    glClearColor(85.0f / 255.0f, 180.0f / 255.0f, 1.0f, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float camera_position[3] = { 0.0f, 0.0f, 0.0f };

    if (m_observer.valid()) {
        entityx::ptr<Position> position = m_observer.component<Position>();

        if (position) {
            camera_position[0] = position->x;
            camera_position[1] = position->y;
//            camera_position[2] = position->z;
        }
    }


//  gluPerspective(60.0f, w / h, 1.0f, 1024.0f);
    glOrtho(-w2 - camera_position[0], w2 - camera_position[0], -h2 - camera_position[1], h2 - camera_position[1], -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
//  glTranslatef(0.0f, 0.0f, -1.0f);
    glScalef(1.0f, 1.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);

    m_textureShader.Bind();

    for (auto layer_option : m_layers) {
        for (auto entity : entities->entities_with_components<Position, Sprite>()) {
            entityx::ptr<Position> position = entity.component<Position>();
            entityx::ptr<Sprite> sprite = entity.component<Sprite>();
            entityx::ptr<Layer> layer = entity.component<Layer>();

            if ((layer && layer_option.name == layer->name) || (layer_option.name == "default" && !layer)) {
                float x = position->x;
                float y = position->y;

                float w2 = sprite->width / 2.0f;
                float h2 = sprite->height / 2.0f;

                float u0 = sprite->u0;
                float v0 = sprite->v0;
                float u1 = sprite->u1;
                float v1 = sprite->v1;

                TextureMap::iterator t = m_textures.find(sprite->filename);
                if (t == m_textures.end()) {
                    m_textures[sprite->filename] = CTexturePtr(new CTexture(sprite->filename.c_str()));
                    t = m_textures.find(sprite->filename);
                }

                glPushMatrix();
//                glTranslatef(layer_option.x, layer_option.y, layer_option.z);
                glTranslatef(x, y, 0.0f);
                glRotatef(position->angle, 0.0f, 0.0f, 1.0f);
                glScalef(w2, h2, 1.0f);

                t->second->Bind();

                glBegin(GL_QUADS);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

                    glTexCoord2f(u0, v1); glVertex3f(-1.0f, -1.0f, 0.0f);
                    glTexCoord2f(u1, v1); glVertex3f( 1.0f, -1.0f, 0.0f);
                    glTexCoord2f(u1, v0); glVertex3f( 1.0f,  1.0f, 0.0f);
                    glTexCoord2f(u0, v0); glVertex3f(-1.0f,  1.0f, 0.0f);
                glEnd();

                t->second->UnBind();
                glPopMatrix();
            }
        }
    }

#ifdef DEBUG_PHYSICS
    m_colorShader.Bind();
    for (auto entity : entities->entities_with_components<physics::Body>()) {
        entityx::ptr<Position> position = entity.component<Position>();
        entityx::ptr<physics::Body> dynamic = entity.component<physics::Body>();

        glPushMatrix();
        glTranslatef(position->x, position->y, 0.0f);
        glRotatef(position->angle, 0.0f, 0.0f, 1.0f);

        if (dynamic->shape->is("polygon")) {
            entityx::ptr<Polygon> polygon = std::dynamic_pointer_cast<Polygon>(dynamic->shape);

            std::vector<vec2> vertices = polygon->vertices;

            glBegin(GL_LINE_LOOP);
                glColor3f(1.0f, 0.0f, 0.0f);

                for (std::vector<vec2>::iterator itr = vertices.begin(); itr != vertices.end(); itr++) {
                    glVertex2f(itr->x, itr->y);
                }
            glEnd();
        } else if (dynamic->shape->is("rectangle")) {
            entityx::ptr<Rectangle> rectangle = std::dynamic_pointer_cast<Rectangle>(dynamic->shape);

            float w2 = rectangle->width / 2.0f;
            float h2 = rectangle->height / 2.0f;

            glBegin(GL_LINE_LOOP);
                glColor3f(0.0f, 1.0f, 0.0f);

                glVertex2f(-w2, -h2);
                glVertex2f( w2, -h2);
                glVertex2f( w2,  h2);
                glVertex2f(-w2,  h2);
            glEnd();
        } else if (dynamic->shape->is("circle")) {
            entityx::ptr<Circle> circle = std::dynamic_pointer_cast<Circle>(dynamic->shape);
            float r = circle->diameter * 0.5f;

	        glBegin(GL_LINE_LOOP);
                glColor3f(0.0f, 0.0f, 1.0f);
                unsigned int num_segments = 8;
	            for(int ii = 0; ii < num_segments; ii++) {
		            float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);

		            float x = r * cosf(theta);
		            float y = r * sinf(theta);

		            glVertex2f(x, y);
	            }
	        glEnd();
        } else if (dynamic->shape->is("line")) {
            entityx::ptr<Line> line = std::dynamic_pointer_cast<Line>(dynamic->shape);

            std::vector<vec2> vertices = line->vertices;

            glBegin(GL_LINE_STRIP);
                glColor3f(1.0f, 1.0f, 0.0f);

                for (std::vector<vec2>::iterator itr = vertices.begin(); itr != vertices.end(); itr++) {
                    glVertex2f(itr->x, itr->y);
                }
            glEnd();
        }

        glPopMatrix();
    }
#endif
}


void CSDLOpenGLSystem::receive(const CreateLayerEvent &e)
{
    bool updated = false;
    for (auto layer : m_layers) {
        if (layer.name == e.name) {
            layer.x = e.x;
            layer.y = e.y;
            layer.z = e.z;
            updated = true;
        }
    }

    if (!updated) {
        m_layers.push_back(LayerOption(e.name, e.x, e.y, e.z));
    }

    std::sort(m_layers.begin(), m_layers.end(), [](LayerOption const & a, LayerOption const & b) -> bool { return a.z < b.z; });
}

void CSDLOpenGLSystem::receive(const UpdateObserverEvent &e)
{
    m_observer = e.entity;
}
