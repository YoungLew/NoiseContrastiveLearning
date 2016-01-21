/*
*     File Name           :     generate_data.cxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-20 16:56]
*     Last Modified       :     [2016-01-21 08:57]
*     Description         :     generate training data set and testing data set using cpp language 
**/

#include "./mrfmodel.hxx"
#include "./data.hxx"
#include "./util_sample.hxx"
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

#define MAX_STRING_LEN 100

int main(){
    char train_filename[MAX_STRING_LEN]="./train.data";
    char test_filename[MAX_STRING_LEN]="./test.data";
    int training_instance_number = 5000;
    int testing_instance_number = 1000;
    char model_filename[MAX_STRING_LEN] ="./demo.model";
    int grid_size = 5;
    
    std::vector<Data> training_data;
    std::vector<Data> testing_data;
    training_data.clear(); testing_data.clear();
    for (int i = 0; i < training_instance_number; i++) training_data.push_back(Data(grid_size));
    for (int i = 0; i < testing_instance_number; i++) testing_data.push_back(Data(grid_size));
    MRFModel *m = new MRFModel(grid_size);
    m->load_from_file(model_filename);
    m->sample_several_points_gibbs_directional(training_data, training_instance_number);
    m->sample_several_points_gibbs_directional(testing_data, testing_instance_number);
    ofstream output(train_filename);
    for (int i = 0; i < training_instance_number; i++) {
        training_data[i].print_to_file(output); 
        output << std::endl;
    }

    output.close();
    ofstream output1(test_filename);
    for (int i = 0; i < testing_instance_number; i++) {
        testing_data[i].print_to_file(output1);
        output << std::endl;
    }
    output1.close();
    delete m;
    return 0;
    
}
