/*
*     File Name           :     model.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-18 13:36]
*     Last Modified       :     [2016-01-21 08:55]
*     Description         :     storage grid MRF Model 
**/

#ifndef __MRF_MODEL_H__
#define __MRF_MODEL_H__

#include"./data.hxx"
#include "./util_sample.hxx"
#include "./util_logdouble.hxx"
#include <fstream>
#include <math.h>
using namespace std;
class MRFModel{
    public:
        double **theta_a, **theta_b, **phi;
        int n, state_number;
        double logZ;
        MRFModel(int N = 5);
        ~MRFModel();
        LOGDOUBLE calculate_norm_log_prob(Data & d);
        LOGDOUBLE calculate_unnorm_log_prob(Data &d);
        void load_from_file(const char* filename);
        void print();
        void sample_several_points_gibbs_directional(std::vector<Data> & datas, int num_samples);
        void sample_several_points_gibbs_bidirectional(std::vector<Data>& datas, int num_samples);
    private:
        void initialize(int N);
        void finalize();
        void calculate_forward(double*** prob_forward);
        void calculate_backward(double*** prob_backward);
        void probability_initialize(double*** prob_forward, double*** prob_backward);
        void probability_finalize(double*** prob_forward, double*** prob_backward);
};
MRFModel::MRFModel(int N){
    this->initialize(N);
}
MRFModel::~MRFModel(){
    this->finalize();
}
void MRFModel::initialize(int N){
    this->theta_a = new double*[N - 1];
    this->theta_b = new double*[N];
    this->phi = new double*[N];
    for (int i = 0; i < N; i++) this->phi[i] = new double[N];
    for (int i = 0; i < N - 1; i++) this->theta_a[i] = new double[N];   // the v-pairwise parameters
    for (int i = 0; i < N; i++) this->theta_b[i] = new double[N - 1];   // the h-pairwise parameters
    this->n = N;                                                        // the size of grid
    this->logZ = 0.0;
}
void MRFModel::finalize(){
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
void MRFModel::sample_several_points_gibbs_directional(std::vector<Data> &datas, int num_samples){
    int n = this->n;
    int state_number = 1;
    for (int i = 0; i < n; i++) state_number *= 2;                  // total state number of one line
    this->state_number = state_number; 
    double*** prob_forward, ***prob_backward;
    prob_forward = new double**[n];
    prob_backward = new double**[n];
    this->probability_initialize(prob_forward, prob_backward);
    this->calculate_forward(prob_forward);
    this->calculate_backward(prob_backward);
    int *offset = new int[n];
    offset[n - 1] = 1;
    for (int i = n - 2; i >= 0; i--) offset[i] = offset[i + 1]  * 2;
    for (int k = 0; k < num_samples; k++){
        int old_state = 0;
        for (int i = 0; i < this-> n; i++){
            if (i == 0) old_state = sample_from_distribution(prob_forward[0][0], this->state_number);
            else old_state = sample_from_distribution(prob_forward[i][old_state], this->state_number);
            for (int j = 0; j < n; j++)datas[k].get(i, j) = (int)((old_state & offset[j]) != 0);
        }
    }
    delete[] offset;
    this->probability_finalize(prob_forward, prob_backward);
}
void MRFModel::sample_several_points_gibbs_bidirectional(std::vector<Data>& datas, int num_samples){
    int n = this-> n;
    int state_number = 1;
    for (int i = 0; i < n; i++) state_number *= 2;
    this->state_number = state_number;
    double*** prob_forward, ***prob_backward;
    prob_forward = new double**[n];
    prob_backward = new double **[n];
    this->probability_initialize(prob_forward, prob_backward);
    this->calculate_forward(prob_forward);
    this->calculate_backward(prob_backward);
    int *offset = new int[n];
    offset[n - 1] = 1;
    for (int i = n - 2; i >= 0; i--) offset[i] = offset[i + 1]  * 2;
    for (int k = 0; k < num_samples; k++){
        int old_state = 0;
        for (int i = 0; i < n; i++){
            if (i == 0) old_state = sample_from_distribution(prob_forward[0][0], this->state_number);
            else old_state = sample_from_distribution(prob_forward[i][old_state], this->state_number);
        }
        for (int j = 0; j < n; j++) datas[k].get(n-1, j) = (int)((old_state & offset[j]) != 0);
        for (int i = n - 2; i >= 0; i--){
            old_state = sample_from_distribution(prob_backward[i][old_state], this->state_number);
            for (int j = 0; j < n; j++) datas[k].get(i, j) = (int)((old_state & offset[j]) != 0); 
        }
    }
    delete[] offset;
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
    int * offset = new int[n];
    offset[n - 1] = 1;
    for (int i = n - 2; i >= 0; i--) offset[i] = offset[i + 1] * 2;
    for (int i = n - 1; i >= 0; i--){
        for (int si = 0; si < state_number; si++){
            for (int sj = 0; sj < state_number; sj++){
                // given si, which is the state of row i + 1 while sj is the state of row i
                // we need to calculat the conditional probability
                double tmp = 0.0;
                for (int k = 0; k < n; k++) if ((offset[k] & sj) != 0) tmp += this->phi[i][k];
                for (int k = 0; k < n - 1; k++) if (((offset[k] & sj) != 0) && ((offset[k + 1] & sj) != 0)) tmp += this->theta_b[i][k];
                if (i != n - 1)
                   for (int k = 0; k < n; k++) if (((offset[k] & si)!=0) && ((offset[k] & sj)!= 0)) tmp += this->theta_a[i][k]; 
                prob_backward[i][si][sj] = exp(tmp);
            }
            double normal = 0.0;
            for (int sj = 0; sj < state_number; sj++) normal += prob_backward[i][si][sj];
            for (int sj = 0; sj < state_number; sj++) prob_backward[i][si][sj] /= ((double)(normal));
            for (int sj = 1; sj < state_number; sj++) prob_backward[i][si][sj] += prob_backward[i][si][sj - 1];
            for (int sj = 0; sj < state_number; sj++) std::cout << prob_backward[i][si][sj] << " ";
            std::cout << std::endl;
        }
    }

    delete[] offset; 
}
void MRFModel::load_from_file(const char* filename){
    ifstream input(filename);
    //load the model
    this->finalize();
    int cnt_variable;
    input >> cnt_variable;
    this->initialize(cnt_variable);
    for (int i = 0; i < cnt_variable; i++)
        for (int j = 0; j < cnt_variable; j++)
            input >> this->phi[i][j];
    for (int i = 0; i < cnt_variable - 1; i++)
        for (int j = 0; j < cnt_variable; j++)
            input >> this->theta_a[i][j];
    for (int i = 0; i < cnt_variable; i++)
        for (int j = 0; j < cnt_variable - 1; j++)
            input >> this->theta_b[i][j];
    input.close();
}
void MRFModel::print(){
    std::cout << "======================PHI=======================" << std::endl;
    for (int i = 0; i < this-> n; i++){
        for (int j = 0; j < this-> n; j++) std::cout << this->phi[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << "=======================THETA_A======================" << std::endl;
    for (int i = 0; i < this-> n - 1; i++){
        for (int j = 0; j < this-> n; j++) std::cout << this->theta_a[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << "======================THETA_B=======================" << std::endl;
    for (int i = 0; i < this-> n; i++){
        for (int j = 0; j < this-> n - 1; j++) std::cout << this->theta_b[i][j] << " ";
        std::cout << std::endl;
    }
}
#endif
