#include <procgen/procgen.h>
#include "xml.h"

using namespace ProcGen;

void saveResultAsSVGFile(json result, char* fileName);
int main(int argc, char** argv)
{
	ProcGen::Procgen pg;
	pg.setDebugOn(false);

	pg.parseFile("input.procgen");
	if(pg.isReady())
	{
        // set uniform
        pg.setUniform("ticks", 1000);
        if(pg.runInit() == false)
            return 1;
		std::cout << "It's dones" << std::endl;
		pg.run(1);
        json result = pg.serialize();
        std::cout << "Result" << result.dump(1) << "\n";

	saveResultAsSVGFile(result, "test.svg");
        
	} else {
        std::cout << "Failed...\n";
    }
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


XML::Entity saveCircle(json circle)
{
	XML::Entity xmlCircle("circle");
	xmlCircle["cx"] = std::to_string(circle["x"].get<float>());
	xmlCircle["cy"] = std::to_string(circle["y"].get<float>());
	xmlCircle["r"] = std::to_string(circle["radius"].get<float>());
	return xmlCircle;
}



void saveResultAsSVGFile(json result, char* fileName)
{
	// Create XML tree for SVG file 
	XML::Exporter svgFile;
	auto& svgTree = svgFile.getRoot();
	svgTree.setName("svg");
	svgTree["width"] = "500";
	svgTree["height"] = "500";

	// for all top-level structures in result
	for(auto &structure: result)	
	{
		// if structure doesn't have any type, than we can not process it 
		// and thus it's discarded 
		if(structure.find("_type") == structure.end())
			continue;
		// get user-defined type of structure from script
		std::string typeOfStructure = structure["_type"].get<std::string>();	

		if(typeOfStructure == "ellipse")
			svgTree.addChild(saveEllipse(structure));

		if(typeOfStructure == "circle")
			svgTree.addChild(saveCircle(structure));
	}

	// Ready to dump it out
	std::cout << "Ready to dump: " << svgTree.serialize() << std::endl;
	FILE* output = fopen(fileName, "w");
	svgTree.dump(output);
	fclose(output);
}
