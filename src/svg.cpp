#include <procgen/procgen.h>

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
        
	} else {
        std::cout << "Failed...\n";
    }
}


void saveEllipse(json ellipse, FILE* output)
{

}

void saveResultAsSVGFile(json result, char* fileName)
{
	FILE* file = fopen(fileName, "w");
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
			saveEllipse(structure,file);
	}
	fclose(file);
}
