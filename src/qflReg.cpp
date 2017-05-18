#include <algorithm>
#include <fstream>
#include <forward_list>
#include <array>
#include <vector>
#include <functional>      // For greater<int>( )
#include <iostream>
#include <assert.h>
#include "QflReg.h"
#include <string>

#ifdef WIN32
#include <windows.h>
#include <sys/utime.h>
#include <sstream>
#include <ctime>
#else
#include <ctime>
#endif

using namespace std;

static bool QDbyListSize(std::pair<std::string, QflReg::QFL_Church> const & a, std::pair<std::string, QflReg::QFL_Church> const & b)
{
	QflReg::QFL_Church church_a = a.second;
	QflReg::QFL_Church church_b = b.second;
	int32_t num_a = church_a.persons[QflReg::qEU].size() + church_a.persons[QflReg::qCabrini].size() + church_a.persons[QflReg::qChild].size() + church_a.persons[QflReg::qSenior].size();
	int32_t num_b = church_b.persons[QflReg::qEU].size() + church_b.persons[QflReg::qCabrini].size() + church_b.persons[QflReg::qChild].size() + church_b.persons[QflReg::qSenior].size();
	return (num_a > num_b);
};



QflReg::QflReg()
{
	m_data.clear();
	m_registrants.clear();
	m_person_info.clear();
	m_attendee_list_byChurch.clear();
	m_room_building_list.clear();
	m_rccc_sheqin_list.clear();
	m_cellid = 1;
	m_cancelled = 0;
}

QflReg::~QflReg()
{}

std::string QflReg::intToString(int i)
{
	std::stringstream ss;
	std::string s;
	ss << i;
	s = ss.str();

	return s;
}

std::string QflReg::getCurTime()
{
	std::string cur_time;
#ifdef WIN32
	SYSTEMTIME  system_time;
	GetLocalTime(&system_time);
	cur_time = std::to_string(system_time.wYear) + "_" +
		std::to_string(system_time.wMonth) + "_" +
		std::to_string(system_time.wDay);
#else
	std::stringstream currentDateTime;
	// current date/time based on current system
	time_t ttNow = time(0);
	struct tm * now = localtime(&ttNow);
	cur_time = std::to_string(now->tm_year + 1900) + std::to_string(now->tm_mon + 1) + std::to_string(now->tm_mday);
#endif
	return cur_time;
}

int32_t QflReg::readChurchList(const char* churchname, int32_t year)
{
	int status;
	m_year = year;
	status = m_church_list.readInChurchList(churchname);
	m_church_list.sortbyState();
	return status;
}


int32_t QflReg::readRegistrants(const char *filename)
{
	int32_t i = 0;
	if (filename == NULL)
		return -1;

	std::ifstream fin(filename);
	if (!fin.is_open())
		return -2;

	std::vector<std::string> tmp;
	size_t y;
	// read in all the data
	while (!fin.eof()) {
		std::string line0, line1;
		size_t pos = std::string::npos;
		std::getline(fin, line0);
		pos = line0.find_last_of("\"");
		while (pos != line0.size() - 1) {
			line1.clear();
			std::getline(fin, line1);
			line0 = line0 + line1;
			pos = line0.find_last_of("\"");
		}

		tmp.clear();
		//loop through the line,
		while ((pos = line0.find("\",")) != std::string::npos)
		{
			//extract the component sans ,
			pos++;
			tmp.push_back(line0.substr(0, pos));
			//erase the val including ,
			line0.erase(0, pos + 1);
		}
		tmp.push_back(line0);
		line0.erase();
		m_data.push_back(tmp);
		i++;
	}

	std::vector<std::string> person = m_data[0];
	size_t n = person.size();

	for (y = 1; y < m_data.size(); y++){
		person = m_data[y];

		if (person.size() != n) {
			m_data.erase(m_data.begin() + y);
			y--;
		}
	}

	return 0;
}

/* The order of the parsing fields are:
0: person id
1: party id
2: church
3: contact person
4: party type
5: first name
6: last name
7: Chinese name
8: room number
9: cell group
10: need room
11: age group
12: gender
13: grade

20: need ride
21: offer ride
22: is_christian
23: occupation
24: mobile phone
25: email
26: city
27: state
28: zip
29: functional groups
30: services
*/
int32_t QflReg::parseAllFields()
{
	int32_t i;
	Registrant a_regist;
	if (m_data.size() == 0)
		return -1;

	std::vector<std::string> person = m_data[0];
	for (i = 1; i < m_data.size(); i++){
		person = m_data[i];
		// cancelled persons are removed from the list
		if (person[17].compare(Status::Cancelled) == 0) {
			printf("cancelled: %s %s, %s %s\n", person[0].c_str(), person[1].c_str(), person[2].c_str(), person[4].c_str());
			m_cancelled++;
			continue;
		}

		if (person[5].substr(1, person[5].size() - 2).size() == 0 && person[6].substr(1, person[5].size() - 2).size() == 0)
			continue;

		if (person[0].size() > 2)
			a_regist.person_id = std::stoi(person[0].substr(1, person[0].size() - 2));
		if (person[1].size() > 2)
			a_regist.party = std::stoi(person[1].substr(1, person[1].size() - 2));
		if (person[2].size() > 2)
			a_regist.church = person[2].substr(1, person[2].size() - 2);
		if (person[3].size() > 2)
			a_regist.contact_person = person[3].substr(1, person[3].size() - 2);
		a_regist.party_type = person[4].substr(1, person[4].size() - 2);
		a_regist.first_name = person[5].substr(1, person[5].size() - 2);
		a_regist.last_name = person[6].substr(1, person[6].size() - 2);
		a_regist.chinese_name = person[7].substr(1, person[7].size() - 2);

		a_regist.room = person[8].substr(1, person[8].size() - 2);
		a_regist.cell_group = person[9].substr(1, person[9].size() - 2);
		a_regist.need_room = (person[10].substr(1, person[10].size() - 2).compare(NeedRoom::RoomNeeded) == 0);

		a_regist.age_group = person[11].substr(1, person[11].size() - 2);
		a_regist.gender = person[12].substr(1, person[12].size() - 2);
		a_regist.grade = person[13].substr(1, person[13].size() - 2);
		a_regist.notes = person[17].substr(1, person[17].size() - 2);
		a_regist.special_need = person[18].substr(1, person[18].size() - 2);

		a_regist.need_ride = person[20].substr(1, person[20].size() - 2);
		a_regist.offer_ride = person[21].substr(1, person[21].size() - 2);
		a_regist.is_christian = (person[22].substr(1, person[22].size() - 2).compare("Yes") == 0);
		a_regist.occupation = person[23].substr(1, person[23].size() - 2);
		a_regist.mobile_phone = person[24].substr(1, person[24].size() - 2);
		a_regist.email = person[25].substr(1, person[25].size() - 2);
		a_regist.city = person[26].substr(1, person[26].size() - 2);;
		a_regist.state = person[27].substr(1, person[27].size() - 2);;
		a_regist.zip = std::stoi(person[28].substr(1, person[28].size() - 2));
		a_regist.functional_group = person[29].substr(1, person[29].size() - 2);
		a_regist.services = person[30].substr(1, person[30].size() - 2);
		m_registrants.push_back(a_regist);
		m_person_info[a_regist.person_id] = a_regist;
	}


	return 0;
}

