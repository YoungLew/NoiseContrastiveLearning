/*
*     File Name           :     data.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-03 10:18]
*     Last Modified       :     [2016-01-18 14:06]
*     Description         :     2d-array data 
**/

#ifndef __DATA__
#define __DATA__
#include <iostream>
struct Data{
    Data(int n){
        this->size = n;
        this->elem = new double*[n];
        for (int i = 0; i < n; i++) this->elem[i] = new double[n];
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
    void print(){
        for (int i = 0; i < this->size; i++){
            for (int j = 0; j < this->size; j++)
                std::cout << this->elem[i][j] << " ";
            std::cout << std::endl;
        }
    }
};

#endif

