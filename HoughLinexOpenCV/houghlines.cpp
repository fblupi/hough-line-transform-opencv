#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

#define MAX_LINES 5

static void help() {
	cout << "\nThis program demonstrates line finding with the Hough transform.\n"
		"Usage:\n"
		"./houghlines <image_name>, Default is ../data/pic1.png\n" << endl;
}

bool longerLine(pair<pair<Point, Point>, double> i, pair<pair<Point, Point>, double> j) {
	return i.second > j.second;
}

int main(int argc, char** argv) {
	CommandLineParser parser(argc, argv, "{help h||}{@image|../data/pic1.png|}");
	if (parser.has("help")) {
		help();
		return 0;
	}
	string filename = parser.get<string>("@image");
	if (filename.empty()) {
		help();
		cout << "no image_name provided" << endl;
		return -1;
	}
	Mat src = imread(filename, 0);
	if (src.empty()) {
		help();
		cout << "can not open " << filename << endl;
		return -1;
	}

	Mat dst, cdst;
	//blur(src, src, Size(3, 3));
	Canny(src, dst, 30, 80, 3);
	cvtColor(dst, cdst, COLOR_GRAY2BGR);

	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
	vector<pair<pair<Point, Point>, double> > foundLines;
	for (size_t i = 0; i < lines.size(); i++) {
		pair<pair<Point, Point>, double> l = make_pair(
			make_pair(
				Point(lines[i][0], lines[i][1]), 
				Point(lines[i][2], lines[i][3])
			), 
			norm(Point(lines[i][0], lines[i][1]) - Point(lines[i][2], lines[i][3])));
		foundLines.push_back(l);
	}
	sort(foundLines.begin(), foundLines.end(), longerLine);

	size_t numLines = foundLines.size() > MAX_LINES ? MAX_LINES : foundLines.size();

	for (size_t i = 0; i < numLines; i++) {
		Point p0 = foundLines[i].first.first, p1 = foundLines[i].first.second;
		Scalar color = i == 0 ? Scalar(255, 0, 0) : Scalar(0, 0, 255);
		line(cdst, p0, p1, color, 1, LINE_AA);
	}

	imshow("source", src);
	imshow("detected lines", cdst);

	waitKey();

	return 0;
}