#include "xml.h"
#include <procgen/procgen.h>

using namespace ProcGen;

const int WIDTH = 1024;
const int HEIGHT = 1024;

void saveResultAsSVGFile(json result, char* fileName);
int main(int argc, char** argv)
{

    if (argc < 3) {
        std::cout << "USAGE: " << argv[0] << " <fileName> <resultName>" << std::endl;
        return 1;
    }

    ProcGen::Procgen pg;
    pg.setDebugOn(false);

    //pg.parseFile("input.procgen");
    pg.parseFile(argv[1]);
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
        saveResultAsSVGFile(result, argv[2]);

    } else {
        std::cout << "Failed...\n";
        return 1;
    }
    return 0;
}

XML::Entity saveEllipse(json ellipse)
{
    XML::Entity xmlEllipse("ellipse");
    xmlEllipse["cx"] = std::to_string(ellipse["x"].get<float>());
    xmlEllipse["cy"] = std::to_string(ellipse["y"].get<float>());
    xmlEllipse["rx"] = std::to_string(ellipse["dx"].get<float>());
    xmlEllipse["ry"] = std::to_string(ellipse["dy"].get<float>());
    return xmlEllipse;
}

XML::Entity saveLine(json line)
{
    XML::Entity xmlLine("line");
    xmlLine["x1"] = std::to_string(line["x"].get<float>());
    xmlLine["y1"] = std::to_string(line["y"].get<float>());
    xmlLine["x2"] = std::to_string(line["xb"].get<float>());
    xmlLine["y2"] = std::to_string(line["yb"].get<float>());
    return xmlLine;
}

XML::Entity savePath(json arrayOfPoints)
{
    using point_type = std::pair<float, float>;
    std::vector<point_type> points;
    XML::Entity path("path");
    /*xmlLine["x1"] = std::to_string(line["x"].get<float>());
    xmlLine["y1"] = std::to_string(line["y"].get<float>());
    xmlLine["x2"] = std::to_string(line["xb"].get<float>());
    xmlLine["y2"] = std::to_string(line["yb"].get<float>());
    */

    std::stringstream lineToString;
    for (auto& point : arrayOfPoints) {
        points.push_back(std::make_pair(point["x"].get<float>(), point["y"].get<float>()));
        lineToString << "L" << points.back().first << " " << points.back().second << " ";
    }

    std::stringstream pathString;

    pathString << "M0 0 " << lineToString.str() << "Z";

    path["d"] = pathString.str();

    return path;
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
            ss << "stroke: " << rgbToString(styleElement["color"]) << ";";
        }
        // decode strokeWidthStyle
        if (styleElement["_type"] == "strokeWidthStyle") {
            ss << "stroke-width: " << styleElement["thickness"].get<float>() << ";";
        }
    }
    return ss.str();
}

void saveResultAsSVGFile(json result, char* fileName)
{
    // Create XML tree for SVG file
    XML::Exporter svgFile;
    auto& svgTree = svgFile.getRoot();
    // Build <svg> with correct parameters
    svgTree.setName("svg");
    svgTree["xmlns"] = "http://www.w3.org/2000/svg";
    svgTree["version"] = "1.1";

    // Set viewbox according to VIEW and HEIGHT
    std::stringstream viewBoxString;
    viewBoxString << "0 0 " << WIDTH << " " << HEIGHT;
    svgTree["viewBox"] = viewBoxString.str();

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

        if (typeOfStructure == "path")
            resultEntity = savePath(structure["points"]);

        if (structure.find("style") != structure.end()) {
            auto styleString = decodeStyle(structure["style"]);
            resultEntity["style"] = styleString;
        }

        svgTree.addChild(resultEntity);
    }

    // Ready to dump it out
    std::cout << "Ready to dump: " << svgTree.serialize() << std::endl;
    FILE* output = fopen(fileName, "w");

    fprintf(output, "<?xml version=\"1.0\" standalone=\"no\"?>\n");
    fprintf(output, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");

    if (output != nullptr) {
        svgTree.dump(output);
        fclose(output);
    }
}
