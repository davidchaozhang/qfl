#include <algorithm>
#include <fstream>
#include <forward_list>
#include <array>
#include <vector>
#include <functional>      // For greater<int>( )
#include <iostream>
#include <assert.h>
#include "roomassign.h"
#include <string>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#include <sys/utime.h>
#include <sstream>
#include <ctime>
#else
#include <ctime>
#endif

using namespace std;

RoomAssign::RoomAssign()
{
}

RoomAssign::~RoomAssign()
{
}

int32_t RoomAssign::readInputs(const char* church_name, const char *buildings_name, const char *registration_name, int32_t year)
{
	int32_t status = 0;
	Attendees::SortChurchList sortmethod = fByRank;
	status = readChurchList(church_name, sortmethod, year);
	if (status < 0)
		return status;

	status = readBuildingRooms(buildings_name);
	if (status < 0)
		return status;

	status = readRegistrants(registration_name);
	if (status < 0)
		return status;

	return status;
}

int32_t RoomAssign::preprocessData()
{
	int32_t status = 0;
	status = parseAllFields();
	if (status < 0)
		return status;
	status = classifications();
	if (status < 0)
		return status;
	status = refinement();
	if (status < 0)
		return status;
	status = sortAttendeesByChurches();
	if (status < 0)
		return status;

	return status;
}

int32_t RoomAssign::preprocessData1()
{
	int32_t status = 0;
	status = parseAllFields();
	if (status < 0)
		return status;

	status = separateEU_CabriniCampus();
	if (status < 0)
		return status;

	status = classifications1();
	if (status < 0)
		return status;

	status = sortAttendeesByChurches();
	if (status < 0)
		return status;

	return status;
}