int32_t QflReg::classifications()
{
	int32_t i, j;
	if (m_registrants.size() == 0)
		return -1;

	Registrant a_regist, b_regist;
	Family a_family;
	for (i = 0; i < m_registrants.size(); i++){
		a_regist = m_registrants[i];
		if (a_regist.grade.find("Stay with Youth") != std::string::npos || a_regist.grade.find("stay with Youth") != std::string::npos)
			m_youth_camp_list.push_back(a_regist.person_id);
		if (a_regist.grade.find("Stay with Parent") != std::string::npos)
			m_youth_stays_with_parent_list.push_back(a_regist.person_id);
		if (a_regist.services.find("Service Youth") != std::string::npos || a_regist.cell_group.find("Youth SGLeaders") != std::string::npos)
			m_youth_leader_list.push_back(a_regist.person_id);
		if (a_regist.services.find("Service Child") != std::string::npos)
			m_child_leader_list.push_back(a_regist.person_id);
		if (a_regist.is_christian)
			m_christian_list.push_back(a_regist.person_id);
		else
			m_non_christian_list.push_back(a_regist.person_id);

		if (a_regist.age_group.compare(AgeGroup::A1) == 0) {
			m_baby_list.push_back(a_regist.person_id);
		}
		else if (a_regist.age_group.compare(AgeGroup::A66_69) == 0 || a_regist.age_group.compare(AgeGroup::A70) == 0) {
			m_senior_list.push_back(a_regist.person_id);
		}
		else if (a_regist.notes.find("è½®æ¤…é€šé“") != std::string::npos || a_regist.special_need.find("è½®æ¤…é€šé“") != std::string::npos) {
			m_special_need_list.push_back(a_regist.person_id);
		}

		if (!a_regist.need_room)
			m_commute_list.push_back(a_regist.person_id);

		if (a_regist.services.find("Logistics") != std::string::npos || a_regist.services.find("Recording") != std::string::npos ||
			a_regist.services.find("Traffic Control") != std::string::npos || a_regist.services.find("Usher") != std::string::npos ||
			a_regist.services.find("Friday Snack Service") != std::string::npos || a_regist.services.find("Recording") != std::string::npos ||
			a_regist.services.find("Any of the above services") != std::string::npos)
			m_logistics_list.push_back(a_regist.person_id);

		// family registrations
		if (a_regist.party_type.find(PartyType::qFamily) != std::string::npos) {
			a_family.party = a_regist.party;
			a_family.church = a_regist.church;
			a_family.city = a_regist.city;
			a_family.contact_person = a_regist.contact_person;
			a_family.email = a_regist.email;
			a_family.mobile_phone = a_regist.mobile_phone;
			a_family.state = a_regist.state;
			a_family.zip = a_regist.zip;
			bool matched = false;
			std::map<int32_t, Family>::iterator it = m_family_list.begin();
			while (it != m_family_list.end()) {
				int32_t party_index = it->first;
				Family family = it->second;
				if (party_index == a_regist.party && family.contact_person.compare(a_regist.contact_person) == 0) {
					matched = true;
					m_family_list[a_family.party].attendee_list.push_back(a_regist.person_id);
				}
				it++;
			}
			if (!matched) {
				m_family_list[a_family.party] = a_family;
				m_family_list[a_family.party].attendee_list.push_back(i);
			}
		}
		// individual registrations
		else {
			if (a_regist.gender.compare("Female") == 0) {
				m_female_list.push_back(a_regist.person_id);
			}
			else {
				m_male_list.push_back(a_regist.person_id);
			}
		}
	}


	return 0;
}

int32_t QflReg::sortAttendeesByChurches()
{
	int32_t i, j, cnt = 0;
	int32_t adult_christians = 0;
	int32_t adult_non_christians = 0;
	int32_t youth_christians = 0;
	int32_t youth_non_christians = 0;
	int32_t children = 0;

	int32_t rccc_adult_christians = 0;
	int32_t rccc_adult_non_christians = 0;
	int32_t rccc_youth_christians = 0;
	int32_t rccc_youth_non_christians = 0;
	int32_t rccc_children = 0;


	QFL_Church attendee_list;
	if (m_registrants.size() == 0)
		return -1;

	std::vector<ChurchList::QFLChurch> *church_list = m_church_list.getChurchList();
	m_attendee_list_byChurch.resize(church_list->size());
	for (j = 0; j < church_list->size(); j++) {
		m_attendee_list_byChurch[j].first = (*church_list)[j].church_name;
		m_attendee_list_byChurch[j].second = attendee_list;
		std::vector<CellGroup> cell_group;
		m_cell_groups[(*church_list)[j].church_name] = cell_group;
	}

	std::map<std::string, std::vector<CellGroup>> m_cell_groups;

	Registrant a_regist;
	for (i = 0; i < m_registrants.size(); i++){
		a_regist = m_registrants[i];
		for (j = 0; j < church_list->size(); j++) {
			std::string church_name = (*church_list)[j].church_name;
			std::string church_ini = (*church_list)[j].church_ini;
			int32_t church_code = (*church_list)[j].church_code;

			if (a_regist.church.compare(church_name) == 0) {
				// check EU or Cabrini
				bool youth = (a_regist.grade.find("Stay with Youth") != std::string::npos || a_regist.grade.find("stay with Youth") != std::string::npos
					|| a_regist.grade.find("Stay with Parent") != std::string::npos || a_regist.services.find("Service Youth") != std::string::npos
					|| a_regist.cell_group.find("Youth SGLeaders") != std::string::npos);

				bool child = (a_regist.age_group.compare(AgeGroup::A1) == 0 ||
					a_regist.age_group.compare(AgeGroup::A2) == 0) || (a_regist.age_group.compare(AgeGroup::A3) == 0)
					|| (a_regist.age_group.compare(AgeGroup::A4_5) == 0) || (a_regist.age_group.compare(AgeGroup::A6_11) == 0);

				bool senior = (a_regist.age_group.compare(AgeGroup::A66_69) == 0) || (a_regist.age_group.compare(AgeGroup::A70) == 0);

				bool little_coworker = a_regist.functional_group.compare("Childcare Coworker") == 0;

				m_attendee_list_byChurch[j].second.church_name = church_name;
				cnt++;
				if (youth) {
					m_attendee_list_byChurch[j].second.persons[QflReg::qCabrini].push_back(a_regist.person_id);
					if (a_regist.is_christian)
						youth_christians++;
					else
						youth_non_christians++;

					if (a_regist.church.compare("Rutgers Community Christian Church") == 0) {
						if (a_regist.is_christian)
							rccc_youth_christians++;
						else
							rccc_youth_non_christians++;
					}
				}
				else if (child) {
					m_attendee_list_byChurch[j].second.persons[QflReg::qChild].push_back(a_regist.person_id);
					children++;
					if (a_regist.church.compare("Rutgers Community Christian Church") == 0) {
						rccc_children++;
					}
				}
				else if (a_regist.age_group.compare(AgeGroup::A12_14) == 0 && !little_coworker) {
					m_attendee_list_byChurch[j].second.persons[QflReg::qChild].push_back(a_regist.person_id);
					children++;
					if (a_regist.church.compare("Rutgers Community Christian Church") == 0) {
						rccc_children++;
					}
				}
				else if (senior)
				{
					m_attendee_list_byChurch[j].second.persons[QflReg::qSenior].push_back(a_regist.person_id);
					if (a_regist.is_christian)
						adult_christians++;
					else
						adult_non_christians++;
					if (a_regist.church.compare("Rutgers Community Christian Church") == 0) {
						if (a_regist.is_christian)
							rccc_adult_christians++;
						else
							rccc_adult_non_christians++;
					}
				}
				else {
					m_attendee_list_byChurch[j].second.persons[QflReg::qEU].push_back(a_regist.person_id);
					if (a_regist.is_christian)
						adult_christians++;
					else
						adult_non_christians++;
					if (a_regist.church.compare("Rutgers Community Christian Church") == 0) {
						if (a_regist.is_christian)
							rccc_adult_christians++;
						else
							rccc_adult_non_christians++;
					}
				}
				break;
			}
		}
	}

	std::sort(m_attendee_list_byChurch.begin(), m_attendee_list_byChurch.end(), QDbyListSize);
	assert(cnt == m_registrants.size());
	// all church
	m_allchurch.adult_christian_ratio  = adult_christians / (adult_christians + adult_non_christians);
	m_allchurch.adult_christians = adult_christians;
	m_allchurch.adult_non_christians = adult_non_christians;

	m_allchurch.youth_christian_ratio = youth_christians / (youth_christians + youth_non_christians);
	m_allchurch.youth_christians = youth_christians;
	m_allchurch.youth_non_christians = youth_non_christians;

	m_allchurch.christian_ratio = (adult_christians + youth_christians) / (adult_christians + adult_non_christians
		+ youth_christians + youth_non_christians);
	m_allchurch.children = children;

	// rcc only
	m_rccc.adult_christian_ratio = rccc_adult_christians / (rccc_adult_christians + rccc_adult_non_christians);
	m_rccc.adult_christians = rccc_adult_christians;
	m_rccc.adult_non_christians = rccc_adult_non_christians;

	m_rccc.youth_christian_ratio = rccc_youth_christians / (rccc_youth_christians + rccc_youth_non_christians);
	m_rccc.youth_christians = rccc_youth_christians;
	m_rccc.youth_non_christians = rccc_youth_non_christians;

	m_rccc.christian_ratio = (rccc_adult_christians + rccc_youth_christians) / (rccc_adult_christians + rccc_adult_non_christians
		+ rccc_youth_christians + rccc_youth_non_christians);
	m_rccc.children = rccc_children;
	return 0;
}

