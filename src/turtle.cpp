#include "xml.h"
#include <procgen/procgen.h>
#include <procgen/utils/logger.h>
#include <algorithm>
#include <cmath> // <ctin> ? pun intended

using json = nlohmann::json;

// Define 2D vector to simplify code
struct Point
{
    float x;
    float y;
    Point() = default;
    Point(float _x, float _y): x(_x), y(_y) {};
};

XML::Entity createPolyline(const std::vector<Point>& points ,float thickness = 2.0)
{
    XML::Entity xmlLine("polyline");

    std::stringstream stringPoints;

    // Serialize (format) points to list of points in string
    for(auto &point: points)
    {
        stringPoints << point.x << "," << point.y << " ";
    }

    xmlLine["points"] = stringPoints.str();
    xmlLine["style"] = std::string("stroke-width: ")+std::to_string(thickness)+"; stroke: rgb(255,0,0); fill:none;";
    return xmlLine;
}

Point min(const Point& first, const Point& second)
{
    return Point(std::min(first.x, second.x), std::min(first.y, second.y));
}

Point max(const Point& first, const Point& second)
{
    return Point(std::max(first.x, second.x), std::max(first.y, second.y));
}

void processTurtle(const ProcGen::Procgen& procgen, const char* outputFileName,float thickness = 2.0,float incrementalAngle = 90.0, float stepSize = 10.0)
{
    // define output SVG file
    XML::Exporter svgFile;
    auto& svgTree = svgFile.getRoot();
    svgTree.setName("svg");
    svgTree["xmlns"] = "http://www.w3.org/2000/svg";
    svgTree["version"] = "1.1";

    // define boundaries = smallest X,Y / highest X,Y
    Point smallestPoint = Point(0,0);
    Point highestPoint= Point(0,0);
    // define the start state of turtle
    float angle = -90.0;
    Point turtlePosition = Point(0,0);

    std::vector<Point> currentPolyline = { Point(0.0,0.0)};


    // Stores triple (X,Y, angle) 
    std::stack<std::pair<Point, float>> statesStack;


    // trace the L-system string
    const size_t count = procgen.countOfSymbols();
    for (size_t i = 0; i < count; i++)
    {
        auto symbol = procgen.at(i);
        if (symbol["_type"] == "F") {
            Point newPosition = Point(
                    turtlePosition.x + stepSize * cos(M_PI*angle/180.0),
                    turtlePosition.y + stepSize * sin(M_PI*angle/180.0));

            // push new points to line
            currentPolyline.push_back(newPosition);

            // update current position of turtle
            turtlePosition = newPosition;
           
            // Update boundaries
            smallestPoint = min(smallestPoint, turtlePosition);
            highestPoint = max(highestPoint, turtlePosition);
        }
        if (symbol["_type"] == "Plus") {
            angle += incrementalAngle;
        }

        if (symbol["_type"] == "Minus") {
            angle -= incrementalAngle;
        }

        if (symbol["_type"] == "Push") {
            statesStack.push(std::make_pair(turtlePosition, angle));
        }

        if (symbol["_type"] == "Pop") {
            if(statesStack.size() > 0)
            {
                auto restoredState = statesStack.top();
                statesStack.pop();

                // Terminate polyline
                svgTree.addChild(createPolyline(currentPolyline, thickness));

                // And create new one, starting in the restored position
                currentPolyline.push_back(restoredState.first);

                turtlePosition = restoredState.first;
                angle = restoredState.second;
            }
        }
    }

    // Save current polyline
    svgTree.addChild(createPolyline(currentPolyline, thickness));
    // Update viewport

    std::stringstream ss;
    ss << smallestPoint.x << " " << smallestPoint.y << " " << highestPoint.x-smallestPoint.x << " " << highestPoint.y-smallestPoint.y;
    svgTree["viewBox"] = ss.str();

    // Save SVG to file
    std::cout << "Ready to dump: " << svgTree.serialize() << std::endl;
    FILE* output = fopen(outputFileName, "w");

    fprintf(output, "<?xml version=\"1.0\" standalone=\"no\"?>\n");
    fprintf(output, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");

    if (output != nullptr) {
        svgTree.dump(output);
        fclose(output);
    }

    std::cout << "Image size: " << ss.str() << std::endl;
}

int main(int argc, char ** argv)
{
    // if neither input nor output files are defined
    if (argc < 3) {
        std::cout << "USAGE: " << argv[0] << " <fileName> <resultName>" << std::endl;
        return 1;
    }

    char* inputFile = argv[1];

    // create an instance of ProcGen language enviroment
    ProcGen::Procgen pg;

    // parse input file
    pg.parseFile(inputFile);

    // was compilation successfuly ?
    if (pg.isReady()) {
        // call init() function
        if (pg.runInit() == false)
            return 1;

        // Initialization is done, now run the derivation
        pg.run(1);

        // Serialize to JSON
        //json result = pg.serialize();
        //std::cout << "Result: " << result.dump(1) << "\n";

        float thickness = 2.0;
        pg.getUniform("THICKNESS", &thickness);

        float angle= 90.0;
        pg.getUniform("ANGLE", &angle);

        float stepsize = 10.0;
        pg.getUniform("STEPSIZE", &stepsize);
        // Process resulting JSON tree as test.svg
        processTurtle(pg, argv[2],thickness,angle, stepsize);

    } else {
        LOG_ERROR("Failed to compile file '%s'\n", inputFile);
        return 1;
    }
    return 0;
}
