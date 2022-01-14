#include "Point.cpp"
#include <vector>
#include <cmath>

using namespace std;

class Cluster {
	Point* centroid;
	vector<Point> points;

	float Diff(Point* p1, Point* p2) {
		float total = 0;
		float* p1Data = p1->GetData();
		float* p2Data = p2->GetData();
		for (int i = 0; i < sizeof(p1Data); i++) {
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

	void AppendPoint(float* data) {
		Point newPoint = Point(data);
		points.emplace_back(newPoint);
	}

	void ClearPoints() {
		for (int i = 0; i < this->points.size(); i++) {
			delete &this->points.at(i);
		}
		this->points.clear();
	}

	vector<Point> GetPoints() {
		return this->points;
	}

	int RecalculateCentroid() {
		Point* newCentroid = GetMean(this->points);
		if (Diff(newCentroid, this->centroid) > 0) {
			delete this->centroid;
			this->centroid = newCentroid;
			return 0;
		}
		return 1;
	}

	Point* GetMean(vector<Point> points) {
		float mean[sizeof(points.at(0).GetData())];
		for (int i = 0; i < size(mean); i++) {
			mean[i] = 0;
		}

		for (int i = 0; i < points.size(); i++) {
			float* pointData = points.at(i).GetData();
			for (int j = 0; j < sizeof(pointData); j++) {
				mean[j] = mean[j] + pointData[j];
			}
		}
		for (int i = 0; i < size(mean); i++) {
			mean[i] = round(mean[i] / points.size() * 100) / 100;	
		}
		return new Point(mean);
	}

	Point* GetCentroid() {
		return this->centroid;
	}

};