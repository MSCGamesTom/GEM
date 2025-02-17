

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "InfiniteAreaLight.h"

int main(int argc, char* argv[])
{
    std::unordered_map<std::string, std::string> args;

    // Start at 1 to skip the program name
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Check if the argument starts with a dash
        if (!arg.empty() && arg[0] == '-') {
            // Remove the dash to get the argument name
            std::string argName = arg;

            // Ensure that there is a value following the argument name
            if (i + 1 < argc) {
                std::string argValue = argv[++i];  // Move to the next element for the value
                args[argName] = argValue;
            } else {
                std::cerr << "Error: Missing value for argument '" << arg << "'\n";
            }
        } else {
            std::cerr << "Warning: Ignoring unexpected argument '" << arg << "'\n";
        }
    }

    SkyParams params;
    std::string filename = "default.hdr";
    for (const auto& pair : args)
    {
        if (pair.first == "-turbidity")
        {
            params.turbidity = stof(pair.second);
        }
        if (pair.first == "-skyScale")
        {
            params.skyScale = stof(pair.second);
        }
        if (pair.first == "-sunScale")
        {
            params.sunScale = stof(pair.second);
        }
        if (pair.first == "-sunRadiusScale")
        {
            params.sunRadiusScale = stof(pair.second);
        }
        if (pair.first == "-sunDirectionX")
        {
            params.sunDir.x = stof(pair.second);
        }
        if (pair.first == "-sunDirectionY")
        {
            params.sunDir.y = stof(pair.second);
        }
        if (pair.first == "-sunDirectionZ")
        {
            params.sunDir.z = stof(pair.second);
        }
        if (pair.first == "-width")
        {
            params.width = stoi(pair.second);
            params.height = params.width / 2;
        }
        if (pair.first == "-height")
        {
            params.height = stoi(pair.second);
            params.width = params.height * 2;
        }
        if (pair.first == "-filename")
        {
            filename = pair.second;
        }
        std::cout << pair.first << " = " << pair.second << "\n";
    }

    params.init();

    Sky sky;
    sky.init(params);
    sky.save(filename.c_str());

	return 0;
}