int32_t QflReg::ageStatistics()
{
	int32_t i, j;
	std::vector<int32_t> attendee_list;
	if (m_registrants.size() == 0)
		return -1;

	m_age_statistics.resize(13);
	for (i = 0; i < m_age_statistics.size(); i++)
		m_age_statistics[i] = 0;

	Registrant a_regist;
	for (i = 0; i < m_registrants.size(); i++){
		a_regist = m_registrants[i];
		if (a_regist.age_group.find(AgeGroup::A70) != std::string::npos)
			m_age_statistics[12]++;
		else if (a_regist.age_group.find(AgeGroup::A66_69) != std::string::npos)
			m_age_statistics[11]++;
		else if (a_regist.age_group.find(AgeGroup::A56_65) != std::string::npos)
			m_age_statistics[10]++;
		else if (a_regist.age_group.find(AgeGroup::A40_55) != std::string::npos)
			m_age_statistics[9]++;
		else if (a_regist.age_group.find(AgeGroup::A26_39) != std::string::npos)
			m_age_statistics[8]++;
		else if (a_regist.age_group.find(AgeGroup::A18_25) != std::string::npos)
			m_age_statistics[7]++;
		else if (a_regist.age_group.find(AgeGroup::A15_17) != std::string::npos)
			m_age_statistics[6]++;
		else if (a_regist.age_group.find(AgeGroup::A12_14) != std::string::npos)
			m_age_statistics[5]++;
		else if (a_regist.age_group.find(AgeGroup::A6_11) != std::string::npos)
			m_age_statistics[4]++;
		else if (a_regist.age_group.find(AgeGroup::A4_5) != std::string::npos)
			m_age_statistics[3]++;
		else if (a_regist.age_group.find(AgeGroup::A3) != std::string::npos)
			m_age_statistics[2]++;
		else if (a_regist.age_group.find(AgeGroup::A2) != std::string::npos)
			m_age_statistics[1]++;
		else if (a_regist.age_group.find(AgeGroup::A1) != std::string::npos)
			m_age_statistics[0]++;
		else
			printf("Invalid age\n");
	}

	return 0;
}

QflReg::QFL_Church *QflReg::getRCCCLilst()
{
	int32_t i;
	QFL_Church *church_attendees = NULL;
	if (m_attendee_list_byChurch.size() < 1)
		return NULL;

	std::string church_name = "Rutgers Community Christian Church";

	for (i = 0; i < m_attendee_list_byChurch.size(); i++)
	{
		std::string name = m_attendee_list_byChurch[i].first;
		if (name.compare(church_name) == 0) {
			church_attendees = &(m_attendee_list_byChurch[i].second);
			break;
		}
	}
	return church_attendees;
}

int32_t QflReg::exclude_rccc_List()
{

	return 0;
}

int32_t QflReg::gen_rccc_functional_groups()
{
	int32_t i, j;
	QFL_Church *rccc = getRCCCLilst();
	if (!rccc)
		return -1;
	bool xiang_yin_flag = false;
	bool she_qing_flag = false;
	bool student_fellowship_flag = false;
	bool senior_flag = false;

	// xiang yin
	QflReg::CellGroup xy_cell_group;
	xy_cell_group.church = rccc->church_name;
	xy_cell_group.functions = "Xiang Yin";
	xy_cell_group.cell_group_id = m_cellid++;

	// She Qing
	QflReg::CellGroup sq_cell_group;
	sq_cell_group.church = rccc->church_name;
	sq_cell_group.functions = "She Qing";
	sq_cell_group.cell_group_id = m_cellid++;

	// student fellowship
	QflReg::CellGroup sf_cell_group;
	sf_cell_group.church = rccc->church_name;
	sf_cell_group.functions = "Student Fellowship";
	sf_cell_group.cell_group_id = m_cellid++;

	// senior group
	QflReg::CellGroup sg_cell_group;
	sg_cell_group.church = rccc->church_name;
	sg_cell_group.functions = "Evergreen";
	sg_cell_group.cell_group_id = m_cellid++;

	m_temp_group.church = rccc->church_name;
	m_temp_group.functions = "Other";

	std::vector<int32_t> q1 = rccc->persons[qEU];
	std::vector<int32_t> q2 = rccc->persons[qSenior];
	q1.insert(q1.end(), q2.begin(), q2.end());

	for (j = 0; j < q1.size(); j++) {
		Registrant person = m_person_info[q1[j]];
		std::string fg = person.functional_group;
		std::string age = person.age_group;
		bool little_childcare_coworker_flag = false;

		// xiang yin
		xiang_yin_flag = false;
		if (fg.find(std::string("æ„›ä¹‹å…‰")) != std::string::npos || fg.find("ä¹¡éŸ³") != std::string::npos ||
			fg.find("é„‰éŸ³") != std::string::npos || fg.find("Xiang Yin") != std::string::npos)
		{
			if (person.is_christian)
				xy_cell_group.christian_list.push_back(person.person_id);
			else
				xy_cell_group.non_christian_list.push_back(person.person_id);
			xiang_yin_flag = true;
		}

		// childcare coworkers
		if (fg.compare("Childcare Coworker") == 0)
			little_childcare_coworker_flag = true;

		bool age_exclude = (age.compare(AgeGroup::A1) == 0 || age.compare(AgeGroup::A2) == 0 || age.compare(AgeGroup::A3) == 0
			|| age.compare(AgeGroup::A4_5) == 0 || age.compare(AgeGroup::A6_11) == 0 || age.compare(AgeGroup::A12_14) == 0
			|| age.compare(AgeGroup::A15_17) == 0);

		if (age_exclude || little_childcare_coworker_flag)
			continue;

		// she qing
		she_qing_flag = false;
		if (fg.find("She Qing") != std::string::npos || fg.find("ç¤¾é’") != std::string::npos)
		{
			if (person.is_christian)
				sq_cell_group.christian_list.push_back(person.person_id);
			else
				sq_cell_group.non_christian_list.push_back(person.person_id);
			she_qing_flag = true;
		}

		// student fellowship
		student_fellowship_flag = false;
		if (fg.find(std::string("­å­¦ç”Ÿåœ˜å¥‘")) != std::string::npos || fg.find(std::string("å­¸ç”Ÿå›¢å¥‘")) != std::string::npos ||
			fg.find(std::string("å­¦ç”Ÿå›¢å¥‘")) != std::string::npos || fg.find(std::string("RBSG")) != std::string::npos)
		{
			if (person.is_christian)
				sf_cell_group.christian_list.push_back(person.person_id);
			else
				sf_cell_group.non_christian_list.push_back(person.person_id);
			student_fellowship_flag = true;
		}

		// Evergreen - senior group
		senior_flag = false;
		if ((age.compare(AgeGroup::A66_69) == 0 || age.compare(AgeGroup::A70) == 0) && !xiang_yin_flag) {
			if (person.is_christian)
				sg_cell_group.christian_list.push_back(person.person_id);
			else
				sg_cell_group.non_christian_list.push_back(person.person_id);
			senior_flag = true;
		}

		if (!(senior_flag || student_fellowship_flag || she_qing_flag || xiang_yin_flag)) {
			if (person.is_christian)
				m_temp_group.christian_list.push_back(person.person_id);
			else
				m_temp_group.non_christian_list.push_back(person.person_id);
		}
	}

	std::vector<CellGroup> cell_groups;
	cell_groups.push_back(xy_cell_group);
	cell_groups.push_back(sq_cell_group);
	cell_groups.push_back(sf_cell_group);
	cell_groups.push_back(sg_cell_group);
	cell_groups.push_back(m_temp_group);
	m_cell_groups[rccc->church_name] = cell_groups;

	return 0;
}

