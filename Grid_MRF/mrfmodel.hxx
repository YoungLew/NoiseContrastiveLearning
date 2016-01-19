/*
*     File Name           :     model.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-18 13:36]
*     Last Modified       :     [2016-01-19 11:53]
*     Description         :     storage grid MRF Model 
**/

#ifndef __MRF_MODEL_H__
#define __MRF_MODEL_H__

#include"./data.hxx"
#include "./util_sample.hxx"
#include "./util_logdouble.hxx"
class MRFModel{
    public:
        double **theta_a, **theta_b, **phi;
        int n, state_number;
        double logZ;
        MRFModel(int N = 5);
        ~MRFModel();
        LOGDOUBLE calculate_norm_log_prob(Data & d);
        LOGDOUBLE calculate_unnorm_log_prob(Data &d);
        void sample_several_points(std::vector<Data> & datas, int num_samples);
    private:
        void calculate_forward(double*** prob_forward);
        void calculate_backward(double*** prob_backward);
        void probability_initialize(double*** prob_forward, double*** prob_backward);
        void probability_finalize(double*** prob_forward, double*** prob_backward);
};

MRFModel::MRFModel(int N){
    this->theta_a = new double*[N - 1];
    this->theta_b = new double*[N];
    this->phi = new double*[N];
    for (int i = 0; i < N; i++) this->phi[i] = new double[N];
    for (int i = 0; i < N - 1; i++) this->theta_a[i] = new double[N];
    for (int i = 0; i < N; i++) this->theta_b[i] = new double[N -1];
    this->n = N;
    this->logZ = 0.0;
}

MRFModel::~MRFModel(){
    int N = this->n;
    for (int i = 0; i < N; i++) delete[] this->phi[i];
    for (int i = 0; i < N; i++) delete[] this->theta_b[i];
    for (int i = 0; i < N - 1; i++) delete[] this->theta_a[i];
    delete[] this->phi;
    delete[] this->theta_a;
    delete[] this->theta_b;
}
LOGDOUBLE MRFModel::calculate_unnorm_log_prob(Data& d){
    int n = this->n;
    LOGDOUBLE tmp = 0.0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (d.get(i, j) == 1)
                tmp += this->phi[i][j];
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n; j++)
            if ((d.get(i, j) == d.get(i + 1, j)) && (d.get(i, j) == 1))
                tmp += this->theta_a[i][j];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n - 1; j++)
            if ((d.get(i, j) == d.get(i, j + 1)) && (d.get(i, j) == 1))
                tmp += this->theta_b[i][j];
    return tmp;
}
LOGDOUBLE MRFModel::calculate_norm_log_prob(Data& d){
    return this->calculate_unnorm_log_prob(d) - this->logZ;
}
void MRFModel::sample_several_points(std::vector<Data> &datas, int num_samples){
    int state_number = 1;
    for (int i = 0; i < n; i++) state_number *= 2;
    this->state_number = state_number; 
    double*** prob_forward, ***prob_backward;
    this->probability_initialize(prob_forward, prob_backward);

    //this->calculate_forward(prob_forward);
    //this->calculate_backward(prob_backward);
    for (int i = 0; i < this->n - 1; i++)
        for (int j = 0; j < this->state_number; j++)
            for (int k = 0; k < this->state_number; k++){
                std::cout << i << ' ' << j << ' ' << k << ' ';
                std::cout << prob_forward[i][j][k] << " ";
            }
    this->probability_finalize(prob_forward, prob_backward);
}
void MRFModel::probability_initialize(double*** prob_forward, double*** prob_backward){
    int n = this->n;
    int state_number = this->state_number;
    prob_forward = new double**[n - 1];
    prob_backward = new double**[n - 1];
    for (int i= 0; i < n-1; i++){
        prob_forward[i] = new double*[state_number];
        for (int j = 0; j < state_number; j++) prob_forward[i][j] = new double[state_number];
        prob_backward[i] = new double*[state_number];
        for (int j = 0; j < state_number; j++) prob_backward[i][j] = new double[state_number];
    }
}
void MRFModel::probability_finalize(double*** prob_forward, double*** prob_backward){
    int n = this->n;
    int state_number = this->state_number;
    for (int i = 0; i < n - 1; i++){
        for (int j = 0; j < state_number; j++) delete[] prob_forward[i][j];
        delete[] prob_forward[i];
    }
    delete[] prob_forward;
    for (int i = 0; i < n - 1; i++){
        for (int j = 0; j < state_number; j++) delete[] prob_backward[i][j];
        delete[] prob_backward[i];
    }
    delete[] prob_backward;
}
void MRFModel::calculate_forward(double*** prob_forward){
    int n = this->n;
    int state_number = this->state_number;   
    for (int i = 0; i < n - 1; i++)
       for (int si = 0; si < state_number ; si++){
            for (int sj = 0; sj < state_number; sj++){
                
            }
       }

}
void MRFModel::calculate_backward(double*** prob_backward){
    int n = this->n;
    int state_number = this->state_number;
}
#endif
