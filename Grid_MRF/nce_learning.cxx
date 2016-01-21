/*
*     File Name           :     nce_learning.cxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-21 16:04]
*     Last Modified       :     [2016-01-21 16:14]
*     Description         :     nce learning process 
**/

#include "./mrfmodel.hxx"
#include "./data.hxx"
#include <fstream>
using namespace std;

#define MAX_STR_LEN  50

void load_data_from_file(const char* filename, std::vector<Data>& out_vec, int grid_size){
    out_vec.clear();
    ifstream input(filename);
    int cnt_data;
    input >> cnt_data;
    for (int i = 0; i < cnt_data; i++) out_vec.push_back(Data(grid_size));
    for (int i = 0; i < cnt_data; i++) out_vec[i].load_from_file(input);
    input.close();
}
int main(){
    std::vector<Data> vec;
    char TRAIN_FILENAME[MAX_STR_LEN]="./train.data";
    char TEST_FILENAME[MAX_STR_LEN] = "./test.data";
    load_data_from_file(TRAIN_FILENAME, vec, 5);
    vec[0].print();
    vec[1].print();
    vec[2].print();
    return 0;
}
