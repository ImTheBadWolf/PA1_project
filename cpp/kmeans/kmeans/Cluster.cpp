
#include <mutex>
#include "Point.cpp"
#include <cmath>
#include <iostream>
using namespace std;

class Cluster {
	Point centroid;
	vector<Point> points;
	static inline mutex mtx;

	float Diff(Point p1, Point p2, int len) {
		float total = 0;
		vector<float> p1Data = p1.GetData();
		vector<float> p2Data = p2.GetData();
		for (int i = 0; i < len; i++) {
			total += abs(p1Data[i] - p2Data[i]);
		}
		return total;
	}

public:

	Cluster() {

	}

	Cluster(Point centroid) {
		this->centroid = centroid;
		this->points = vector<Point>();
	}

	void AppendPoint(Point point) {
		mtx.lock();
		points.push_back(point);
		mtx.unlock();
	}

	void ClearPoints() {
		this->points.clear();
	}

	vector<Point> GetPoints() {
		return this->points;
	}

	int RecalculateCentroid() {
		int len = this->points.at(0).GetData().size();
		Point newCentroid = GetMean(this->points, len);
		if (Diff(newCentroid, this->centroid, len) > 0) {
			this->centroid = newCentroid;
			return 0;
		}
		return 1;
	}

	Point GetMean(vector<Point> points, int len) {
		vector<float> mean;
		for (int i = 0; i < len; i++) {
			mean.push_back(0);
		}

		for (int i = 0; i < points.size(); i++) {
			vector<float> pointData = points.at(i).GetData();
			for (int j = 0; j < len; j++) {
				mean[j] = mean[j] + pointData[j];
			}
		}
		for (int i = 0; i < len; i++) {
			mean[i] = round(mean[i] / points.size() * 100) / 100;	
		}
		Point nP(mean);
		return nP;
	}

	Point GetCentroid() {
		return this->centroid;
	}

};