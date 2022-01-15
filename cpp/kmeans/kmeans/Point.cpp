
using namespace std;
#include<vector>

class Point {
	vector<float> data;
	
public:
	Point() {

	}
	Point(vector<float> data) {
		this->data = data;
	}
	Point(Point* point) {
		int a = 5;
	}
	vector<float> GetData() {
		return this->data;
	}
};