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

/*
* find family, allocate room
* find individual, allocate rooms
*/
int32_t RoomAssign::assignRooms2Choir()
{
	int32_t i, j;
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
		else if (aregist[0]->gender.compare("Male")==0)
			male_lst[party_id] = (m_choir_list[party_id]);
		else if (aregist[0]->gender.compare("Female")==0)
			female_lst[party_id] = (m_choir_list[party_id]);
	}

	// family allocate first
	for (it = family_lst.begin(); it != family_lst.end(); ++it) {
		int32_t party_id = it->first;
		std::vector<Registrant*> aregist = it->second;
		for (i = 0; i < roomlist.size(); i++) {
			BuildingRoomList::EURoom* eu_room = roomlist[i];
			if (eu_room->capacity < aregist.size())
				continue;
			if (eu_room->room_status == RoomStatus::qReservedCH) {
				for (j = 0; j < aregist.size(); j++) {
					aregist[j]->assigned_room = roomlist[i];
					aregist[j]->room = roomlist[i]->room;
					roomlist[i]->persons = (void*)(&m_family_info[party_id]);

				}
			}
		}
	}

	// male and female are not neighbors


	return 0;
}


