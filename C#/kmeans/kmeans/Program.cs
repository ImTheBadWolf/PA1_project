using System;
using System.Collections.Generic;
using System.IO;

namespace kmeans
{
    class Program
    {
        private int k = 3;
        private int[] randomOverride = { 620, 1552, 1115 };
        private List<Point> points;
        private List<Cluster> clusters;

        static void Main(string[] args)
        {
            Program p = new Program();
            p.start();
        }

        private void start()
        {
            points = new List<Point>();
            clusters = new List<Cluster>(k);

            String header = loadData("../../../../../../MOCK_DATA.csv", points);

            for(int i=0; i<k; i++)
            {
                int randomIndex = randomOverride[i];

                //Random rnd = new Random();
                //int randomIndex = rnd.Next(0, points.Count);
                Cluster tmpCluster = new Cluster(points[randomIndex]);
                clusters.Add(tmpCluster);
            }

            Point mean = clusters[0].getMean(points);

            Console.WriteLine("Hell");
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
