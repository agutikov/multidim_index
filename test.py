#!/usr/bin/env python3


# Load list of points from csv

# Count all distances

# Apply knn to get first level of clusters
# calculate centroid and radius (max distance between centroid and element) of each clusters

# Hierarchical clustering with one of next criteria:
#  - knn applied to centers of clusters on previous level
#  - knn applied to each point in cluster:
#     - for each point in cluster find nearest that is not in given cluster and get clusters of found points
#  - knn with radius-based criteria:
#     - nearest borders, intersected borders
#     - centroids nearest to border
#     - points nearest to border
#  - Summary: points, centroid, borders and knn applied to different combinations
#
# Each cluster of higher level has:
#  - r radius - max distance between centroid and subcluster centers
#  - R radius - sum of r radius and max R radius of subclusters
#  - R raius - max distance between centroid and point


# Search descent with pruning:
#  - pruning:
#     - With given point calculate distance to nearest and most distant point of cluster border
#     - Prune cluster if nearest border is far than most distant border of other cluster
#  - open cluesters that left - calculate with smaller clusters



import csv
import sys
from pprint import pprint, pformat
import random
import numpy as np
import itertools
import json
from collections import namedtuple
import copy

delimiter = ','

def help():
    print("Commands: gen, clusters, clusters_tree, clusters_json, find", file=sys.stderr)
    quit(1)

def main():
    if len(sys.argv) < 2:
        help()
    if sys.argv[1] == 'gen':
        gen()
    elif sys.argv[1] == 'clusters_tree':
        do_clusters(flatten=False)
    elif sys.argv[1] == 'clusters_json':
        do_clusters(flatten=False, format='json')
    elif sys.argv[1] == 'clusters':
        do_clusters(flatten=True)
    elif sys.argv[1] == 'find':
        do_find()
    elif sys.argv[1] == 'distance':
        do_distance()
    else:
        help()

def gen():
    count = int(sys.argv[2])
    vec_len = int(sys.argv[3])
    print("Making %d records, vector size=%d\n" % (count, vec_len), file=sys.stderr)

    writer = csv.writer(sys.stdout, delimiter=delimiter)

    for i in range(0, count):
      writer.writerow([random.random() for j in range(0, vec_len)])

def do_distance():
    points = None
    with open(sys.argv[2], newline='') as csvfile:
         reader = csv.reader(csvfile, quoting=csv.QUOTE_NONNUMERIC, delimiter=delimiter)
         points = list([tuple(row) for row in reader])
    #d = [np.linalg.norm(np.array(p1) - np.array(p2)) for p1,p2 in combinations(points)]
    #print("min=%f, mean=%f, max=%f, max-min=%f, std=%f, cv=%f" %
    #    (np.min(d), np.mean(d), np.max(d), np.max(d)-np.min(d), np.std(d), np.std(d)/np.mean(d)), file=sys.stderr)
    #print(np.histogram(d, range=[np.min(d), np.max(d)]))
    center = [0.5]*len(points[1])
    d = [np.linalg.norm(np.array(center) - np.array(p)) for p in points]
    print("min=%f, mean=%f, median=%f max=%f, max-min=%f, std=%f, cv=%f" %
        (np.min(d), np.mean(d), np.median(d), np.max(d), np.max(d)-np.min(d), np.std(d), np.std(d)/np.mean(d)), file=sys.stderr)
    print(np.histogram(d, bins=21, range=[np.min(d), np.max(d)]))


################################################################################



