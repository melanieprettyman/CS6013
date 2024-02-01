#include "cmdline.h"
#include <string>
#include <iostream>

// Created by Melanie Prettyman on 1/11/24.

void use_arguments(int argc, char **argv) {
    //Bool testSeen set to false
    bool testSeen = false;

    //extract argument from argV
    for (int i = 1; i < argc; i++) {
        std::string argument = argv[i];

        //ff argument is --help, print help menu and exit
        if (argument == "--help") {
            std::cout << "Choose from the following flags:" << '\n';
            std::cout << "--test" << '\n';
            std::cout << "--interp" << '\n';
            exit(0);
        }
        //If argument is --test and test has not been seen
        //print 'test passed' and set testSeen to true
        if (argument == "--test") {
            if (!testSeen) {
                std::cout << "Tests passed" << std::endl;
                testSeen = true;
            } else {
                std::cerr << "Error: '--test' argument already seen." << '\n';
                exit(1);
            }
        } else {
            std::cerr << "Error: bad flag." << '\n';
            exit(1);
        }
    }
}












/*
introduce a new class for variable expressions (VarExpr) based on the given grammar.
Implement the equals method for this new class. Update the test cases to include tests for variable expressions.
CHECK((new VarExpr("x"))->equals(new VarExpr("x")) == true);
CHECK((new VarExpr("x"))->equals(new VarExpr("y")) == false);
CHECK((new NumExpr(1))->equals(new VarExpr("x")) == false);
CHECK((new AddExpr(new NumExpr(2), new NumExpr(3)))->equals(new AddExpr(new NumExpr(2), new NumExpr(3))) == true);
CHECK((new AddExpr(new NumExpr(2), new NumExpr(3)))->equals(new AddExpr(new NumExpr(3), new NumExpr(2))) == false);
// Add more test cases for variable expressions
*/