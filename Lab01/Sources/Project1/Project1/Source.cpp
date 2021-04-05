#include "Converter.h"
#include "ColorTransformer.h"

int main(int argc, char* argv[])
{
	string command = argv[1];
	string input = argv[2];
	string output = argv[3];
	if (haveImageReader(input) == false)
	{
		std::cout << "Input image path not invalid.";
		return 1;
	}

	Mat source = imread(input);
	Mat dest;
	
	if (argc == 4) {
		int type;
		if (command == "-rgb2gray")
			type = 0;
		else if (command == "-rgb2hsv")
			type = 1;
		else if (command == "-drawhist")
			type = 2;
		else if (command == "-equalhist")
			type = 3;
		else {
			cout << "Invalid command" << endl;
			return 1;
		}
		if (type == 2) {
			ColorTransformer transformer;
			transformer.DrawHistogram(source, dest);
		}
		else if (type == 3) {
			ColorTransformer transformer;
			transformer.HistogramEqualization(source, dest);
		}
		else {
			Converter converter;
			converter.Convert(source, dest, type);
		}
	}
	else if (argc == 5) {
		string value = argv[4];
		ColorTransformer transformer;
		if (command == "-bright") {
			float param = stof(value);
			transformer.ChangeBrighness(source, dest, param);
		}
		else if (command == "-contrast") {
			float param = stof(value);
			transformer.ChangeContrast(source, dest, param);
		}
		else {
			cout << "Invalid commad" << endl;
			return 1;
		}
	}
	else {
		cout << "Wrong" << endl;
		return 1;
	}
	imshow("Original", source);
	imwrite(argv[3], dest);
	imshow("result", dest);
	waitKey(0);
	return 0;
}
