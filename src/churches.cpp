#include "churches.h"
#include <fstream>
#include <algorithm>


static bool UDbyName(const ChurchList::QFLChurch &a, const ChurchList::QFLChurch &b)
{
	return (a.church_name > b.church_name);
}
static bool UDbyZip(const ChurchList::QFLChurch &a, const ChurchList::QFLChurch &b)
{
	return (a.church_zip < b.church_zip);
}
static bool UDbyState(const ChurchList::QFLChurch &a, const ChurchList::QFLChurch &b)
{
	return (a.church_state < b.church_state);
}

static bool UDbyChurchId(const ChurchList::QFLChurch &a, const ChurchList::QFLChurch &b)
{
	return (a.church_id < b.church_id);
}

static bool UDbyChurchCode(const ChurchList::QFLChurch &a, const ChurchList::QFLChurch &b)
{
	return (a.church_code < b.church_code);
}

static bool UDbyChurchRank(const ChurchList::QFLChurch &a, const ChurchList::QFLChurch &b)
{
	return (a.rank < b.rank);
}

ChurchList::ChurchList()
{}

ChurchList::~ChurchList()
{}

int ChurchList::readInChurchList(const char* churches)
{
	if (churches == NULL)
		return -1;

	std::ifstream fin(churches);
	if (!fin.is_open())
		return -2;

	std::vector<std::string> tmp;
	std::vector<std::vector<std::string>> church_array;
	size_t y;
	// read in all the data
	while (!fin.eof()) {
		std::string line0;
		size_t pos = std::string::npos;
		std::getline(fin, line0);
		tmp.clear();
		//loop through the line,
		while ((pos = line0.find_first_of(",")) != std::string::npos)
		{
			//extract the component sans ,
			if ((pos = line0.find_first_of("\"", pos - 1)) != std::string::npos)
			{
				pos++;
				tmp.push_back(line0.substr(0, pos));
				//erase the val including ,
				line0.erase(0, pos + 1);
			}
		}
			tmp.push_back(line0);
			line0.erase();
			church_array.push_back(tmp);
	}

	QFLChurch qflc;
	std::vector<std::string> achurch = church_array[0];
	size_t n = achurch.size();

	for (y = 1; y < church_array.size(); y++){ 
		achurch = church_array[y];
		if (achurch.size() != n) {
			church_array.erase(church_array.begin() + y);
			y--;
			continue;
		}

		qflc.church_id = achurch[0].substr(1, achurch[0].size() - 2);
		qflc.church_code = std::stoi(achurch[1].substr(1, achurch[1].size()- 2));
		qflc.church_name = achurch[2].substr(1, achurch[2].size() - 2);
		qflc.church_ini = achurch[3].substr(1, achurch[3].size() - 2);
		qflc.church_in_chinese = achurch[4].substr(1, achurch[4].size() - 2);
		if (achurch[6].size() > 2)
			qflc.church_city = achurch[6].substr(1, achurch[6].size() - 2);
		else
			qflc.church_city = "";

		if (achurch[7].size() > 2)
			qflc.church_zip = std::stoi(achurch[7].substr(1, achurch[7].size() - 2));
		else
			qflc.church_zip = 0;

		if (achurch[8].size() > 2)
			qflc.church_state = achurch[8].substr(1, achurch[8].size() - 2);
		else
			qflc.church_state = "";

		// contact
		if (achurch[9].size() > 2)
			qflc.church_contact = achurch[9].substr(1, achurch[12].size() - 2);
		else
			qflc.church_contact = "";

		// phone number
		if (achurch[10].size() > 2)
			qflc.church_phone = achurch[10].substr(1, achurch[12].size() - 2);
		else
			qflc.church_phone = "";

		// email
		if (achurch[12].size() > 2)
			qflc.church_email = achurch[12].substr(1, achurch[12].size() - 2);
		else
			qflc.church_email = "";

		// church website
		if (achurch[11].size() > 2)
			qflc.church_web = achurch[11].substr(1, achurch[11].size() - 2);
		else
			qflc.church_web = "";

//		if (achurch[13].size() > 2)
//			qflc.rank = std::stoi(achurch[13].substr(1, achurch[13].size() - 2));
		qflc.rank = (int32_t)y; // change me when new data is available
		m_churches.push_back(qflc);
	}

	return 0;
}

ChurchList::QFLChurch *ChurchList::getChurch(std::string church_name)
{
	std::vector<QFLChurch> *chlst = getChurchList();
	ChurchList::QFLChurch * ch = NULL;
	for (int i = 0; i < chlst->size(); i++) {
		std::string name = (*chlst)[i].church_name;
		if (name.compare(church_name) == 0) {
			ch = &(*chlst)[i];
			break;
		}
	}
	return ch;
}

void ChurchList::sortbyName()
{
	std::sort(m_churches.begin(), m_churches.end(), UDbyName);
}

void ChurchList::sortbyZip()
{
	std::sort(m_churches.begin(), m_churches.end(), UDbyZip);
}

void ChurchList::sortbyState()
{
	std::sort(m_churches.begin(), m_churches.end(), UDbyState);
}

void ChurchList::sortbyChurchId()
{
	std::sort(m_churches.begin(), m_churches.end(), UDbyChurchId);
}

void ChurchList::sortbyChurchCode()
{
	std::sort(m_churches.begin(), m_churches.end(), UDbyChurchCode);
}

void ChurchList::sortbyRank()
{
	std::sort(m_churches.begin(), m_churches.end(), UDbyChurchRank);
}