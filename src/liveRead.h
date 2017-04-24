#ifndef LIVEREAD_H
#define LIVEREAD_H

#include "qflLib_defs.h"
#include <string> /*for size_t*/
#include <cstdint>
#include <zbar.h>
#include <opencv2/opencv.hpp>

class QFLLIB_EXPORT LiveRead {
public:

	//! QR code format
	typedef struct KeyWords{
		std::string person_id;
		std::string party_type;
		std::string church;
	} KeyWords;

	//! default constructor
	LiveRead();
	//! default destructor
	~LiveRead(){};

	//! QR code detection from live video using zbar library
	int32_t zbar_video_detect(void);

	//! QR code detection on an image
	int32_t zbar_qrcode_detect(cv::Mat &img);

	//! stop QR code detect
	void stop();

	//! QR code parsing to code format
	int32_t parse_code(std::string input_code);

	int32_t checkSalesforce(std::string base_URL,  std::string words);

	//! QR code string
	inline std::string getCode() { return m_data; }

	//! key word
	inline std::string getKeyword() { return m_kwords.person_id; }

	//! get record words
	inline KeyWords *getRecordWords() { return &m_kwords; }

	//! get test url
	inline std::string getTestURL() { return m_test_base_URL; }

	//! get official url
	inline std::string getOfficialURL() { return m_official_base_URL; }

	inline void EnableStopFlag(bool flag) { m_stop_flag = flag; }

private:

	std::string m_test_base_URL; //!< salesforce URL for test
	std::string m_official_base_URL; //!< salesforce URL for official check-in
	std::string m_data; //!< detected QR code string
	KeyWords m_kwords;  //!< parsed key words
	bool m_stop_flag; //!< stop bar code detect
	zbar::ImageScanner m_scanner;
};

#endif