class cluster:
    id_counter = itertools.count(1)

    def __init__(self, centroid=None, radius=None, id=None):
        if id is None:
            self.id = next(self.id_counter)
        else:
            self.id = id
        self.points = set()
        self.level = -1
        if centroid is not None:
            self.centroid = tuple(centroid)
            self.points.add(tuple(centroid))
            self.level = 0
        else:
            self.centroid = None
        self.radius = radius
        self.subclusters = {}

    def to_dict(self):
        return {
            'id': self.id,
            'level': self.level,
            'centroid': list(self.centroid),
            'radius': self.radius,
            'points': [list(point) for point in self.points],
            'subclusters': {id:c.to_dict() for id,c in self.subclusters.items()},
        }

    def from_dict(data):
        c = cluster(centroid=data['centroid'], radius=data['radius'], id=data['id'])
        c.level = data['level']
        c.points = data['points']
        c.subclusters = {int(id):cluster.from_dict(c) for id,c in data['subclusters'].items()}
        return c

    def __repr__(self):
        return pformat(self.to_dict(), width=128)

    def _get_centroid(self):
        points = [np.array(point) for point in self.points]
        return np.mean(points, axis=0)

    def _get_radius(self):
        # Max distance from centroid to point
        return max([np.linalg.norm(self.centroid - point) for point in self.points])

        # Max distance to outer border of subcluster
        # i.e. max distance from centroid to center of subclusters plus radius of subcluster
        #return max([np.linalg.norm(self.centroid - c.centroid) + c.radius for c in self.subclusters.values()])

    def _get_level(self):
        return 1 + max([0] + [c.level for c in self.subclusters.values()])

    def _update(self):
        self.centroid = self._get_centroid()
        self.radius = self._get_radius()
        self.level = self._get_level()

    def add_subcluster(self, subcluster):
        if self.id == subcluster.id:
            return
        self.points |= subcluster.points
        self.subclusters[subcluster.id] = subcluster
        self._update()

    def merge(self, another):
        """ Merge all subclusters of two clusters into single cluster.
        Do not create new cluster, updates existing one.
        """
        if len(self.subclusters) == 0:
            raise ValueError("Can't merge into level=0 cluster (i.e. point, without subclusters)")
        if len(another.subclusters) == 0:
            self.add_subcluster(another)
        else:
            for id,c in another.subclusters.items():
                self.add_subcluster(c)
        self.points |= another.points
        self._update()
        return self

    def centroid_distance(self, point):
        return np.linalg.norm(np.array(self.centroid) - np.array(point))

    def border_distance(self, point):
        return self.centroid_distance(point) - self.radius

    def contains_point(self, point):
        return self.border_distance(point) <= 0

    def border_centroid_distance(self, another):
        return self.border_distance(another.centroid)

    def contains_centroid(self, another):
        return self.contains_point(another.centroid)

    def contains_any_point(self, another):
        for point in another.points:
            if self.contains_point(point):
                return True
        return False

    def min_border_point_distance(self, another):
        d = None
        for point in another.points:
            d1 = self.border_distance(point)
            if d is None or d1 < d:
                d = d1
        return d

def centroid_distance(c1, c2):
    return np.linalg.norm(np.array(c1.centroid) - np.array(c2.centroid))

def border_distance(c1, c2):
    return np.linalg.norm(np.array(c1.centroid) - np.array(c2.centroid)) - (c1.radius + c2.radius)

def is_intersected(c1, c2):
    """ Hyperspheras have common volume.
    """
    return border_distance(c1, c2) <= 0

def is_overlapped(c1, c2):
    """ Clusters have common points.
    """
    return len(c1.points & c2.points) > 0

def combinations(source):
    for p1 in range(len(source)):
        for p2 in range(p1+1,len(source)):
            yield source[p1], source[p2]

def permutations(source):
    for p1 in range(len(source)):
        for p2 in range(len(source)):
            if p1 != p2:
                yield source[p1], source[p2]

def supercluster(*clusters):
    """ Union not overlapped clusters in one bigger cluster.
    """
    for c1,c2 in combinations(clusters):
        if is_overlapped(c1, c2):
            raise ValueError("Can't put overlapped clusters ({}, {}) into supercluster".format(c1.id, c2.id))
    result = cluster()
    for c in clusters:
        result.add_subcluster(c)
    return result

def merge_clusters(clusters, contains_check_f):
    merged = True
    while merged:
        merged = False
        for c1,c2 in permutations(list(clusters.values())):
            if contains_check_f(c1, c2):
                c1.merge(c2)
                del clusters[c2.id]
                merged = True
                break

def merge_sets(*sets):
    s = set()
    for s1 in sets:
        s |= set(s1)
    return s

def supercluster_radius(*clusters):
    return supercluster(*clusters).radius

def find_supercluster(clusters, score_f):
    pair_cluster_scores = {}

    # calculate score for each pair of clusters
    for c1,c2 in permutations(list(clusters.values())):
        d = score_f(c1, c2)
        if d not in pair_cluster_scores:
            pair_cluster_scores[d] = set()
        pair_cluster_scores[d].add((c1.id, c2.id))

    # select smallest score
    smallest_score = min(pair_cluster_scores)

    ids_to_join = pair_cluster_scores[smallest_score]
    ids_to_join = merge_sets(*ids_to_join)
    return {id:c for id,c in clusters.items() if id in ids_to_join}

def join_step(clusters, score_f):
    # find clusters to join into supercluster
    clusters_to_join = find_supercluster(clusters, score_f)

    # join clusters into supercluster
    s = supercluster(*clusters_to_join.values())

    # add supercluster to clusters
    clusters[s.id] = s

    # remove joined clusters
    for id in clusters_to_join:
        del clusters[id]

    return s

def merge_step(clusters, contains_f):
    merge_clusters(clusters, contains_f)