int32_t QflReg::gen_rccc_zip_groups()
{
	int32_t i, j;
	int32_t total = m_temp_group.christian_list.size() + m_temp_group.non_christian_list.size();
	if (total < 1)
		return -1;

	QFL_Church *rccc = getRCCCLilst();
	if (!rccc)
		return -2;

	std::vector<int32_t> q1 = rccc->persons[qEU];
	std::vector<int32_t> q2 = rccc->persons[qSenior];
	q1.insert(q1.end(), q2.begin(), q2.end());

	std::vector<int32_t> zip_list;
	std::vector<int32_t> temp_list;
	temp_list = m_temp_group.christian_list;
	temp_list.insert(temp_list.end(), m_temp_group.non_christian_list.begin(), m_temp_group.non_christian_list.end());

	printf("total = %d\n", total);
	for (i = 0; i < total; i++) {
		bool matched = false;
		Registrant person = m_person_info[temp_list[i]];
		std::string fg = person.functional_group;
		int32_t zip = person.zip;
		for (j = 0; j < zip_list.size(); j++) {
			if (zip == zip_list[j]) {
				matched = true;
				break;
			}
		}
		if (!matched)
			zip_list.push_back(zip);
	}

	for (j = 0; j < zip_list.size(); j++) {
		int32_t cnt = 0;
		for (i = 0; i < total; i++) {
			Registrant person = m_person_info[temp_list[i]];
			int32_t zip = person.zip;
			if (zip == zip_list[j]) {
				//				printf("zip=%d, id=%d, city=%s, state=%s, function=%s, contact=%s\n", zip, person.person_id, person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.contact_person.c_str());
				cnt++;
			}

		}

		printf("zip=%d, number = %d\n", zip_list[j], cnt);
	}

	return 0;
}

void QflReg::printOutStatistics(const char*filename)
{
	int32_t i;
	if (filename == NULL)
		return;
	FILE *hf = fopen(filename, "w+");
	if (hf == NULL)
		return;

	// print number of registrants
	fprintf(hf, "Time = %s\n", getCurTime().c_str());
	fprintf(hf, "Registrants: %d, Cancelled: %d\n", m_registrants.size(), m_cancelled);
	fprintf(hf, "Christians = %d, non-christian = %d\n", m_christian_list.size(), m_non_christian_list.size());
	fprintf(hf, "Stay overnight = %d, Commute = %d\n", m_registrants.size() - m_commute_list.size(), m_commute_list.size());
	fprintf(hf, "EU: Adult Christians = %d, Adult Non - Christians = %d, ratio = %f \n", m_allchurch.adult_christians, m_allchurch.adult_non_christians,
		(float)m_allchurch.adult_christians / (float)m_allchurch.adult_non_christians);

	// print family, male, female
	fprintf(hf, "family: %d, male: %d, female: %d\n", m_family_list.size(), m_male_list.size(), m_female_list.size());

	// print youth with parents
	fprintf(hf, "Youth with Parents: %d\n", m_youth_stays_with_parent_list.size());
	// print youth in camp
	fprintf(hf, "Youth in Camp: %d\n", m_youth_camp_list.size());
	// print youth leaders
	fprintf(hf, "Youth leader: %d\n", m_youth_leader_list.size());
	// print Child care
	fprintf(hf, "Child care: %d\n", m_child_leader_list.size());

	// print age distribution
	fprintf(hf, "_%s, %d\n", AgeGroup::A1, m_age_statistics[0]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A2, m_age_statistics[1]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A3, m_age_statistics[2]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A4_5, m_age_statistics[3]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A6_11, m_age_statistics[4]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A12_14, m_age_statistics[5]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A15_17, m_age_statistics[6]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A18_25, m_age_statistics[7]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A26_39, m_age_statistics[8]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A40_55, m_age_statistics[9]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A56_65, m_age_statistics[10]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A66_69, m_age_statistics[11]);
	fprintf(hf, "_%s, %d\n", AgeGroup::A70, m_age_statistics[12]);

	fprintf(hf, "Church, Adult Christians, Youth Christians\n");
	fprintf(hf, "RCCC, %d, %d\n", m_rccc.adult_christians, m_rccc.youth_christians);
	fprintf(hf, "Others, %d, %d\n", m_allchurch.adult_christians - m_rccc.adult_christians, m_allchurch.youth_christians - m_rccc.youth_christians);

	fprintf(hf, "Church, Adult Non-Christians, Youth Non-Christians, Children\n");
	fprintf(hf, "RCCC, %d, %d, %d\n", m_rccc.adult_non_christians, m_rccc.youth_non_christians, m_rccc.children);
	fprintf(hf, "Others, %d, %d, %d\n", m_allchurch.adult_non_christians - m_rccc.adult_non_christians, m_allchurch.youth_non_christians - m_rccc.youth_non_christians, m_allchurch.children);

	// print top 15 church list
	for (i = 0; i < m_attendee_list_byChurch.size(); i++) {
		int32_t total = m_attendee_list_byChurch[i].second.persons[qEU].size() + m_attendee_list_byChurch[i].second.persons[qCabrini].size()
			+ m_attendee_list_byChurch[i].second.persons[qChild].size() + m_attendee_list_byChurch[i].second.persons[qSenior].size();
		if (total > 0)
			fprintf(hf, "%s, %d\n", m_attendee_list_byChurch[i].first.c_str(), total);
	}
	fclose(hf);

	return;
}

void QflReg::printOutRCCC_statistics(const char*filename)
{
	int32_t rccc_christians = 0;
	int32_t rccc_non_christians = 0;

	std::vector<CellGroup> cell_groups = m_cell_groups["Rutgers Community Christian Church"];
	if (cell_groups.size() < 1)
		return;

	int32_t total = m_attendee_list_byChurch[0].second.persons[qEU].size() +
		m_attendee_list_byChurch[0].second.persons[qChild].size() +
		m_attendee_list_byChurch[0].second.persons[qSenior].size() +
		m_attendee_list_byChurch[0].second.persons[qCabrini].size();

	if (filename == NULL)
		return;
	FILE *hf = fopen(filename, "w+");
	if (hf == NULL)
		return;

	fprintf(hf, "Time = %s\n", getCurTime().c_str());
	fprintf(hf, "Church = %s, total attendees = %d\n", cell_groups[0].church.c_str(), total);

	fprintf(hf, "EU Adults: %d\n", m_attendee_list_byChurch[0].second.persons[qEU].size());
	fprintf(hf, "EU 0-11 yr Children: %d\n", m_attendee_list_byChurch[0].second.persons[qChild].size());
	fprintf(hf, "EU Seniors: %d\n", m_attendee_list_byChurch[0].second.persons[qSenior].size());
	fprintf(hf, "Cabrini attendees: %d\n", m_attendee_list_byChurch[0].second.persons[qCabrini].size());


	fprintf(hf, "\nEU Cell Group List\n");
	for (int32_t i = 0; i < cell_groups.size(); i++) {
		std::string church = cell_groups[i].church;
		std::string function = cell_groups[i].functions;
		int32_t christians = cell_groups[i].christian_list.size();
		int32_t non_christians = cell_groups[i].non_christian_list.size();
		rccc_christians += christians;
		rccc_non_christians += non_christians;

		fprintf(hf, "%s, %d Christians, %d Non-Chrstians\n", function.c_str(), christians, non_christians);
	}

	fprintf(hf, "Overall:, RCCC Christians = %d, Non-christians = %d, ratio = %f\n", rccc_christians, rccc_non_christians, (float)rccc_christians / (float)rccc_non_christians);
	fclose(hf);

	return;
}