int32_t RoomAssign::assignRooms2SpecialNeeds()
{
	bool enable_extrabeds = true;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSPRooms();
	std::map<int32_t, std::vector<Registrant*>> spnlist = m_special_need_list;

	printf("=== Assign rooms to Special Needs: Reserved Special Needs rooms = %zd, Special Needs family size = %zd\n", roomlist.size(), spnlist.size());
	// family assignment first
	if (spnlist.size() > 0) {
		if (familyRoomAssign(spnlist, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2SpecialNeeds(): Family room assignment failed\n");
	}
	return 0;
}

/*
* find family, allocate room
* find individual, allocate rooms
*/
int32_t RoomAssign::assignRooms2Choir()
{
	bool enable_extrabeds = false;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedCHRooms();
	std::map<int32_t, std::vector<Registrant*>> choirlist = m_choir_list;

	printf("=== Assign rooms to Choir: Reserved Choir rooms = %zd, Choir family size = %zd\n", roomlist.size(), choirlist.size());
	// get family list
	std::map<int32_t, std::vector<Registrant*>> family_lst;
	std::map<int32_t, std::vector<Registrant*>> female_lst;
	std::map<int32_t, std::vector<Registrant*>> male_lst;

	std::map < int32_t, std::vector<Registrant*>>::iterator it;
	for (it = m_choir_list.begin(); it != m_choir_list.end(); ++it) {
		int32_t party_id = it->first;
		std::vector<Registrant*> aregist = it->second;
		// if assigned already, then skip it
		if (aregist[0]->assigned_room)
			continue;
		if (aregist.size() > 1)
			family_lst[party_id] = (m_choir_list[party_id]);
		else if (aregist[0]->gender.compare("Male") == 0)
			male_lst[party_id] = (m_choir_list[party_id]);
		else if (aregist[0]->gender.compare("Female") == 0)
			female_lst[party_id] = (m_choir_list[party_id]);
	}

	if (male_lst.size() >= female_lst.size()) {
		// male room assignment - male do not share bathroom with females 
		if (male_lst.size() > 0) {
			if (IndividualRoomAssign2(male_lst, BuildingRoomList::eMale, roomlist) <= 0)
				printf("assignRooms2Choir(): Male room assignment failed\n");
		}
		// female room assignment - male do not share bathroom with females 
		if (female_lst.size() > 0) {
			if (IndividualRoomAssign2(female_lst, BuildingRoomList::eFemale, roomlist) <= 0)
				printf("assignRooms2Choir(): Female room assignment failed\n");
		}
	}
	else {
		// female room assignment - male do not share bathroom with females 
		if (female_lst.size() > 0) {
			if (IndividualRoomAssign2(female_lst, BuildingRoomList::eFemale, roomlist) <= 0)
				printf("assignRooms2Choir(): Female room assignment failed\n");
		}
		// male room assignment - male do not share bathroom with females 
		if (male_lst.size() > 0) {
			if (IndividualRoomAssign2(male_lst, BuildingRoomList::eMale, roomlist) <= 0)
				printf("assignRooms2Choir(): Male room assignment failed\n");
		}
	}

	// family assignment first
	bool enable_extrabeds_for_family = true;
	if (family_lst.size() > 0) {
		if (familyRoomAssign(family_lst, roomlist, enable_extrabeds_for_family) <= 0)
			printf("assignRooms2Choir(): Family room assignment failed\n");
	}

	return 0;
}

/*
* find family, allocate room
* find individual, allocate rooms
*/
int32_t RoomAssign::assignRooms2DramaTeam()
{
	bool enable_extrabeds = false;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedCHRooms(); // may change
	std::map<int32_t, std::vector<Registrant*>> dramalist = m_drama_list;

	printf("=== Assign rooms to Drama: Reserved Drama rooms = %zd, Drama family size = %zd\n", roomlist.size(), dramalist.size());
	// get family list
	std::map<int32_t, std::vector<Registrant*>> family_lst;
	std::map<int32_t, std::vector<Registrant*>> female_lst;
	std::map<int32_t, std::vector<Registrant*>> male_lst;

	std::map < int32_t, std::vector<Registrant*>>::iterator it;
	for (it = m_drama_list.begin(); it != m_drama_list.end(); ++it) {
		int32_t party_id = it->first;
		std::vector<Registrant*> aregist = it->second;
		// if assigned already, then skip it
		if (aregist[0]->assigned_room)
			continue;
		if (aregist.size() > 1)
			family_lst[party_id] = (m_drama_list[party_id]);
		else if (aregist[0]->gender.compare("Male") == 0)
			male_lst[party_id] = (m_drama_list[party_id]);
		else if (aregist[0]->gender.compare("Female") == 0)
			female_lst[party_id] = (m_drama_list[party_id]);
	}

	if (female_lst.size() > male_lst.size()) {
		// female room assignment - male do not share bathroom with females 
		if (female_lst.size() > 0) {
			if (IndividualRoomAssign2(female_lst, BuildingRoomList::eFemale, roomlist) <= 0)
				printf("assignRooms2DramaTeam(): Female room assignment failed\n");
		}
		// male room assignment - male do not share bathroom with females 
		if (male_lst.size() > 0) {
			if (IndividualRoomAssign2(male_lst, BuildingRoomList::eMale, roomlist) <= 0)
				printf("assignRooms2DramaTeam(): Male room assignment failed\n");
		}
	}
	else {
		// male room assignment - male do not share bathroom with females 
		if (male_lst.size() > 0) {
			if (IndividualRoomAssign2(male_lst, BuildingRoomList::eMale, roomlist) <= 0)
				printf("assignRooms2DramaTeam(): Male room assignment failed\n");
		}
		// female room assignment - male do not share bathroom with females 
		if (female_lst.size() > 0) {
			if (IndividualRoomAssign2(female_lst, BuildingRoomList::eFemale, roomlist) <= 0)
				printf("assignRooms2DramaTeam(): Female room assignment failed\n");
		}
	}

	// family assignment first
	if (family_lst.size() > 0) {
		if (familyRoomAssign(family_lst, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2DramaTeam(): Family room assignment failed\n");
	}

	return 0;
}

int32_t RoomAssign::assignRooms2ChildcareWorkers()
{
	int32_t i;
	std::string contacts[2] = { "Hong Li", "Jing Pan"};
	bool enable_extrabeds = false;
	bool coordinate_flag = false;

	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedCCRooms();
	std::vector<BuildingRoomList::EURoom*> male_roomlist;
	std::vector<BuildingRoomList::EURoom*> female_roomlist;

	std::map<int32_t, Registrant*> childleadlist;
	std::map<int32_t, std::vector<Registrant*>> female_lst;
	std::map<int32_t, std::vector<Registrant*>> male_lst;
	std::map<int32_t, std::vector<Registrant*>> family_lst;
	std::map<int32_t, std::vector<Registrant*>> family_adult_male_lst;
	std::map<int32_t, std::vector<Registrant*>> family_adult_female_lst;
	std::map<int32_t, std::vector<Registrant*>> coordinator_family_lst;
	printf("=== Assign rooms to Childcare: Reserved Childcare worker rooms = %zd, Child leader # = %zd\n", roomlist.size(), m_child_leader_list.size());
	// find Li Hong's family, and Pan Jing's family
	// assign Li hong's family to stay in the same building
	// assign Pan Jing's family to stay in the same building
	// find the rest of the boy and girl counselors 
	// assign them to boy and girl sections respectively
	// find other family members and assign them elsewhere

	// generate child counselor list (age range: 14-25)
	std::vector<Registrant*> aparty;
	std::map<int32_t, std::vector<Registrant*>>::iterator it;
	for (it = m_child_leader_list.begin(); it != m_child_leader_list.end(); it++) {
		int32_t party_id = it->first;
		bool age_flag = false;
		coordinate_flag = false;
		std::vector<Registrant*> party = it->second;
		if (party.size() == 0)
			continue;
		std::string contact = party[0]->contact_person;
		aparty.clear();
		for (i = 0; i < party.size(); i++) {
			int32_t person_id = party[i]->person_id;
			coordinate_flag = (contact.compare(contacts[0]) == 0 || contact.compare(contacts[1]) == 0);
			bool isMale = (party[i]->gender.compare("Male") == 0);
			std::string age = party[i]->age_group;
			if (age.compare(AgeGroup::A12_14) == 0 || age.compare(AgeGroup::A15_17) == 0 || age.compare(AgeGroup::A18_25) == 0)
			{
				childleadlist[person_id] = m_person_info[person_id];
				if (isMale)
					male_lst[person_id].push_back(m_person_info[person_id]);
				else
					female_lst[person_id].push_back(m_person_info[person_id]);
			}
			else
				aparty.push_back(m_person_info[person_id]);
		}

		if (coordinate_flag && aparty.size() != 0) {
			coordinator_family_lst[party_id] = aparty;
		}
		else if(aparty.size()  > 0) {
			if (aparty.size() == 1) {
				if (party[0]->gender.compare("Male") == 0)
					family_adult_male_lst[party_id] = aparty;
				else if (party[0]->gender.compare("Female") == 0)
					family_adult_female_lst[party_id] = aparty;
			}
			else if (aparty.size() > 1) {
				family_lst[party_id] = aparty;
			}
		}
	}

	// generate female and male lists
	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* proom = roomlist[i];
		if (proom->room_type == RoomTypes::qMale)
			male_roomlist.push_back(roomlist[i]);
		else
			female_roomlist.push_back(roomlist[i]);
	}

	// male room assignment - male do not share bathroom with females 
	if (male_lst.size() > 0) {
		if (IndividualRoomAssign2(male_lst, BuildingRoomList::eMale, male_roomlist) <= 0)
			printf("assignRooms2ChildcareWorkers(): room assignment failed\n");
	}

	// female room assignment - male do not share bathroom with females 
	if (female_lst.size() > 0) {
		if (IndividualRoomAssign2(female_lst, BuildingRoomList::eFemale, female_roomlist) <= 0)
			printf("assignRooms2ChildcareWorkers(): female room assignment failed\n");
	}

	std::vector<BuildingRoomList::EURoom*> family_roomlist;
	std::vector<BuildingRoomList::EURoom*> adult_male_roomlist;
	std::vector<BuildingRoomList::EURoom*> adult_female_roomlist;

	for (i = 0; i < m_br_list.queryFamilyPrivateRooms().size(); i++) {
		std::string status = m_br_list.queryFamilyPrivateRooms()[i]->room_status;
		std::string room = m_br_list.queryFamilyPrivateRooms()[i]->room;
		if (status.compare("Available") == 0) {
			family_roomlist.push_back(m_br_list.queryFamilyPrivateRooms()[i]);
		}
	}
	for (i = 0; i < m_br_list.queryMaleRooms().size(); i++) {
		std::string status = m_br_list.queryMaleRooms()[i]->room_status;
		std::string room = m_br_list.queryMaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			adult_male_roomlist.push_back(m_br_list.queryMaleRooms()[i]);
		}
	}
	for (i = 0; i < m_br_list.queryFemaleRooms().size(); i++) {
		std::string status = m_br_list.queryFemaleRooms()[i]->room_status;
		std::string room = m_br_list.queryFemaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			adult_female_roomlist.push_back(m_br_list.queryFemaleRooms()[i]);
		}
	}

	// assign Li hong and Pan Jing's family to stay in a family private room 
	if (coordinator_family_lst.size() > 0) {
		if (familyRoomAssign(coordinator_family_lst, family_roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2ChildcareWorkers(): Family room assignment failed\n");
	}

	if (family_lst.size() > 0) {
		if (familyRoomAssign(family_lst, family_roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2ChildcareWorkers(): Family room assignment failed\n");
	}

	if (family_adult_male_lst.size() > 0) {
		if (IndividualRoomAssign2(family_adult_male_lst, BuildingRoomList::eMale, adult_male_roomlist) <= 0)
			printf("assignRooms2ChildcareWorkers(): Male room assignment failed\n");
	}

	if (family_adult_female_lst.size() > 0) {
		if (IndividualRoomAssign2(family_adult_female_lst, BuildingRoomList::eFemale, adult_female_roomlist) <= 0)
			printf("assignRooms2ChildcareWorkers(): Female room assignment failed\n");
	}

	return 0;
}

int32_t RoomAssign::assignRooms2Speakers()
{
	bool enable_extrabeds = false;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSKRooms();
	std::map<int32_t, std::vector<Registrant*>> spk_list = m_speaker_list;

	printf("=== Assign rooms to Speakers: Reserved Speaker rooms = %zd, Speaker family size = %zd\n", roomlist.size(), spk_list.size());
	// family assignment first
	if (spk_list.size() > 0) {
		if (familyRoomAssign(spk_list, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2Speakers(): Family room assignment failed\n");
	}
	return 0;
}

// recording workers do not share rooms if they are registered as individuals
int32_t RoomAssign::assignRooms2Recordings()
{
	bool enable_extrabeds = false;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSKRooms();
	std::map<int32_t, std::vector<Registrant*>> recording_list = m_recording_list;

	printf("=== Assign rooms to Recordings: Reserved Speaker rooms = %zd, Recording family size = %zd\n", roomlist.size(), recording_list.size());
	// family assignment first
	if (recording_list.size() > 0) {
		if (familyRoomAssign(recording_list, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2Recordings(): Family room assignment failed\n");
	}
	return 0;
}

int32_t RoomAssign::assignRooms2Babies()
{
	bool enable_extrabeds = true; // can change to true since baby is likely to stay on floor when sleep
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSPRooms();
	std::map<int32_t, std::vector<Registrant*>> baby_list = m_baby_list;

	printf("=== Assign rooms to Baby families: Reserved SpecialNeed rooms = %zd, Baby family size = %zd\n", roomlist.size(), baby_list.size());
	// family assignment first
	if (baby_list.size() > 0) {
		if (familyRoomAssign(baby_list, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2Babies(): Family room assignment failed\n");
	}
	return 0;
}

// families stay in a room 
// individuls share rooms
// male room and female room do not share bathroom
int32_t RoomAssign::assignRooms2Seniors()
{
	bool enable_extrabeds = false;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSPRooms();
	std::map<int32_t, std::vector<Registrant*>> senior_list = m_senior_list;

	printf("=== Assign rooms to Seniors: Reserved SpecialNeed rooms = %zd, Senior family size = %zd\n", roomlist.size(), senior_list.size());
	// family assignment first
	if (senior_list.size() > 0) {
		if (familyRoomAssign(senior_list, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2Seniors(): Family room assignment failed\n");
	}
	return 0;
}

int32_t RoomAssign::assignRooms2Families()
{
	int32_t i, j;
	int32_t cnt_family = 0, cnt_rooms = 0;
	bool enable_extrabeds = true;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist, male_roomlist, female_roomlist;
	std::map<int32_t, std::vector<Registrant*>> all_family_list = m_family_info;
	std::vector<ChurchList::QFLChurch> *churches = getChurchHandle()->getChurchList();

	for (i = 0; i < m_br_list.queryFamilyPrivateRooms().size(); i++) {
		std::string status = m_br_list.queryFamilyPrivateRooms()[i]->room_status;
		std::string room = m_br_list.queryFamilyPrivateRooms()[i]->room;
		if (status.compare("Available") == 0) {
			roomlist.push_back(m_br_list.queryFamilyPrivateRooms()[i]);
			//printf("Family Private: %s %s\n", room.c_str(), status.c_str());
		}
	}

	printf("=== Available rooms to families: rooms = %zd, family size = %zd\n", roomlist.size(), all_family_list.size());

	for (i = (int32_t)churches->size() - 1; i >= 0; i--) {
		ChurchList::QFLChurch achurch = (*churches)[i];
		std::string name = achurch.church_name;
		Church thechurch;
		thechurch = m_attendee_list_byChurch[name];

		std::map<int32_t, Party>::iterator it;
		for (it = thechurch.family_list.begin(); it != thechurch.family_list.end(); it++) {
			int32_t party_id = it->first;
			std::map<int32_t, std::vector<Registrant*>> family_list = it->second.attendee_list_b;
			if (family_list.size() == 0)
				continue;

			cnt_family++;
			// family room assignment 
			int32_t room_nums = familyRoomAssign(family_list, roomlist, enable_extrabeds);
			if (room_nums <= 0) {
				//printf("assignRooms2Families(): Family room assignment failed\n");
				room_nums = 0;
			}
			cnt_rooms += room_nums;
			//printf("families %d, rooms %d\n", cnt_family, cnt_rooms);

		}
	}

	// assign family_male rooms
	for (i = 0; i < m_br_list.queryFamilyMaleRooms().size(); i++) {
		std::string status = m_br_list.queryFamilyMaleRooms()[i]->room_status;
		std::string room = m_br_list.queryFamilyMaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			male_roomlist.push_back(m_br_list.queryFamilyMaleRooms()[i]);
			//printf("Family Male: %s %s\n", room.c_str(), status.c_str());
		}
	}
	// assign family_female rooms
	for (i = 0; i < m_br_list.queryFamilyFemaleRooms().size(); i++) {
		std::string status = m_br_list.queryFamilyFemaleRooms()[i]->room_status;
		std::string room = m_br_list.queryFamilyFemaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			female_roomlist.push_back(m_br_list.queryFamilyFemaleRooms()[i]);
			//printf("Family Female: %s %s\n", room.c_str(), status.c_str());
		}
	}

	for (i = 0; i < churches->size(); i++) {
		ChurchList::QFLChurch achurch = (*churches)[i];
		std::string name = achurch.church_name;
		Church thechurch;
		thechurch = m_attendee_list_byChurch[name];

		std::map<int32_t, Party>::iterator it;
		for (it = thechurch.family_list.begin(); it != thechurch.family_list.end(); it++) {
			int32_t party_id = it->first;
			std::map<int32_t, std::vector<Registrant*>> family_list = it->second.attendee_list_b;
			std::map<int32_t, std::vector<Registrant*>>::iterator fit;
			for (fit = family_list.begin(); fit != family_list.end(); fit++) {
				int32_t family_id = fit->first;
				std::vector<Registrant*> family = fit->second;
				if (family[0]->assigned_room)
					continue;

				// room not assigned
				int32_t male_cnt = 0;
				int32_t female_cnt = 0;
				int32_t youngest_cnt = 0;
				bool youngest_male = false;
				bool male_room = false;
				int32_t room_nums;
				for (j = 0; j < family.size(); j++) {
					Registrant *registant = family[j];
					if (registant->gender.compare("Male") == 0)
						male_cnt++;
					else if (registant->gender.compare("Female") == 0)
						female_cnt++;
					if (registant->age_group.compare(AgeGroup::A1) == 0 || registant->age_group.compare(AgeGroup::A2) == 0 || registant->age_group.compare(AgeGroup::A3) == 0
						|| registant->age_group.compare(AgeGroup::A4_5) == 0 || registant->age_group.compare(AgeGroup::A6_11) == 0 || registant->age_group.compare(AgeGroup::A12_14) == 0) {
						youngest_male = (registant->gender.compare("Male") == 0);
						if (youngest_male)
							youngest_cnt++;
						else
							youngest_cnt--;
					}
				}
				cnt_family++;

				if (youngest_cnt > 0) {
					if (youngest_male)
						room_nums = familyMaleRoomAssign(family, male_roomlist, false);
					else
						room_nums = familyFemaleRoomAssign(family, female_roomlist, false);
				}
				else {
					if (male_cnt > female_cnt)
						room_nums = familyMaleRoomAssign(family, male_roomlist, false);
					else
						room_nums = familyFemaleRoomAssign(family, female_roomlist, false);
				}
				if (room_nums <= 0) {
					printf("assignRooms2Families(): Family room assignment failed\n");
					room_nums = 0;
				}
				cnt_rooms += room_nums;
				//printf("families %d, rooms %d\n", cnt_family, cnt_rooms);
			}
		}
	}

	return 0;
}

int32_t RoomAssign::assignRooms2Males()
{
	int32_t i;
	bool enable_extrabeds = false;
	int32_t cnt_person = 0, cnt_beds = 0;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist;
	std::map<int32_t, std::vector<Registrant*>> all_male_list = m_male_list;
	std::vector<ChurchList::QFLChurch> *churches = getChurchHandle()->getChurchList();

	printf("=== Assign rooms to males: rooms = %zd, male size = %zd\n", m_br_list.queryMaleRooms().size(), all_male_list.size());

	for (i = 0; i < m_br_list.queryMaleRooms().size(); i++) {
		std::string status = m_br_list.queryMaleRooms()[i]->room_status;
		std::string room = m_br_list.queryMaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			roomlist.push_back(m_br_list.queryMaleRooms()[i]);
			//printf("%s %s\n", room.c_str(), status.c_str());
		}
	}

	for (i = 0; i < churches->size(); i++) {
		ChurchList::QFLChurch achurch = (*churches)[i];
		std::string name = achurch.church_name;
		Church thechurch;
		thechurch = m_attendee_list_byChurch[name];

		std::map<int32_t, std::vector<Registrant*>> male_list = thechurch.male_list;

		if (male_list.size() == 0)
			continue;

		cnt_person += (int32_t)male_list.size();
		// male room assignment - male do not share bathroom with females 
		int32_t bed_nums = IndividualRoomAssign(male_list, BuildingRoomList::eMale, roomlist);
		if (bed_nums <= 0) {
			bed_nums = 0;
			printf("assignRooms2Males(): room assignment failed\n");
		}
		cnt_beds += bed_nums;
		//printf("persons %d, beds %d\n", cnt_person, cnt_beds);
	}

	return 0;
}

int32_t RoomAssign::assignRooms2Females()
{
	int32_t i;
	bool enable_extrabeds = false;
	int32_t cnt_person = 0, cnt_beds = 0;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist;
	std::map<int32_t, std::vector<Registrant*>> all_female_list = m_female_list;
	std::vector<ChurchList::QFLChurch> *churches = getChurchHandle()->getChurchList();

	printf("=== Assign rooms to females: rooms = %zd, female size = %zd\n", m_br_list.queryFemaleRooms().size(), all_female_list.size());

	for (i = 0; i < m_br_list.queryFemaleRooms().size(); i++) {
		std::string status = m_br_list.queryFemaleRooms()[i]->room_status;
		std::string room = m_br_list.queryFemaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			roomlist.push_back(m_br_list.queryFemaleRooms()[i]);
			//printf("%s %s\n", room.c_str(), status.c_str());
		}
	}

	for (i = 0; i < churches->size(); i++) {
		ChurchList::QFLChurch achurch = (*churches)[i];
		std::string name = achurch.church_name;
		Church thechurch;
		thechurch = m_attendee_list_byChurch[name];

		std::map<int32_t, std::vector<Registrant*>> female_list = thechurch.female_list;

		if (female_list.size() == 0)
			continue;

		cnt_person += (int32_t)female_list.size();
		// male room assignment - male do not share bathroom with females 
		int32_t bed_nums = IndividualRoomAssign(female_list, BuildingRoomList::eFemale, roomlist);
		if (bed_nums <= 0) {
			bed_nums = 0;
			printf("assignRooms2Females(): room assignment failed\n");
		}
		cnt_beds += bed_nums;
		//printf("persons %d, rooms %d\n", cnt_person, cnt_beds);
	}

	return 0;
}


std::vector<BuildingRoomList::EURoom*> RoomAssign::queryRoomList(std::vector<BuildingRoomList::EURoom*> &myroomlist, int32_t num)
{
	int32_t i, j;
	std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> rms;
	std::vector<BuildingRoomList::EURoom* > frl;
	int32_t total_capacity = 0, sz = (int32_t)myroomlist.size();
	if (sz == 0)
		return frl;

	for (i = 0; i < sz; i++) {
		BuildingRoomList::EURoom* eur = myroomlist[i];
		bool status = eur->room_status == RoomStatus::qReservedCH || eur->room_status == RoomStatus::qReservedGE ||
			eur->room_status == RoomStatus::qReservedSK || eur->room_status == RoomStatus::qReservedSP ||
			eur->room_status == RoomStatus::qReservedCC ||
			eur->room_status == RoomStatus::qAvailable || eur->room_status == RoomStatus::qPartiallyAssigned;
		if (status && (!eur->considered)) {
			int32_t capacity = eur->capacity;
			int32_t score = eur->score;
			total_capacity += capacity;
			rms[score].push_back(myroomlist[i]);
		}
	}

	// find room that matches its capacity to num
	total_capacity = 0;
	std::map < int32_t, std::vector<BuildingRoomList::EURoom*>>::iterator it;
	for (it = rms.begin(); it != rms.end(); ++it) {
		int32_t score = it->first;
		std::vector<BuildingRoomList::EURoom*> roomlist = it->second;
		for (j = 0; j < roomlist.size(); j++) {
			total_capacity = roomlist[j]->capacity;
			if (total_capacity == num) {
				frl.push_back(roomlist[j]);
				return frl;
			}
		}
	}
	// find shared rooms that matches their capacity to num
	for (it = rms.begin(); it != rms.end(); ++it) {
		total_capacity = 0;
		int32_t score = it->first;
		std::vector<BuildingRoomList::EURoom*> roomlist = it->second;

		// add shared room capacity
		for (j = 0; j < roomlist.size(); j++)
			total_capacity += roomlist[j]->capacity;

		if (total_capacity == num) {
			frl.push_back(roomlist[0]);
			frl.push_back(roomlist[1]);
			return frl;
		}
	}

	// otherwise, accumulate room capacity to match up to the people size
	total_capacity = 0;
	for (it = rms.begin(); it != rms.end(); ++it) {
		int32_t score = it->first;
		std::vector<BuildingRoomList::EURoom*> roomlist = it->second;
		for (j = 0; j < roomlist.size(); j++) {
			total_capacity += roomlist[j]->capacity;
			if (total_capacity < num) {
				frl.push_back(roomlist[j]);
			}
			else {
				frl.push_back(roomlist[j]);
				return frl;
			}
		}
	}

	return frl;
}


std::vector<BuildingRoomList::EURoom*> RoomAssign::queryFamilyRoomList(std::vector<BuildingRoomList::EURoom*> &myroomlist, int32_t num, Registrant* registrant, bool enable_extrabed)
{
	int32_t i;
	std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> rms;
	std::vector<BuildingRoomList::EURoom* > frl; frl.clear();
	int32_t total_capacity = 0, sz = (int32_t)myroomlist.size();
	if (sz == 0)
		return frl;

	for (i = 0; i < sz; i++) {
		BuildingRoomList::EURoom* eur = myroomlist[i];
		//if (eur->room_type != RoomTypes::qFamily_Private)
			//continue;

		if (eur->room_status == RoomStatus::qReservedCH || eur->room_status == RoomStatus::qReservedGE ||
			eur->room_status == RoomStatus::qReservedSK || eur->room_status == RoomStatus::qReservedSP ||
			eur->room_status == RoomStatus::qAvailable) {
			int32_t capacity = eur->capacity;
			int32_t score = eur->score;
			total_capacity += capacity + (enable_extrabed ? 1 : 0);
			rms[score].push_back(myroomlist[i]);
		}
	}

	std::map < int32_t, std::vector<BuildingRoomList::EURoom*>>::iterator it;

	// match party number to the # of beds per room or per shared rooms 
	frl = RoomCapacityMatch(rms, num, registrant, enable_extrabed);
	if (!frl.empty())
		return frl;

	// find the capacity of room matches the number of family size
	frl = RoomSpecialMatch(rms, num, registrant, enable_extrabed);
	if (!frl.empty())
		return frl;

	// if capacityMatch() and SpecialMatch() both fail, then find the least score room(s)
	// until the sum of capacity of all rooms are >= family size
	// return the list of the rooms
	frl = RoomExtraMatch(rms, num, enable_extrabed);
	if (!frl.empty())
		return frl;

	return frl;
}

int32_t RoomAssign::familyRoomAssign(std::map<int32_t, std::vector<Registrant*>> &family_lst, std::vector<BuildingRoomList::EURoom*> &roomlist, bool enable_extrabeds)
{
	int32_t i, j;
	int32_t cnt = 0;
	if (family_lst.size() == 0 || roomlist.size() == 0)
		return -1;

	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* eur = roomlist[i];
		eur->considered = false;
	}

	std::map < int32_t, std::vector<Registrant*>>::iterator it;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;

	for (it = family_lst.begin(); it != family_lst.end(); ++it) {
		int32_t party_id = it->first;
		std::vector<Registrant*> aregist = it->second;
		int32_t fs = (int32_t)aregist.size();

		temp_roomlist = queryFamilyRoomList(roomlist, fs, aregist[0], enable_extrabeds);
		int32_t left = fs;
		int32_t accumu = 0;
		int32_t start = 0;
		for (i = 0; i < temp_roomlist.size(); i++) {
			BuildingRoomList::EURoom* eu_room = temp_roomlist[i];
			assert(eu_room->room_status != RoomStatus::qInactive);
			assert(eu_room->room_status != RoomStatus::qFullyAssigned);
			assert(eu_room->room_status != RoomStatus::qPartiallyAssigned);

			temp_roomlist[i]->bed_assigned = ((temp_roomlist[i]->capacity) > left ? left : temp_roomlist[i]->capacity);
			if (temp_roomlist[i]->bed_assigned == temp_roomlist[i]->capacity)
				temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned;
			else
				temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned; // qPartiallyAssigned;

			temp_roomlist[i]->extra = ((enable_extrabeds) ? 1 : 0);
			accumu += temp_roomlist[i]->bed_assigned + temp_roomlist[i]->extra;
			left = fs - accumu;
			if (left < 0) {
				left = 0;
				accumu = fs;
			}
			for (j = start; j < accumu; j++) {
				aregist[j]->assigned_room = temp_roomlist[i];
				aregist[j]->room = temp_roomlist[i]->room;
				aregist[j]->building = ((BuildingRoomList::EUBuilding*)temp_roomlist[i]->building)->building_name;
				temp_roomlist[i]->persons.push_back(aregist[j]->person_id);
			}
			cnt++;
			if (accumu == 1 && aregist[0]->party_type.find(PartyType::qIndividual) != std::string::npos) {
				std::string gender0 = aregist[0]->gender;
				if (gender0.compare("Male") == 0)
					temp_roomlist[i]->stype = BuildingRoomList::eMale;
				else if (gender0.compare("Female") == 0)
					temp_roomlist[i]->stype = BuildingRoomList::eFemale;
			}
			else
				temp_roomlist[i]->stype = BuildingRoomList::eMix;
			start = accumu;
			if (left == 0)
				break;
		}
	}

	return cnt;
}

std::vector<BuildingRoomList::EURoom* > RoomAssign::RoomCapacityMatch(std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> &rms, int32_t num, Registrant* registrant, bool enable_extrabed)
{
	int32_t i, j, k;
	int32_t total_capacity = 0;
	std::vector<BuildingRoomList::EURoom* > frl;
	std::vector<BuildingRoomList::EURoom* > frl_has_neighbor;
	std::vector<BuildingRoomList::EURoom* > frl_has_no_neighbor;
	BuildingRoomList::SexType stype;
	std::map < int32_t, std::vector<BuildingRoomList::EURoom*>>::iterator it;

	// match party number to the # of beds per room or per shared rooms 
	// collect all candidates that satisfies the criteria
	for (it = rms.begin(); it != rms.end(); ++it) {
		int32_t score = it->first;
		std::vector<BuildingRoomList::EURoom*> roomlist = it->second;
		for (j = 0; j < roomlist.size(); j++) {
			total_capacity = roomlist[j]->capacity + (enable_extrabed ? 1 : 0);
			if (total_capacity == num) {
				if (registrant->party_type.find(PartyType::qIndividual) != std::string::npos) {
					std::string gender0 = registrant->gender;
					if (gender0.compare("Male") == 0)
						stype = BuildingRoomList::eMale;
					else if (gender0.compare("Female") == 0)
						stype = BuildingRoomList::eFemale;
					if (roomlist[j]->neighbors.size() > 0 && roomlist[j]->neighbors[0]->considered) {
						for (k = 0; k < roomlist[j]->neighbors.size(); k++) {
							BuildingRoomList::SexType st = roomlist[j]->neighbors[k]->stype;
							if (stype == BuildingRoomList::eMale && st == BuildingRoomList::eMale) {
								frl_has_neighbor.push_back(roomlist[j]);
							}
							else if (stype == BuildingRoomList::eFemale && st == BuildingRoomList::eFemale) {
								frl_has_neighbor.push_back(roomlist[j]);
							}
							if (stype == BuildingRoomList::eMale && (st == BuildingRoomList::eFemale || st == BuildingRoomList::eMix))
								continue;
							else if (stype == BuildingRoomList::eFemale && (st == BuildingRoomList::eMale || st == BuildingRoomList::eMix))
								continue;
						}
					}
					else
						frl_has_no_neighbor.push_back(roomlist[j]);
				}
				else {// family type
					if (roomlist[j]->neighbors.size() > 0 && roomlist[j]->neighbors[0]->room_status == RoomStatus::qFullyAssigned) {
						for (k = 0; k < roomlist[j]->neighbors.size(); k++) {
							BuildingRoomList::SexType st = roomlist[j]->neighbors[k]->stype;
							if (st == BuildingRoomList::eMix)
								frl_has_neighbor.push_back(roomlist[j]);
							else
								continue;
						}
					}
					else
						frl_has_no_neighbor.push_back(roomlist[j]);
				}
			}
		}
	}

	// first pick the room that its neighbor has been occupied and they have the same family type
	for (i = 0; i < (int32_t)frl_has_neighbor.size(); i++) {
		frl.push_back(frl_has_neighbor[0]);
		return frl;
	}
	// second pick the room that its neighbor room is not occupied
	for (i = 0; i < (int32_t)frl_has_no_neighbor.size(); i++) {
		frl.push_back(frl_has_no_neighbor[0]);
		return frl;
	}

	// now check if shared rooms are available to hold the number of people greater than a single room can hold
	for (it = rms.begin(); it != rms.end(); ++it) {
		int32_t score = it->first;
		std::vector<BuildingRoomList::EURoom*> roomlist = it->second;
		for (int32_t j = 0; j < roomlist.size(); j++) {
			total_capacity = roomlist[j]->capacity + (enable_extrabed ? 1 : 0);
			if (total_capacity < num) {
				if (roomlist.size() > 1 && (roomlist[j]->neighbors.size() > 0) && (!(roomlist[j]->neighbors[0]->room_status != RoomStatus::qFullyAssigned))) {
					// at most two rooms have shared bathrooms
					int32_t total_beds = roomlist[j]->capacity + roomlist[j]->neighbors[0]->capacity + (enable_extrabed ? 1 : 0);
					if (total_beds == num) {
						frl.push_back(roomlist[j]); // return the first room
						frl.push_back(roomlist[j]->neighbors[0]); // return the second room
						return frl;
					}
				}
			}
		}
	}

	frl.clear();
	return frl;
}

std::vector<BuildingRoomList::EURoom* > RoomAssign::RoomSpecialMatch(std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> &rms, int32_t num, Registrant* registrant, bool enable_extrabed)
{
	int32_t i, j, k;
	int32_t total_capacity = 0;
	std::vector<BuildingRoomList::EURoom* > frl;
	std::vector<BuildingRoomList::EURoom* > frl_has_neighbor;
	std::vector<BuildingRoomList::EURoom* > frl_has_no_neighbor;
	BuildingRoomList::SexType stype;
	std::map < int32_t, std::vector<BuildingRoomList::EURoom*>>::iterator it;

	// case 1: capacity of room <= the number of family size
	for (it = rms.begin(); it != rms.end(); ++it) {
		int32_t score = it->first;
		std::vector<BuildingRoomList::EURoom*> roomlist = it->second;

		// return 2 rooms that share the bathroom
		for (j = 0; j < roomlist.size(); j++) {
			total_capacity = roomlist[j]->capacity + (enable_extrabed ? 1 : 0);
			if (total_capacity < num) {
				if (roomlist.size() > 1 && (!(roomlist[j]->neighbors[0]->room_status != RoomStatus::qFullyAssigned))) {
					// at most two rooms have shared bathrooms
					total_capacity = roomlist[j]->capacity + roomlist[j]->neighbors[0]->capacity + (enable_extrabed ? 1 : 0);
					if (total_capacity == num || total_capacity == num + 1) {
						frl.push_back(roomlist[j]); // return the first room
						frl.push_back(roomlist[j]->neighbors[0]); // return the first room
						return frl;
					}
				}
			} // total_capacity
		} // j
	} // it

	  // case 1: capacity of room >= the number of family size
	for (i = 0; i < 3; i++) {
		for (it = rms.begin(); it != rms.end(); ++it) {
			int32_t score = it->first;
			std::vector<BuildingRoomList::EURoom*> roomlist = it->second;

			for (j = 0; j < roomlist.size(); j++) {
				total_capacity = roomlist[j]->capacity + (enable_extrabed ? 1 : 0);
				if (total_capacity >= num && total_capacity - i == num) {
					if (registrant->party_type.find(PartyType::qIndividual) != std::string::npos) {
						std::string gender0 = registrant->gender;
						if (gender0.compare("Male") == 0)
							stype = BuildingRoomList::eMale;
						else if (gender0.compare("Female") == 0)
							stype = BuildingRoomList::eFemale;
						if (roomlist[j]->neighbors.size() > 0) {
							for (k = 0; k < roomlist[j]->neighbors.size(); k++) {
								BuildingRoomList::SexType st = roomlist[j]->neighbors[k]->stype;
								if (stype == BuildingRoomList::eMale && st == BuildingRoomList::eMale) {
									frl_has_neighbor.push_back(roomlist[j]);
								}
								else if (stype == BuildingRoomList::eFemale && st == BuildingRoomList::eFemale) {
									frl_has_neighbor.push_back(roomlist[j]);
								}
								if (stype == BuildingRoomList::eMale && (st == BuildingRoomList::eFemale || st == BuildingRoomList::eMix))
									continue;
								else if (stype == BuildingRoomList::eFemale && (st == BuildingRoomList::eMale || st == BuildingRoomList::eMix))
									continue;
							}
						}
						else
							frl_has_no_neighbor.push_back(roomlist[j]);
					}
					else {// family type
						if (roomlist[j]->neighbors.size() > 0 && roomlist[j]->neighbors[0]->room_status == RoomStatus::qFullyAssigned) {
							for (k = 0; k < roomlist[j]->neighbors.size(); k++) {
								BuildingRoomList::SexType st = roomlist[j]->neighbors[k]->stype;
								if (st == BuildingRoomList::eMix)
									frl_has_neighbor.push_back(roomlist[j]);
								else
									continue;
							}
						}
						else
							frl_has_no_neighbor.push_back(roomlist[j]);
					}
				}
			}// for room
		}// for it

		 // first pick the room that its neighbor has been occupied and they have the same family type
		for (k = 0; k < (int32_t)frl_has_neighbor.size(); k++) {
			frl.push_back(frl_has_neighbor[0]);
			return frl;
		}
		// second pick the room that its neighbor room is not occupied
		for (k = 0; k < (int32_t)frl_has_no_neighbor.size(); k++) {
			frl.push_back(frl_has_no_neighbor[0]);
			return frl;
		}
	}// for i

	frl.clear();
	return frl;
}

std::vector<BuildingRoomList::EURoom* > RoomAssign::RoomExtraMatch(std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> &rms, int32_t num, bool enable_extrabed)
{
	int32_t total_capacity = 0;
	std::vector<BuildingRoomList::EURoom* > frl;
	std::map < int32_t, std::vector<BuildingRoomList::EURoom*>>::iterator it;
	
	frl.clear();
	for (it = rms.begin(); it != rms.end(); ++it) {
		int32_t score = it->first;
		std::vector<BuildingRoomList::EURoom*> roomlist = it->second;
		for (int32_t j = 0; j < (int32_t)roomlist.size(); j++) {
			total_capacity += roomlist[j]->capacity + (enable_extrabed ? 1 : 0);
			if (total_capacity < num) {
				frl.push_back(roomlist[j]);
			}
			else {
				frl.push_back(roomlist[j]);
				return frl;
			}
		}
	}
	return frl;
}

int32_t RoomAssign::IndividualRoomAssign2(std::map<int32_t, std::vector<Registrant*>> &individual_lst, BuildingRoomList::SexType stype, std::vector<BuildingRoomList::EURoom*> &roomlist)
{
	int32_t i, j;
	int32_t cnt = 0;
	BuildingRoomList::SexType excludeType;
	int32_t score = 0;
	std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> valid_roomlist;

	if (individual_lst.size() == 0 || roomlist.size() == 0)
		return -1;

	std::map < int32_t, std::vector<Registrant*>>::iterator it;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;

	if (stype == BuildingRoomList::eMale)
		excludeType = BuildingRoomList::eFemale;
	else if (stype == BuildingRoomList::eFemale)
		excludeType = BuildingRoomList::eMale;

	// filter rooms to have the same genders in the shared rooms
	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* eur = roomlist[i];
		if (eur->room_status != RoomStatus::qFullyAssigned) {
			if (eur->neighbors.size() > 0) {
				if (eur->neighbors[0]->stype != excludeType && eur->neighbors[0]->stype != BuildingRoomList::eMix) {
					eur->considered = false;
					valid_roomlist[score].push_back(eur);
				}
			}
			else {
				eur->considered = false;
				valid_roomlist[score].push_back(eur);
			}
		}
	}

	// find a room for the list of individuals
	int32_t total_indiv_num = 0;
	bool enable_extrabed = false;
	std::vector<Registrant*> registrants;
	for (it = individual_lst.begin(); it != individual_lst.end(); ++it) {
		int32_t party_id = it->first;
		std::vector<Registrant*> aregist = it->second;
		int32_t fs = (int32_t)aregist.size();
		total_indiv_num += fs;
		for (i = 0; i < fs; i++)
			registrants.push_back(aregist[i]);
	}
	std::vector<BuildingRoomList::EURoom* > frl = RoomCapacityMatch(valid_roomlist, total_indiv_num, registrants[0], enable_extrabed);
	if (frl.size() > 0) {
		int32_t k = 0;
		for (i = 0; i < (int32_t)frl.size(); i++) {
			for (j = 0; j < frl[i]->capacity; j++) {
				frl[i]->bed_assigned++;
				frl[i]->stype = stype;
				registrants[k]->assigned_room = frl[i];
				registrants[k]->room = frl[i]->room;
				registrants[k]->building = ((BuildingRoomList::EUBuilding*)frl[i]->building)->building_name;
				frl[i]->persons.push_back(registrants[k]->person_id);
				k++;
			}
		}
		for (i = 0; i < (int32_t)frl.size(); i++) {
			if (frl[i]->bed_assigned == frl[i]->capacity)
				frl[i]->room_status = RoomStatus::qFullyAssigned;
			else
				frl[i]->room_status = RoomStatus::qPartiallyAssigned;
		}
		return (int32_t)frl.size();
	}

	// do not mix female and male in room assignment - do not share bathrooms bewteen male room and female room 
	for (it = individual_lst.begin(); it != individual_lst.end(); ++it) {
		int32_t party_id = it->first;
		std::vector<Registrant*> aregist = it->second;
		int32_t fs = (int32_t)aregist.size();

		temp_roomlist = queryRoomList(valid_roomlist[score], fs);
		if (temp_roomlist.size() == 0)
			return -2;

		int32_t left = fs;
		int32_t accumu = 0;
		int32_t start = 0;
		for (i = 0; i < temp_roomlist.size(); i++) {
			BuildingRoomList::EURoom* eu_room = temp_roomlist[i];
			assert(eu_room->room_status != RoomStatus::qInactive);
			assert(eu_room->room_status != RoomStatus::qFullyAssigned);
			if (eu_room->stype == excludeType || eu_room->stype == BuildingRoomList::eMix) {
				eu_room->considered = true;
				continue;
			}
			std::vector<BuildingRoomList::EURoom*> eu_next_room = eu_room->neighbors;
			if (eu_next_room.size() > 0) {
				if (eu_next_room[0]->stype == excludeType || eu_next_room[0]->stype == BuildingRoomList::eMix) {
					eu_room->considered = true;
					continue;
				}
			}

			int32_t taken = min((temp_roomlist[i]->capacity), (temp_roomlist[i]->bed_assigned + left));
			if (temp_roomlist[i]->bed_assigned + left > temp_roomlist[i]->capacity) {
				left = temp_roomlist[i]->bed_assigned + left - temp_roomlist[i]->capacity;
				accumu += temp_roomlist[i]->capacity - temp_roomlist[i]->bed_assigned;
				temp_roomlist[i]->bed_assigned = temp_roomlist[i]->capacity;
			}
			else {
				accumu += left;
				temp_roomlist[i]->bed_assigned = temp_roomlist[i]->bed_assigned + left;
				left = 0;
			}

			//temp_roomlist[i]->bed_assigned = min((temp_roomlist[i]->capacity), (temp_roomlist[i]->bed_assigned + left)); 
			if (temp_roomlist[i]->bed_assigned == temp_roomlist[i]->capacity)
				temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned;
			else
				temp_roomlist[i]->room_status = RoomStatus::qPartiallyAssigned;

			for (j = start; j < fs; j++) {
				aregist[j]->assigned_room = temp_roomlist[i];
				aregist[j]->room = temp_roomlist[i]->room;
				aregist[j]->building = ((BuildingRoomList::EUBuilding*)temp_roomlist[i]->building)->building_name;
				temp_roomlist[i]->persons.push_back(aregist[j]->person_id);
			}
			cnt++;
			temp_roomlist[i]->stype = stype;
			start = accumu;
			if (left == 0)
				break;
		}
	}

	return cnt;

}

int32_t RoomAssign::IndividualRoomAssign(std::map<int32_t, std::vector<Registrant*>> &individual_lst, BuildingRoomList::SexType stype, std::vector<BuildingRoomList::EURoom*> &roomlist)
{
	int32_t i, j;
	int32_t cnt = 0;
	if (individual_lst.size() == 0 || roomlist.size() == 0)
		return -1;
	BuildingRoomList::SexType excludeType;

	std::map < int32_t, std::vector<Registrant*>>::iterator it;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;

	if (stype == BuildingRoomList::eMale)
		excludeType = BuildingRoomList::eFemale;
	else if (stype == BuildingRoomList::eFemale)
		excludeType = BuildingRoomList::eMale;

	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* eur = roomlist[i];
		eur->considered = false;
	}

	// do not mix female and male in room assignment - do not share bathrooms bewteen male room and female room 
	for (it = individual_lst.begin(); it != individual_lst.end(); ++it) {
		int32_t party_id = it->first;
		std::vector<Registrant*> aregist = it->second;
		int32_t fs = (int32_t)aregist.size();

		temp_roomlist = queryRoomList(roomlist, fs);
		if (temp_roomlist.size() == 0)
			return -2;

		int32_t left = fs;
		int32_t accumu = 0;
		int32_t start = 0;
		for (i = 0; i < temp_roomlist.size(); i++) {
			BuildingRoomList::EURoom* eu_room = temp_roomlist[i];
			assert(eu_room->room_status != RoomStatus::qInactive);
			assert(eu_room->room_status != RoomStatus::qFullyAssigned);
			if (eu_room->stype == excludeType || eu_room->stype == BuildingRoomList::eMix) {
				eu_room->considered = true;
				continue;
			}
			std::vector<BuildingRoomList::EURoom*> eu_next_room = eu_room->neighbors;
			if (eu_next_room.size() > 0) {
				if (eu_next_room[0]->stype == excludeType || eu_next_room[0]->stype == BuildingRoomList::eMix) {
					eu_room->considered = true;
					continue;
				}
			}

			int32_t taken = min((temp_roomlist[i]->capacity), (temp_roomlist[i]->bed_assigned + left));
			if (temp_roomlist[i]->bed_assigned + left > temp_roomlist[i]->capacity) {
				left = temp_roomlist[i]->bed_assigned + left - temp_roomlist[i]->capacity;
				accumu += temp_roomlist[i]->capacity - temp_roomlist[i]->bed_assigned;
				temp_roomlist[i]->bed_assigned = temp_roomlist[i]->capacity;
			}
			else {
				accumu += left;
				temp_roomlist[i]->bed_assigned = temp_roomlist[i]->bed_assigned + left;
				left = 0;
			}

			//temp_roomlist[i]->bed_assigned = min((temp_roomlist[i]->capacity), (temp_roomlist[i]->bed_assigned + left)); 
			if (temp_roomlist[i]->bed_assigned == temp_roomlist[i]->capacity)
				temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned;
			else
				temp_roomlist[i]->room_status = RoomStatus::qPartiallyAssigned;

			for (j = start; j < fs; j++) {
				aregist[j]->assigned_room = temp_roomlist[i];
				aregist[j]->room = temp_roomlist[i]->room;
				aregist[j]->building = ((BuildingRoomList::EUBuilding*)temp_roomlist[i]->building)->building_name;
				temp_roomlist[i]->persons.push_back(aregist[j]->person_id);
			}
			cnt++;
			temp_roomlist[i]->stype = stype;
			start = accumu;
			if (left == 0)
				break;
		}
	}

	return cnt;
}

int32_t RoomAssign::familyMaleRoomAssign(std::vector<Registrant*> &family_member_lst, std::vector<BuildingRoomList::EURoom*> &roomlist, bool enable_extrabeds)
{
	int32_t i, j;
	int32_t cnt = 0;
	if (family_member_lst.size() == 0 || roomlist.size() == 0)
		return -1;

	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* eur = roomlist[i];
		eur->considered = false;
	}

	std::vector<Registrant*>::iterator it;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	int32_t fs = (int32_t)family_member_lst.size();

	temp_roomlist = queryFamilyRoomList(roomlist, fs, family_member_lst[0], enable_extrabeds);
	int32_t left = fs;
	int32_t accumu = 0;
	int32_t start = 0;
	for (i = 0; i < temp_roomlist.size(); i++) {
		BuildingRoomList::EURoom* eu_room = temp_roomlist[i];
		assert(eu_room->room_status != RoomStatus::qInactive);
		assert(eu_room->room_status != RoomStatus::qFullyAssigned);
		assert(eu_room->room_status != RoomStatus::qPartiallyAssigned);

		temp_roomlist[i]->bed_assigned = ((temp_roomlist[i]->capacity) > left ? left : temp_roomlist[i]->capacity);
		if (temp_roomlist[i]->bed_assigned == temp_roomlist[i]->capacity)
			temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned;
		else
			temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned; // qPartiallyAssigned;

		temp_roomlist[i]->extra = ((enable_extrabeds) ? 1 : 0);
		accumu += temp_roomlist[i]->bed_assigned + temp_roomlist[i]->extra;
		left = fs - accumu;
		if (left < 0) {
			left = 0;
			accumu = fs;
		}
		for (j = start; j < accumu; j++) {
			family_member_lst[j]->assigned_room = temp_roomlist[i];
			family_member_lst[j]->room = temp_roomlist[i]->room;
			family_member_lst[j]->building = ((BuildingRoomList::EUBuilding*)temp_roomlist[i]->building)->building_name;
			temp_roomlist[i]->persons.push_back(family_member_lst[j]->person_id);
		}
		cnt++;
		if (accumu == 1 && family_member_lst[0]->party_type.find(PartyType::qIndividual) != std::string::npos) {
			std::string gender0 = family_member_lst[0]->gender;
			if (gender0.compare("Male") == 0)
				temp_roomlist[i]->stype = BuildingRoomList::eMale;
			else if (gender0.compare("Female") == 0)
				temp_roomlist[i]->stype = BuildingRoomList::eFemale;
		}
		else
			temp_roomlist[i]->stype = BuildingRoomList::eMix;
		start = accumu;
		if (left == 0)
			break;
	}

	return cnt;
}

int32_t RoomAssign::familyFemaleRoomAssign(std::vector<Registrant*> &family_member_lst, std::vector<BuildingRoomList::EURoom*> &roomlist, bool enable_extrabeds)
{
	int32_t i, j;
	int32_t cnt = 0;
	if (family_member_lst.size() == 0 || roomlist.size() == 0)
		return -1;

	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* eur = roomlist[i];
		eur->considered = false;
	}

	std::vector<Registrant*>::iterator it;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	int32_t fs = (int32_t)family_member_lst.size();

	temp_roomlist = queryFamilyRoomList(roomlist, fs, family_member_lst[0], enable_extrabeds);
	int32_t left = fs;
	int32_t accumu = 0;
	int32_t start = 0;
	for (i = 0; i < temp_roomlist.size(); i++) {
		BuildingRoomList::EURoom* eu_room = temp_roomlist[i];
		assert(eu_room->room_status != RoomStatus::qInactive);
		assert(eu_room->room_status != RoomStatus::qFullyAssigned);
		assert(eu_room->room_status != RoomStatus::qPartiallyAssigned);

		temp_roomlist[i]->bed_assigned = ((temp_roomlist[i]->capacity) > left ? left : temp_roomlist[i]->capacity);
		if (temp_roomlist[i]->bed_assigned == temp_roomlist[i]->capacity)
			temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned;
		else
			temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned; // qPartiallyAssigned;

		temp_roomlist[i]->extra = ((enable_extrabeds) ? 1 : 0);
		accumu += temp_roomlist[i]->bed_assigned + temp_roomlist[i]->extra;
		left = fs - accumu;
		if (left < 0) {
			left = 0;
			accumu = fs;
		}
		for (j = start; j < accumu; j++) {
			family_member_lst[j]->assigned_room = temp_roomlist[i];
			family_member_lst[j]->room = temp_roomlist[i]->room;
			family_member_lst[j]->building = ((BuildingRoomList::EUBuilding*)temp_roomlist[i]->building)->building_name;
			temp_roomlist[i]->persons.push_back(family_member_lst[j]->person_id);
		}
		cnt++;
		if (accumu == 1 && family_member_lst[0]->party_type.find(PartyType::qIndividual) != std::string::npos) {
			std::string gender0 = family_member_lst[0]->gender;
			if (gender0.compare("Male") == 0)
				temp_roomlist[i]->stype = BuildingRoomList::eMale;
			else if (gender0.compare("Female") == 0)
				temp_roomlist[i]->stype = BuildingRoomList::eFemale;
		}
		else
			temp_roomlist[i]->stype = BuildingRoomList::eMix;
		start = accumu;
		if (left == 0)
			break;
	}

	return cnt;
}

/* The order of the report fields are:
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

14: reg fee
15: key fee
16: party fee
17: status
18: special_need
19: friends

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
bool RoomAssign::printRoomAssignment(const char* filename)
{
	int32_t i, j;
	int32_t cnt_family = 0, cnt_rooms = 0;
	bool enable_extrabeds = true;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist, male_roomlist, female_roomlist;
	std::map<int32_t, std::vector<Registrant*>> all_family_list = m_family_info;
	std::vector<ChurchList::QFLChurch> *churches = getChurchHandle()->getChurchList();

	if (filename == NULL)
		return false;

	std::ofstream fout(filename);
	if (!fout.is_open())
		return false;

	int32_t cnt = 0;
	fout << "Church," << "PersonId," << "PartyId," << "ContactPerson," << "PartyType," << "FirstName,"
		<< "LastName," << "ChineseName," << "RoomNumber," <<"extra_bed," << "CellGroup," << "NeedRoom," << "Age,"
		<< "Gender," << "Grade," << "NeedRide," << "OfferRide," << "is_Christian," 
		<< "MobilePhone," << "Email," << "City," << "State," << "Zip," << "FunctionalGroup," << "Services," 
		<< "notes," << "needs" << std::endl;

	char zip_str[128];
	int32_t total_parties = 0;
	int32_t total_attendees = 0;
	std::map<int32_t, std::vector<Registrant*>>::iterator it;
	std::map<int32_t, std::vector<Registrant*>> *glists[] = 
	{ &m_child_leader_list, &m_choir_list, &m_drama_list, &m_recording_list, &m_speaker_list};
	for (j = 0; j < sizeof(glists)/sizeof(glists[0]); j++) {
		for (it = glists[j]->begin(); it != glists[j]->end(); it++) {
			int32_t party_id = it->first;
			std::vector<Registrant*> attendees = it->second;
			total_parties++;
			for (i = 0; i < attendees.size(); i++) {
				total_attendees++;
				Registrant *attendee = attendees[i];
				int32_t person_id = attendee->person_id;
				int32_t party_id = attendee->party;
				std::string church = attendee->church;
				std::string church_chinese = attendee->church_cname;
				std::string contact_person = attendee->contact_person;
				size_t j0 = contact_person.find_first_of(",");
				if (j0 != std::string::npos)
					contact_person[j0] = '\0';
				std::string party_type = attendee->party_type;
				std::string first_name = attendee->first_name;
				std::string last_name = attendee->last_name;
				std::string chinese_name = attendee->chinese_name;
				std::string room_number = attendee->room;
				int32_t extra_bed = 0;
				if(attendee->assigned_room != NULL)
					extra_bed = ((BuildingRoomList::EURoom*)(attendee->assigned_room))->extra;
				std::string cell_group = attendee->cell_group;
				bool need_room = attendee->need_room;
				std::string age = attendee->age_group;
				std::string gender = attendee->gender;
				std::string grade = attendee->grade;
				size_t j1 = grade.find_first_of(",");
				if (j1 != std::string::npos)
					grade[j1] = '-';
				std::string need_ride = attendee->need_ride;
				std::string offer_ride = attendee->offer_ride;
				bool is_christian = attendee->is_christian;
				std::string occupation = attendee->occupation;
				std::string mobile = attendee->mobile_phone;
				std::string email = attendee->email;
				std::string city = attendee->city;
				size_t j2 = city.find_first_of(",");
				if(j2!= std::string::npos)
					city[j2] = '\0';
				std::string state = attendee->state;
				int32_t zip = attendee->zip;
				if (zip < 9999)
					sprintf_s(zip_str, "%05d", zip);
				else
					sprintf_s(zip_str, "%d", zip);
				std::string functional = attendee->functional_group;
				std::string services = attendee->services;
				std::string admin_notes = attendee->notes;
				std::string needs = attendee->special_need;

				fout << church_chinese << ", " << person_id << ", " << party_id << ", " << contact_person << ", " << party_type << ", "
					<< first_name << ", " << last_name << ", " << chinese_name + " " << ", " << room_number << ", " << extra_bed << ", " << cell_group + " " << ", " << need_room << ", "
					<< age << ", " << gender << ", " << grade + " " << ", " << need_ride << ", " << offer_ride << ", " << is_christian << ", "
					<< mobile << ", " << email << ", " << city << ", " << state << ", " << zip_str << ", "
					<< functional << ", " << services << ", " << admin_notes << ", " << needs << std::endl;
			}
		}
	}

	for (i = 0; i < churches->size(); i++) {
		ChurchList::QFLChurch achurch = (*churches)[i];
		std::string name = achurch.church_name;
		Church thechurch;
		thechurch = m_attendee_list_byChurch[name];

		std::vector<Registrant*>::iterator it;
		for (it = thechurch.persons.begin(); it != thechurch.persons.end(); it++) {
			Registrant *attendee = *it;
			int32_t person_id = attendee->person_id;
			int32_t party_id = attendee->party;
			std::string church = attendee->church;
			std::string church_chinese = attendee->church_cname;
			std::string contact_person = attendee->contact_person;
			size_t j0 = contact_person.find_first_of(",");
			if (j0 != std::string::npos)
				contact_person[j0] = '\0';
			std::string party_type = attendee->party_type;
			std::string first_name = attendee->first_name;
			std::string last_name = attendee->last_name;
			std::string chinese_name = attendee->chinese_name;
			std::string room_number = attendee->room;
			int32_t extra_bed = 0;
			if (attendee->assigned_room != NULL)
				extra_bed = ((BuildingRoomList::EURoom*)(attendee->assigned_room))->extra;
			std::string cell_group = attendee->cell_group;
			bool need_room = attendee->need_room;
			std::string age = attendee->age_group;
			std::string gender = attendee->gender;
			std::string grade = attendee->grade;
			size_t j1 = grade.find_first_of(",");
			if (j1 != std::string::npos)
				grade[j1] = '-';
			std::string need_ride = attendee->need_ride;
			std::string offer_ride = attendee->offer_ride;
			bool is_christian = attendee->is_christian;
			std::string occupation = attendee->occupation;
			std::string mobile = attendee->mobile_phone;
			std::string email = attendee->email;
			std::string city = attendee->city;
			std::string state = attendee->state;
			int32_t zip = attendee->zip;
			if (zip < 9999)
				sprintf_s(zip_str, "%05d", zip);
			else
				sprintf_s(zip_str, "%d", zip);
			std::string functional = attendee->functional_group;
			std::string services = attendee->services;
			std::string admin_notes = attendee->notes;
			std::string needs = attendee->special_need;

			fout << church_chinese <<", " << person_id << ", " << party_id << ", " << contact_person << ", " << party_type << ", "
				<< first_name << ", " << last_name << ", " << chinese_name + " " << ", " << room_number << ", " << extra_bed << ", " << cell_group + " " << ", " << need_room << ", "
				<< age << ", " << gender << ", " << grade + " " << ", " << need_ride << ", " << offer_ride << ", " << is_christian << ", "
				<< mobile << ", " << email << ", " << city << ", " << state << ", " << zip_str << ", "
				<< functional << ", " << services << ", " << admin_notes << ", " << needs << std::endl;
		}
	}

	return true;
}


int32_t RoomAssign::roomAllocationStats() 
{
	std::vector<BuildingRoomList::EURoom*> available_roomlist = m_br_list.queryAvailableRooms();
	std::vector<BuildingRoomList::EURoom*> general_reserve_roomlist = m_br_list.queryReservedGERooms();
	std::vector<BuildingRoomList::EURoom*> choir_roomlist = m_br_list.queryReservedCHRooms();
	std::vector<BuildingRoomList::EURoom*> special_purpose_roomlist = m_br_list.queryReservedSPRooms();
	std::vector<BuildingRoomList::EURoom*> speaker_recording_roomlist = m_br_list.queryReservedSKRooms();
	std::vector<BuildingRoomList::EURoom*> childcare_roomlist = m_br_list.queryReservedCCRooms();

	std::vector<BuildingRoomList::EURoom*> family_private_roomlist = m_br_list.queryFamilyPrivateRooms();
	std::vector<BuildingRoomList::EURoom*> family_male_roomlist = m_br_list.queryFamilyMaleRooms();
	std::vector<BuildingRoomList::EURoom*> family_female_roomlist = m_br_list.queryFamilyFemaleRooms();
	std::vector<BuildingRoomList::EURoom*> male_roomlist = m_br_list.queryMaleRooms();
	std::vector<BuildingRoomList::EURoom*> female_roomlist = m_br_list.queryFemaleRooms();

	m_alloc_rooms.total_rooms = m_br_list.getTotalActiveRooms();
	m_alloc_rooms.available_rooms = (int32_t)available_roomlist.size();
	m_alloc_rooms.rooms_childcare_workers = (int32_t)childcare_roomlist.size();
	m_alloc_rooms.rooms_choir = (int32_t)choir_roomlist.size();
	m_alloc_rooms.rooms_special_needs = (int32_t)special_purpose_roomlist.size();
	m_alloc_rooms.rooms_speakers_recordings = (int32_t)speaker_recording_roomlist.size();
	m_alloc_rooms.general_reserves = (int32_t)general_reserve_roomlist.size();
	m_alloc_rooms.rooms_family_private = (int32_t)family_private_roomlist.size();
	m_alloc_rooms.rooms_family_male = (int32_t)family_male_roomlist.size();
	m_alloc_rooms.rooms_family_female = (int32_t)family_female_roomlist.size();
	m_alloc_rooms.rooms_males = (int32_t)male_roomlist.size();
	m_alloc_rooms.rooms_females = (int32_t)female_roomlist.size();

	printf("=== Room Statistics:\n");
	printf("total active rooms=%d\n", m_alloc_rooms.total_rooms);
	printf("available = %d, childcare_rooms = %d, choir_rooms = %d, specialneeds_rooms = %d, speaker_rooms = %d, reserved_rooms = %d\n",
		m_alloc_rooms.available_rooms, m_alloc_rooms.rooms_childcare_workers, m_alloc_rooms.rooms_choir, m_alloc_rooms.rooms_special_needs,
		m_alloc_rooms.rooms_speakers_recordings, m_alloc_rooms.general_reserves);
	printf("family private rooms=%d, family male rooms=%d, family female rooms=%d, males only rooms=%d, females only rooms=%d\n", 
		m_alloc_rooms.rooms_family_private, m_alloc_rooms.rooms_family_male, m_alloc_rooms.rooms_family_female, 
		m_alloc_rooms.rooms_males, m_alloc_rooms.rooms_females);

	return 0;
}

int32_t RoomAssign::lodgePeopleStats(const char* filename)
{
	m_eu_lodge_people.single_males = (int32_t)m_male_list.size();// party id
	m_eu_lodge_people.single_females = (int32_t)m_female_list.size();// party id
	m_eu_lodge_people.person_childcare_workers = (int32_t)m_child_leader_list.size();// party id
	m_eu_lodge_people.family_choir = (int32_t)m_choir_list.size();// party id
	m_eu_lodge_people.family_drama = (int32_t)m_drama_list.size(); //party id
	m_eu_lodge_people.family_speakers_recordings = (int32_t)m_recording_list.size() + (int32_t)m_speaker_list.size();// party id

	m_eu_lodge_people.family_seniors = (int32_t)m_senior_list.size();// party id
	m_eu_lodge_people.family_babies = (int32_t)m_baby_list.size();// party id
	m_eu_lodge_people.special_needs = (int32_t)m_special_need_list.size();// party id
	m_eu_lodge_people.families = (int32_t)m_family_info.size();// party id

	std::map<int32_t, std::vector<Registrant*>> all_lists[] = { m_child_leader_list, m_choir_list, m_drama_list, 
		m_speaker_list, m_recording_list, m_senior_list, m_baby_list, m_special_need_list, m_family_info, m_male_list, m_female_list };
	int total_attendees = 0;
	for (int i = 0; i < sizeof(all_lists)/sizeof(all_lists[0]); i++) {
		total_attendees += printSortedAttendees(filename, all_lists[i], (i==0));
	}

	printf("=== Attendee Statistics:\n");
	printf("Total attendees are %d\n", total_attendees);
	printf("Total families=%d, family seniors=%d, family babies=%d, special needs=%d, speakers+recordings=%d, choir=%d, drama=%d, chilecare workders=%d\n",
		m_eu_lodge_people.families, m_eu_lodge_people.family_seniors, m_eu_lodge_people.family_babies, m_eu_lodge_people.special_needs,
		m_eu_lodge_people.family_speakers_recordings, m_eu_lodge_people.family_choir, m_eu_lodge_people.family_drama, m_eu_lodge_people.person_childcare_workers);
	printf("Males only=%d, females only=%d\n", m_eu_lodge_people.single_males, m_eu_lodge_people.single_females);

	return 0;
}

bool RoomAssign::extrabed_update()
{
	int32_t i;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryAssignedRooms();
	int32_t extra_bed_rooms = 0;

	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* aroom = roomlist[i];
		int32_t num = aroom->persons.size();
		int32_t assigned_beds = aroom->bed_assigned;

		if (num > assigned_beds) {
			aroom->extra = 1;
			extra_bed_rooms++;
		}
		else
			aroom->extra = 0;
	}

	printf("Out of %d assigned rooms, %d rooms need an extra bed\n", roomlist.size(), extra_bed_rooms);
	return true;
}


int32_t RoomAssign::printRooms2Choir()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> choir_list = m_choir_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedCHRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator cit;
	for (cit = choir_list.begin(); cit != choir_list.end(); cit++) {
		int32_t party_id = cit->first;
		std::vector<Registrant*> flist = cit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size();
	printf("=== Choir room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd Choir families/individuals, %d families/individuals are assigned with a room, and %d families are not assigned with a room\n", choir_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::printRooms2DramaTeam()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> drama_list = m_drama_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedCHRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator dit;
	for (dit = drama_list.begin(); dit != drama_list.end(); dit++) {
		int32_t party_id = dit->first;
		std::vector<Registrant*> flist = dit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size();
	printf("=== Drama room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd Drama families/individuals, %d families/individuals are assigned with a room, and %d families are not assigned with a room\n", drama_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::printRooms2Speakers()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> speaker_list = m_speaker_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSKRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator cit;
	for (cit = speaker_list.begin(); cit != speaker_list.end(); cit++) {
		int32_t party_id = cit->first;
		std::vector<Registrant*> flist = cit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size();
	printf("=== Speaker room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd Speaker families, %d families are assigned with a room, and %d families are not assigned with a room\n", speaker_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::printRooms2Recordings()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> recording_list = m_recording_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSKRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator cit;
	for (cit = recording_list.begin(); cit != recording_list.end(); cit++) {
		int32_t party_id = cit->first;
		std::vector<Registrant*> flist = cit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size();
	printf("=== Recording room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd Recording families, %d families are assigned with a room, and %d families are not assigned with a room\n", recording_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::printRooms2ChildcareWorkers()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> childlead_list = m_child_leader_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedCCRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator cit;
	for (cit = childlead_list.begin(); cit != childlead_list.end(); cit++) {
		int32_t party_id = cit->first;
		std::vector<Registrant*> flist = cit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size();
	printf("=== Childcare Leader room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd ChildLead families, %d families are assigned with a room, and %d families are not assigned with a room\n", childlead_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;

}


int32_t RoomAssign::printRooms2Babies()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> baby_list = m_baby_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSPRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator bit;
	for (bit = baby_list.begin(); bit != baby_list.end(); bit++) {
		int32_t party_id = bit->first;
		std::vector<Registrant*> flist = bit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size();
	printf("=== Baby family room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	//printf(" Out of %zd rooms, %d are assigned, %d are not assigned\n", roomlist.size(), assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd baby families, %d families are assigned with a room, and %d families are not assigned with a room\n", baby_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::printRooms2SpecialNeeds()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> spn_list = m_special_need_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSPRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator sit;
	for (sit = spn_list.begin(); sit != spn_list.end(); sit++) {
		int32_t party_id = sit->first;
		std::vector<Registrant*> flist = sit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size();
	printf("=== Special Need people room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	//printf(" Out of %zd rooms, %d are assigned, %d are not assigned\n", roomlist.size(), assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd special need families, %d families are assigned with a room, and %d families are not assigned with a room\n", spn_list.size(), assigned_family_cnt, unassigned_family_cnt);
	return 0;
}

int32_t RoomAssign::printRooms2Seniors()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string , int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> senior_list = m_senior_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSPRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator sit;
	for (sit = senior_list.begin(); sit != senior_list.end(); sit++) {
		int32_t party_id = sit->first;
		std::vector<Registrant*> flist = sit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size();
	printf("=== Senior people room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	//printf(" Out of %zd rooms, %d are assigned, %d are not assigned\n", roomlist.size(), assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd senior families, %d families are assigned with a room, and %d families are not assigned with a room\n", senior_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::printRooms2Families()
{
	int32_t i, j;
	int32_t available_rooms = 0;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;
	int32_t family_list_size = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist[3] = { m_br_list.queryFamilyPrivateRooms(),
		m_br_list.queryFamilyMaleRooms(), m_br_list.queryFamilyFemaleRooms() };

	std::map<int32_t, std::vector<Registrant*>> *glists2[] =
	{ &m_family_info , &m_choir_list, &m_recording_list, &m_senior_list,
		&m_baby_list, &m_speaker_list, &m_special_need_list, &m_drama_list };

	std::map<int32_t, std::vector<Registrant*>>::iterator sit;
	for (j = 0; j < sizeof(glists2) / sizeof(glists2[0]); j++) {
		family_list_size += glists2[j]->size();
		for (sit = glists2[j]->begin(); sit != glists2[j]->end(); sit++) {
			int32_t party_id = sit->first;
			std::vector<Registrant*> flist = sit->second;
			bool room_assigned = true;
			for (i = 0; i < flist.size(); i++) {
				room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
			}

			if (room_assigned) {
				for (i = 0; i < flist.size(); i++) {
					assigned_roomlist[flist[i]->assigned_room->room] = 1;
				}
				assigned_family_cnt++;
			}
			else
				unassigned_family_cnt++;
		}
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	available_rooms = (int32_t)roomlist[0].size()+ (int32_t)roomlist[1].size()+ (int32_t)roomlist[2].size();
	printf("=== Family room assignment:\n");
	printf("Total assigned family (private, female, male) rooms = %d, Available family (private, female, male) rooms = %d\n", assigned_room_cnt, available_rooms - assigned_room_cnt);
	//printf(" Out of %zd rooms, %d are assigned, %d are not assigned\n", roomlist.size(), assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd families, %d families are assigned with a room, and %d families are not assigned with a room\n", family_list_size, assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::printRooms2Males()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> male_list = m_male_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryMaleRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator sit;
	for (sit = male_list.begin(); sit != male_list.end(); sit++) {
		int32_t party_id = sit->first;
		std::vector<Registrant*> flist = sit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size() - assigned_room_cnt;
	printf("=== Male room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	//printf(" Out of %zd rooms, %d are assigned, %d are not assigned\n", roomlist.size(), assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd individual males, %d males are assigned with a room, and %d males are not assigned with a room\n", male_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::printRooms2Females()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>> female_list = m_female_list;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryFemaleRooms();

	std::map<int32_t, std::vector<Registrant*>>::iterator sit;
	for (sit = female_list.begin(); sit != female_list.end(); sit++) {
		int32_t party_id = sit->first;
		std::vector<Registrant*> flist = sit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			assigned_family_cnt++;
		}
		else
			unassigned_family_cnt++;
	}

	assigned_room_cnt = (int32_t)assigned_roomlist.size();
	unassigned_room_cnt = (int32_t)roomlist.size() - assigned_room_cnt;
	printf("=== Female room assignment:\n");
	printf("Total assigned rooms = %d, Available rooms = %d\n", assigned_room_cnt, unassigned_room_cnt);
	//printf(" Out of %zd rooms, %d are assigned, %d are not assigned\n", roomlist.size(), assigned_room_cnt, unassigned_room_cnt);
	printf("Out of %zd individual females, %d females are assigned with a room, and %d females are not assigned with a room\n", female_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}

int32_t RoomAssign::trackStatus(const char* op)
{
	int32_t i;
	TrackData td;
	td.assigned_room_cnt = 0;
	td.unassigned_room_cnt = 0;
	td.assigned_party_cnt = 0;
	td.unassigned_party_cnt = 0;
	td.assigned_attendee_cnt = 0;
	td.unassigned_attendee_cnt = 0;

	int32_t total_rooms = m_br_list.getTotalActiveRooms();
	std::vector<BuildingRoomList::EURoom*> assigned_rooms = m_br_list.queryAssignedRooms();
	td.assigned_room_cnt = (int32_t)assigned_rooms.size();
	td.unassigned_room_cnt = total_rooms - td.assigned_room_cnt;

	std::map<int32_t, Registrant*> unassigned_persons_temp;

	std::map<int32_t, Registrant*>::iterator pit; // person id
	for (pit = m_EU_person_info.begin(); pit != m_EU_person_info.end(); pit++) {
		int32_t person_id = pit->first;
		Registrant* plist = pit->second;
		bool room_assigned = (plist->assigned_room != NULL);
		if (room_assigned)
			td.assigned_attendee_cnt++;
		else {
			unassigned_persons_temp[person_id] = m_EU_person_info[person_id];
			td.unassigned_attendee_cnt++;
		}
	}

	std::map<std::string, int32_t> assigned_roomlist;
	std::map<int32_t, std::vector<Registrant*>>::iterator fit; // party id
	for (fit = m_EU_party_info.begin(); fit != m_EU_party_info.end(); fit++) {
		int32_t party_id = fit->first;
		std::vector<Registrant*> flist = fit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned) {
			for (i = 0; i < flist.size(); i++) {
				assigned_roomlist[flist[i]->assigned_room->room] = 1;
			}
			td.assigned_party_cnt++;
		}
		else
			td.unassigned_party_cnt++;
	}

	m_track_data[op] = td;

	return 0;
}

bool RoomAssign::printChurchDistributionPerBuilding(const char *filename)
{
	int32_t i, j;
	int32_t cnt_family = 0, cnt_rooms = 0;
	bool enable_extrabeds = true;
	std::vector<ChurchList::QFLChurch> *churches = getChurchHandle()->getChurchList();

	if (filename == NULL)
		return false;

	std::ofstream fout(filename);
	if (!fout.is_open())
		return false;

	int32_t cnt = 0;
	fout << "Church," << "PersonId," << "PartyId," << "ContactPerson," << "PartyType," << "FirstName,"
		<< "LastName," << "ChineseName," << "RoomNumber," << "Extra_bed," << "CellGroup," << "NeedRoom," << "Age,"
		<< "Gender," << "Grade," << "is_Christian," << "MobilePhone," << "Email," << "City," << "State," << "Zip," 
		<< "FunctionalGroup," << "Services," << "Notes," << "Needs" << std::endl;

	char zip_str[128];
	int32_t total_parties = 0;
	int32_t total_attendees = 0;
	std::map<int32_t, std::vector<Registrant*>>::iterator it;
	std::map<std::string, std::vector<Registrant*>> church_distr_map;

	for (i = 0; i < churches->size(); i++) {
		church_distr_map.clear();
		ChurchList::QFLChurch achurch = (*churches)[i];
		std::string name = achurch.church_name;
		Church thechurch;
		thechurch = m_attendee_list_byChurch[name];

		if (thechurch.persons.size() == 0)
			continue;
		std::vector<Registrant*>::iterator it;
		for (it = thechurch.persons.begin(); it != thechurch.persons.end(); it++) {
			Registrant *attendee = *it;
			std::string building = attendee->building;
			church_distr_map[building].push_back(attendee);
		}
		fout << name << ":" << std::endl;
		std::map<std::string, std::vector<Registrant*>>::iterator cit;
		for (cit = church_distr_map.begin(); cit != church_distr_map.end(); cit++) {
			std::string bdname = cit->first;
			std::vector<Registrant*> rlist = cit->second;
			fout << bdname << ":" << std::endl;
			for (j = 0; j < rlist.size(); j++) {
				Registrant *attendee = rlist[j];
				int32_t person_id = attendee->person_id;
				int32_t party_id = attendee->party;
				std::string church = attendee->church;
				std::string church_chinese = attendee->church_cname;
				std::string contact_person = attendee->contact_person;
				std::string party_type = attendee->party_type;
				std::string first_name = attendee->first_name;
				std::string last_name = attendee->last_name;
				std::string chinese_name = attendee->chinese_name;
				std::string room_number = attendee->room;
				int32_t extra_bed = 0;
				if (attendee->assigned_room != NULL)
					extra_bed = ((BuildingRoomList::EURoom*)(attendee->assigned_room))->extra;
				std::string cell_group = attendee->cell_group;
				bool need_room = attendee->need_room;
				std::string age = attendee->age_group;
				std::string gender = attendee->gender;
				std::string grade = attendee->grade;
				size_t found = grade.find(",");
				if (found != string::npos) {
					grade.replace(found, 1, "-");
				}
				bool is_christian = attendee->is_christian;
				std::string occupation = attendee->occupation;
				std::string mobile = attendee->mobile_phone;
				std::string email = attendee->email;
				std::string city = attendee->city;
				std::string state = attendee->state;
				std::string functional = attendee->functional_group;
				std::string services = attendee->services;
				sprintf(zip_str, "%05d", attendee->zip);
				std::string notes = attendee->notes;
				std::string needs = attendee->special_need;

				fout << church_chinese << ", " << person_id << ", " << party_id << ", " << contact_person << ", " << party_type << ", "
					<< first_name << ", " << last_name << ", " << chinese_name + " " << ", " << room_number << ", " << extra_bed << ", "
					<< cell_group + " " << ", " << need_room << ", "
					<< age << ", " << gender << ", " << grade + " " << ", " << is_christian << ", "
					<< mobile << ", " << email << ", " << city << ", " << state << ", " 
					<< zip_str << ", " << functional << ", " << services << ", " << notes << ", " << needs << std::endl;

			}
		}
	}

	fout.close();
	return true;
}

int32_t RoomAssign::printSortedAttendees(const char* filename, std::map<int32_t, std::vector<Registrant*>> slist, bool print_title)
{
	int i, j;
	char zip_str[128];
	if (filename == NULL)
		return -1;

	std::ofstream fout;
	fout.open(filename, std::ofstream::out | std::ofstream::app);
	if (!fout.is_open())
		return -2;

	int32_t cnt = 0;
	if (print_title) {
		fout << "PersonId," << "PartyId," << "ContactPerson," << "PartyType," << "FirstName,"
			<< "LastName," << "ChineseName," << "RoomNumber," << "CellGroup," << "NeedRoom," << "Age,"
			<< "Gender," << "Grade," << "is_Christian," << "Church,"
			<< "MobilePhone," << "Email," << "City," << "State," << "Zip," << "FunctionalGroup," << "Services" << std::endl;
	}

	if (slist.size() == 0)
		return cnt;

	std::map<int32_t, std::vector<Registrant*>>::iterator fit; // party id
	for (fit = slist.begin(); fit != slist.end(); fit++) {
		int32_t party_id = fit->first;
		std::vector<Registrant*> rlist = fit->second;
		for (j = 0; j < rlist.size(); j++) {
			Registrant *attendee = rlist[j];
			int32_t person_id = attendee->person_id;
			int32_t party_id = attendee->party;
			std::string church = attendee->church;
			std::string contact_person = attendee->contact_person;
			std::string party_type = attendee->party_type;
			std::string first_name = attendee->first_name;
			std::string last_name = attendee->last_name;
			std::string chinese_name = attendee->chinese_name;
			std::string room_number = attendee->room;
			std::string cell_group = attendee->cell_group;
			bool need_room = attendee->need_room;
			std::string age = attendee->age_group;
			std::string gender = attendee->gender;
			std::string grade = attendee->grade;
			size_t found = grade.find(",");
			if (found != string::npos) {
				grade.replace(found, 1, "-");
			}
			bool is_christian = attendee->is_christian;
			std::string church0 = attendee->church;
			std::string mobile = attendee->mobile_phone;
			std::string email = attendee->email;
			std::string city = attendee->city;
			std::string state = attendee->state;
			std::string functional = attendee->functional_group;
			std::string services = attendee->services;
			sprintf(zip_str, "%05d", attendee->zip);

			fout << person_id << ", " << party_id << ", " << contact_person << ", " << party_type << ", "
				<< first_name << ", " << last_name << ", " << chinese_name + " " << ", " << room_number << ", "
				<< cell_group + " " << ", " << need_room << ", "
				<< age << ", " << gender << ", " << grade + " " << ", " << is_christian << ", "
				<< church0 << ", " << mobile << ", " << email << ", " << city << ", " << state << ", "
				<< zip_str << ", " << functional << ", " << services << std::endl;
			cnt++;
		}
	}
	fout << "\n";
	fout.close();
	return cnt;
}

bool RoomAssign::printEU_for_cellgroup(const char* filename)
{
	int i, j;
	char zip_str[128];
	if (filename == NULL)
		return false;

	std::ofstream fout;
	fout.open(filename, std::ofstream::out);
	if (!fout.is_open())
		return false;

	int32_t cancelled = 0;
	int32_t youth_camp = 0;
	int32_t eu_camp = 0;
	int32_t cnt = 0;
	fout << "PersonId," << "Church," << "FirstName," << "LastName," << "ChineseName," << "NeedRoom," << "Age,"
		<< "Gender," << "is_Christian," << "City," << "State," << "Zip," << "FunctionalGroup," << "Services" << std::endl;
	
	for (i = 0; i < m_registrants.size(); i++) {
		Registrant attendee = m_registrants[i];
		if (!attendee.cancelled) {
			if (!attendee.youth_camp_flag) {
				int32_t person_id = attendee.person_id;
				int32_t party_id = attendee.party;
				std::string church = attendee.church;
				std::string church_chinese = attendee.church_cname;
				std::string contact_person = attendee.contact_person;
				std::string party_type = attendee.party_type;
				std::string first_name = attendee.first_name;
				std::string last_name = attendee.last_name;
				std::string chinese_name = attendee.chinese_name;
				std::string room_number = attendee.room;
				std::string cell_group = attendee.cell_group;
				bool need_room = attendee.need_room;
				std::string age = attendee.age_group;
				std::string gender = attendee.gender;
				std::string grade = attendee.grade;
				size_t found = grade.find(",");
				if (found != string::npos) {
					grade.replace(found, 1, "-");
				}
				bool is_christian = attendee.is_christian;
				std::string church0 = attendee.church;
				std::string mobile = attendee.mobile_phone;
				std::string email = attendee.email;
				std::string city = attendee.city;
				std::string state = attendee.state;
				std::string functional = attendee.functional_group;
				std::string services = attendee.services;
				sprintf(zip_str, "%05d", attendee.zip);

				bool under_age = (age.compare(AgeGroup::A1) == 0 || age.compare(AgeGroup::A2) == 0 ||
					age.compare(AgeGroup::A3) == 0 || age.compare(AgeGroup::A4_5) == 0 ||
					age.compare(AgeGroup::A6_11) == 0);
					//|| age.compare(AgeGroup::A12_14) == 0 || age.compare(AgeGroup::A15_17) == 0);
				if (under_age)
					continue;
				fout << person_id << ", " << church_chinese << ", "<< first_name << ", " << last_name << ", " << chinese_name + " " << ", " 
					<< need_room << ", " << age << ", " << gender << ", " << is_christian << ", " << city << ", " << state << ", " << zip_str << ", " << functional << ", " << services << std::endl;

				eu_camp++;
			}
			else
				youth_camp++;
		}
		else
			cancelled++;
	}

	fout << "\n";
	fout.close();


	return true;
}