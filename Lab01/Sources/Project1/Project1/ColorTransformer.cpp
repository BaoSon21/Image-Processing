#include "ColorTransformer.h"

int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b)
{
	if (sourceImage.empty())
		return 0;
	int width = sourceImage.cols, height = sourceImage.rows;
	int nChannels = sourceImage.channels();
	destinationImage = sourceImage.clone();

	for (int y = 0; y < height; y++)
	{
		uchar* pRow = (uchar*)sourceImage.ptr<uchar>(y);
		uchar* pRow_des = destinationImage.ptr<uchar>(y);
		for (int x = 0; x < width; x++, pRow += nChannels, pRow_des += nChannels)
		{
			float B = pRow[0];
			float G = pRow[1];
			float R = pRow[2];
			pRow_des[0] = saturate_cast<uchar>(B + b);
			pRow_des[1] = saturate_cast<uchar>(G + b);
			pRow_des[2] = saturate_cast<uchar>(R + b);
		}
	}
	return 1;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c)
{
	if (sourceImage.empty())
		return 0;
	int width = sourceImage.cols, height = sourceImage.rows;
	int nChannels = sourceImage.channels();
	destinationImage = sourceImage.clone();

	for (int y = 0; y < height; y++)
	{
		uchar* pRow = (uchar*)sourceImage.ptr<uchar>(y);
		uchar* pRow_des = destinationImage.ptr<uchar>(y);
		for (int x = 0; x < width; x++, pRow += nChannels, pRow_des += nChannels)
		{
			float B = pRow[0];
			float G = pRow[1];
			float R = pRow[2];
			pRow_des[0] = saturate_cast<uchar>(B * c);
			pRow_des[1] = saturate_cast<uchar>(G * c);
			pRow_des[2] = saturate_cast<uchar>(R * c);
		}
	}
	return 1;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix)
{
	if (sourceImage.data == NULL)
		return 0;
	int width = sourceImage.cols, height = sourceImage.rows;
	int nChannels = sourceImage.channels();
	histMatrix = Mat::zeros(Size(256, nChannels), CV_32S);

	int* pHistRowB = (int*)(histMatrix.ptr<int>(0));
	int* pHistRowG = NULL;
	int* pHistRowR = NULL;
	if (nChannels == 3)
	{
		pHistRowG = (int*)(histMatrix.ptr<int>(1));
		pHistRowR = (int*)(histMatrix.ptr<int>(2));
	}

	for (int y = 0; y < height; y++)
	{
		uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
		for (int x = 0; x < width; x++, pSourceRow += nChannels)
		{
			if (nChannels == 3)	
			{
				// Get value r,g,b of each pixel
				int B = (int)(pSourceRow[0]);
				int G = (int)(pSourceRow[1]);
				int R = (int)(pSourceRow[2]);
				pHistRowB[B] += 1;
				pHistRowG[G] += 1;
				pHistRowR[R] += 1;
			}
			else
			{
				int index = (int)(pSourceRow[0]);
				pHistRowB[index]++;
			}
			
		}
	}
	return 1;
}


