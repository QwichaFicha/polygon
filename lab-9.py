import numpy as np


def even_numbers(array):
    arr = np.array(array)
    unique, counts = np.unique(arr, return_counts=True)
    arr = np.asarray((unique, counts)).T
    res = arr[arr[:, 1] % 2 == 0]
    res = np.repeat(res[:, 0], res[:, 1])
    return res


print(even_numbers([1, 1, 2, 3, 3, 3]))