void QflReg::printOutForChildWorkers_2_5yr(const char*filename)
{
	int32_t i, j;
	if (filename == NULL)
		return;
	FILE *hf = fopen(filename, "w+");
	if (hf == NULL)
		return;
	fprintf(hf, "Time = %s\n", getCurTime().c_str());
	fprintf(hf, "QFL 2017 2-5 year old Child Counselors\n");
	fprintf(hf, "Person ID, Party ID, Party Type, Chinese Name, First Name, Last Name, Gender, Age, Contact, ");
	fprintf(hf, "Service, Church, City, State, Function Group, Email, Phone\n");

	for (j = 0; j < m_registrants.size(); j++) {
		Registrant person = m_registrants[j];
		for (i = 0; i < m_child_leader_list.size(); i++) {
			int32_t p = m_child_leader_list[i];

			if (p == m_registrants[j].person_id) {
				bool exclude_child = (person.age_group.compare(AgeGroup::A2) == 0) || (person.age_group.compare(AgeGroup::A3) == 0)
					|| (person.age_group.compare(AgeGroup::A4_5) == 0);

				if ((!exclude_child) && person.services.find("2-5Yr") != std::string::npos) {
					fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %s, ", person.person_id, person.party, person.party_type.c_str(), person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
					fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str(), person.mobile_phone.c_str());
				}
				break;
			}
		}
	}

	fprintf(hf, "\nQFL 2017 2-5 year old Children\n");
	for (j = 0; j < m_registrants.size(); j++) {
		Registrant person = m_registrants[j];
		std::string age = person.age_group;
		std::string grade = person.grade;

		bool child_2_5 = (person.age_group.compare(AgeGroup::A2) == 0) || (person.age_group.compare(AgeGroup::A3) == 0)
			|| (person.age_group.compare(AgeGroup::A4_5) == 0);

		if (child_2_5) {
			fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %s, ", person.person_id, person.party, person.party_type.c_str(), person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
			fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str(), person.mobile_phone.c_str());
		}
	}

	fclose(hf);
	return;
}

void QflReg::printOutForChildWorkers_6_11yr(const char*filename)
{
	int32_t i, j;
	if (filename == NULL)
		return;
	FILE *hf = fopen(filename, "w+");
	if (hf == NULL)
		return;
	fprintf(hf, "Time = %s\n", getCurTime().c_str());
	fprintf(hf, "QFL 2017 6-11 year old Child Counselors\n");
	fprintf(hf, "Person ID, Party ID, Party Type, Chinese Name, First Name, Last Name, Gender, Age, Contact, ");
	fprintf(hf, "Service, Church, City, State, Function Group, Email, Phone, room\n");

	for (j = 0; j < m_registrants.size(); j++) {
		Registrant person = m_registrants[j];
		for (i = 0; i < m_child_leader_list.size(); i++) {
			int32_t p = m_child_leader_list[i];

			if (p == m_registrants[j].person_id) {
				if (person.services.find("6-11Yr") != std::string::npos &&person.age_group.compare(AgeGroup::A6_11) != 0) {
					fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %s, ", person.person_id, person.party, person.party_type.c_str(), person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
					fprintf(hf, "%s, %s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str(), person.mobile_phone.c_str(), person.room.c_str());
				}
				break;
			}
		}
	}

	fprintf(hf, "\nQFL 2017 6-11 year old Children\n");
	for (j = 0; j < m_registrants.size(); j++) {
		Registrant person = m_registrants[j];
		std::string age = person.age_group;
		std::string grade = person.grade;

		bool exclude_youth = (person.grade.find("Stay with Youth") != std::string::npos
			|| person.grade.find("stay with Youth") != std::string::npos);
		if (exclude_youth)
			continue;

		if (age.compare(AgeGroup::A6_11) == 0) {
			fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %s, ", person.person_id, person.party, person.party_type.c_str(), person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
			fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str(), person.mobile_phone.c_str());
		}
	}

	fclose(hf);
	return;
}


void QflReg::printOutForLogistics(const char*filename)
{
	int32_t i, j;
	if (filename == NULL)
		return;
	FILE *hf = fopen(filename, "w+");
	if (hf == NULL)
		return;

	for (i = 0; i < m_logistics_list.size(); i++) {
		int32_t p = m_logistics_list[i];

		for (j = 0; j < m_registrants.size(); j++) {
			if (p == m_registrants[j].person_id) {
				Registrant person = m_registrants[j];
				fprintf(hf, "%d, %s, %s, %s, %s, %s, ", person.person_id, person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str());
			}
		}
	}
	fclose(hf);
	return;
}

void QflReg::printOutForYouth(const char*filename)
{
	int32_t i, j;
	if (filename == NULL)
		return;
	FILE *hf = fopen(filename, "w+");
	if (hf == NULL)
		return;

	int32_t youth_christians = 0;
	int32_t youth_non_christians = 0;
	int32_t rccc_youth_christians = 0;
	int32_t rccc_youth_non_christians = 0;

	fprintf(hf, "Youth Campers, total = %d\n", m_youth_camp_list.size());
	for (i = 0; i < m_youth_camp_list.size(); i++) {
		int32_t p = m_youth_camp_list[i];

		for (j = 0; j < m_registrants.size(); j++) {
			if (p == m_registrants[j].person_id) {
				Registrant person = m_registrants[j];
				bool is_christian = person.is_christian;
				bool is_rccc = (person.church.compare("Rutgers Community Christian Church") == 0);
				if (is_christian)
					youth_christians++;
				else
					youth_non_christians++;

				if (is_rccc) {
					if (is_christian)
						rccc_youth_christians++;
					else if (!is_christian)
						rccc_youth_non_christians++;
				}

				fprintf(hf, "%d, %s, %s, %s, %s, %s, ", person.person_id, person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.cell_group.c_str(), person.email.c_str());
			}
		}
	}

	fprintf(hf, "Youth Camper Leaders, total = %d\n", m_youth_leader_list.size());
	for (i = 0; i < m_youth_leader_list.size(); i++) {
		int32_t p = m_youth_leader_list[i];

		for (j = 0; j < m_registrants.size(); j++) {
			if (p == m_registrants[j].person_id) {
				Registrant person = m_registrants[j];

				bool is_christian = person.is_christian;
				bool is_rccc = (person.church.compare("Rutgers Community Christian Church") == 0);
				if (is_christian)
					youth_christians++;
				else
					youth_non_christians++;

				if (is_rccc) {
					if (is_christian)
						rccc_youth_christians++;
					else if (!is_christian)
						rccc_youth_non_christians++;
				}

				fprintf(hf, "%d, %s, %s, %s, %s, %s, ", person.person_id, person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.cell_group.c_str(), person.email.c_str());
			}
		}
	}

	fprintf(hf, "Youth Stay with Parents, total = %d\n", m_youth_stays_with_parent_list.size());
	for (i = 0; i < m_youth_stays_with_parent_list.size(); i++) {
		int32_t p = m_youth_stays_with_parent_list[i];

		for (j = 0; j < m_registrants.size(); j++) {
			if (p == m_registrants[j].person_id) {
				Registrant person = m_registrants[j];
				bool is_christian = person.is_christian;
				bool is_rccc = (person.church.compare("Rutgers Community Christian Church") == 0);
				if (is_christian)
					youth_christians++;
				else
					youth_non_christians++;

				if (is_rccc) {
					if (is_christian)
						rccc_youth_christians++;
					else if (!is_christian)
						rccc_youth_non_christians++;
				}
				fprintf(hf, "%d, %s, %s, %s, %s, %s, ", person.person_id, person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.cell_group.c_str(), person.email.c_str());
			}
		}
	}

	fclose(hf);

	printf("Youth camp, Young Christians = %d, Young Non_Christians = %d\n", youth_christians, youth_non_christians);
	printf("RCCC only, Young Christians = %d, Young Non_Christians = %d\n", rccc_youth_christians, rccc_youth_non_christians);

	return;
}
void QflReg::printOutEUListAllChurch_separated(const char*filedir)
{
	int32_t yr = m_year;
	int32_t i, j, k, n;
	int32_t total, christians, nchristians;
	int32_t child_2, child2_5, child6_11;
	int32_t youth_camp;
	int32_t adults;
	int32_t seniors;
	int32_t stay, commute;

	if (filedir == NULL)
		return;

	if (m_attendee_list_byChurch.size() < 1)
		return;

	for (i = 0; i < m_attendee_list_byChurch.size(); i++) {
		total = 0;
		christians = 0;
		nchristians = 0;
		child_2 = 0;
		child2_5 = 0;
		child6_11 = 0;
		youth_camp = 0;
		adults = 0;
		seniors = 0;
		stay = 0;
		commute = 0;

		std::pair<std::string, QFL_Church> ch = m_attendee_list_byChurch[i];
		std::string name = ch.first;
		std::vector<int32_t > pid_list[4];

		pid_list[0] = ch.second.persons[qEU];
		pid_list[1] = ch.second.persons[qSenior];

		if (pid_list[0].size() + pid_list[1].size() == 0)
			continue;

		std::string filename = std::string(filedir) + "/" + name + "_" + getCurTime() + ".csv";
		FILE *hf = fopen(filename.c_str(), "w+");
		if (hf == NULL)
			return;

		fprintf(hf, "QFL: %d -- EU Campus\n", yr);
		fprintf(hf, "Church name: %s\n", name.c_str());
		fprintf(hf, "Person ID, Party ID, party Type, Chinese Name, First Name, Last Name, Gender, Age, Christian, Stay overnight, ");
		fprintf(hf, "Need Ride, Offer Ride, Contact Person, City, State, Function Group, Phone, Email,Cell Group#, Cell Group Leaders\n");

		for (n = 0; n < 2; n++) {
			if (n == 0)
				fprintf(hf, "\nEU Adult Attendees\n");
			else if (n == 1)
				fprintf(hf, "\nEU Senior Attendees\n");

			for (k = 0; k < pid_list[n].size(); k++) {
				int32_t id = pid_list[n][k];
				for (j = 0; j < m_registrants.size(); j++) {
					Registrant p = m_registrants[j];
					int32_t pid = p.person_id;
					if (id == pid) {
						total++;

						if (p.need_room)
							stay++;
						else
							commute++;

						if (p.is_christian)
							christians++;
						else
							nchristians++;

						if (p.age_group.compare(AgeGroup::A66_69) == 0 || p.age_group.compare(AgeGroup::A70) == 0)
							seniors++;
						else
							adults++;

						fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %d, %d, ", p.person_id, p.party, p.party_type.c_str(), p.chinese_name.c_str(), p.first_name.c_str(), p.last_name.c_str(), p.gender.c_str(), p.age_group.c_str(), p.is_christian, p.need_room);
						fprintf(hf, "%s, %s, %s, %s, %s, %s, %s, %s\n", p.need_ride.substr(0, 1).c_str(), p.offer_ride.substr(0, 1).c_str(), p.contact_person.c_str(), p.city.c_str(), p.state.c_str(), p.functional_group.c_str(), p.mobile_phone.c_str(), p.email.c_str());
						break;
					}
				}
			}
		}
		fprintf(hf, "\n\nTotal EU Adult registrants = %d, Christians = %d, non-Christians = %d\n", total, christians, nchristians);
		fprintf(hf, "Adult EU campers: %d\n", adults);
		fprintf(hf, "Seniors: %d\n", seniors);
		fclose(hf);
		hf = NULL;
	}

	return;
}

