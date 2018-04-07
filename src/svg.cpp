#include "xml.h"
#include <procgen/procgen.h>

using namespace ProcGen;

const int WIDTH = 1024;
const int HEIGHT = 1024;

void saveResultAsSVGFile(json result, char* fileName);
int main(int argc, char** argv)
{
    ProcGen::Procgen pg;
    pg.setDebugOn(false);

    pg.parseFile("input.procgen");
    if (pg.isReady()) {
	// Set uniforms
        pg.setUniform("WIDTH", WIDTH);
        pg.setUniform("HEIGHT", HEIGHT);
        if (pg.runInit() == false)
            return 1;

	// Initialization is done, now run the derivation
        pg.run(1);

	// Serialize to JSON
        json result = pg.serialize();
        std::cout << "Result: " << result.dump(1) << "\n";

	// Process resulting JSON tree as test.svg
        saveResultAsSVGFile(result, "test.svg");

    } else {
        std::cout << "Failed...\n";
	return 1;
    }
    return 0;
}

XML::Entity saveEllipse(json ellipse)
{
    XML::Entity xmlEllipse("ellipse");
    xmlEllipse["cx"] = ellipse["x"].get<float>();
    xmlEllipse["cy"] = ellipse["y"].get<float>();
    xmlEllipse["rx"] = ellipse["dx"].get<float>();
    xmlEllipse["ry"] = ellipse["dy"].get<float>();
    return xmlEllipse;
}

XML::Entity saveLine(json line)
{
    XML::Entity xmlLine("line");
    xmlLine["x1"] = line["x"].get<float>();
    xmlLine["y1"] = line["y"].get<float>();
    xmlLine["x2"] = line["xb"].get<float>();
    xmlLine["y2"] = line["yb"].get<float>();
    return xmlLine;
}



XML::Entity saveCircle(json circle)
{
    XML::Entity xmlCircle("circle");
    xmlCircle["cx"] = std::to_string(circle["x"].get<float>());
    xmlCircle["cy"] = std::to_string(circle["y"].get<float>());
    xmlCircle["r"] = std::to_string(circle["radius"].get<float>());
    return xmlCircle;
}

std::string rgbToString(json rgb)
{
    std::stringstream ss;
    ss << "rgb(" << rgb["r"] << "," << rgb["g"] << "," << rgb["b"] << ")";
    return ss.str();
}
std::string decodeStyle(json styleCollection)
{

    if (!styleCollection.is_array()) {
        return "";
    }
    std::stringstream ss;
    // for each style element in collection
    for (auto& styleElement : styleCollection) {
        if (!styleElement.is_structured())
            continue;
        if (styleElement.find("_type") == styleElement.end())
            continue;
        // decode fillColorStyle
        if (styleElement["_type"] == "fillColorStyle") {
            ss << "fill: " << rgbToString(styleElement["color"]) << ";";
        }
        // decode strokeColorStyle
        if (styleElement["_type"] == "strokeColorStyle") {
            ss << "color: " << rgbToString(styleElement["color"]) << ";";
        }
    }
    return ss.str();
}

void saveResultAsSVGFile(json result, char* fileName)
{
    // Create XML tree for SVG file
    XML::Exporter svgFile;
    auto& svgTree = svgFile.getRoot();
    svgTree.setName("svg");
    svgTree["width"] = std::to_string(WIDTH);
    svgTree["height"] = std::to_string(HEIGHT);

    // for all top-level structures in result
    for (auto& structure : result) {
        // if structure doesn't have any type, than we can not process it
        // and thus it's discarded
        if (structure.find("_type") == structure.end())
            continue;
        // get user-defined type of structure from script
        std::string typeOfStructure = structure["_type"].get<std::string>();

        XML::Entity resultEntity;


        // DETECT type of primitive and parse it HERE
        if (typeOfStructure == "ellipse")
            resultEntity = saveEllipse(structure);

        if (typeOfStructure == "circle")
            resultEntity = saveCircle(structure);

        if (typeOfStructure == "line")
            resultEntity = saveLine(structure);

        if (structure.find("style") != structure.end()) {
            auto styleString = decodeStyle(structure["style"]);
            resultEntity["style"] = styleString;
        }

        svgTree.addChild(resultEntity);
    }

    // Ready to dump it out
    std::cout << "Ready to dump: " << svgTree.serialize() << std::endl;
    FILE* output = fopen(fileName, "w");
    if (output != nullptr) {
        svgTree.dump(output);
        fclose(output);
    }
}
