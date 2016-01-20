/*
*     File Name           :     model.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-18 13:36]
*     Last Modified       :     [2016-01-20 14:40]
*     Description         :     storage grid MRF Model 
**/

#ifndef __MRF_MODEL_H__
#define __MRF_MODEL_H__

#include"./data.hxx"
#include "./util_sample.hxx"
#include "./util_logdouble.hxx"
#include <math.h>
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
    for (int i = 0; i < N - 1; i++) this->theta_a[i] = new double[N];   // the v-pairwise parameters
    for (int i = 0; i < N; i++) this->theta_b[i] = new double[N - 1];   // the h-pairwise parameters
    this->n = N;                                                        // the size of grid
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
LOGDOUBLE MRFModel::calculate_unnorm_log_prob(Data& d){             // Calculate the unnormlized probability
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
    for (int i = 0; i < n; i++) state_number *= 2;                  // total state number of one line
    this->state_number = state_number; 
    double*** prob_forward, ***prob_backward;
    prob_forward = new double**[n];
    prob_backward = new double**[n];
    this->probability_initialize(prob_forward, prob_backward);
    this->calculate_forward(prob_forward);
    //this->calculate_backward(prob_backward);
    this->probability_finalize(prob_forward, prob_backward);
}
void MRFModel::probability_initialize(double*** prob_forward, double*** prob_backward){     // function to initialize the probability data structure n * state * state
    int n = this->n;
    int state_number = this->state_number;
    for (int i= 0; i < n; i++){
        prob_forward[i] = new double*[state_number];
        for (int j = 0; j < state_number; j++) prob_forward[i][j] = new double[state_number];
        prob_backward[i] = new double*[state_number];
        for (int j = 0; j < state_number; j++) prob_backward[i][j] = new double[state_number];
    }
}
void MRFModel::probability_finalize(double*** prob_forward, double*** prob_backward){
    int n = this->n;
    int state_number = this->state_number;
    for (int i = 0; i < n; i++){
        for (int j = 0; j < state_number; j++) delete[] prob_forward[i][j];
        delete[] prob_forward[i];
    }
    delete[] prob_forward;
    for (int i = 0; i < n ; i++){
        for (int j = 0; j < state_number; j++) delete[] prob_backward[i][j];
        delete[] prob_backward[i];
    }
    delete[] prob_backward;
}
void MRFModel::calculate_forward(double*** prob_forward){                               // calculate the condition probability
    int n = this->n;
    int state_number = this->state_number;   
    int* offset = new int[n];
    offset[n - 1] = 1;

    for (int i = n - 2; i >= 0; i--) offset[i] = offset[i + 1] * 2;                     // offset array: x & 2**k => the kth value 
    for (int i = 0; i < n; i++) std::cout << offset[i] << " ";
    std::cout << std::endl;
    for (int i = 0; i < n; i++)
       for (int si = 0; si < state_number; si++){
            for (int sj = 0; sj < state_number; sj++){
                //given si, the probability of sj, which is the binary representation of one line
                //line i => sj, line i-1 => si, when i = 0, all si is the same since there is no condition
                double tmp = 0.0;
                for (int k = 0; k < n; k++) if ((offset[k] & sj) != 0) tmp += this->phi[i][k];
                for (int k = 0; k < n - 1; k++) if (((offset[k] & sj) != 0) && ((offset[k+1] & sj) != 0))  tmp += this->theta_b[i][k];
                if (i != 0)
                    for (int k = 0; k < n; k++) if (((offset[k] & sj) != 0) && ((offset[k] & si) != 0)) tmp += this->theta_a[i - 1][k];
                prob_forward[i][si][sj] = exp(tmp);
            }
            //normalize
            double norm = 0.0;
            for (int sj = 0; sj < state_number; sj++) norm += prob_forward[i][si][sj];
            for (int sj = 0; sj < state_number; sj++) prob_forward[i][si][sj] /= ((double)(norm));
            for (int sj = 1; sj < state_number; sj++) prob_forward[i][si][sj] += prob_forward[i][si][sj - 1];
            for (int sj = 0; sj < state_number; sj++) std::cout << prob_forward[i][si][sj] << " ";std::cout << std::endl;
       }

    delete[] offset;
}
void MRFModel::calculate_backward(double*** prob_backward){
    int n = this->n;
    int state_number = this->state_number;
}
#endif
