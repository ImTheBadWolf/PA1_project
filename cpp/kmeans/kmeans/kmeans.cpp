#include <iostream>
#include <vector>
#include "Cluster.cpp"
#include <string>
#include <sstream>
#include <fstream>


using namespace std;


int randomOverride[] = { 620, 1552, 1115 };

void LoadData(string dataPath, vector<Point>* points) {
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
			float* values = new float[counter];
			stringstream str(line);
			while (getline(str, word, ',')) {
				values[i] = stof(word);
				i++;
			}
			points->emplace_back(Point(values));
		}
	}


}

void start(int k, int thredNum, string dataPath) {
    vector<Cluster> initialClusters;
    vector<Cluster> clusters;
    vector<Point> points;

    LoadData(dataPath, &points);
	cout << "Data loaded" << endl;
}

int main()
{
	const int k = 3;
	const int threadNum = 1;
	const string dataPath = "../../../smol_data.csv";
	std::cout << "Hello World!\n";
	start(k, threadNum, dataPath);
}