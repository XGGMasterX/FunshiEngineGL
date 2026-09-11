#ifndef SCENESERIALIZER_H
#define SCENESERIALIZER_H

#include <fstream>
#include <string>

class EditorController;
class SceneRegistry;
class GameObject;

template <typename T>
class Position;

class SceneSerializer {
private:
    SceneRegistry* scene;
    EditorController* editor;

    void savePreOrder(Position<GameObject*>* root,
                       std::ofstream& archive,
                       const std::string& filename);

    void loadPreOrder(std::ifstream& file,
                      GameObject* parent,
                      const std::string& semiPath);

public:
    SceneSerializer(SceneRegistry* value,
                    EditorController* controller);

    void save(const std::string& filename);

    void load(const std::string& pathTxt,
              const std::string& semiPath);
};

#endif