void QflReg::printOutEUListAllChurchInOne(const char*filedir)
{
	int32_t yr = m_year;
	int32_t i, j, k, n;
	int32_t total, christians, nchristians;
	int32_t child_2, child2_5, child6_11;
	int32_t youth_camp;
	int32_t adults;
	int32_t seniors;
	int32_t stay, commute;
	std::string name = "QFL_AllChurch_list_withkids";

	if (filedir == NULL)
		return;

	if (m_attendee_list_byChurch.size() < 1)
		return;

	std::string filename = std::string(filedir) + "/" + name + "_" + getCurTime() + ".csv";
	printf("---%s\n", filename.c_str());
	FILE *hf = fopen(filename.c_str(), "w+");
	if (hf == NULL)
		return;

	fprintf(hf, "QFL: %d -- EU Campus\n", yr);
	fprintf(hf, "Person ID, Party ID, Church, Chinese Name, First Name, Last Name, Gender, Age, Christian,  ");
	fprintf(hf, "Contact Person, Function Group, Phone, Email, Cell Group#, Cell Group Leaders\n");

	for (i = 0; i < m_attendee_list_byChurch.size(); i++) {
		total = 0;
		christians = 0;
		nchristians = 0;
		child_2 = 0;
		child2_5 = 0;
		child6_11 = 0;
		youth_camp = 0;
		adults = 0;
		seniors = 0;
		stay = 0;
		commute = 0;

		std::pair<std::string, QFL_Church> ch = m_attendee_list_byChurch[i];
		std::string name = ch.first;
		std::vector<int32_t > pid_list[4];

		pid_list[0] = ch.second.persons[qEU];
		pid_list[1] = ch.second.persons[qSenior];
		pid_list[2] = ch.second.persons[qChild];

		if (pid_list[0].size() + pid_list[1].size() == 0)
			continue;

		for (n = 0; n < 3; n++) {
			for (k = 0; k < pid_list[n].size(); k++) {
				int32_t id = pid_list[n][k];
				for (j = 0; j < m_registrants.size(); j++) {
					Registrant p = m_registrants[j];
					int32_t pid = p.person_id;
					if (id == pid) {
						total++;

						if (p.need_room)
							stay++;
						else
							commute++;

						if (p.is_christian)
							christians++;
						else
							nchristians++;

						if (p.age_group.compare(AgeGroup::A66_69) == 0 || p.age_group.compare(AgeGroup::A70) == 0)
							seniors++;
						else
							adults++;

						fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %d, ", p.person_id, p.party, p.church.c_str(), p.chinese_name.c_str(), p.first_name.c_str(), p.last_name.c_str(), p.gender.c_str(), p.age_group.c_str(), p.is_christian);
						fprintf(hf, "%s, %s, %s, %s\n", p.contact_person.c_str(), p.functional_group.c_str(), p.mobile_phone.c_str(), p.email.c_str());
						break;
					}
				}
			}
		}
	}

	fclose(hf);
	hf = NULL;
}

