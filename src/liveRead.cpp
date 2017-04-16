
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <zbar.h>
#include "liveRead.h"
#include <process.h>

using namespace zbar;

LiveRead::LiveRead()
{
	m_test_base_URL = "start \"C:/Program Files (x86)/Internet Explorer/iexplore.exe\" \"https://csdev1-registerqfl.cs41.force.com/PassQR?";
	m_official_base_URL = "start \"C:/Program Files (x86)/Internet Explorer/iexplore.exe\" \"https://csdev1-registerqfl.cs41.force.com/PassQR?";
	m_stop_flag = false;
}

int32_t LiveRead::parse_code(std::string input_code)
{
	if (input_code.empty())
		return -1;
	std::size_t found0, found1;
	std::string temp = input_code;

	found0 = temp.find(',');
	if (found0 != std::string::npos)
		m_kwords.person_id = temp.substr(0, found0);
	temp = temp.substr(found0 + 1);
	found1 = temp.find(',');
	if (found1 != std::string::npos)
		m_kwords.party_type = temp.substr(0, found1);
	temp = temp.substr(found1 + 1);
	m_kwords.church = temp.substr(0, -1);

	return 0;
}


int32_t LiveRead::zbar_video_detect(void)
{
	int32_t ret;
	m_data = "";
	cv::VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		std::cout << "Cannot open the video cam" << std::endl;
		ret = -1;
		return ret;
	}

	bool detected = false;
	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	std::cout << "Frame size : " << dWidth << " x " << dHeight << std::endl;

	cv::namedWindow("QR Code Scanning Window", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	while (1)
	{
		cv::Mat frame;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			std::cout << "Cannot read a frame from video stream" << std::endl;
			ret = -2;
			break;
		}

		cv::Mat grey;
		cvtColor(frame, grey, CV_BGR2GRAY);

		int32_t width = frame.cols;
		int32_t height = frame.rows;
		uchar *raw = (uchar *)grey.data;
		// wrap image data  
		Image image(width, height, "Y800", raw, width * height);
		// scan the image for barcodes  
		int32_t n = scanner.scan(image);

		if (n != 0)
			detected = true;
		// extract results  
		for (Image::SymbolIterator symbol = image.symbol_begin();
			symbol != image.symbol_end();
			++symbol) {
			std::vector<cv::Point> vp;
			// do something useful with results  
			m_data = symbol->get_data();
			std::cout << "decoded " << symbol->get_type_name() << " symbol \"" << symbol->get_data() << '"' << " " << std::endl;

			// find 4 corners of qrcode
			int32_t n = symbol->get_location_size();
			for (int32_t i = 0; i < n; i++){
				vp.push_back(cv::Point(symbol->get_location_x(i), symbol->get_location_y(i)));
			}
			cv::RotatedRect r = minAreaRect(vp);
			cv::Point2f pts[4];
			r.points(pts);
			for (int32_t i = 0; i < 4; i++){
				line(frame, pts[i], pts[(i + 1) % 4], cv::Scalar(255, 0, 0), 3);
			}
			std::cout << "Angle: " << r.angle << std::endl;
		}

		imshow("QR Code Scanning Window", frame); //show the frame in "MyVideo" window

		char c = cv::waitKey(30);
		if(c == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			std::cout << "Esc key is pressed by user" << std::endl;
			cvDestroyWindow("QR Code Scanning Window");
			ret = -3;
			break;
		}

		if (detected) {
			ret = 0;
			cvDestroyWindow("QR Code Scanning Window");
			break;
		}

		if (m_stop_flag) {
			ret = -4;
			cvDestroyWindow("QR Code Scanning Window");
			break;
		}
	}
	return ret;
}

int32_t LiveRead::checkSalesforce(std::string base_URL, std::string words)
{
	std::string search_URL;
	search_URL = base_URL + words + "\"";
	const char *url = search_URL.c_str();
	std::system(url);
	std::cout << "Attendee ID: \"" << words << "\"\n";

	//ShellExecuteA(NULL, "open", search_URL.c_str(), NULL, NULL, SW_SHOWNORMAL);
	return 0;
}


void LiveRead::stop()
{
	m_stop_flag = true;
}

