/*
*     File Name           :     nce_learning.cxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-21 16:04]
*     Last Modified       :     [2016-01-26 12:54]
*     Description         :     nce learning process 
**/

#include "./mrfmodel.hxx"
#include "./data.hxx"
#include "./Optimizer.hxx"
#include "./util_sample.hxx"
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
    char TRAIN_FILENAME[MAX_STR_LEN]="./train.data";
    char TEST_FILENAME[MAX_STR_LEN] = "./test.data";
    char MODLE_FILE[MAX_STR_LEN] = "./demo.model";
    char EXP_FILENAME[MAX_STR_LEN] = "./experiment_result_5x5";
    std::vector<Data> training_data, testing_data, noise_data;
    training_data.clear();testing_data.clear();noise_data.clear();
    load_data_from_file(TRAIN_FILENAME, training_data, 5);
    load_data_from_file(TEST_FILENAME, testing_data, 5);
    MRFModel * model = new MRFModel(5);
    //model->random_initialize();
    model->noise_initialize(training_data);
    MRFModel *noise = new MRFModel(5);
    noise->noise_initialize(training_data);
    //generate the noise_data
    //noise->sample_several_points_gibbs_directional(noise_data, training_data.size());

    MRFModel *std = new MRFModel(5);
    std->load_from_file(MODLE_FILE);
    std::cout << std->evaluate_log_probability(testing_data) << std::endl;

    //save to parameters
    std::vector<double> parameters;parameters.clear();
    model->save_to_vector(parameters);
    /*
    Optimizer *opt = new Optimizer(training_data, noise_data, *noise);
    opt->LBFGS(parameters, 10);
    int pnt = 0;
    noise->load_from_vector(parameters);
    std::cout << noise->evaluate_log_probability(testing_data) << std::endl;
    noise_data.clear();
    noise->sample_several_points_gibbs_directional(noise_data, training_data.size());
    delete opt;
    opt = new Optimizer(training_data, noise_data, *noise);
    opt->LBFGS(parameters, 10);
    noise->load_from_vector(parameters);
    std::cout << noise->evaluate_log_probability(testing_data) << std::endl;
     */
    Optimizer *opt = NULL;
    ofstream output(EXP_FILENAME); 
    for (int iteration = 0; iteration < 100; iteration++) {
        noise_data.clear();
        noise->sample_several_points_gibbs_directional(noise_data, training_data.size() * 1.0);
        opt = new Optimizer(training_data, noise_data, *noise, 1.0);
        opt->LBFGS(parameters, 30);
        noise->load_from_vector(parameters);
        //std::cout << noise->evaluate_log_probability(testing_data) << " " << noise->compare_error(std) << std::endl;
        std::cout << noise->evaluate_log_probability(testing_data) << " " << noise->logZ << " " << noise->evaluate_log_probability(testing_data) + (noise->logZ) * testing_data.size() << std::endl;
        std::cout << noise->compare_error(std)  << std::endl;
        //output << noise->evaluate_log_probability(testing_data) << " " << noise->compare_error(std) <<  std::endl;
        noise->save_to_file("Model.output");
    }
    output.close();
    delete opt;
    delete model;
    delete noise;
    delete std;
    return 0;
}
