#! /usr/bin/env python
#
#     File Name           :     plot_from_exp.py
#     Created By          :     largelymfs
#     Creation Date       :     [2016-01-25 16:29]
#     Last Modified       :     [2016-01-25 22:54]
#     Description         :     plot the negative-log-likelihood 
#

if __name__=='__main__':
    import matplotlib.pyplot as plt
    data = []
    with open("./experiment_result_5x5") as fin:
        while True:
            line = fin.readline()
            if not line:
                break
            data.append(float(line.strip()) * -1.0)
    plt.plot(range(len(data)), data)
    ANS= 71805.9
    plt.plot(range(len(data)), [ANS for _ in range(len(data))])
    plt.show();
