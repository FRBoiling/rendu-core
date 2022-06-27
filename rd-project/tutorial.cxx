#include <string>
#include <iostream>
#include "tutorial_config.h"

#include "math_functions.h"

int main(int argc, char *argv[])
{
    // if(argc<2){
    //     fprintf(stdout, "Uage: %s number\n", argv[0]);
    //     return 1;
    // }
    if (argc < 2)
    {
        // report version
        std::cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "."
                  << Tutorial_VERSION_MINOR << std::endl;
        std::cout << "Usage: " << argv[0] << " number" << std::endl;
        return 1;
    }
    // double inputValue = atof(argv[1]);
    const double inputValue = std::stod(argv[1]);

    const double outputValue = mathfunctions::sqrt(inputValue);

    fprintf(stdout, "The square root of %g is %g\n", inputValue, outputValue);
    return 0;
}
