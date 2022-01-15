#include <iostream>
#include <vector>
#include "Cluster.cpp"
#include <string>
#include <sstream>
#include <fstream>


using namespace std;


int randomOverride[] = { 2,4,6 };//{ 620, 1552, 1115 };

int LoadData(string dataPath, vector<Point>* points) {
	string line, word, header;
	fstream file(dataPath, ios::in);
	int dimensions;
	if (file.is_open())
	{
		getline(file, header);
		stringstream str(header);
		int counter = 0;
		while (getline(str, word, ','))
			counter++;
		while (getline(file, line))
		{
			int i = 0;
			float* values = new float[counter];
			stringstream str(line);
			while (getline(str, word, ',')) {
				values[i] = stof(word);
				i++;
			}
			dimensions = i;
			points->emplace_back(Point(values));
		}
	}
	return dimensions;
}

double EuclidDistance(Point* p1, Point* p2, int dimensions) {
	double total = 0;
	float* p1Data = p1->GetData();
	float* p2Data = p2->GetData();
	for (int i = 0; i < dimensions; i++)
	{
		total += pow(p1Data[i] - p2Data[i], 2);
	}
	return sqrt(total);
}

void Kmeans(vector<Cluster>* clusters, vector<Point> points, int dimensions) {
	for(int i=0; i<points.size(); i++)
	{
		double minDistance = 999999999;
		Cluster* pointCluster = new Cluster();
		for(int j=0; j<clusters->size(); j++)
		{
			double distance = EuclidDistance(&points.at(i), clusters->at(j).GetCentroid(), dimensions);
			if (distance < minDistance)
			{
				minDistance = distance;
				pointCluster = &clusters->at(j);
			}
		}
		//lock(pointCluster)
		pointCluster->AppendPoint(points.at(i));
	}
}

void start(int k, int threadNum, string dataPath) {
    vector<Cluster> initialClusters;
    vector<Cluster> clusters;
    vector<Point> points;

    int dimensions = LoadData(dataPath, &points);

	for (int i = 0; i < k; i++)
	{
		int randomIndex = randomOverride[i];

		//Random rnd = new Random();
		//int randomIndex = rnd.Next(0, points.Count);
		
		clusters.emplace_back(Cluster(&points.at(randomIndex)));
		initialClusters.emplace_back(Cluster(&points.at(randomIndex)));
	}
	Point* mean = clusters.at(0).GetMean(points, dimensions);

	int pointsPerThread = points.size() / threadNum;
	//List<double> iterationTimes = new List<double>();

	int l = 0;
	while (l != k)
	{
		for (int i = 0; i < clusters.size(); i++)
			clusters.at(i).ClearPoints();
		
		
		/*Parallel.For(0, threadNum, i = >
		{*/
		for (int i = 0; i < threadNum; i++) {
			int from = i * pointsPerThread;
			int to = i != threadNum - 1 ? pointsPerThread : pointsPerThread + (points.size() - pointsPerThread * threadNum);
			vector<Point> part(points.begin() + from, points.begin() + from + to);
			Kmeans(&clusters, part, dimensions);
		}
		//});
		
		l = 0;
		for (int j = 0; j < clusters.size(); j++)
			l += clusters.at(j).RecalculateCentroid();
		cout << "iteracia";
		cout << endl;
	}


	cout << "EEEEE";
}

int main()
{
	const int k = 3;
	const int threadNum = 3;
	const string dataPath = "../../../smol_data.csv";
	start(k, threadNum, dataPath);
}