#include <iostream>
#include "Program.h"
#include "Project.h"

int main(int argc, char *argv[])
{
    using namespace sda;

    Program program;
    auto project = Project::Create(&program, "Test");

    return 0;
}