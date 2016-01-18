/*
*     File Name           :     model.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-18 13:36]
*     Last Modified       :     [2016-01-18 16:23]
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
        int n;
        double logZ;
        MRFModel(int N = 5);
        ~MRFModel();
        LOGDOUBLE calculate_norm_log_prob(Data & d);
        LOGDOUBLE calculate_unnorm_log_prob(Data &d);
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
#endif