def join_nearest_clusters(clusters, clusters_pair_score_f=supercluster_radius, contains_cluster_f=cluster.contains_any_point):
    """

    clusters_pair_score_f = supercluster_radius | centroid_distance | cluster.min_border_point_distance | border_distance | cluster.border_centroid_distance
    contains_cluster_f = cluster.contains_any_point | cluster.contains_centroid
    """
    while len(clusters) > 1:
        print(len(clusters), file=sys.stderr)
        join_step(clusters, clusters_pair_score_f)
        merge_step(clusters, contains_cluster_f)
    return next(iter(clusters.values()))

clusters_pair_score_functions = {
    'supercluster_radius': supercluster_radius,
    'centroid_distance': centroid_distance,
    'min_border_point_distance': cluster.min_border_point_distance,
    'border_distance': border_distance,
    'border_centroid_distance': cluster.border_centroid_distance
}

contains_cluster_functions = {
    'contains_any_point': cluster.contains_any_point,
    'contains_centroid': cluster.contains_centroid,
    'none': lambda x,y: False,
}

def do_flatten(cluster):
    a = [[list(cluster.centroid), cluster.radius]]
    for subcluster in cluster.subclusters.values():
        if subcluster.radius > 0:
            a += do_flatten(subcluster)
    return a

def do_clusters(flatten, format=None, pair_score='centroid_distance', contains_cluster='none'):
    points = None
    with open(sys.argv[2], newline='') as csvfile:
         reader = csv.reader(csvfile, quoting=csv.QUOTE_NONNUMERIC, delimiter=delimiter)
         points = list([tuple(row) for row in reader])
    clusters = [cluster(point, 0) for point in points]
    clusters = {c.id:c for c in clusters}

    c = join_nearest_clusters(clusters,
            clusters_pair_score_f=clusters_pair_score_functions[pair_score],
            contains_cluster_f=contains_cluster_functions[contains_cluster])

    if flatten:
        flattened = do_flatten(c)
        for c in flattened:
            print(delimiter.join(map(lambda x: '{}'.format(x), c[0])) + delimiter + '{}'.format(c[1]))
    else:
        if format == 'json':
            print(json.dumps(c.to_dict()))
        else:
            pprint(c)


def _json_object_hook(d):
    try:
        return namedtuple('JSON_Object', d.keys())(*d.values())
    except:
        return dict(d)


def find_closest(clusters, point):
    count = 0
    distance_cache = {}
    x = {np.linalg.norm(np.array(p) - np.array(point)):p for p in list(clusters.values())[0].points}
    #pprint(x)
    while len(clusters) != 1 or next(iter(clusters.values())).level != 0:
        #print(len(clusters))
        # open single cluster
        while len(clusters) == 1:
            clusters = next(iter(clusters.values())).subclusters
        #print(len(clusters))

        ids_to_remove = set()
        # select too far away clusters
        for c1,c2 in permutations(list(clusters.values())):
            #pprint(distance_cache)
            d1 = distance_cache.get(c1.id, None)
            if d1 is None:
                count += 1
                d1 = c1.centroid_distance(point)
                distance_cache[c1.id] = d1
            #print(c1.id, d1)
            d2 = distance_cache.get(c2.id, None)
            if d2 is None:
                count += 1
                d2 = c2.centroid_distance(point)
                distance_cache[c2.id] = d2
            #print(c2.id, d2)
            if d1 - c1.radius > d2 + c2.radius:
                ids_to_remove.add(c1.id)
            if d1 + c1.radius < d2 - c2.radius:
                ids_to_remove.add(c2.id)
        # prune
        for id in ids_to_remove:
            del clusters[id]

        clusters_to_add = []
        ids_to_remove = set()
        # open next level clusters
        for c in clusters.values():
            if len(c.subclusters) > 0:
                clusters_to_add += list(c.subclusters.values())
                ids_to_remove.add(c.id)
        for id in ids_to_remove:
            del clusters[id]
        for c in clusters_to_add:
            clusters[c.id] = c

    s = next(iter(clusters.values()))
    #print(s)
    return s.centroid, distance_cache[s.id], count


def do_find():
    with open(sys.argv[2]) as input:
        j = json.load(input)
    #print(j)
    root = cluster.from_dict(j)
    #print(root)
    vec_len = len(root.points[0])

    repeat = 1
    if len(sys.argv) > 3:
        repeat = int(sys.argv[3])

    for i in range(repeat):
        print(i)
        point = [random.random() for j in range(0, vec_len)]
        #print(point)

        clusters = {root.id:copy.deepcopy(root)}
        result, distance, count = find_closest(clusters, point)
        print(distance, count)

        d = 1000
        simple_result = None
        for p in root.points:
            d1 = np.linalg.norm(np.array(p) - np.array(point))
            if d1 < d:
                d = d1
                simple_result = p
        print(d, len(root.points))
        if tuple(result) != tuple(simple_result):
            quit(1)


################################################################################

if __name__ == "__main__":
    main()
