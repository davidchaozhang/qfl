#ifndef _CHURCHES_H_
#define _CHURCHES_H_

#include "qflLib_defs.h"
#include <string> /*for size_t*/
#include <cstdint>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>


class QFLLIB_EXPORT ChurchList {
public:
	typedef struct {
		std::string church_id;
		int32_t church_code;
		std::string church_name;
		std::string church_in_chinese;
		std::string church_ini;
		std::string church_city;
		int32_t church_zip;
		std::string church_contact;
		std::string church_state;
		std::string church_phone;
		std::string church_email;
		std::string church_web;
		int32_t rank; // order by church size in attendees
	} QFLChurch;

	ChurchList();
	~ChurchList();

	int readInChurchList(const char * churches);
	QFLChurch *getChurch(std::string church_name);

	void sortbyName();
	void sortbyZip();
	void sortbyState();
	void sortbyChurchId();
	void sortbyChurchCode();
	void sortbyRank();

	std::vector<QFLChurch*> matchbyName(std::string name);
	std::vector<QFLChurch*> matchbyZip(std::string name, int32_t range);
	std::vector<QFLChurch*> matchbyCity(std::string city);
	std::vector<QFLChurch*> matchbyState(std::string state);

	inline std::vector<QFLChurch> * getChurchList() {
		return &m_churches;
	}

private:

	std::vector<QFLChurch> m_churches;
};
#endif
