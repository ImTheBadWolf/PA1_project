#include <iostream>
#include <vector>
#include <mutex>
#include "Cluster.cpp"
#include <numeric>
#include <string>
#include <chrono>
#include <sstream>
#include <fstream>
#include <thread>

using namespace std;


int randomOverride[] = { 620, 1552, 1115 };//{ 620, 1552, 1115 };

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

static void Kmeans(vector<Cluster>* clusters, vector<Point> points, int dimensions) {
	for(int i=0; i<points.size(); i++)
	{
		double minDistance = 999999999;
		Cluster* pointCluster = new Cluster(new Point(new float[] {0}));
		mutex pointCluster_mutex;
		for(int j=0; j<clusters->size(); j++)
		{
			double distance = EuclidDistance(&points.at(i), clusters->at(j).GetCentroid(), dimensions);
			if (distance < minDistance)
			{
				minDistance = distance;
				pointCluster = &clusters->at(j);
			}
		}
		
		pointCluster_mutex.lock();
		pointCluster->AppendPoint(points.at(i));
		pointCluster_mutex.unlock();
	}
}

void start(int k, int threadNum, string dataPath) {
	auto start = chrono::high_resolution_clock::now();
	vector<long long> iterattionTimes;
	vector<thread> threads;
    vector<Cluster> initialClusters;
    vector<Cluster> clusters;
    vector<Point> points;

    int dimensions = LoadData(dataPath, &points);

	for (int i = 0; i < k; i++)
	{
		int randomIndex = randomOverride[i];
		
		clusters.emplace_back(Cluster(&points.at(randomIndex)));
		initialClusters.emplace_back(Cluster(&points.at(randomIndex)));
	}
	Point* mean = clusters.at(0).GetMean(points, dimensions);

	int pointsPerThread = points.size() / threadNum;

	int l = 0;
	while (l != k)
	{
		auto startI = chrono::high_resolution_clock::now();
		for (int i = 0; i < clusters.size(); i++)
			clusters.at(i).ClearPoints();
		
		for (int i = 0; i < threadNum; i++) {
			int from = i * pointsPerThread;
			int to = i != threadNum - 1 ? pointsPerThread : pointsPerThread + (points.size() - pointsPerThread * threadNum);
			vector<Point> part(points.begin() + from, points.begin() + from + to);
			threads.emplace_back(thread(Kmeans, &clusters, part, dimensions));
		}
		for (int i = 0; i < threadNum; i++)
			threads[i].join();
		
		l = 0;
		for (int j = 0; j < clusters.size(); j++)
			l += clusters.at(j).RecalculateCentroid();

		auto stopI = chrono::high_resolution_clock::now();
		auto durationI = chrono::duration_cast<chrono::milliseconds>(stopI - startI);
		iterattionTimes.emplace_back(durationI.count());
	}
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
	cout << "Kmeans on " << threadNum << " threads" << endl << "Elapsed time: " << duration.count() << "s" << endl;
	cout << "Average iteration time: " << accumulate(iterattionTimes.begin(), iterattionTimes.end(), 0) / iterattionTimes.size() << "ms" << endl << "Number of iterations: " << iterattionTimes.size() << endl;
}

int main()
{
	const int k = 3;
	const int threadNum = 3;
	const string dataPath = "../../../diamonds_numeric.csv";
	start(k, threadNum, dataPath);
}