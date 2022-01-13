import math
from mimetypes import init
from random import randint
from random import seed
from typing import List


class Point:
    def __init__(self, *args):
        self.data = []
        for arg in args:
            self.data = arg


class Cluster:
    def __init__(self, centroid: Point):
        self.centroid = centroid
        self.points = []

    def recalculate_centroid(self):
        new_centroid = get_mean(self.points)
        if diff(new_centroid.data, self.centroid.data) > 0:
            self.centroid = new_centroid
            return 0
        else:
            return 1

    def clear_points(self):
        self.points = []


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


def preprocess(data):
    points = []
    for row in data:
        tmp_row = []
        for d in row:
            tmp_row.append(float(d))
        tmp_point = Point(tmp_row)
        points.append(tmp_point)
    return points


def get_mean(points: List[Point]):
    mean = {}
    res = []
    for point in points:
        for valI, value in enumerate(point.data):
            mean[valI] = value + \
                (mean.get(valI) if mean.get(valI) != None else 0)
    mean = mean.values()
    for m in mean:
        res.append(m / len(points))
    return Point(res)


def euclid_distance(obj1, obj2):
    total = 0
    for i in range(len(obj1)):
        total += (obj1[i] - obj2[i])**2
    return math.sqrt(total)


def kmeans(k, clusters: List[Cluster], points: List[Point]):
    l = k
    counter = 0
    while l != 0:
        for cluster in clusters:
            cluster.clear_points()
        for point in points:  # data rozdelit na N casti => n threadov, centroids a clusters zdielat medzithreadovo
            min_distance = float('inf')
            point_cluster = None
            for cluster in clusters:
                d = euclid_distance(point.data, cluster.centroid.data)
                if d < min_distance:
                    min_distance = d
                    point_cluster = cluster
            point_cluster.points.append(point)

        l = k
        for cluster in clusters:
            l -= cluster.recalculate_centroid()
        counter += 1

    return counter


def diff(obj1, obj2):
    total = 0
    for i, a in enumerate(obj1):
        total += abs(a - obj2[i])
    return total


def return_result(kmeans_result, initial_centroids, original_data, deleted_indices, data_path, method, mean, length):
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
                                    length, len(original_data[0]) -
                                    len(deleted_indices),
                                    counter, sse,
                                    print_centroids(
                                        initial_centroids), "deleted" if method == 0 else "replaced by mean",
                                    print_clusters(centroids, clusters, mean, deleted_indices,
                                                   original_data[0], len(original_data)-1)
                                    )

    return result


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


def start(path, k):
    k = int(k)
    data = load_data(path)
    original_data = data.copy()
    points = preprocess(data[1:])

    initial_clusters = []
    # override to match weka kmeans with seed = 1, diamonds_reduced_numeric.csv
    random_override = [0, 8, 14]
    for i in range(k):
        #randIndex = randint(0, len(points))
        randIndex = random_override[i]

        tmp_cluster = Cluster(points[randIndex])
        initial_clusters.append(tmp_cluster)
    kmeans_result = kmeans(k, initial_clusters.copy(), points)
    """ return return_result(kmeans_result, initial_centroids,
                         original_data, deleted_indices, path, method, mean, len(data)) """


start("test_data.csv", 3)
