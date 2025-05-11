#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include "logger.hpp"



int main()
{

    std::ofstream out;
    out.open("hello.txt", std::ios::app);

    Logger *logger = LoggerBuilder()
                         .set_level(Logger::INFO)
                         .add_handler(std::cout)
                         .add_handler(out)
                         .add_handler(std::make_unique<std::ofstream>("test.txt"))
                         .make_object();
    for (size_t i = 0; i < 16; i++)
    {
        logger->PrintMessage("Hello-hello!", Logger::INFO);
    }

    out.close();
    delete logger;
    return 0;
}