void QflReg::printRCCCFunctions(const char *dirname)
{
	int32_t i, j;
	if (dirname == NULL)
		return;

	const std::string RCCC = "Rutgers Community Christian Church";

	// xiang yin
	std::string xiangyin_name = std::string(dirname) + "/" + "xiangyin.csv";
	FILE *hf = fopen(xiangyin_name.c_str(), "w+");
	if (hf == NULL)
		return;

	fprintf(hf, "Person ID, Chinese Name, First Name, Last Name, Gender, Age, Contact, ");
	fprintf(hf, "Service, City, State, Function Group, Email\n");

	// XiangYin
	for (j = 0; j < m_registrants.size(); j++) {
		if (m_registrants[j].church.compare(RCCC) == 0) {
			Registrant person = m_registrants[j];
			std::string fg = person.functional_group;
			bool exclude_youth = (person.grade.find("Stay with Youth") != std::string::npos
				|| person.grade.find("stay with Youth") != std::string::npos
				|| person.services.find("Service Youth") != std::string::npos
				|| person.cell_group.find("Youth SGLeaders") != std::string::npos
				|| person.grade.find("Stay with Parent") != std::string::npos);

			if (exclude_youth)
				continue;

			if (fg.find(std::string("æ„›ä¹‹å…‰")) != std::string::npos || fg.find("ä¹¡éŸ³") != std::string::npos ||
				fg.find("é„‰éŸ³") != std::string::npos || fg.find("Xiang Yin") != std::string::npos)
			{
				fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", person.person_id, person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s\n", person.services.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str());
				m_rccc_xiangyin_list.push_back(person.person_id);
			}
		}
	}
	fclose(hf);

	// sheqing
	std::string sheqing_name = std::string(dirname) + "/" + "sheqing.csv";
	hf = fopen(sheqing_name.c_str(), "w+");
	if (hf == NULL)
		return;

	fprintf(hf, "Person ID, Chinese Name, First Name, Last Name, Gender, Age, Contact, ");
	fprintf(hf, "Service, City, State, Function Group, Email\n");

	for (j = 0; j < m_registrants.size(); j++) {
		if (m_registrants[j].church.compare(RCCC) == 0) {
			Registrant person = m_registrants[j];
			std::string fg = person.functional_group;
			std::string age = person.age_group;
			bool exclude_youth = (person.grade.find("Stay with Youth") != std::string::npos
				|| person.grade.find("stay with Youth") != std::string::npos
				|| person.services.find("Service Youth") != std::string::npos
				|| person.cell_group.find("Youth SGLeaders") != std::string::npos
				|| person.grade.find("Stay with Parent") != std::string::npos);

			if (exclude_youth)
				continue;

			bool age_exclude = (age.compare(AgeGroup::A1) == 0 || age.compare(AgeGroup::A2) == 0 || age.compare(AgeGroup::A3) == 0
				|| age.compare(AgeGroup::A4_5) == 0 || age.compare(AgeGroup::A6_11) == 0 || age.compare(AgeGroup::A12_14) == 0
				|| age.compare(AgeGroup::A15_17) == 0);

			if (age_exclude)
				continue;

			if (fg.find("She Qing") != std::string::npos || fg.find("ç¤¾é’") != std::string::npos)
			{
				fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", person.person_id, person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s\n", person.services.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str());
				m_rccc_sheqin_list.push_back(person.person_id);
			}
		}
	}
	fclose(hf);

	// choir
	std::string choir_name = std::string(dirname) + "/" + "choir.csv";
	hf = fopen(choir_name.c_str(), "w+");
	if (hf == NULL)
		return;
	fprintf(hf, "Person ID, Chinese Name, First Name, Last Name, Gender, Age, Contact, ");
	fprintf(hf, "Service, City, State, Function Group, Email\n");

	for (j = 0; j < m_registrants.size(); j++) {
		if (m_registrants[j].church.compare(RCCC) == 0) {
			Registrant person = m_registrants[j];
			std::string chor = person.functional_group;
			std::string chor1 = person.services;
			std::string age = person.age_group;
			bool age_exclude = (age.compare(AgeGroup::A1) == 0 || age.compare(AgeGroup::A2) == 0 || age.compare(AgeGroup::A3) == 0
				|| age.compare(AgeGroup::A4_5) == 0 || age.compare(AgeGroup::A6_11) == 0 || age.compare(AgeGroup::A12_14) == 0
				|| age.compare(AgeGroup::A15_17) == 0);

			if (age_exclude)
				continue;

			if (chor.find(std::string("è¯—ç­")) != std::string::npos || chor.find("è©©ç") != std::string::npos ||
				chor.find(std::string("Choir")) != std::string::npos || chor1.compare("QFL Choir") == 0)
			{
				fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", person.person_id, person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s\n", person.services.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str());
			}
		}
	}
	fclose(hf);

	// student fellowship
	std::string sfellowship_name = std::string(dirname) + "/" + "student_fellowship.csv";
	hf = fopen(sfellowship_name.c_str(), "w+");
	if (hf == NULL)
		return;
	fprintf(hf, "Person ID, Chinese Name, First Name, Last Name, Gender, Age, Contact, ");
	fprintf(hf, "Service, City, State, Function Group, Email\n");

	for (j = 0; j < m_registrants.size(); j++) {
		if (m_registrants[j].church.compare(RCCC) == 0) {
			Registrant person = m_registrants[j];
			std::string sf = person.functional_group;
			std::string age = person.age_group;

			bool exclude_youth = (person.grade.find("Stay with Youth") != std::string::npos
				|| person.grade.find("stay with Youth") != std::string::npos
				|| person.services.find("Service Youth") != std::string::npos
				|| person.cell_group.find("Youth SGLeaders") != std::string::npos
				|| person.grade.find("Stay with Parent") != std::string::npos);

			if (exclude_youth)
				continue;

			bool age_exclude = (age.compare(AgeGroup::A1) == 0 || age.compare(AgeGroup::A2) == 0 || age.compare(AgeGroup::A3) == 0
				|| age.compare(AgeGroup::A4_5) == 0 || age.compare(AgeGroup::A6_11) == 0 || age.compare(AgeGroup::A12_14) == 0
				|| age.compare(AgeGroup::A15_17) == 0);

			if (age_exclude)
				continue;

			if (sf.find(std::string("­å­¦ç”Ÿåœ˜å¥‘")) != std::string::npos || sf.find(std::string("å­¸ç”Ÿå›¢å¥‘")) != std::string::npos ||
				sf.find(std::string("å­¦ç”Ÿå›¢å¥‘")) != std::string::npos || sf.find(std::string("RBSG")) != std::string::npos)
			{
				fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", person.person_id, person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s\n", person.services.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str());
			}
		}
	}
	fclose(hf);

	return;
}


void QflReg::printDoubtfulRegistrants(const char*filename)
{
	int32_t i, j;
	if (filename == NULL)
		return;
	FILE *hf = fopen(filename, "w+");
	if (hf == NULL)
		return;

	for (j = 0; j < m_registrants.size(); j++) {
		Registrant p = m_registrants[j];
		std::string first = p.first_name;
		std::string last = p.last_name;
		std::string age = p.age_group;
		std::string grade = p.grade;
		std::string occupation = p.occupation;

		for (i = j + 1; i < m_registrants.size(); i++) {
			Registrant person = m_registrants[i];
			if (person.first_name.compare(first) == 0 && person.last_name.compare(last) == 0) {
				fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", p.person_id, p.party_type.c_str(), p.chinese_name.c_str(), p.first_name.c_str(), p.last_name.c_str(), p.gender.c_str(), p.age_group.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", p.services.c_str(), p.contact_person.c_str(), p.church.c_str(), p.city.c_str(), p.state.c_str(), p.functional_group.c_str(), p.email.c_str());
				fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", person.person_id, person.party_type.c_str(), person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.contact_person.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str());
			}
		}
		if (occupation.compare("Minister") == 0) {
			fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", p.person_id, p.party_type.c_str(), p.chinese_name.c_str(), p.first_name.c_str(), p.last_name.c_str(), p.gender.c_str(), p.age_group.c_str());
			fprintf(hf, "%s, %s, %s, %s, %s, %s, %s, %s\n", p.services.c_str(), p.contact_person.c_str(), p.church.c_str(), p.city.c_str(), p.state.c_str(), p.functional_group.c_str(), p.email.c_str(), p.occupation.c_str());
		}
	}

	fclose(hf);
	return;
}

void QflReg::printRidesList(const char*filename)
{
	int32_t i, j;
	std::string need;
	std::string offer;
	int32_t need_num, offer_num;

	for (j = 0; j < m_registrants.size(); j++) {
		Registrant p = m_registrants[j];
		int32_t id = p.party;
		std::string need_ride = p.need_ride;
		std::string offer_ride = p.offer_ride;
		need_num = 0;
		offer_num = 0;

		if (!need_ride.empty()) {
			need = need_ride.substr(0, 1);
			need_num = std::atoi(need.c_str());
		}

		if (!offer_ride.empty()) {
			offer = offer_ride.substr(0, 1);
			offer_num = std::atoi(offer.c_str());
		}
		if (offer_num > 0) {
			bool matched = false;
			for (i = 0; i < m_ride_list.size(); i++) {
				if (m_ride_list[i] == id) {
					matched = true;
					break;
				}
			}
			if (!matched)
				m_ride_list.push_back(id);
		}
		else if (need_num > 0) {
			bool matched = false;
			for (i = 0; i < m_need_ride_list.size(); i++) {
				if (m_need_ride_list[i] == id) {
					matched = true;
					break;
				}
			}
			if (!matched)
				m_need_ride_list.push_back(id);
		}
	}

	if (filename == NULL)
		return;
	FILE *hf = fopen(filename, "w+");
	if (hf == NULL)
		return;

	fprintf(hf, "Party ID, party Type, Chinese Name, First Name, Last Name, Gender, Age, ");
	fprintf(hf, "Need Ride, Offer Ride, Contact Person, Church, City, State, Function Group, Email, Phone\n");

	for (i = 0; i < m_ride_list.size(); i++) {
		int32_t id = m_ride_list[i];
		for (j = 0; j < m_registrants.size(); j++) {
			Registrant p = m_registrants[j];
			int32_t pid = p.party;
			if (id == pid) {
				fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", p.party, p.party_type.c_str(), p.chinese_name.c_str(), p.first_name.c_str(), p.last_name.c_str(), p.gender.c_str(), p.age_group.c_str());
				fprintf(hf, ", %s, %s, %s, %s, %s, %s, %s, %s\n", p.offer_ride.substr(0, 1).c_str(), p.contact_person.c_str(), p.church.c_str(), p.city.c_str(), p.state.c_str(), p.functional_group.c_str(), p.email.c_str(), p.mobile_phone.c_str());
				break;
			}
		}
	}

	for (i = 0; i < m_need_ride_list.size(); i++) {
		int32_t id = m_need_ride_list[i];
		for (j = 0; j < m_registrants.size(); j++) {
			Registrant p = m_registrants[j];
			int32_t pid = p.party;
			if (id == pid) {
				fprintf(hf, "%d, %s, %s, %s, %s, %s, %s, ", p.party, p.party_type.c_str(), p.chinese_name.c_str(), p.first_name.c_str(), p.last_name.c_str(), p.gender.c_str(), p.age_group.c_str());
				fprintf(hf, "%s, , %s, %s, %s, %s, %s, %s, %s\n", p.need_ride.substr(0, 1).c_str(), p.contact_person.c_str(), p.church.c_str(), p.city.c_str(), p.state.c_str(), p.functional_group.c_str(), p.email.c_str(), p.mobile_phone.c_str());
				break;
			}
		}
	}

	fclose(hf);
	return;
}

