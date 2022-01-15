#include <iostream>
#include "Cluster.cpp"
#include <numeric>
#include <string>
#include <chrono>
#include <sstream>
#include <fstream>
#include <thread>

using namespace std;


int randomOverride[] = { 620, 1552, 1115 };//{ 620, 1552, 1115 };

vector<Point> LoadData(string dataPath) {
	vector<Point> points;
	string line, word, header;
	fstream file(dataPath, ios::in);
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
			vector<float> values;
			values.reserve(counter);
			stringstream str(line);
			while (getline(str, word, ',')) {
				values.push_back(stof(word));
				i++;
			}
			points.emplace_back(Point(values));
		}
	}
	else {
		cout << "File not found" << endl;
	}
	return points;
}

double EuclidDistance(Point p1, Point p2, int dimension) {
	double total = 0;
	vector<float> p1Data = p1.GetData();
	vector<float> p2Data = p2.GetData();
	for (int i = 0; i < dimension; i++)
	{
		total += pow(p1Data[i] - p2Data[i], 2);
	}
	return sqrt(total);
}

static void Kmeans(vector<Cluster>& clusters, vector<Point> points, int dimension) {
	for(int i=0; i<points.size(); i++)
	{
		double minDistance = 999999999;
		Cluster* pointCluster = new Cluster();
		for(int j=0; j<clusters.size(); j++)
		{
			double distance = EuclidDistance(points.at(i), clusters.at(j).GetCentroid(), dimension);
			if (distance < minDistance)
			{
				minDistance = distance;
				pointCluster = &clusters.at(j);
			}
		}
		pointCluster->AppendPoint(points.at(i));
		
	}
}

void start(int k, int threadNum, string dataPath) {
	auto start = chrono::high_resolution_clock::now();
	vector<long long> iterattionTimes;
    vector<Cluster> initialClusters;
    vector<Cluster> clusters;
	
	vector<Point> points = LoadData(dataPath);
	int dimension = points[0].GetData().size();

	for (int i = 0; i < k; i++)
	{
		int randomIndex = randomOverride[i];
		
		clusters.emplace_back(Cluster(points.at(randomIndex)));
		initialClusters.emplace_back(Cluster(points.at(randomIndex)));
	}
	Point mean = clusters.at(0).GetMean(points, dimension);

	int pointsPerThread = points.size() / threadNum;

	int l = 0;
	while (l != k)
	{
		vector<thread> threads;
		auto startI = chrono::high_resolution_clock::now();
		for (int i = 0; i < clusters.size(); i++)
			clusters.at(i).ClearPoints();
		
		for (int i = 0; i < threadNum; i++) {
			int from = i * pointsPerThread;
			int to = i != threadNum - 1 ? pointsPerThread : pointsPerThread + (points.size() - pointsPerThread * threadNum);
			vector<Point> part(points.begin() + from, points.begin() + from + to);
			threads.emplace_back(thread(Kmeans, ref(clusters), part, dimension));
			//Kmeans(&clusters, part, dimension);
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
	//const int threadNum = 1;
	const string dataPath = "../../../generated.csv";
	for (int threadNum = 1; threadNum < 8; threadNum++) {
		start(k, threadNum, dataPath);
	}
	
	
}