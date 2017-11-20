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
	status = readRegistrants(registration_name);
	if (status < 0)
		return status;
	status = readBuildingRooms(buildings_name);

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
	status = sortAttendeesPerBuilidng();

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
	status = sortAttendeesPerBuilidng();

	return status;
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

	// family assignment first
	if (family_lst.size() > 0) {
		if (familyRoomAssign(family_lst, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2Choir(): Family room assignment failed\n");
	}

	// male room assignment - male do not share bathroom with females 
	if (male_lst.size() > 0) {
		if (IndividualRoomAssign(male_lst, BuildingRoomList::eMale, roomlist) <= 0)
			printf("assignRooms2Choir(): Male room assignment failed\n");
	}
	// female room assignment - male do not share bathroom with females 
	if (female_lst.size() > 0) {
		if (IndividualRoomAssign(female_lst, BuildingRoomList::eFemale, roomlist) <= 0)
			printf("assignRooms2Choir(): Female room assignment failed\n");
	}

	return 0;
}

int32_t RoomAssign::assignRooms2ChildcareWorkers()
{
	int32_t i;
	std::string contacts[2] = { "Hong Li", "Jing Pan" };
	bool enable_extrabeds = false;

	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedCCRooms();
	std::vector<BuildingRoomList::EURoom*> male_roomlist;
	std::vector<BuildingRoomList::EURoom*> female_roomlist;

	std::map<int32_t, Registrant*> childleadlist;
	std::map<int32_t, std::vector<Registrant*>> female_lst;
	std::map<int32_t, std::vector<Registrant*>> male_lst;

	// generate child counselor list (age range: 14-25)
	std::map<int32_t, std::vector<Registrant*>>::iterator it;
	for (it = m_child_leader_list.begin(); it != m_child_leader_list.end(); it++) {
		int32_t party_id = it->first;
		std::vector<Registrant*> party = it->second;
		for (i = 0; i < party.size(); i++) {
			bool contact_flag = false;
			bool age_flag = false;
			int32_t person_id = party[i]->person_id;
			bool isMale = (party[i]->gender.compare("Male") == 0);
			std::string contact = party[i]->contact_person;
			if (contact.compare(contacts[0]) == 0 || contact.compare(contacts[1]) == 0)
				contact_flag = true;
			std::string age = party[i]->age_group;
			if (age.compare(AgeGroup::A12_14) == 0 || age.compare(AgeGroup::A15_17) == 0 || age.compare(AgeGroup::A18_25) == 0)
				age_flag = true;

			if (contact_flag && age_flag) {
				childleadlist[person_id] = m_person_info[person_id];
				if (isMale)
					male_lst[person_id].push_back(m_person_info[person_id]);
				else
					female_lst[person_id].push_back(m_person_info[person_id]);
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
		if (IndividualRoomAssign(male_lst, BuildingRoomList::eMale, male_roomlist) <= 0)
			printf("assignRooms2ChildcareWorkers(): room assignment failed\n");
	}

	// female room assignment - male do not share bathroom with females 
	if (female_lst.size() > 0) {
		if (IndividualRoomAssign(female_lst, BuildingRoomList::eFemale, female_roomlist) <= 0)
			printf("assignRooms2ChildcareWorkers(): female room assignment failed\n");
	}
	return 0;
}

int32_t RoomAssign::assignRooms2Speakers()
{
	bool enable_extrabeds = false;
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSKRooms();
	std::map<int32_t, std::vector<Registrant*>> spk_list = m_speaker_list;

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

	// family assignment first
	if (recording_list.size() > 0) {
		if (familyRoomAssign(recording_list, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2Recordings(): Family room assignment failed\n");
	}
	return 0;
}

int32_t RoomAssign::assignRooms2Babies()
{
	bool enable_extrabeds = false; // can change to true since baby is likely to stay on floor when sleep
	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSPRooms();
	std::map<int32_t, std::vector<Registrant*>> baby_list = m_baby_list;

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

	// family assignment first
	if (senior_list.size() > 0) {
		if (familyRoomAssign(senior_list, roomlist, enable_extrabeds) <= 0)
			printf("assignRooms2Babies(): Family room assignment failed\n");
	}

	printf("\n");
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
			printf("Family Private: %s %s\n", room.c_str(), status.c_str());
		}
	}

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
				printf("assignRooms2Families(): Family room assignment failed\n");
				room_nums = 0;
			}
			cnt_rooms += room_nums;
			printf("families %d, rooms %d\n", cnt_family, cnt_rooms);

		}
	}

	// assign family_male rooms
	for (i = 0; i < m_br_list.queryFamilyMaleRooms().size(); i++) {
		std::string status = m_br_list.queryFamilyMaleRooms()[i]->room_status;
		std::string room = m_br_list.queryFamilyMaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			male_roomlist.push_back(m_br_list.queryFamilyMaleRooms()[i]);
			printf("Family Male: %s %s\n", room.c_str(), status.c_str());
		}
	}
	// assign family_female rooms
	for (i = 0; i < m_br_list.queryFamilyFemaleRooms().size(); i++) {
		std::string status = m_br_list.queryFamilyFemaleRooms()[i]->room_status;
		std::string room = m_br_list.queryFamilyFemaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			female_roomlist.push_back(m_br_list.queryFamilyFemaleRooms()[i]);
			printf("Family Female: %s %s\n", room.c_str(), status.c_str());
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
				printf("families %d, rooms %d\n", cnt_family, cnt_rooms);
			}
		}
	}

	printf("room assignment: \n");
	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* aroom = roomlist[i];
		std::string buildingname = ((BuildingRoomList::EUBuilding*)aroom->building)->building_name;
		std::string roomname = aroom->room;
		std::string roomtype = aroom->room_type;
		int32_t capacity = aroom->capacity;
		printf("%s %s %s %dc\t", buildingname.c_str(), roomname.c_str(), roomtype.c_str(), capacity);
		for (j = 0; j < aroom->persons.size(); j++) {
			if (j == 0) {
				Registrant *attendee = getRegistrant(aroom->persons[j]);
				printf("%s ", attendee->church.c_str());
			}
			int32_t person_id = aroom->persons[j];
			printf(" %d ", person_id);
		}
		printf("\n");
	}

	printf(" families not yet assigned a room:\n");
	int32_t cnt = 0;
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
				for (j = 0; j < family.size(); j++) {
					if (!family[j]->assigned_room) {
						printf("%s, %d %d\n", name.c_str(), family_id, family[j]->person_id);
						if (j == 0)
							cnt++;
					}
				}
			}
		}
	}
	printf(" -- %d families not assigned a room\n", cnt);

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

	for (i = 0; i < m_br_list.queryMaleRooms().size(); i++) {
		std::string status = m_br_list.queryMaleRooms()[i]->room_status;
		std::string room = m_br_list.queryMaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			roomlist.push_back(m_br_list.queryMaleRooms()[i]);
			printf("%s %s\n", room.c_str(), status.c_str());
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
		printf("persons %d, beds %d\n", cnt_person, cnt_beds);
	}

	printf("\n");

	// print how many rooms are available
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

	for (i = 0; i < m_br_list.queryFemaleRooms().size(); i++) {
		std::string status = m_br_list.queryFemaleRooms()[i]->room_status;
		std::string room = m_br_list.queryFemaleRooms()[i]->room;
		if (status.compare("Available") == 0) {
			roomlist.push_back(m_br_list.queryFemaleRooms()[i]);
			printf("%s %s\n", room.c_str(), status.c_str());
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
		printf("persons %d, rooms %d\n", cnt_person, cnt_beds);
	}

	printf("\n");

	// print how many rooms are available

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

	total_capacity = 0;
	std::map < int32_t, std::vector<BuildingRoomList::EURoom*>>::iterator it;
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
	int32_t i, j, k;
	std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> rms;
	std::vector<BuildingRoomList::EURoom* > frl;
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

	BuildingRoomList::SexType stype;
	// find the capacity of room matches the number of family size
	std::map < int32_t, std::vector<BuildingRoomList::EURoom*>>::iterator it;
	for (i = 0; i < 3; i++) {
		for (it = rms.begin(); it != rms.end(); ++it) {
			int32_t score = it->first;
			std::vector<BuildingRoomList::EURoom*> roomlist = it->second;

			for (j = 0; j < roomlist.size(); j++) {
				total_capacity = roomlist[j]->capacity + (enable_extrabed ? 1 : 0);
				if (total_capacity < num) {
					if (roomlist.size() > 1 && (!roomlist[1]->considered)) {
						// at most two rooms have shared bathrooms
						total_capacity = roomlist[0]->capacity + roomlist[1]->capacity + (enable_extrabed ? 1 : 0);
						if (total_capacity == num || total_capacity == num + 1) {
							frl.push_back(roomlist[0]); // return the first room
							frl.push_back(roomlist[1]); // return the first room
							return frl;
						}
					}
				}
				else {
					if (total_capacity - i == num) {
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
										frl.push_back(roomlist[j]);
										return frl;
									}
									else if (stype == BuildingRoomList::eFemale && st == BuildingRoomList::eFemale) {
										frl.push_back(roomlist[j]);
										return frl;
									}
									if (stype == BuildingRoomList::eMale && (st == BuildingRoomList::eFemale || st == BuildingRoomList::eMix))
										continue;
									else if (stype == BuildingRoomList::eFemale && (st == BuildingRoomList::eMale || st == BuildingRoomList::eMix))
										continue;
								}
							}
						}
						frl.push_back(roomlist[j]);
						return frl;
					}
				}
			}
		}
	}

	// find the least score room
	total_capacity = 0;
	for (it = rms.begin(); it != rms.end(); ++it) {
		int32_t score = it->first;
		std::vector<BuildingRoomList::EURoom*> roomlist = it->second;
		for (j = 0; j < roomlist.size(); j++) {
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

	int32_t cnt = 0;
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
				for (j = 0; j < family.size(); j++) {
	
				}
			}
		}
	}

	return true;
}


