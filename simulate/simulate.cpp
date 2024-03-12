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
bool helper(std::vector<double> lhs, std::vector<double> rhs){
    // sorts a 2d double vector (in this case resoot) by element 1 descending (in this case scores)
    return(lhs[1]>rhs[1]);
}
int main(int argc, char** argv){
    std::vector<std::string> names;
    std::vector<double> means;
    std::vector<double> stdev;
    // Parse the input text files
    std::ifstream f1("names.txt");
    std::string s;
    int maxlen=0;
    while(std::getline(f1,s)){
        // changes windows strings into unix-style strings
        if(s[s.length()-1]=='\r'){
            s=s.substr(0,s.length()-1);
        }
        if(s.length()>maxlen) maxlen = s.length();
        names.push_back(s);
        
    }
    f1.close();
    std::ifstream f2("means.txt");
    s.clear();
    while(f2 >> s){means.push_back(stod(s));}
    f2.close();
    std::ifstream f3("stdev.txt");
    s.clear();
    while(f3 >> s){stdev.push_back(stod(s));}
    f3.close();
    assert(names.size() == means.size() && means.size() == stdev.size());

    // Ask for number of simulations
    std::cout<<"How many simulations? ";
    unsigned long sims;
    std::cin>>sims;
    bool print = false;
    if(sims == 1lu){
        print = true;
    }
    else if (sims == 0lu){
        std::cout<<"Well that was an easy day at the office."<<std::endl;
        exit(69);
    }
    std::vector<std::vector<double>> resoot;
    std::default_random_engine generator;
    std::vector<std::vector<int>> matrix;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    for(unsigned long owo = 0; owo < sims; owo++){
        resoot.clear();
        for(unsigned long i =0; i<names.size();i++){
            if(owo == 0){
                std::vector<int> fof(names.size(),0);
                matrix.push_back(fof);
            } 
            std::vector<double> temp;
            temp.push_back(1.0*i);
            std::normal_distribution<double> distribution(means[i],stdev[i]);
            temp.push_back(distribution(generator)*100);
            resoot.push_back(temp);
        }
        std::sort(resoot.begin(),resoot.end(),helper);
        for(unsigned long i =0; i<names.size(); i++){
            int contestant = (int)resoot[i][0];
            matrix[contestant][i]++;
        }

        // time check
        if(owo%10000==0 && owo){
            std::chrono::high_resolution_clock::time_point t2= std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
            std::cout<<"First "<<owo<<" sims:"<<time_span.count() << " seconds"<<std::endl;
        }
        else if(owo+1 == sims){
            std::chrono::high_resolution_clock::time_point t2= std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
            std::cout<<"Full runtime:"<<time_span.count() << " seconds"<<std::endl;
        }
        
    }
    std::cout<<"Final simulation results"<<std::endl;
    if(true){
        int prizes = (5+names.size())/10;
        int deaths = (2+names.size())/5;
        if(names.size()==2) deaths = 1; // finales
        if(names.size()==7) deaths = 2; // 7->5 supremacy
        for(unsigned long i =0; i<names.size(); i++){
            printf("%3ld - %s - %.2f\%\n",i+1,names[(int)resoot[i][0]].c_str(),resoot[i][1]);
            if(1+i==prizes && prizes >0) std::cout<<"---PRIZE ZONE---"<<std::endl;
            if(1+i+deaths==names.size() && deaths >0) std::cout<<"---DEATH ZONE---"<<std::endl;
        }
    }
    std::cout<<"Predictions Matrix"<<std::endl;
    for(unsigned long i =0; i<names.size(); i++){
        if(maxlen<=20){
            printf("%20s",names[i].c_str());
        }
        else{
            printf("%32s",names[i].c_str());
        }
        for(unsigned long j =0; j<names.size();j++){
            printf("\t%6.2f",100.0*matrix[i][j]/sims);
            std::cout<<"%";
        }
        std::cout<<std::endl;
    }

    return 0;
}