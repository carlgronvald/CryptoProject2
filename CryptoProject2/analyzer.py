# -*- coding: utf-8 -*-
"""
Created on Wed Mar  9 13:16:42 2022

@author: carl
"""

import numpy as np
import matplotlib.pyplot as plt

# Find the maximum correlation value at the given timestep, along with the key that gives it
# returns (correlation value, key)
# An absolute correlation matrix should be used(negative and positive correlation are equally good)
def findmax(corr, timestep):
    return (np.max(corr[:,timestep]), np.argmax(corr[:,timestep]))

# Calculates the moving average with width n for vector a.
def moving_average(a, n=3) :
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n

# Smooths one row in the correlation matrix by moving average with width given by smoothing
def smooth_corr(corr, key, smoothing=3):
    return moving_average(corr[key,:], smoothing)

# Smooths the entire correlation matrix by moving average with width given by smoothing
def smooth_corr_matrix(corr, smoothing=3):
    smoothed_corr = np.zeros( (corr.shape[0], corr.shape[1]-smoothing+1) )
    for i in range(0,corr.shape[0]):
        smoothed_corr[i, :] = smooth_corr(corr, i, smoothing)
    
    return smoothed_corr

# Finds the best keys at different smoothing scales.
# The 'best' key is the key that has the highest absolute correlation somewhere
# in the correlation matrix.
# The correlation matrix is smoothed in absolute form, since it turns out
# that correlating well just means higher absolute value for the correlation
# signal. That correlation is not necessarily in any way localized.
def best_keys(corr):
    scale_keys = []
    for scale in range(1, 15, 1):
        smoothed_corr = (smooth_corr_matrix(np.abs(corr), scale))
        scale_max_value = 0;
        scale_best_key = 0;
        for timestep in range(0, smoothed_corr.shape[1]):
            value, key = findmax(smoothed_corr, timestep)
            if value > scale_max_value:
                scale_max_value = value
                scale_best_key = key
        scale_keys += [scale_best_key]
        
    return scale_keys


# Return the key that is the 'best' key in the most scales using the output
# from the best_keys function above.
def best_key(best_keys):
    (uniques, counts) = np.unique(best_keys, return_counts = True)
    return uniques[np.argmax(counts)]    

# Finds the best keys for the noise series generated by the project, and
# checks whether they're correct.
# Returns (noise range, number of correct keys found using best keys)
def check_noise_series(correct_key):
    hits = []
    first_hit = []
    correct = []
    noise_range = range(10,295,5);
    for noise_amplitude in noise_range:
        corr = np.genfromtxt('output/correlation_matrix_noise' + str(noise_amplitude) + ".csv", delimiter = ',')
        keys = best_keys(corr)
        amplitude_hits = 0;
        for k in keys:
            if k == correct_key:
                amplitude_hits += 1
        hits += [amplitude_hits]
        if keys[0] == correct_key:
            first_hit += [1]
        else:
            first_hit += [0]
        if best_key(keys) == correct_key:
            correct += [1]
        else:
            correct += [0]
    return (noise_range, hits, first_hit, correct)


corr = np.genfromtxt('output/correlation_matrix.csv', delimiter=',')
keys = best_keys(corr)
correct_key = best_key(keys)
print("Keys identified at different scales:", keys)
print("Best key is identified as:", correct_key)

(noise_range, hits, first_hit, correct) = check_noise_series(correct_key)
plt.plot(noise_range, np.array(first_hit))
plt.plot(noise_range, np.array(correct))

plt.title("Correctness vs white noise amplitude")
plt.xlabel("White noise amplitude on T matrix")
plt.ylabel("Key guess correctness")

plt.legend(["Best key without smoothing","Best key using all smoothing levels"])

plt.savefig("output/correctness.pdf")
