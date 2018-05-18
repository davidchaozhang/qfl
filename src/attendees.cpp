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
	m_party_info.clear();
	m_attendee_list_byChurch.clear();
	m_rccc_sheqin_list.clear();
	m_cellid = 1;
	m_cancelled = 0;
	m_uncertain = 0;
	gradeMapping();
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

void Attendees::gradeMapping()
{
	int i;
	for (i = 0; i < sizeof(GradeGroup::G0)/sizeof(GradeGroup::G0[0]); i++) {
		m_grade_map[string(GradeGroup::G0[i])] = Campus::qEU;
	}
	for (i = 0; i < sizeof(GradeGroup::G1)/ sizeof(GradeGroup::G1[0]); i++) {
		m_grade_map[string(GradeGroup::G1[i])] = Campus::qEU;
	}
	for (i = 0; i < sizeof(GradeGroup::G2)/ sizeof(GradeGroup::G2[0]); i++) {
		m_grade_map[string(GradeGroup::G2[i])] = Campus::qCabrini;
	}
	for (i = 0; i < sizeof(GradeGroup::G2p)/ sizeof(GradeGroup::G2p[0]); i++) {
		m_grade_map[string(GradeGroup::G2p[i])] = Campus::qEU;
	}
	m_grade_map[string(GradeGroup::G3)] = Campus::qNotSure;
	m_grade_map[string(GradeGroup::G4)] = Campus::qNotSure;
	m_grade_map[string(GradeGroup::G5)] = Campus::qNotSure;
}


ChurchList::QFLChurch* Attendees::getChurch(int32_t person_id)
{
	if (m_person_info.size() == 0)
		return NULL;

	Registrant *rt = m_person_info[person_id];
	ChurchList::QFLChurch *ch = rt->from_church;

	return ch;
}

BuildingRoomList::EURoom *Attendees::getRoom(int32_t person_id)
{
	if (m_person_info.size() == 0)
		return NULL;

	BuildingRoomList::EURoom *rm = NULL;
	Registrant *rt = m_person_info[person_id];

	if (rt->assigned_room != NULL)
		rm = rt->assigned_room;
	return rm;
}

Attendees::Registrant *Attendees::getRegistrant(int32_t person_id)
{
	if (m_person_info.size() == 0)
		return NULL;

	Registrant *rt = m_person_info[person_id];
	return rt;
}


int32_t Attendees::readChurchList(const char* churchname, SortChurchList sortmethod, int32_t year)
{
	int status;
	status = m_church_list.readInChurchList(churchname);
	if (status < 0)
		return status;

	switch (sortmethod) {
	case fByState:
	default:
		m_church_list.sortbyState();
		break;
	case fByZip:
		m_church_list.sortbyZip();
		break;
	case fByName:
		m_church_list.sortbyName();
		break;
	case fByChurchId:
		m_church_list.sortbyChurchId();
		break;
	case fByChurchCode:
		m_church_list.sortbyChurchCode();
		break;
	case fByRank:
		m_church_list.sortbyRank();
		break;
	}

	return status;
}

