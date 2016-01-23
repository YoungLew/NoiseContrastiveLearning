/*
*     File Name           :     Optimizer.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-21 16:21]
*     Last Modified       :     [2016-01-22 17:58]
*     Description         :     optimizer for NCE Learning 
**/

#ifndef __OPTIMIZER__
#define __OPTIMIZER__

#include "./LBFGS.hxx"
#include "./mrfmodel.hxx"
#include<vector>
#define EXP_MAX 10000
#define EXP_MIN -10000
#define MIN_DOUBLE  0.0001
using namespace std;

class Optimizer: public Minimizer{
    public:
        Optimizer(std::vector<Data>& _training_data,
                  std::vector<Data>& _noise_data,
                  MRFModel & _noise_model): Minimizer(false), training_data(_training_data),
                  noise_data(_noise_data), noise_model(_noise_model){
                     this->grid_size = _noise_model.n; 
                     this->gamma = (double)(_noise_data.size()) / (double)(_training_data.size());
                }
        ~Optimizer(){}
        void ComputeGradient(vector<double>&g, const vector<double>& x, bool bCalculateGate);
        double ComputeFunction(const vector<double>& x);
        void Report(const vector<double>& theta, int iteration, double objective, double step_length);
        void Report(const string& s);
        double calculate_norm_log_prob(const std::vector<double>& parameters, Data& d);
        std::vector<Data>& training_data;
        std::vector<Data>& noise_data;
        MRFModel& noise_model;
        int grid_size;
        double gamma;
};

void Optimizer::ComputeGradient(vector<double>& g, const vector<double>& x, bool bCalculateGate){
    g.clear();
    for (int i = 0; i < x.size(); i++) g.push_back(0.0);
    for (int i = 0; i < this->training_data.size(); i++){
        double log_noise = this->noise_model.calculate_norm_log_prob(this->training_data[i]);
        double log_model = this->calculate_norm_log_prob(x, this->training_data[i]);
        double function_g = log_model - log_noise;
        function_g = function_g * this->gamma * (-1.0);
        double h = 0.0;
        if (function_g < EXP_MIN){
            h = 1.0;
        }else{
            if (function_g > EXP_MAX){
                h = 0.0;
            }else{
                h = 1.0/(1.0 + exp(function_g));
            }
        }
        //update the gradient
        int pnt = 0;
        int n = this-> grid_size; 
        for (int x = 0; x < n; x++)
            for (int y = 0; y < n; y++){
                if (this->training_data[i].get(x, y) == 1)
                    g[pnt] += (this->gamma) * (1.0 - h);
                pnt++;
            }
        for (int x = 0; x < n - 1; x++)
            for (int y = 0; y < n; y++){
                if ((this->training_data[i].get(x,y) == 1) && (this->training_data[i].get(x+1,y) == 1))
                    g[pnt] += (this->gamma) * (1.0 - h);
                pnt++;
            }
       for (int x = 0; x < n; x++)
          for (int y = 0; y < n - 1; y++){
              if ((this->training_data[i].get(x,y) == 1) && (this->training_data[i].get(x,y+1) == 1))
                  g[pnt] += (this->gamma) * (1.0 - h);
              pnt++;
          }
        g[n * n * 3 - 2 * n] = -1 * (this-> gamma * (1.0 - h));
    }
    for (int i = 0; i < this->noise_data.size(); i++){
        double log_noise = this->noise_model.calculate_norm_log_prob(this->noise_data[i]);
        double log_model = this->calculate_norm_log_prob(x, this->noise_data[i]);
        double function_g = log_model - log_noise;
        double h = 0.0;
        if (function_g < EXP_MIN){ 
            h = 1.0;
        }else{
            if (function_g> EXP_MAX){
                h = 0.0;
            }else{
                h = 1.0/(1.0 + exp(function_g));
            }
        }
        //update the gradient
        int pnt = 0;
        int n = this->grid_size;
        for (int x = 0; x < n; x++)
            for (int y = 0; y < n; y++){
                if (this->noise_data[i].get(x,y) = 1)
                    g[pnt] += (this->gamma) * (-h);
                pnt++;
            }
        for (int x = 0; x < n - 1; x++)
            for (int y = 0; y < n; y++){
                if ((this->noise_data[i].get(x,y) == 1) && (this->noise_data[i].get(x+1,y)==1))
                    g[pnt] += (this->gamma) * (-h);
                pnt++;
            }
        for (int x = 0; x < n; x++)
            for (int y = 0; y < n - 1; y++){
                if ((this->noise_data[i].get(x,y) == 1) && (this->noise_data[i].get(x,y+1)==1))
                    g[pnt] += (this->gamma) * (-h);
                pnt++;
            }
        g[n * n * 3 - 2 * n] += -1 * (this->gamma) * (-h);
    }
} 
double Optimizer::ComputeFunction(const vector<double>& x){
    //compute the loss function
    double target = 0.0;
    for (int i = 0; i < this->training_data.size(); i++){
        double log_noise = this->noise_model.calculate_norm_log_prob(this->training_data[i]);
        //calculate the log probability of now model
         double log_model = this->calculate_norm_log_prob(x, this->training_data[i]);
         double g = log_model - log_noise;
         g = g * this->gamma * (-1.0);
         double h = 0.0;
         if (g < EXP_MIN){
             h = 1.0;
         }else{
             if (g > EXP_MAX){
                 h = 0.0;
             }else{
                 h = (1.0) / (exp(g) + 1.0);
             }
         }
        target += log(h + MIN_DOUBLE);
    }
    for (int i = 0; i < this->noise_data.size(); i++){
        double log_noise = this->noise_model.calculate_norm_log_prob(this->noise_data[i]);
        double log_model = this->calculate_norm_log_prob(x, this->noise_data[i]);
        double g = log_model - log_noise;
        g = g * this->gamma * (-1.0);
        double h = 0.0;
        if (g < EXP_MIN){
            h = 1.0;
        }else{
           if (g > EXP_MAX){
               h = 0.0;
           }else{
               h = (1.0) / (exp(g) + 1.0);
           } 
        }
        target += log(1 - h + MIN_DOUBLE);
    }
    return target;
}
double Optimizer::calculate_norm_log_prob(const std::vector<double>& parameters, Data& d){
    double res = 0.0;
    int pnt = 0;
    for (int i = 0; i < this->grid_size; i++)
        for (int j = 0; j < this->grid_size; j++){
            if (d.get(i, j) == 1) res += parameters[pnt];
            pnt++;
        }
    for (int i = 0; i < this->grid_size - 1; i++)
        for (int j = 0; j < this->grid_size; j++){
            if ((d.get(i, j) == 1) && (d.get(i+1, j) == 1)) res += parameters[pnt];
            pnt++;
        }
    for (int i = 0; i < this->grid_size; i++)
        for (int j = 0; j < this->grid_size - 1; j++){
            if ((d.get(i, j) == 1) && (d.get(i, j+1) == 1)) res += parameters[pnt];
            pnt++;
        }
    res = res - parameters[pnt];
    return res;
}

void Optimizer::Report(const vector<double>& theta, int iteration, double objective, double step_length){
}
void Optimizer::Report(const string& s){
}
#endif