int32_t RoomAssign::roomAllocationStats() 
{
	std::vector<BuildingRoomList::EURoom*> general_roomlist = m_br_list.queryAvailableRooms();
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
	m_alloc_rooms.general_rooms = (int32_t)general_roomlist.size();
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

	return 0;
}

int32_t RoomAssign::printRooms2Seniors()
{
	int32_t i;
	int32_t assigned_room_cnt = 0;
	int32_t unassigned_room_cnt = 0;
	int32_t assigned_family_cnt = 0;
	int32_t unassigned_family_cnt = 0;

	std::vector<BuildingRoomList::EURoom*> temp_roomlist;
	std::vector<BuildingRoomList::EURoom*> roomlist = m_br_list.queryReservedSPRooms();
	std::map<int32_t, std::vector<Registrant*>> senior_list = m_senior_list;

	for (i = 0; i < roomlist.size(); i++) {
		BuildingRoomList::EURoom* room = roomlist[i];
		if (room->room_status.compare(RoomStatus::qFullyAssigned) == 0)
			assigned_room_cnt++;
		else
			unassigned_room_cnt++;
	}

	std::map<int32_t, std::vector<Registrant*>>::iterator sit;
	for (sit = senior_list.begin(); sit != senior_list.end(); sit++) {
		int32_t party_id = sit->first;
		std::vector<Registrant*> flist = sit->second;
		bool room_assigned = true;
		for (i = 0; i < flist.size(); i++) {
			room_assigned = room_assigned && (flist[i]->assigned_room != NULL);
		}

		if (room_assigned)
			assigned_family_cnt++;
		else
			unassigned_family_cnt++;
	}

	printf(" == Senior people room assignment:\n");
	printf(" Out of %zd rooms, %d are assigned, %d are not assigned\n", roomlist.size(), assigned_room_cnt, unassigned_room_cnt);
	printf(" Out of %zd senior families, %d families are assigned with a room, and %d families are not assigned with a room\n", senior_list.size(), assigned_family_cnt, unassigned_family_cnt);

	return 0;
}