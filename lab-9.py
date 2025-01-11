import numpy as np

def even_numbers(array):
    # Преобразуем входной список в массив numpy
    arr = np.array(array)
    
    # Находим уникальные элементы и их количество
    unique, counts = np.unique(arr, return_counts=True)
    
    # Создаем массив с уникальными элементами и их количеством(для удобства транспонируем)
    arr = np.asarray((unique, counts)).T
    
    # Отбираем элементы с чётным количеством вхождений
    res = arr[arr[:, 1] % 2 == 0]
    
    # Повторяем элементы с чётным количеством вхождений
    res = np.repeat(res[:, 0], res[:, 1])
    
    return res

print(even_numbers([1, 1, 2, 3, 3, 3]))
