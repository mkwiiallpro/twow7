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
std::vector<std::string> csv_handler(const std::string &s){
    if(s.length() ==0) return NULL;
    std::vector<std::string> result;
    std::string buffer;
    for(char c: s){

    }
    return result;
}
int main(int argc, char** argv){
    std::cout<<"Hello World"<<std::endl;
    std::cout<<"The number of arguments is "<<argc<<std::endl;
    
    /* Rules about Rules
       - The CSV must be in the order you want reveals to be in
       - The line format is name, response, Mu, Sigma
       - DRPs and Dummies are not supported at this time.
       TODO: Integrate this all with a vote parser 
     */
    std::vector<std::string> names;
    std::vector<std::string> responses;
    std::vector<double> means;
    std::vector<double> stdev;
    std::ifstream f1("results.csv"); 
    std::string s;
    while(std::getline(f1,s)){
        // changes windows strings into unix-style strings
            if(s[s.length()-1]=='\r'){
            s=s.substr(0,s.length()-1);
        }
    }
    f1.close();

    
    return 0;
}