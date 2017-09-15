#include <algorithm>
#include <fstream>
#include <forward_list>
#include <array>
#include <vector>
#include <functional>      // For greater<int>( )
#include <iostream>
#include <assert.h>
#include "attendees.h"
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


Attendees::Attendees()
{
	m_data.clear();
	m_registrants.clear();
	m_person_info.clear();
	m_attendee_list_byChurch.clear();
	m_rccc_sheqin_list.clear();
	m_cellid = 1;
	m_cancelled = 0;
	m_uncertain = 0;
}

Attendees::~Attendees()
{}

std::string Attendees::intToString(int i)
{
	std::stringstream ss;
	std::string s;
	ss << i;
	s = ss.str();

	return s;
}

std::string Attendees::getCurTime()
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

int32_t Attendees::readChurchList(const char* churchname, int32_t year)
{
	int status;
	status = m_church_list.readInChurchList(churchname);
	m_church_list.sortbyState();
	return status;
}


int32_t Attendees::readRegistrants(const char *filename)
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
int32_t Attendees::parseAllFields()
{
	int32_t i, j;
	bool available;

	Registrant a_regist;
	if (m_data.size() == 0)
		return -1;

	std::vector<std::string> person = m_data[0];
	for (i = 1; i < m_data.size(); i++){
		available = false;
		person = m_data[i];
		// cancelled persons are removed from the list
		if (person[17].compare(Status::Cancelled) == 0) {
			printf("cancelled: %s %s, %s %s\n", person[0].c_str(), person[1].c_str(), person[2].c_str(), person[4].c_str());
			m_cancelled++;
			continue;
		}

		if (person[32].compare(Status::CheckedIn) == 0 || person[36].compare(Status::KeyReturned) == 0 || person[38].compare(Status::YouthCheckedIn) == 0)
			available = true;

		if (!available) {
			m_uncertain++;
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
		a_regist.registrtion_fee = std::stoi(person[14].substr(1, person[14].size() - 2));
		a_regist.key_deposit = std::stoi(person[15].substr(1, person[15].size() - 2));
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
		a_regist.checkin = std::stoi(person[32].substr(1, person[32].size() - 2)) > 0;
		a_regist.cancelled = false;

		if (person[34].substr(1, person[34].size() - 2).size() > 0)
			a_regist.paid = std::stoi(person[34].substr(1, person[34].size() - 2));
		else
			a_regist.paid = 0;

		a_regist.key_returned = std::stoi(person[36].substr(1, person[36].size() - 2)) > 0;
		a_regist.youth_checkins = std::stoi(person[38].substr(1, person[38].size() - 2)) > 0;

		m_registrants.push_back(a_regist);
	}

	// remove repeated entries
	std::vector<int32_t> entries;
	for (i = 0; i < m_registrants.size(); i++) {
		Registrant attdee_i = m_registrants[i];
		for (j = i+1; j < m_registrants.size(); j++) {
			Registrant attdee_j = m_registrants[j];
			if (attdee_i.first_name.compare(attdee_j.first_name) == 0 && attdee_i.last_name.compare(attdee_j.last_name) == 0
				&& attdee_i.contact_person.compare(attdee_j.contact_person) == 0 && attdee_i.church.compare(attdee_j.church) == 0) {
				printf("repeat i=%d:%d  j=%d:%d\n", i, attdee_i.person_id, j, attdee_j.person_id);
				entries.push_back(i);
			}
		}
	}

	std::vector<Registrant>::iterator it = m_registrants.begin();
	for (i = entries.size() - 1; i >= 0; i--) {
		m_registrants.erase(it + entries[i]);
	}

	for (i = 0; i < m_registrants.size(); i++) {
		m_person_info[m_registrants[i].person_id] = &(m_registrants[i]);
	}

	printf("Total attendees = %d, Cancelled = %d, Attendee not counted = %d\n", m_registrants.size(), m_cancelled, m_uncertain);

	return 0;
}


int32_t Attendees::classifications()
{
	int32_t i;
	m_family_info.clear();
	m_attendee_list_byChurch.clear();
	m_child_leader_list.clear();
	m_male_list.clear();
	m_female_list.clear();
	m_choir_list.clear();
	m_recording_list.clear();
	m_senior_list.clear();
	m_baby_list.clear();
	m_speaker_list.clear();
	m_special_need_list.clear();

	for (i = 0; i < m_registrants.size(); i++) {
		Registrant rt = m_registrants[i];
		int32_t family_id = rt.party;
		std::string gender = rt.gender;
		bool is_family = (rt.party_type.find(PartyType::qFamily) != std::string::npos);
		bool youth_camp = false;
		bool need_room = true;
		
		if (rt.grade.find("Stay with Youth") != std::string::npos || rt.grade.find("stay with Youth") != std::string::npos ||
			rt.services.find("Service Youth") != std::string::npos || rt.cell_group.find("Youth SGLeaders") != std::string::npos)
			youth_camp = true;
		if (youth_camp)
			continue;
		if (!rt.need_room)
			continue;

		if (rt.services.find("Service Child (6-11Yr)") != std::string::npos) {
			m_child_leader_list[family_id].push_back(&m_registrants[i]);
			continue;
		}

		if (rt.services.find("QFL Choir") != std::string::npos) {
			m_choir_list[family_id].push_back(&m_registrants[i]);
			continue;
		}
		if (rt.services.find("Recording") != std::string::npos) {
			m_recording_list[family_id].push_back(&m_registrants[i]);
			continue;
		}

		if (rt.special_need.find("speaker") != std::string::npos && rt.occupation.find("Minister") != std::string::npos) {
			m_speaker_list[family_id].push_back(&m_registrants[i]);
			continue;
		}

		if (rt.notes.find("轮椅通道") != std::string::npos || rt.special_need.find("轮椅通道") != std::string::npos) {
			m_special_need_list[family_id].push_back(&m_registrants[i]);
			continue;
		}

		if (rt.age_group.compare(AgeGroup::A1) == 0) {
			m_baby_list[family_id].push_back(&m_registrants[i]);
			continue;
		}
		else if (rt.age_group.compare(AgeGroup::A66_69) == 0 || rt.age_group.compare(AgeGroup::A70) == 0) {
			m_senior_list[family_id].push_back(&m_registrants[i]);
			continue;
		}

		if (is_family) {
			if (m_family_info[family_id].size() == 0) {
				std::vector<Registrant*> fm;
				fm.push_back(&m_registrants[i]);
				m_family_info[family_id] = fm;
			}
			else {
				m_family_info[family_id].push_back(&m_registrants[i]);
			}
		}
		else {// individual
			if (gender.compare("Male") == 0) {
				m_male_list[family_id].push_back(&m_registrants[i]);

			}
			else if (gender.compare("Female") == 0) {
				m_female_list[family_id].push_back(&m_registrants[i]);
			}
		}
	}

	std::map < int32_t, std::vector<Registrant*>>::iterator it, temp;
	for (it = m_family_info.begin(); it != m_family_info.end(); ++it) {
		int32_t family_id = it->first;
		std::vector<Registrant*> lst = it->second;
		if (lst.size() == 1) {
			temp = it;
			temp--;
			m_family_info.erase(it);
			it = temp;
		}
	}
	return 0;
}

int32_t Attendees::refinement()
{
	int32_t i;
	std::map < int32_t, std::vector<Registrant*>>::iterator it_family, temp;

	// refine choir list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_choir;
	for (it_choir = m_choir_list.begin(); it_choir != m_choir_list.end(); ++it_choir) {
		int32_t choir_id = it_choir->first;
		std::vector<Registrant*> lst_choir = it_choir->second;
		for (it_family = m_family_info.begin(); it_family != m_family_info.end(); ++it_family) {
			int32_t family_id = it_family->first;
			std::vector<Registrant*> lst_family = it_family->second;
			if (choir_id == family_id) {
				for (i = 0; i < lst_family.size(); i++) {
					m_choir_list[choir_id].push_back(lst_family[i]);
					temp = it_family;
					temp--;
					m_family_info.erase(it_family);
					it_family = temp;
				}
			}
		}
	}

	// refine recording list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_recording;
	for (it_recording = m_recording_list.begin(); it_recording != m_recording_list.end(); ++it_recording) {
		int32_t recording_id = it_recording->first;
		std::vector<Registrant*> lst_recording = it_recording->second;
		for (it_family = m_family_info.begin(); it_family != m_family_info.end(); ++it_family) {
			int32_t family_id = it_family->first;
			std::vector<Registrant*> lst_family = it_family->second;
			if (recording_id == family_id) {
				for (i = 0; i < lst_family.size(); i++) {
					m_recording_list[recording_id].push_back(lst_family[i]);
					temp = it_family;
					temp--;
					m_family_info.erase(it_family);
					it_family = temp;
				}
			}
		}
	}

	// refine speaker list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_speaker;
	for (it_speaker = m_speaker_list.begin(); it_speaker != m_speaker_list.end(); ++it_speaker) {
		int32_t speaker_id = it_speaker->first;
		std::vector<Registrant*> lst_speaker = it_speaker->second;
		for (it_family = m_family_info.begin(); it_family != m_family_info.end(); ++it_family) {
			int32_t family_id = it_family->first;
			std::vector<Registrant*> lst_family = it_family->second;
			if (speaker_id == family_id) {
				for (i = 0; i < lst_family.size(); i++) {
					m_speaker_list[speaker_id].push_back(lst_family[i]);
					temp = it_family;
					temp--;
					m_family_info.erase(it_family);
					it_family = temp;
				}
			}
		}
	}

	// refine special need list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_special_need;
	for (it_special_need = m_special_need_list.begin(); it_special_need != m_special_need_list.end(); ++it_special_need) {
		int32_t special_need_id = it_special_need->first;
		std::vector<Registrant*> lst_special_need = it_special_need->second;
		for (it_family = m_family_info.begin(); it_family != m_family_info.end(); ++it_family) {
			int32_t family_id = it_family->first;
			std::vector<Registrant*> lst_family = it_family->second;
			if (special_need_id == family_id) {
				for (i = 0; i < lst_family.size(); i++) {
					m_special_need_list[special_need_id].push_back(lst_family[i]);
					temp = it_family;
					temp--;
					m_family_info.erase(it_family);
					it_family = temp;
				}
			}
		}
	}

	// refine senior list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_senior;
	for (it_senior = m_senior_list.begin(); it_senior != m_senior_list.end(); ++it_senior) {
		int32_t senior_id = it_senior->first;
		std::vector<Registrant*> lst_senior = it_senior->second;
		for (it_family = m_family_info.begin(); it_family != m_family_info.end(); ++it_family) {
			int32_t family_id = it_family->first;
			std::vector<Registrant*> lst_family = it_family->second;
			if (senior_id == family_id) {
				for (i = 0; i < lst_family.size(); i++) {
					m_senior_list[senior_id].push_back(lst_family[i]);
					temp = it_family;
					temp--;
					m_family_info.erase(it_family);
					it_family = temp;
				}
			}
		}
	}

	// refine baby list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_baby;
	for (it_baby = m_baby_list.begin(); it_baby != m_baby_list.end(); ++it_baby) {
		int32_t baby_id = it_baby->first;
		std::vector<Registrant*> lst_baby = it_baby->second;
		for (it_family = m_family_info.begin(); it_family != m_family_info.end(); ++it_family) {
			int32_t family_id = it_family->first;
			std::vector<Registrant*> lst_family = it_family->second;
			if (baby_id == family_id) {
				for (i = 0; i < lst_family.size(); i++) {
					m_baby_list[baby_id].push_back(lst_family[i]);
					temp = it_family;
					temp--;
					m_family_info.erase(it_family);
					it_family = temp;
				}
			}
		}
	}

	return 0;
}