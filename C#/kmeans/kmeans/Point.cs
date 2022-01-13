using System;
using System.Collections.Generic;
using System.Text;

namespace kmeans
{
    class Point
    {
        private float[] data;
        public Point(float[] data)
        {
            this.data = data;
        }
        public float[] getData()
        {
            return this.data;
        }
    }
}
