#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H

// Subsistema de iluminacion: es dueno del estado GL de luces (GL_LIGHT0..
// GL_LIGHT7 y GL_LIGHT_MODEL_AMBIENT). Cada frame escanea los GameObjects de
// la escena, toma sus componentes Light y parametriza los slots hardware.
// GameScene delega en el; no vive logica de luz en el frame loop ni en los
// componentes (Light es solo data).
class GameObject;

template <typename T>
class Position;

template <typename T>
class ListaDE;

class LightSystem {
public:
    LightSystem();
    ~LightSystem() = default;

    // Luz global del modelo (GL_LIGHT_MODEL_AMBIENT), default gris tenue.
    void setGlobalAmbient(float r, float g, float b);

    // Dueno del estado GL de luces: habilita GL_LIGHTING, configura el modelo,
    // apaga los 8 slots y enciende/parametriza cada componente Light encontrado
    // en los objetos de la escena. Debe llamarse cada frame, antes de dibujar.
    void beginFrame(ListaDE<GameObject*>* objects);

private:
    void aplicarLuz(ListaDE<GameObject*>* objects, int& slotsEnabled);

    float globalAmbient_[4] = {0.15f, 0.15f, 0.15f, 1.f};
};

#endif