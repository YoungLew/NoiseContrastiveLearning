/*
*     File Name           :     test.cxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-18 14:14]
*     Last Modified       :     [2016-01-18 14:26]
*     Description         :     test code for MRF Experiment 
**/

#include "./mrfmodel.hxx"
#include "./data.hxx"
#include "./util_sample.hxx"
#include <iostream>
using namespace std;
void generate_data(Data* d){
    random_init();
    int n = d->size;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            d->get(i, j) = sample_binary(0.5);
}
void generate_model(MRFModel *m){
    int n = m->n;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            m->phi[i][j] = sample_binary(0.5);
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n; j++)
            m->theta_a[i][j] = sample_binary(0.5) * 10;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n - 1; j++)
            m->theta_b[i][j] = sample_binary(0.5) * 10;
}
    
void test_data(){
    int n = 5;
    Data* d = new Data(n);
    generate_data(d);
    d->print();
    delete d;
}

void test_model(){
    int n = 5;
    Data* d = new Data(n);
    generate_data(d);
    MRFModel * m = new MRFModel(n);
    generate_model(m);
    std::cout << m->calculate_norm_log_prob(*d) << std::endl;
    delete d;
    delete m;
}

int main(){
    test_data();
    test_model();
    return 0;
}

