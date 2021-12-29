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
    new_data = []
    deleted_indices = []
    for row in data:
        new_row = []
        for vI, value in enumerate(row):
            try:
                new_row.append(float(value))
            except:
                if len(value) == 0:
                    new_row.append("")
                else:
                    if vI not in deleted_indices:
                        deleted_indices.append(vI)
        new_data.append(new_row)
    return resolve_empty_vals(new_data, method, deleted_indices)


def resolve_empty_vals(data, method, deleted_indices):
    mean = get_mean(data)
    recalculate_mean = False
    objects_to_remove = []
    for rowI, row in enumerate(data):
        for valI, value in enumerate(row):
            if type(value) == str:
                if method == 0 and row not in objects_to_remove:
                    objects_to_remove.append(row)
                elif method == 1:
                    data[rowI][valI] = mean[valI]
                    recalculate_mean = True
                    pass
    for obj in objects_to_remove:
        data.remove(obj)
    if recalculate_mean:
        mean = get_mean(data)
    return (data, mean, deleted_indices)


def get_mean(data):
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
    for m in mean:
        res.append(m / (len(data)-skip_counter))
    return res


def euclid_distance(obj1, obj2):
    total = 0
    for i in range(len(obj1)):
        total += (obj1[i] - obj2[i])**2
    return math.sqrt(total)


def kmeans(k, centroids, data):
    difference_threshold = 0
    l = k
    counter = 0
    while l != 0:
        clusters = []
        for i in range(len(centroids)):
            clusters.insert(i, [])

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
            new_centroid = get_mean(cluster)
            if diff(new_centroid, centroids[i]) > difference_threshold:
                centroids[i] = new_centroid
            else:
                l -= 1
        counter += 1
    sse = get_sse(centroids, clusters)
    return(counter, sse, centroids, clusters)


def get_sse(centroids, clusters):
    sse = 0
    for i, centroid in enumerate(centroids):
        for obj in clusters[i]:
            sse += pow(euclid_distance(obj, centroid), 2)
    return sse


def diff(obj1, obj2):
    total = 0
    for i, a in enumerate(obj1):
        total += abs(a - obj2[i])
    return total


def show_result(kmeans_result, initial_centroids, original_data, deleted_indices, data_path, method, mean):
    (counter, sse, centroids, clusters) = kmeans_result
    result_template = """
Clustering result for {0}: \nInstances: {1}, attributes: {2}.
=============
kMeans:
Iterations: {3}
SSE: {4}
Initial starting centroids:
{5}
Missing values were {6}.

Cluster centroids/mean:
{7}
"""
    result = result_template.format(data_path,
                                    len(original_data) -
                                    1, len(original_data[0]) -
                                    len(deleted_indices),
                                    counter, sse,
                                    print_centroids(
                                        initial_centroids), "deleted" if method == 1 else "replaced by mean",
                                    print_clusters(centroids, clusters, mean, deleted_indices,
                                                   original_data[0], len(original_data)-1)
                                    )

    print(result)


def print_centroids(centroids):
    result = ""
    for i, centroid in enumerate(centroids):
        result += "Cluster " + str(i) + ":" + str(centroid) + "\n"
    return result


def print_clusters(centroids, clusters, mean, deleted_indices, header, data_len):
    header = [att for idx, att in enumerate(
        header) if idx not in deleted_indices]
    table_header = "Attribute \tFull data ("+str(data_len)+")"
    result = ""
    for i in range(len(centroids)):
        table_header += "\tC" + \
            str(i) + " (" + str(len(clusters[i])) + ", " + \
            str(round(len(clusters[i])/data_len*100, 1)) + "%)"
    for i, attribute in enumerate(header):
        result += str(attribute) + "\t"+str(round(mean[i], 2)) + "\t"
        for centroid in centroids:
            result += "\t\t" + str(round(centroid[i], 2))
        result += "\n"

    result = table_header + "\n" +\
        str("".join(
            ["=" for i in range(len(table_header) + len(centroids)*8+8)])) + "\n" + result
    return result


data_path = "iris_numeric.csv"
method = 1
data = load_data(data_path)
original_data = data.copy()
(data, mean, deleted_indices) = preprocess(data[1:], method)


k = 3
# seed(10)
initial_centroids = []
# override to match weka kmeans with seed = 1
random_override = [135, 72, 111]
for i in range(k):
        # randIndex = randint(0, len(data))
    randIndex = random_override[i]
    initial_centroids.append(data[randIndex])
kmeans_result = kmeans(k, initial_centroids.copy(), data)
show_result(kmeans_result, initial_centroids,
            original_data, deleted_indices, data_path, method, mean)
