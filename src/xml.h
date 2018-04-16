/*
 * Simple XML tree exporter
 * Written by Roman Dobias
 * 2018, MIT License
 */
#include <map>
#include <sstream>
#include <vector>
namespace XML {
class Entity : public std::map<std::string, std::string> {
    std::vector<Entity> children;
    // Entity name
    std::string name;

public:
    Entity()
        : name("entity")
    {
    }
    Entity(std::string newname)
        : name(newname)
    {
    }
    ~Entity() = default;

    std::string serialize()
    {
        std::stringstream ss;

        ss << "<" << this->name;
        if (this->size() == 0)
            ss << ">\n";
        else {
            ss << " ";
            /*for(auto attribute: this->getAttributes())
					{
						ss << attribute.first << "= '" << attribute.second << "' ";
					}
					*/
            for (auto it = this->begin(); it != this->end(); ++it) {
                ss << it->first << " = \"" << it->second << "\" ";
            }
            ss << ">\n";
        }
        for (auto& child : this->children) {
            ss << child.serialize();
        }
        ss << "</" << this->name << ">\n";

        return ss.str();
    }
    void dump(FILE* file)
    {
        fprintf(file, "%s", this->serialize().c_str());
    }

    std::map<std::string, std::string>* getAttributes()
    {
        return this;
    }

    void setName(std::string name) { this->name = name; }
    const std::string& getName() const { return this->name;}

    void addChild(Entity entity) { this->children.push_back(entity); }
};

class EntityText : public Entity {
    std::string content;
};
class Exporter {
    Entity rootEntity;

public:
    Exporter() = default;
    ~Exporter() = default;

    Entity& getRoot() { return this->rootEntity; }
    void dump(FILE* file = stdout)
    {
        this->rootEntity.dump(file);
    }
};
};