void QflReg::printAttendeesByAllChurch(const char* filedir)
{
	int32_t yr = m_year;
	int32_t i, j, k, n;
	int32_t total, christians, nchristians;
	int32_t child_2, child2_5, child6_11;
	int32_t youth_camp;
	int32_t adults;
	int32_t seniors;
	int32_t stay, commute;

	if (filedir == NULL)
		return;

	if (m_attendee_list_byChurch.size() < 1)
		return;

	for (i = 0; i < m_attendee_list_byChurch.size(); i++) {
		total = 0;
		christians = 0;
		nchristians = 0;
		child_2 = 0;
		child2_5 = 0;
		child6_11 = 0;
		youth_camp = 0;
		adults = 0;
		seniors = 0;
		stay = 0;
		commute = 0;

		std::pair<std::string, QFL_Church> ch = m_attendee_list_byChurch[i];
		std::string name = ch.first;
		std::vector<int32_t > pid_list[4];

		pid_list[0] = ch.second.persons[qEU];
		pid_list[1] = ch.second.persons[qSenior];
		pid_list[2] = ch.second.persons[qChild];
		pid_list[3] = ch.second.persons[qCabrini];

		if (pid_list[0].size() + pid_list[1].size() + pid_list[2].size() + pid_list[3].size() == 0)
			continue;

		std::string filename = std::string(filedir) + "/" + name + "_" + getCurTime() + ".csv";
		FILE *hf = fopen(filename.c_str(), "w+");
		if (hf == NULL)
			return;

		fprintf(hf, "QFL: %d\n", yr);
		fprintf(hf, "Church name: %s\n", name.c_str());
		fprintf(hf, "Person ID, Party ID, party Type, Chinese Name, First Name, Last Name, Gender, Age, Christian, Stay overnight, ");
		fprintf(hf, "Need Ride, Offer Ride, Contact Person, City, State, Function Group, Phone, Email,Cell Group#, Cell Group Leaders\n");

		for (n = 0; n < 4; n++) {
			if (n == 0)
				fprintf(hf, "\nEU Adult Attendees\n");
			else if (n == 1)
				fprintf(hf, "\nEU Senior Attendees\n");
			else if (n == 2)
				fprintf(hf, "\nEU Child (0-11) Attendees\n");
			else if (n == 3)
				fprintf(hf, "\nYouth Camp Attendees\n");

			for (k = 0; k < pid_list[n].size(); k++) {
				int32_t id = pid_list[n][k];
				for (j = 0; j < m_registrants.size(); j++) {
					Registrant p = m_registrants[j];
					int32_t pid = p.person_id;
					if (id == pid) {
						total++;

						if (p.need_room)
							stay++;
						else
							commute++;

						if (p.is_christian)
							christians++;
						else
							nchristians++;

						if (p.age_group.compare(AgeGroup::A1) == 0)
							child_2++;
						else if (p.age_group.compare(AgeGroup::A66_69) == 0 || p.age_group.compare(AgeGroup::A70) == 0)
							seniors++;
						else if (p.age_group.compare(AgeGroup::A2) == 0 || p.age_group.compare(AgeGroup::A3) == 0 || p.age_group.compare(AgeGroup::A4_5) == 0)
							child2_5++;
						else if (p.age_group.compare(AgeGroup::A6_11) == 0)
							child6_11++;
						else if (p.grade.find("Stay with Youth") != std::string::npos || p.grade.find("stay with Youth") != std::string::npos)
							youth_camp++;
						else if (p.services.find("Service Youth") != std::string::npos || p.cell_group.find("Youth SGLeaders") != std::string::npos)
							youth_camp++;
						else
							adults++;

						fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %d, %d, ", p.person_id, p.party, p.party_type.c_str(), p.chinese_name.c_str(), p.first_name.c_str(), p.last_name.c_str(), p.gender.c_str(), p.age_group.c_str(), p.is_christian, p.need_room);
						fprintf(hf, "%s, %s, %s, %s, %s, %s, %s, %s\n", p.need_ride.substr(0, 1).c_str(), p.offer_ride.substr(0, 1).c_str(), p.contact_person.c_str(), p.city.c_str(), p.state.c_str(), p.functional_group.c_str(), p.mobile_phone.c_str(), p.email.c_str());
						break;
					}
				}
			}
		}
		fprintf(hf, "\n\nTotal registrants = %d, Christians = %d, non-Christians = %d\n", total, christians, nchristians);
		fprintf(hf, "Stay overnight = %d, Commuters = %d\n", stay, commute);
		fprintf(hf, "Children under 2: %d\n", child_2);
		fprintf(hf, "Children 2--5 yr: %d\n", child2_5);
		fprintf(hf, "Children 6--11 yr: %d\n", child6_11);
		fprintf(hf, "Other EU campers: %d\n", adults);
		fprintf(hf, "Seniors: %d\n", seniors);
		fprintf(hf, "Youth campers: %d\n", youth_camp);
		fclose(hf);
		hf = NULL;
	}

	return;
}

void QflReg::printSpecialNeeds(const char*filedir)
{
	int32_t i, j;
	if (filedir == NULL)
		return;

	std::string filename = std::string(filedir) + "/special_needs" + "_" + getCurTime() + ".csv";

	FILE *hf = fopen(filename.c_str(), "w+");
	if (hf == NULL)
		return;

	fprintf(hf, "Person Id, Party Id, Party type, Chinese Name, First Name, Last Name, Gender, Age, Contact, ");
	fprintf(hf, "Service, Church, City, State, Function Group, Email, Special Need\n");

	// baby list
	for (i = 0; i < m_baby_list.size(); i++) {
		int32_t p = m_baby_list[i];

		for (j = 0; j < m_registrants.size(); j++) {
			if (p == m_registrants[j].person_id) {
				Registrant person = m_registrants[j];
				fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %s, ", person.person_id, person.party, person.party_type.c_str(), person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str(), (person.special_need + " " + person.notes).c_str());
			}
		}
	}

	// senior list
	for (i = 0; i < m_senior_list.size(); i++) {
		int32_t p = m_senior_list[i];

		for (j = 0; j < m_registrants.size(); j++) {
			if (p == m_registrants[j].person_id) {
				Registrant person = m_registrants[j];
				fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %s, ", person.person_id, person.party, person.party_type.c_str(), person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str(), (person.special_need + " " + person.notes).c_str());
			}
		}
	}

	// special need list
	for (i = 0; i < m_special_need_list.size(); i++) {
		int32_t p = m_special_need_list[i];

		for (j = 0; j < m_registrants.size(); j++) {
			if (p == m_registrants[j].person_id) {
				Registrant person = m_registrants[j];
				fprintf(hf, "%d, %d, %s, %s, %s, %s, %s, %s, %s, ", person.person_id, person.party, person.party_type.c_str(), person.chinese_name.c_str(), person.first_name.c_str(), person.last_name.c_str(), person.gender.c_str(), person.age_group.c_str(), person.contact_person.c_str());
				fprintf(hf, "%s, %s, %s, %s, %s, %s, %s\n", person.services.c_str(), person.church.c_str(), person.city.c_str(), person.state.c_str(), person.functional_group.c_str(), person.email.c_str(), (person.special_need + " " + person.notes).c_str());
			}
		}
	}

	fclose(hf);
	return;
}


void QflReg::printAttendeesCabrini(const char* filename)
{

}

