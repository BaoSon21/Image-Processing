#include "Converter.h"

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage)
{
	if (sourceImage.empty()) {
		return 1;
	}
	int width = sourceImage.cols, height = sourceImage.rows;
	int nChannels = sourceImage.channels();
	destinationImage = Mat(height, width, CV_8UC1);
	Mat image = sourceImage.clone();

	for (int y = 0; y < height; y++)
	{
		uchar* pRow = image.ptr<uchar>(y);
		uchar* pRow_des = destinationImage.ptr<uchar>(y);
		for (int x = 0; x < width; x++, pRow += nChannels, pRow_des++)
		{
			//B G R 
			uchar gray = (uchar)(0.11 * pRow[0] + 0.59 * pRow[1] + 0.3 * pRow[2]);
			pRow_des[0] = gray;
		}
	}
	return 0;
}

int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage)
{
	if (sourceImage.data == NULL)
		return 1;
	int width = sourceImage.cols, height = sourceImage.rows;
	int nChannels = sourceImage.channels();
	destinationImage = sourceImage.clone();
	float R, G, B, V, H, S, Min,delta;
	for (int y = 0; y < height; y++)
	{
		uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar* pDestinationRow = (uchar*)(destinationImage.ptr<uchar>(y));
		for (int x = 0; x < width; x++, pSourceRow += nChannels, pDestinationRow += nChannels)
		{
			B = pSourceRow[0];
			G = pSourceRow[1];
			R = pSourceRow[2];
			V = max(B,max( G, R));
			Min = min(B, min(G, R));
			delta = V - Min;
			if (V == 0.0f)
			{
				S = 0;
				H = 0;
			}
			else
			{
				S = (V - min(B, min(G, R))) / V;;
				if (V == B)
					H = 1.0 * (60 * (G - B)) / delta;
				if (V == G)
					H = 120 + 1.0 * 60 * (B - R) / delta;
				if (V == R)
					H = 240 + 1.0 * 60 * (R - G) / delta;;
				if (H < 0) H += 360.0f;
			}
			pDestinationRow[0] = saturate_cast<uchar>(V);
			pDestinationRow[1] = saturate_cast<uchar>(S*255); 
			pDestinationRow[2] = saturate_cast<uchar>(H / 2);
		}
	}
	return 0;
}


int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type)
{
	switch (type)
	{
	case 0:
		return RGB2GrayScale(sourceImage, destinationImage);
		break;
	case 1:
		return RGB2HSV(sourceImage, destinationImage);
		break;
	default:
		return 1;
	}
}
