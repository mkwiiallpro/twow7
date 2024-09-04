#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cassert>
int main(int argc, char** argv){
    std::cout<<"Hello World"<<std::endl;
    std::cout<<"The number of arguments is "<<argc<<std::endl;
    
    /* Rules about Rules
       - The CSV must be in the order you want reveals to be in
       - The line format is name, response, Mu, Sigma
       - DRPs and Dummies are not supported at this time.
       TODO: Integrate this all with a vote parser 
     */
    std::ifstream f1("results.csv"); 
    std::vector<std::string> names;
    std::vector<std::string> responses;
    std::vector<double> means;
    std::vector<double> stdev;
    return 0;
}