int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage)
{
	if (sourceImage.data == NULL)
		return 0;
	int width = sourceImage.cols, height = sourceImage.rows;
	int sourceChannels = sourceImage.channels();
	Mat histMatrix;
	//cal histogram of sourceImage into histMatrix
	CalcHistogram(sourceImage, histMatrix);

	int* pHistRowB = (int*)(histMatrix.ptr<int>(0));
	int* pHistRowG = NULL;
	int* pHistRowR = NULL;
	if (sourceChannels == 3)
	{
		pHistRowG = (int*)(histMatrix.ptr<int>(1));
		pHistRowR = (int*)(histMatrix.ptr<int>(2));
	}
	Mat tMatrix = Mat::zeros(Size(256, sourceChannels), CV_32S);
	int* pTMatRowB = (int*)(tMatrix.ptr<int>(0));
	int* pTMatRowG = NULL;
	int* pTMatRowR = NULL;
	if (sourceChannels == 3)
	{
		pTMatRowG = (int*)(tMatrix.ptr<int>(1));
		pTMatRowR = (int*)(tMatrix.ptr<int>(2));
	}
	/*
	Calculate T
	T[0]=H[0]
	T[p]=T[p-1]+H[p]
	*/
	pTMatRowB[0] = pHistRowB[0];
	if (sourceChannels == 3)
	{
		pTMatRowG[0] = pHistRowG[0];
		pTMatRowR[0] = pHistRowR[0];
	}

	for (int i = 1; i < 256; i++)
	{
		pTMatRowB[i] = pTMatRowB[i - 1] + pHistRowB[i];
		if (sourceChannels == 3)
		{
			pTMatRowG[i] = pTMatRowG[i - 1] + pHistRowG[i];
			pTMatRowR[i] = pTMatRowR[i - 1] + pHistRowR[i];
		}
	}
	/*
	Standard T from [0,255]
	T[p]=round(((nG-1)/NM)*T[p])
	*/
	for (int i = 0; i < 256; i++)
	{
		pTMatRowB[i] = cvRound((255.0 / ((double)width * (double)height)) * (double)(pTMatRowB[i]));
		if (sourceChannels == 3)
		{
			pTMatRowG[i] = cvRound((255.0 / ((double)width * (double)height)) * (double)(pTMatRowG[i]));
			pTMatRowR[i] = cvRound((255.0 / ((double)width * (double)height)) * (double)(pTMatRowR[i]));
		}
	}
	destinationImage = Mat(Size(width, height), CV_8UC3);
	if (sourceChannels == 3)
		destinationImage = Mat(Size(width, height), CV_8UC3);
	else
		destinationImage = Mat(Size(width, height), CV_8UC1);
	int destinationChannels = destinationImage.channels();
	/*
	create image result
	g(x,y)=T[f(x,y)]
	*/
	for (int y = 0; y < height; y++)
	{
		uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar* pDestinationRow = (uchar*)(destinationImage.ptr<uchar>(y));
		for (int x = 0; x < width; x++, pSourceRow += sourceChannels, pDestinationRow += destinationChannels)
		{
			if (sourceChannels == 3)
			{
				uchar B = pSourceRow[0];
				uchar G = pSourceRow[1];
				uchar R = pSourceRow[2];
				pDestinationRow[0] = (uchar)pTMatRowB[(int)B];
				pDestinationRow[1] = (uchar)pTMatRowG[(int)G];
				pDestinationRow[2] = (uchar)pTMatRowR[(int)R];
			}
			else
			{
				uchar gray = pSourceRow[0];
				pDestinationRow[0] = (uchar)pTMatRowB[(int)gray];
			}
		}
	}
	return 1;
}

int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage)
{
	if (histMatrix.empty())
		return 0;

	Mat histogram;
	int width = histMatrix.cols, height = histMatrix.rows;
	histImage = Mat::zeros(height, width, CV_8UC3);

	signed max_b = 0, max_g = 0, max_r = 0;
	float coeff[3];
	uchar color[3][3] = { 255, 0, 0, 0, 255, 0, 0, 0, 255 };

	CalcHistogram(histMatrix, histogram);

	//Find the maximum bin of each channel
	for (int i = 0; i < histogram.cols; i++)
	{
		max_r = max(max_r, histogram.at<signed>(2, i));
		if (histMatrix.channels() != 1)
		{
			max_g = max(max_g, histogram.at<signed>(1, i));
			max_b = max(max_b, histogram.at<signed>(0, i));
		}
	}

	// The coefficient that multiply with value of each bin
	// to ensure the maximum height is the height of @histImage
	coeff[2] = float(histImage.rows) / max_r;
	if (histMatrix.channels() != 1)
	{
		coeff[1] = float(histImage.rows) / max_g;
		coeff[0] = float(histImage.rows) / max_b;
	}

	// Have 256 bins, so the distance between 2 bins is @histImage.cols / 255
	float binWidth = float(histImage.cols) / (255);

	for (int channel = 0; channel < histogram.rows; channel++)
	{
		// Cordinate of the 1st bin in @histImage
		float prev_x = 0;
		float prev_y = histImage.rows - histogram.at<signed>(channel, 0)*coeff[channel];

		for (int bin = 1; bin < histogram.cols; bin++)
		{
			float x = binWidth * bin;
			float y = histImage.rows - histogram.at<signed>(channel, bin)*coeff[channel];
			Vec3b color3b;
			if (histMatrix.channels() != 1)
				color3b = Vec3b(color[channel][0], color[channel][1], color[channel][2]);
			else
				color3b = Vec3b(255, 255, 255);

			// Draw line connect current point with previous point
			line(histImage, Point(prev_x, prev_y), Point(x, y), color3b, 2, 8);
			// Update previous point
			prev_x = x;
			prev_y = y;
		}
	}

	return 1;
}

