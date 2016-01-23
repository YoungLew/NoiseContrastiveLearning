/*
*     File Name           :     data.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-03 10:18]
*     Last Modified       :     [2016-01-22 17:53]
*     Description         :     2d-array data 
**/

#ifndef __DATA__
#define __DATA__
#include <iostream>
#include <fstream>
using namespace std;
struct Data{
    Data(int n){
        this->size = n;
        this->elem = new double*[n];
        for (int i = 0; i < n; i++) this->elem[i] = new double[n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) this->elem[i][j] = 0.0;
    }
    Data(const Data& d){
        this->size = d.size;
        this->elem = new double*[this->size];
        for (int i = 0; i < this->size; i++) this->elem[i] = new double[this->size];
        for (int i = 0; i < this->size; i++)
            for (int j = 0; j < this->size; j++)
                this->elem[i][j] = d.const_get(i, j);
    }
    ~Data(){
        for (int i = 0; i < this->size; i++) delete[] this->elem[i];
        delete[] this->elem;
    }
    int size;
    double** elem;
    double& get(int i, int j){
        return this->elem[i][j];
    }
    double& get_with_1d(int index){
        int i, j;
        i = index / this-> size;
        j = index - (i * this->size);
        return this->elem[i][j];
    }
    double const_get(int i, int j) const{
        return this->elem[i][j];
    }
    void print(){
        for (int i = 0; i < this->size; i++){
            for (int j = 0; j < this->size; j++)
                std::cout << this->elem[i][j] << " ";
            std::cout << std::endl;
        }
    }
    void print_to_file(ofstream& output){
        for (int i = 0; i < this->size; i++){
            for (int j = 0; j < this->size; j++)
                output << this->elem[i][j] << " ";
            output << std::endl;
        }
    }
    void load_from_file(ifstream& input){
        for (int i = 0; i < this->size; i++)
            for (int j = 0; j < this->size; j++)
                input >> this->elem[i][j];
    }
};

#endif

