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
            p.start(k, threadNum);
        }

        private void start(int k, int threadNum)
        {
            Stopwatch sw = new Stopwatch();
            Stopwatch swIter = new Stopwatch();
            sw.Start();
            points = new List<Point>();
            clusters = new List<Cluster>(k);

            String header = loadData("../../../../../../diamonds_numeric.csv", points);

            for(int i=0; i<k; i++)
            {
                int randomIndex = randomOverride[i];

                //Random rnd = new Random();
                //int randomIndex = rnd.Next(0, points.Count);
                Cluster tmpCluster = new Cluster(points[randomIndex]);
                clusters.Add(tmpCluster);
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
            Console.ReadKey();
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
