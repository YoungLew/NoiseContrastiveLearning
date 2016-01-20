#! /usr/bin/env python
#
#     File Name           :     generate_grid_mrf_model.py
#     Created By          :     largelymfs
#     Creation Date       :     [2016-01-20 14:42]
#     Last Modified       :     [2016-01-20 14:50]
#     Description         :     the pyscripts to generate mrf grid model 
#


def output_2darray(array, fout):
    for item in array:
        for value in item:
            print >>fout, value,
        print >>fout

if __name__=="__main__":
    import sys
    filename_output = sys.argv[2]
    cnt_variable = int(sys.argv[1])
    import numpy as np
    #generate phi
    phi = np.random.normal(0.0, 0.1, cnt_variable * cnt_variable).reshape((cnt_variable, cnt_variable))
    theta_a = np.random.normal(0.0, 1.0, cnt_variable * (cnt_variable - 1)).reshape((cnt_variable - 1, cnt_variable))
    theta_b = np.random.normal(0.0, 1.0, cnt_variable * (cnt_variable - 1)).reshape((cnt_variable, cnt_variable - 1))
    with open(filename_output,"w") as fout:
        print >>fout, cnt_variable
        print >>fout
        output_2darray(phi, fout)
        print >>fout
        output_2darray(theta_a, fout)
        print >>fout
        output_2darray(theta_b, fout)
