using System;
using System.Collections.Generic;
using System.Text;

namespace kmeans
{
    class Cluster
    {
        private Point centroid;
        private List<Point> points;

        public Cluster(Point centroid)
        {
            this.centroid = centroid;
            this.points = new List<Point>();
        }

        public void clearPoints()
        {
            this.points.Clear();
        }

        public void setPointList(List<Point> points)
        {
            this.points = points;
        }

        public int RecalculateCentroid()
        {
            Point newCentroid = getMean(this.points);
            if(diff(newCentroid, this.centroid) > 0)
            {
                this.centroid = newCentroid;
                return 0;
            }

            return 1;
        }

        public Point getMean(List<Point> points)
        {
            int i = 0;
            float[] mean = new float[points[0].getData().Length];
            foreach(Point point in points)
            {
                i = 0;
                foreach(float f in point.getData())
                {
                    mean[i] = mean[i] + f;
                    i++;
                }
            }
            i = 0;
            foreach (float f in mean)
            {
                mean[i] = MathF.Round( mean[i] / points.Count, 2);
                i++;
            }
            return new Point(mean);
        }
        private float diff(Point p1, Point p2)
        {
            float total = 0;
            float[] p2Data = p2.getData();
            int i = 0;
            foreach(float f in p1.getData())
            {
                total += Math.Abs(f - p2Data[i]);
                i++;
            }
            return total;
        }
    }
}
