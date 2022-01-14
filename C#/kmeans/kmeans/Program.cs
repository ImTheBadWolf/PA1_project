using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Linq;

namespace kmeans
{
    class Program
    {
        private int[] randomOverride = { 620, 1552, 1115 };
        private List<Point> points;
        private List<Cluster> clusters;

        static void Main(string[] args)
        {
            Program p = new Program();
            const int k = 3;
            const int threadNum = 4;
            const String dataPath = "../../../../../../diamonds_numeric.csv";
            p.start(k, threadNum, dataPath);
        }

        private void start(int k, int threadNum, String dataPath)
        {
            Stopwatch sw = new Stopwatch();
            Stopwatch swIter = new Stopwatch();
            List<Cluster> initialClusters = new List<Cluster>();
            sw.Start();
            points = new List<Point>();
            clusters = new List<Cluster>(k);

            String header = loadData(dataPath, points);

            for(int i=0; i<k; i++)
            {
                //int randomIndex = randomOverride[i];

                Random rnd = new Random();
                int randomIndex = rnd.Next(0, points.Count);
                clusters.Add(new Cluster(points[randomIndex]));
                initialClusters.Add(new Cluster(points[randomIndex]));
            }

            Point mean = clusters[0].getMean(points);

            int pointsPerThread = points.Count / threadNum;
            List<double> iterationTimes = new List<double>();

            int l = 0;
            while (l != k)
            {
                swIter.Start();
                foreach(Cluster cluster in clusters)
                    cluster.clearPoints();
                Parallel.For(0, threadNum, i =>
                {
                    int from = i * pointsPerThread;
                    int to = i != threadNum - 1 ? pointsPerThread : pointsPerThread + (points.Count - pointsPerThread * threadNum);
                    List<Point> part = points.GetRange(from, to);

                    Kmeans(clusters, part);
                });
                l = 0;
                foreach (Cluster cluster in clusters)
                    l += cluster.RecalculateCentroid();
                swIter.Stop();
                iterationTimes.Add(swIter.Elapsed.TotalSeconds);
            }
            sw.Stop();
            Console.WriteLine("Kmeans on {0} threads\nElapsed time: {1}s\nAverage iteration time: {2}s\nNumber of iterations: {3}", threadNum, sw.Elapsed.TotalSeconds, iterationTimes.Sum()/iterationTimes.Count, iterationTimes.Count );
            Console.WriteLine(Result(dataPath, points.Count, header, initialClusters, clusters, mean));
            //Console.ReadKey();
        }

        private String Result(String dataPath, int dataLength, String header, List<Cluster> initialClusters, List<Cluster> clusters, Point mean)
        {
            String[] headerValues = header.Replace("\n", "").Split(',');
            String template = @"
Clustering result for {0}:
Instances: {1}, attributes: {2}.
SSE: {3}
=============
kMeans:
Initial starting centroids:
{4}
Cluster centroids/mean:
{5}
";
            return String.Format(template, dataPath, dataLength, headerValues.Length, GetSSE(clusters), FormatCentroids(initialClusters), FormatClusters(clusters, mean, headerValues, dataLength));
        }

        private float GetSSE(List<Cluster> clusters)
        {
            float sse = 0;
            foreach(Cluster cluster in clusters)
            {
                foreach(Point point in cluster.getPointList())
                {
                    sse += MathF.Pow((float)EuclidDistance(point, cluster.getCentroid()), 2);
                }
            }
            return sse;
        }

        private String FormatCentroids(List<Cluster> clusters)
        {
            String result = "";
            for(int i =0; i<clusters.Count; i++)
            {
                result += String.Format("Cluster {0}: {1}\n", i, String.Join(", ", clusters[i].getCentroid().getData()));
            }
            return result;
        }

        private String FormatClusters(List<Cluster> clusters, Point mean, String[] headerValues, int dataLength)
        {
            String tableHeader = "Attribute \tFull data (" + dataLength + ")";
            String result = "";

            for(int i=0; i<clusters.Count; i++)
            {
                int pointsLen = clusters[i].getPointList().Count;
                float percent = MathF.Round((float)pointsLen / (float)dataLength*100, 1);
                tableHeader += String.Format("\tC{0} ({1}, {2}%)", i, pointsLen, percent);
            }
            for(int i=0; i<headerValues.Length; i++)
            {
                result += String.Format("{0}{1}{2}\t", headerValues[i], (headerValues[i].Length < 9 ? "\t\t" : "\t"), MathF.Round(mean.getData()[i],2));

                foreach(Cluster cluster in clusters)
                {
                    result += String.Format("\t\t{0}",MathF.Round(cluster.getCentroid().getData()[i], 2));
                }

                result += "\n";
            }

            result = tableHeader + "\n=====================================\n" + result;
            return result;
        }

        private void Kmeans(List<Cluster> clusters, List<Point> points)
        {
            foreach(Point point in points)
            {
                double minDistance = float.MaxValue;
                Cluster pointCluster = new Cluster();
                foreach(Cluster cluster in clusters)
                {
                    double distance = EuclidDistance(point, cluster.getCentroid());
                    if(distance < minDistance)
                    {
                        minDistance = distance;
                        pointCluster = cluster;
                    }
                }
                lock(pointCluster)
                    pointCluster.appendPoint(point);
            }
        }

        private double EuclidDistance(Point p1, Point p2)
        {
            double total = 0;
            float[] p1Data = p1.getData();
            float[] p2Data = p2.getData();

            for(int i=0; i<p1Data.Length; i++)
            {
                total += Math.Pow(p1Data[i] - p2Data[i], 2);
            }
            return Math.Sqrt(total);
        }

        private String loadData(String path, List<Point> points)
        {
            String header;
            using (var reader = new StreamReader(path))
            {
                header = reader.ReadLine();
                while (!reader.EndOfStream)
                {
                    String line = reader.ReadLine();
                    String[] values = line.Replace("\n", "").Split(',');
                    float[] floatVals = new float[values.Length];
                    int i = 0;
                    foreach (String v in values)
                    {
                        floatVals[i] = float.Parse(v);
                        i++;
                    }
                    Point tmpPoint = new Point(floatVals);
                    points.Add(tmpPoint);
                }
            }
            return header;
        }
    }
}
