/*
*     File Name           :     util_sample.hxx
*     Created By          :     largelymfs
*     Creation Date       :     [2016-01-18 14:08]
*     Last Modified       :     [2016-01-18 14:16]
*     Description         :     some util function for sample algorithms 
**/

#ifndef __UTIL_SAMPLE__
#define __UTIL_SAMPLE__

#include<stdlib.h>
#include<time.h>
#include<vector>

void random_init(){
    srand(time(NULL));
}

double random_double(){
    double d = rand() / 1. / RAND_MAX;
    return d;
}

int sample_from_distribution(std::vector<double>& dist){
    int n = dist.size();
    double f = random_double();
    int tmp = 0;
    while (f > dist[tmp]) tmp++;
    return tmp;
}

int sample_binary(double probability){
    double f = random_double();
    if (f < probability) return 1;
    return 0;
}


#endif