int32_t Attendees::readBuildingRooms(const char* buildingrooms)
{
	int status;
	status = m_br_list.readInBuildingLists(buildingrooms, ',');
	if (status != 0)
		return status;
	status = m_br_list.accumulateRoomInfo();
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
31: check in status
32: chinese church name
33: qrcode
*/
int32_t Attendees::parseAllFields()
{
	int32_t i, j, datasz;
	bool available;
	if (m_data.size() == 0)
		return -1;

	datasz = (int32_t)m_data.size(); //100

	std::vector<std::string> person = m_data[0];
	for (i = 1; i < datasz; i++){
		Registrant a_regist;
		available = true; // false
		person = m_data[i];
		// cancelled persons are removed from the list
		if (person[17].compare(Status::Cancelled) == 0) {
			//printf("cancelled: %s %s, %s %s\n", person[0].c_str(), person[1].c_str(), person[2].c_str(), person[4].c_str());
			m_cancelled++;
			continue;
		}

		if (!available) {
			m_uncertain++;
			continue;
		}
		// check first name and last name: no empty fields
		if (person[5].substr(1, person[5].size() - 2).size() == 0 && person[6].substr(1, person[5].size() - 2).size() == 0)
			continue;
		// get person id
		if (person[0].size() > 2)
			a_regist.person_id = std::stoi(person[0].substr(1, person[0].size() - 2));
		else
			a_regist.person_id = 0;
		// party id starts with P. Remove P and save the rest of the number
		if (person[1].size() > 2)
			a_regist.party = std::stoi(person[1].substr(2, person[1].size() - 2));
		else
			a_regist.party = 0;
		// get church name
		if (person[2].size() > 2) 
			a_regist.church = person[2].substr(1, person[2].size() - 2);
		else 
			a_regist.church = "";
		a_regist.from_church = NULL;
		// get contact person
		if (person[3].size() > 2)
			a_regist.contact_person = person[3].substr(1, person[3].size() - 2);
		else
			a_regist.contact_person = "";
		// party type: family or individual
		a_regist.party_type = person[4].substr(1, person[4].size() - 2);
		// first name
		a_regist.first_name = person[5].substr(1, person[5].size() - 2);
		// last name
		a_regist.last_name = person[6].substr(1, person[6].size() - 2);
		// chinese name
		a_regist.chinese_name = person[7].substr(1, person[7].size() - 2);

		// check if room is assigned
		a_regist.room = person[8].substr(1, person[8].size() - 2);
		if (a_regist.room.size() == 0)
			a_regist.assigned_room = NULL;
		else
			printf("room = %s\n", a_regist.room.c_str());

		a_regist.cell_group = person[9].substr(1, person[9].size() - 2);
		a_regist.need_room = (person[10].substr(1, person[10].size() - 2).compare(NeedRoom::RoomNeeded) == 0);

		a_regist.age_group = person[11].substr(1, person[11].size() - 2);
		a_regist.gender = person[12].substr(1, person[12].size() - 2);
		a_regist.grade = person[13].substr(1, person[13].size() - 2);
		if(person[14].size() > 2)
			a_regist.registration_fee = std::stoi(person[14].substr(1, person[14].size() - 2));
		a_regist.key_deposit = std::stoi(person[15].substr(1, person[15].size() - 2));
		// person[16] is the assigned room status flag
		// person[17] is the status (enrolled or cancelled)
		a_regist.notes = person[18].substr(1, person[18].size() - 2);
		a_regist.special_need = person[19].substr(1, person[19].size() - 2);

		a_regist.need_ride = person[20].substr(1, person[20].size() - 2);
		a_regist.offer_ride = person[21].substr(1, person[21].size() - 2);
		a_regist.is_christian = (person[22].substr(1, person[22].size() - 2).compare("Yes") == 0);
		a_regist.mobile_phone = person[24].substr(1, person[24].size() - 2);
		a_regist.email = person[25].substr(1, person[25].size() - 2);
		a_regist.city = person[26].substr(1, person[26].size() - 2);
		a_regist.state = person[27].substr(1, person[27].size() - 2);
		if (person[28].size() > 2)
			a_regist.zip = std::stoi(person[28].substr(1, person[28].size() - 2));
		else
			a_regist.zip = 0;
		a_regist.functional_group = person[29].substr(1, person[29].size() - 2);
		a_regist.services = person[30].substr(1, person[30].size() - 2);
		if (a_regist.services.find(Services::Minister) != std::string::npos)
			a_regist.occupation = std::string(Services::Minister);
		else
			a_regist.occupation = "";
		a_regist.checkin = std::stoi(person[31].substr(1, person[31].size() - 2)) > 0;
		if (person[32].size() > 2)
			a_regist.church_cname = person[32].substr(1, person[32].size() - 2);
		else
			a_regist.church_cname = a_regist.church;
		a_regist.qrcode = person[33];
		a_regist.cancelled = false;

		m_registrants.push_back(a_regist);
	}

	// remove repeated entries
	std::vector<int32_t> entries;
	for (i = 0; i < m_registrants.size(); i++) {
		Registrant attdee_i = m_registrants[i];
		for (j = i+1; j < m_registrants.size(); j++) {
			Registrant attdee_j = m_registrants[j];
			if (attdee_j.person_id == attdee_i.person_id) {
				if (attdee_i.first_name.compare(attdee_j.first_name) == 0 && attdee_i.last_name.compare(attdee_j.last_name) == 0
					&& attdee_i.contact_person.compare(attdee_j.contact_person) == 0 && attdee_i.church.compare(attdee_j.church) == 0) {
					printf("repeat i=%d:%d  j=%d:%d\n", i, attdee_i.person_id, j, attdee_j.person_id);
					entries.push_back(i);
				}
			}
			else
				break;
		}
	}

	std::vector<Registrant>::iterator it = m_registrants.begin();
	for (i = (int32_t)entries.size() - 1; i >= 0; i--) {
		m_registrants.erase(it + entries[i]);
	}

	for (i = 0; i < m_registrants.size(); i++) {
		std::string chnm = m_registrants[i].church;
		if (chnm.size() > 0)
			m_registrants[i].from_church = m_church_list.getChurch(chnm);
		m_person_info[m_registrants[i].person_id] = &(m_registrants[i]);
	}

	for (i = 0; i < m_registrants.size(); i++) {
		Registrant attdee_i = m_registrants[i];
		int32_t party_id = attdee_i.party;
		if (attdee_i.room.size() > 0) {
			m_registrants[i].assigned_room = m_br_list.getRoomByName(attdee_i.room);
		}
		m_party_info[party_id].push_back(&m_registrants[i]);
	}

	printf("=== Total registered attendees (EU+Cabrini) = %zd, Cancelled = %d, Attendee uncertain = %d\n", m_registrants.size(), m_cancelled, m_uncertain);
	return 0;
}

//
// Attendees on EU campus are seperated from Cabrini campus.
// m_family_info contains all families that need rooms in EU
// m_male_list contains all individual males that need rooms in EU
// m_female_list contains all individual females that need rooms in EU
// the commuters are temprorally excluded
//
int32_t Attendees::separateEU_CabriniCampus()
{
	int32_t i;
	int32_t commute = 0;
	m_family_info.clear();
	m_male_list.clear();
	m_female_list.clear();

	int32_t family_attendees = 0;
	int32_t male_attendees = 0;
	int32_t female_attendees = 0;
	std::map<std::string, int32_t>::iterator it;

	for (i = 0; i < m_registrants.size(); i++) {
		Registrant rt = m_registrants[i];
		int32_t family_id = rt.party;
		int32_t person_id = rt.person_id;

		std::string gender = rt.gender;
		bool is_family = (rt.party_type.find(PartyType::qFamily) != std::string::npos);
		bool youth_camp = false;
		bool need_room = true;
		m_registrants[i].youth_camp_flag = false;

		int32_t campus = Campus::qNotSure;
		it = m_grade_map.find(rt.grade);
		if (it != m_grade_map.end())
			campus = it->second;

		if (rt.grade.compare("") == 0) {
			// if the grade is not set, check grade
			if (rt.age_group.find(AgeGroup::A12_14) != std::string::npos || rt.age_group.find(AgeGroup::A15_17) != std::string::npos) {
				campus = Campus::qCabrini;
				printf("person id=%d, name=%s %s, age=%s, did not set grade...shall go to Youth camp\n",
					rt.person_id, rt.first_name.c_str(), rt.last_name.c_str(), rt.age_group.c_str());
			}
		}
		// exclude students who serve childcare in EU
		if (rt.services.find(Services::ServiceChild_2_5) != std::string::npos ||
			rt.services.find(Services::ServiceChild_6_11) != std::string::npos)
			campus = Campus::qEU;

		if (rt.notes.find("Cabrini") != std::string::npos)
			printf("%s\n", rt.notes.c_str());
		// find if the attendee goes to youth camp
		if (campus == Campus::qCabrini || rt.services.find("QFL Youth Counselor") != std::string::npos ||
			rt.cell_group.find("Youth SGLeaders") != std::string::npos || rt.notes.find("Cabrini") != std::string::npos)
			youth_camp = true;
		if (youth_camp) {
			m_registrants[i].youth_camp_flag = true;
			m_Cabrini_list.push_back(&m_registrants[i]);
			continue;
		}
		// if EU attendee is a commuter, we do not need to assign a room 
		if (!rt.need_room) {
			commute++;
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
			family_attendees++;
		}
		else {// individual
			if (gender.compare("Male") == 0) {
				m_male_list[family_id].push_back(&m_registrants[i]);
				male_attendees++;
			}
			else if (gender.compare("Female") == 0) {
				m_female_list[family_id].push_back(&m_registrants[i]);
				female_attendees++;
			}
		}

		// EU person and EU party do not contain youth camp and commute persons
		m_EU_person_info[person_id] = &m_registrants[i];
		m_EU_party_info[family_id].push_back(&m_registrants[i]);
	}


	printf("=== Lodging in EU: parties = %zd, attendees = %zd\n", m_EU_party_info.size(), m_EU_person_info.size());
	printf("=== EU Commuters: %d\n", commute);
	printf("=== EU lodging: family individual attendees=%d, individual male attendees=%d, individual female attendees=%d\n", family_attendees, male_attendees, female_attendees);
	printf("=== Lodging in EU: families = %zd, male parties = %zd, female parties = %zd\n", m_family_info.size(), m_male_list.size(), m_female_list.size());
	printf("=== Youth campers in Cabrini=%zd\n", m_Cabrini_list.size());
	return 0;
}

int32_t Attendees::classifications1()
{
	int32_t i, j, k;
	m_child_leader_list.clear();
	m_choir_list.clear();
	m_recording_list.clear();
	m_senior_list.clear();
	m_baby_list.clear();
	m_speaker_list.clear();
	m_special_need_list.clear();
	m_drama_list.clear();

	int32_t total_parties = 0;
	int32_t total_attendees = 0;
	int32_t eu_adults = 0;
	int32_t eu_children_11 = 0; // <= 11 year old, or 5th grade
	int32_t eu_children_17 = 0; // <= 17 year old, or 5th grade

	std::map<int32_t, std::vector<Registrant*>> *glists[3] = { &m_family_info , &m_male_list , &m_female_list};

	std::map<int32_t, std::vector<Registrant*>>::iterator it, temp;
	for (j = 0; j < 3; j++) {
		for (it = glists[j]->begin(); it != glists[j]->end(); it++) {
			int32_t party_id = it->first;
			std::vector<Registrant*> attendees = it->second;
			total_parties++;
			for (i = 0; i < attendees.size(); i++) {
				Registrant* registrant = attendees[i];
				if (registrant->age_group.compare(AgeGroup::A1) == 0 || registrant->age_group.compare(AgeGroup::A2) == 0 || registrant->age_group.compare(AgeGroup::A3) == 0
					|| registrant->age_group.compare(AgeGroup::A4_5) == 0 || registrant->age_group.compare(AgeGroup::A6_11) == 0) 
				{
					eu_children_11++;
				}	
				else if(registrant->age_group.compare(AgeGroup::A12_14) == 0 || registrant->age_group.compare(AgeGroup::A15_17) == 0)
				{
					eu_children_17++;
				}
				else
					eu_adults++;


				total_attendees++;
			}
		}
	}
	printf("Debug-: %d %d %d %d %d\n", total_parties, total_attendees, eu_adults, eu_children_11, eu_children_17);
	bool back_one = false;
	for (j = 0; j < 3; j++) {
		for (it = glists[j]->begin(); it != glists[j]->end(); it++) {
			if (back_one) {
				it--;
				back_one = false;
			}
			int32_t party_id = it->first;
			std::vector<Registrant*> attendees = it->second;
			bool child_leader_list = false;
			bool choir_list = false;
			bool recording_list = false;
			bool speaker_list = false;
			bool special_need_list = false;
			bool baby_list = false;
			bool senior_list = false;
			bool drama_list = false;

			for (i = 0; i < attendees.size(); i++) {
				Registrant* rt = attendees[i];
				int32_t family_id = rt->party;
				std::string gender = rt->gender;
				bool is_family = (rt->party_type.find(PartyType::qFamily) != std::string::npos);
				if(j ==0)
					assert(is_family);
				else
					assert(!is_family);

				bool adult = (rt->grade.compare(std::string(GradeGroup::G4)) == 0 || rt->grade.compare(std::string(GradeGroup::G5)) == 0);
				if (rt->services.find(Services::ServiceChild_6_11) != std::string::npos && (!adult))  {
					child_leader_list = true;
					break;
				}
				else if (rt->services.find(Services::QFL_Choir) != std::string::npos) {
					choir_list = true;
					break;
				}
				else if (rt->services.find(Services::QFL_Drama) != std::string::npos) {
					drama_list = true;
					break;
				}
				else if (rt->services.find(Services::Recording) != std::string::npos) {
					recording_list = true;
					break;
				}
				else if (rt->services.find(Services::Speaker) != std::string::npos && rt->special_need.compare("Special Arrangement") != 0) {
					speaker_list = true;
					break;
				}
				else if (rt->notes.find("轮椅通道") != std::string::npos || rt->special_need.find("轮椅通道") != std::string::npos ||
					rt->special_need.compare("Special Arrangement") == 0) {
					special_need_list = true;
					break;
				}
				else if (rt->age_group.compare(AgeGroup::A1) == 0) {
					baby_list = true;
					break;
				}
				else if (rt->age_group.compare(AgeGroup::A66_69) == 0 || rt->age_group.compare(AgeGroup::A70) == 0) {
					senior_list = true;
					break;
				}
			}

			// always in the unit of party id
			if (child_leader_list) {
				if (m_child_leader_list.find(party_id) != m_child_leader_list.end()) {
					for(k = 0; k < attendees.size(); k++)
					m_child_leader_list[party_id].push_back(attendees[k]);
				}
				else
					m_child_leader_list[party_id] = attendees;
			}
			else if (choir_list)
				m_choir_list[party_id] = attendees;
			else if (drama_list)
				m_drama_list[party_id] = attendees;
			else if (recording_list)
				m_recording_list[party_id] = attendees;
			else if (speaker_list)
				m_speaker_list[party_id] = attendees;
			else if (special_need_list)
				m_special_need_list[party_id] = attendees;
			else if (baby_list)
				m_baby_list[party_id] = attendees;
			else if (senior_list)
				m_senior_list[party_id] = attendees;

			if (child_leader_list || choir_list || recording_list || speaker_list || special_need_list || baby_list || senior_list || drama_list) {
				it = glists[j]->erase(it); 
				it--;
				if (it == glists[j]->end()) {
					it = glists[j]->begin();
					back_one = true;
				}
			}
		}
	}

	// some families entries are just singles. Other members just cancelled, or a kid may go to Youth camp
	// move them from families to individual (male or female)
	for (it = m_family_info.begin(); it != m_family_info.end(); it++) {
		int32_t party_id = it->first;
		std::vector<Registrant*> attendees = it->second;
		if (attendees.size() == 1) {
			std::string gender = attendees[0]->gender;
			if (gender.compare("Male") == 0) {
				m_male_list[party_id] = m_family_info[party_id];
			}
			else if (gender.compare("Female") == 0) {
				m_female_list[party_id] = m_family_info[party_id];
			}
			temp = it;
			temp--;
			m_family_info.erase(it);
			it = temp;
		}
	}

	std::map<int32_t, std::vector<Registrant*>> *glists2[] = 
	{ &m_family_info , &m_male_list , &m_female_list,
		&m_child_leader_list, &m_choir_list, &m_recording_list, &m_senior_list,
		&m_baby_list, &m_speaker_list, &m_special_need_list, &m_drama_list};

	 total_parties = 0;
	 total_attendees = 0;
	for (j = 0; j < sizeof(glists2)/sizeof(glists2[0]); j++) {
		for (it = glists2[j]->begin(); it != glists2[j]->end(); it++) {
			int32_t party_id = it->first;
			std::vector<Registrant*> attendees = it->second;
			total_parties++;
			for (i = 0; i < attendees.size(); i++) {
				total_attendees++;
			}
		}
	}
	printf("Debug#: %d %d\n", total_parties, total_attendees);
	return 0;
}

// youth camp and commuters are not included
int32_t Attendees::classifications()
{
	int32_t i, j;
	m_family_info.clear();
	m_child_leader_list.clear();
	m_male_list.clear();
	m_female_list.clear();
	m_choir_list.clear();
	m_drama_list.clear();
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
	
		for (j = 0; j < sizeof(GradeGroup::G2) / sizeof(GradeGroup::G2[0]); j++) {
			youth_camp |= (rt.grade.compare(GradeGroup::G2[j]) == 0);
		}
		if (rt.services.find("Youth Counselor") != std::string::npos || rt.cell_group.find("Youth SGLeaders") != std::string::npos)
			youth_camp = true;

		if (youth_camp)
			continue;
		if (!rt.need_room)
			continue;

		bool adult = (rt.grade.compare(std::string(GradeGroup::G4)) == 0 || rt.grade.compare(std::string(GradeGroup::G5)) == 0);
		if (rt.services.find(Services::ServiceChild_6_11) != std::string::npos && (!adult)) {
			m_child_leader_list[family_id].push_back(&m_registrants[i]);
			continue;
		}

		if (rt.services.find("QFL Choir") != std::string::npos) {
			m_choir_list[family_id].push_back(&m_registrants[i]);
			continue;
		}
		if (rt.services.find(Services::QFL_Drama) != std::string::npos) {
			m_choir_list[family_id].push_back(&m_registrants[i]);
			continue;
		}
		if (rt.services.find("Recording") != std::string::npos) {
			m_recording_list[family_id].push_back(&m_registrants[i]);
			continue;
		}

		if (rt.special_need.find("speaker") != std::string::npos && rt.occupation.find("Minister") != std::string::npos &&
			rt.notes.find("Special Arrangement") == std::string::npos) {
			m_speaker_list[family_id].push_back(&m_registrants[i]);
			continue;
		}

		if (rt.notes.find("轮椅通道") != std::string::npos || rt.special_need.find("轮椅通道") != std::string::npos ||
			rt.notes.find("Special Arrangement") != std::string::npos ) {
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

	// refine drama list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_drama;
	for (it_drama = m_drama_list.begin(); it_drama != m_drama_list.end(); ++it_drama) {
		int32_t drama_id = it_drama->first;
		std::vector<Registrant*> lst_choir = it_drama->second;
		for (it_family = m_family_info.begin(); it_family != m_family_info.end(); ++it_family) {
			int32_t family_id = it_family->first;
			std::vector<Registrant*> lst_family = it_family->second;
			if (drama_id == family_id) {
				for (i = 0; i < lst_family.size(); i++) {
					m_drama_list[drama_id].push_back(lst_family[i]);
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

/*
 	Attendees are sorted by churchs:
	  m_family_info , m_male_list ,m_female_list, m_senior_list, m_baby_list, m_special_need_list.
	The following lists are not sorted by church:
	  m_child_leader_list, m_choir_list, m_drama_list, m_recording_list, m_speaker_list
*/
int32_t Attendees::sortAttendeesByChurches()
{
	int32_t i, j;
	std::vector<ChurchList::QFLChurch> *churches = getChurchHandle()->getChurchList();
	m_attendee_list_byChurch.clear();
	int32_t lodging_attendees = 0;

	for (i = 0; i < churches->size(); i++) {
		ChurchList::QFLChurch achurch = (*churches)[i];
		std::string name = achurch.church_name;
		Church thechurch;
		thechurch.church_name = name;
		thechurch.from_church = &((*churches)[i]);
		m_attendee_list_byChurch[name] = thechurch;
	}

	/// male list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_male;
	for (it_male = m_male_list.begin(); it_male != m_male_list.end(); ++it_male) {
		int32_t male_id = it_male->first;
		std::vector<Registrant*> lst_male = it_male->second;
		Registrant *rt = NULL;
		std::string chname;
		int32_t party_id;
		if (lst_male.size() == 1) {
			rt = lst_male[0];
			chname = rt->church;
			party_id = rt->party;
			m_attendee_list_byChurch[chname].male_list[party_id] = m_male_list[party_id];
			m_attendee_list_byChurch[chname].persons.push_back(lst_male[0]);
			lodging_attendees++;
		}
		else {
			rt = lst_male[0];
			chname = rt->church;
			party_id = rt->party;
			for (j = 0; j < lst_male.size(); j++) {
				assert(lst_male[j]->gender.compare("Male") == 0);
				m_attendee_list_byChurch[chname].persons.push_back(lst_male[j]);
				lodging_attendees++;
			}
			m_attendee_list_byChurch[chname].male_list[party_id] = m_male_list[party_id];
		}
	}

	/// female list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_female;
	for (it_female = m_female_list.begin(); it_female != m_female_list.end(); ++it_female) {
		int32_t female_id = it_female->first;
		std::vector<Registrant*> lst_female = it_female->second;
		Registrant *rt = NULL;
		std::string chname;
		int32_t party_id;
		if (lst_female.size() == 1) {
			rt = lst_female[0];
			chname = rt->church;
			party_id = rt->party;
			if (party_id == 310)
				party_id = 310;
			m_attendee_list_byChurch[chname].female_list[party_id] = m_female_list[party_id];
			m_attendee_list_byChurch[chname].persons.push_back(lst_female[0]);
			lodging_attendees++;
		}
		else {
			rt = lst_female[0];
			chname = rt->church;
			party_id = rt->party;
			if (party_id == 310)
				party_id = 310;
			for (j = 0; j < lst_female.size(); j++) {
				assert(lst_female[j]->gender.compare("Female") == 0);
				m_attendee_list_byChurch[chname].persons.push_back(lst_female[j]);
				lodging_attendees++;
			}
			m_attendee_list_byChurch[chname].female_list[party_id] = m_female_list[party_id];
		}
	}

	/// senior list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_senior;
	for (it_senior = m_senior_list.begin(); it_senior != m_senior_list.end(); ++it_senior) {
		int32_t senior_id = it_senior->first;
		std::vector<Registrant*> lst_senior = it_senior->second;
		for (j = 0; j < lst_senior.size(); j++) {
			Registrant *rt = lst_senior[j];
			std::string chname = rt->church;
			int32_t party_id = rt->party;
			m_attendee_list_byChurch[chname].senior_list.push_back(m_senior_list[party_id][j]);
			m_attendee_list_byChurch[chname].persons.push_back(rt);
			lodging_attendees++;
		}
	}

	/// baby list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_baby;
	for (it_baby = m_baby_list.begin(); it_baby != m_baby_list.end(); ++it_baby) {
		int32_t baby_id = it_baby->first;
		std::vector<Registrant*> lst_baby = it_baby->second;
		for (j = 0; j < lst_baby.size(); j++) {
			Registrant *rt = lst_baby[j];
			std::string chname = rt->church;
			int32_t party_id = rt->party;
			m_attendee_list_byChurch[chname].baby_list.push_back(m_baby_list[party_id][j]);
			m_attendee_list_byChurch[chname].persons.push_back(rt);
			lodging_attendees++;
		}
	}

	/// special need list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_special_need;
	for (it_special_need = m_special_need_list.begin(); it_special_need != m_special_need_list.end(); ++it_special_need) {
		int32_t special_need_id = it_special_need->first;
		std::vector<Registrant*> lst_special_need = it_special_need->second;
		for (j = 0; j < lst_special_need.size(); j++) {
			Registrant *rt = lst_special_need[j];
			std::string chname = rt->church;
			int32_t party_id = rt->party;
			m_attendee_list_byChurch[chname].special_need_list.push_back(m_special_need_list[party_id][j]);
			m_attendee_list_byChurch[chname].persons.push_back(rt);
			lodging_attendees++;
		}
	}

	/// family list
	std::map < int32_t, std::vector<Registrant*>>::iterator it_family;
	for (it_family = m_family_info.begin(); it_family != m_family_info.end(); ++it_family) {
		int32_t family_id = it_family->first;
		std::vector<Registrant*> lst_family = it_family->second;
		Party pty;
		pty.contact_person = lst_family[0]->contact_person;
		pty.email = lst_family[0]->email;
		pty.city = lst_family[0]->city;
		pty.mobile_phone = lst_family[0]->mobile_phone;
		pty.state = lst_family[0]->state;
		pty.zip = lst_family[0]->zip;
		pty.party = lst_family[0]->party;
		pty.church = lst_family[0]->church;
		pty.from_church = lst_family[0]->from_church;

		for (j = 0; j < lst_family.size(); j++) {
			bool matched = false;
			BuildingRoomList::EURoom * eu_room = NULL;
			Registrant *rt = lst_family[j];
			std::string chname = rt->church;
			int32_t party_id = rt->party;
			int32_t person_id = rt->person_id;
			pty.attendee_list_b[family_id].push_back(getRegistrant(person_id));
			m_attendee_list_byChurch[chname].persons.push_back(rt);
			lodging_attendees++;
			eu_room = getRoom(person_id);
			if (eu_room != NULL) {
				for (i = 0; i < pty.assigned_rooms.size(); i++) {
					if (eu_room == pty.assigned_rooms[i]) {
						matched = true;
						break;
					}
				}
				if (!matched)
					pty.assigned_rooms.push_back(getRoom(person_id));
			}
		}
		m_attendee_list_byChurch[pty.church].family_list[pty.party] = pty;
	}

	// not include all attendees
	printf("=== EU selected lodging attendees = %d\n", lodging_attendees);

	// now add all the rest of the groups
	std::map<int32_t, std::vector<Registrant*>>::iterator it;
	std::map<int32_t, std::vector<Registrant*>> *glists[5] = { &m_child_leader_list , &m_choir_list , &m_drama_list, &m_recording_list,  &m_speaker_list};
	int32_t other_attendees = 0;
	for (j = 0; j < sizeof(glists) / sizeof(glists[0]); j++) {
		for (it = glists[j]->begin(); it != glists[j]->end(); it++) {
			int32_t party_id = it->first;
			std::vector<Registrant*> attendees = it->second;
			for (i = 0; i < attendees.size(); i++) {
				other_attendees++;
			}
		}
	}
	printf("Debug!: attendees = %d\n", lodging_attendees + other_attendees);
	return 0;
}
