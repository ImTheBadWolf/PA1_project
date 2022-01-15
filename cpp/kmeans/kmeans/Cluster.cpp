#include "Point.cpp"
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

class Cluster {
	Point* centroid;
	vector<Point> points;

	float Diff(Point* p1, Point* p2, int len) {
		float total = 0;
		float* p1Data = p1->GetData();
		float* p2Data = p2->GetData();
		for (int i = 0; i < len; i++) {
			total += abs(p1Data[i] - p2Data[i]);
		}
		return total;
	}

public:

	Cluster() {

	}

	Cluster(Point* centroid) {
		this->centroid = centroid;
		this->points = vector<Point>();
	}

	void AppendPoint(Point point) {
		points.emplace_back(point);
	}

	void ClearPoints() {
		this->points.clear();
	}

	vector<Point> GetPoints() {
		return this->points;
	}

	int RecalculateCentroid() {
		int len = sizeof(this->points.at(0).GetData());
		Point* newCentroid = GetMean(this->points, len);
		if (Diff(newCentroid, this->centroid, len) > 0) {
			this->centroid = newCentroid;
			return 0;
		}
		return 1;
	}

	Point* GetMean(vector<Point> points, int len) {
		float* mean = new float[len];
		for (int i = 0; i < len; i++) {
			mean[i] = 0;
		}

		for (int i = 0; i < points.size(); i++) {
			float* pointData = points.at(i).GetData();
			for (int j = 0; j < len; j++) {
				mean[j] = mean[j] + pointData[j];
			}
		}
		for (int i = 0; i < len; i++) {
			mean[i] = round(mean[i] / points.size() * 100) / 100;	
		}
		Point* nP = new Point(mean);
		return nP;
	}

	Point* GetCentroid() {
		return this->centroid;
	}

};