import math
from random import randint
from random import seed


def load_data(filename):
    lines = []
    rows = []
    with open(filename) as file:
        lines = file.readlines()
    for line in lines:
        row = line.rstrip("\n").split(",")
        rows.append(row)
    file.close()
    return rows


def preprocess(data, method):
    # method 0 - delete instances with missing data
    # method 1 - replace missing values with avg
    categorical_values = {}
    for rowI, row in enumerate(data):
        for valI, value in enumerate(row):
            try:
                data[rowI][valI] = float(value)
            except:
                if len(value) > 0:
                    if valI in categorical_values:
                        if value not in categorical_values[valI]:
                            categorical_values[valI] += [value]
                    else:
                        categorical_values[valI] = [value]
                    category_index = categorical_values[valI].index(value)
                    data[rowI][valI] = category_index
    return resolve_empty_vals(data, method, categorical_values)


def resolve_empty_vals(data, method, categorical_values):
    mean = get_mean(data, categorical_values)
    recalculate_mean = False
    objects_to_remove = []
    for rowI, row in enumerate(data):
        for valI, value in enumerate(row):
            if type(value) == str and len(value) <= 0:
                if method == 0 and row not in objects_to_remove:
                    objects_to_remove.append(row)
                elif method == 1:
                    data[rowI][valI] = mean[valI]
                    recalculate_mean = True
                    pass
    for obj in objects_to_remove:
        data.remove(obj)
    if recalculate_mean:
        mean = get_mean(data, categorical_values)
    return (data, categorical_values, mean)


def get_mean(data, categorical_values):
    mean = {}
    res = []
    skip_counter = 0
    for row in data:
        for valI, value in enumerate(row):
            if type(value) == str:
                skip_counter += 1
                break
            mean[valI] = float(row[valI]) + (mean.get(valI)
                                             if mean.get(valI) != None else 0)
    mean = mean.values()
    for attI, m in enumerate(mean):
        if attI not in categorical_values.keys():
            res.append(m / (len(data)-skip_counter))
        else:
            res.append(round(m / (len(data)-skip_counter)))
    return res


def euclid_distance(obj1, obj2):
    total = 0
    for i in range(len(obj1)):
        total += (obj1[i] - obj2[i])**2
    return math.sqrt(total)


def kmeans(k, centroids, data, categorical_values):
    difference_threshold = 0.001
    l = k
    while l != 0:
        clusters = []
        for i, centroid in enumerate(centroids):
            clusters.insert(i, [centroid])

        for obj in data:
            min_distance = 999999999
            cIndex = []
            for i in range(k):
                d = euclid_distance(obj, centroids[i])
                if d < min_distance:
                    min_distance = d
                    cIndex = i
            clusters[cIndex].append(obj)
        l = k
        for i, cluster in enumerate(clusters):
            new_centroid = get_mean(cluster, categorical_values)
            if diff(new_centroid, centroids[i]) > difference_threshold:
                centroids[i] = new_centroid
            else:
                l -= 1
    print("a")


def diff(obj1, obj2):
    total = 0
    for i, a in enumerate(obj1):
        total += abs(a - obj2[i])
    return total


data = load_data("iris.csv")
(data, categorical_values, mean) = preprocess(data[1:], 0)
k = 3
# seed(10)
initial_centroids = []
random_override = [135, 72, 111]  # override to match weka kmeans with seed = 1
for i in range(k):
        # randIndex = randint(0, len(data))
    randIndex = random_override[i]

    initial_centroids.append(data[randIndex])
    print("Initial cluster " + str(i) + ": ")
    print(*data[randIndex], sep=", ")
    print("=========================================")
kmeans(k, initial_centroids, data, categorical_values)
