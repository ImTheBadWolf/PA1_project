
using namespace std;
class Point {
	float* data;
	
public:
	Point(float* data) {
		this->data = data;
	}
	float* GetData() {
		return this->data;
	}
};