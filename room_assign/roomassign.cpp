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
		else if (aregist[0]->gender.compare("Male")==0)
			male_lst[party_id] = (m_choir_list[party_id]);
		else if (aregist[0]->gender.compare("Female")==0)
			female_lst[party_id] = (m_choir_list[party_id]);
	}

	// family allocate first
	for (it = family_lst.begin(); it != family_lst.end(); ++it) {
		int32_t party_id = it->first;
		std::vector<Registrant*> aregist = it->second;
		int32_t fs = (int32_t)aregist.size();

		temp_roomlist = queryRoomList(roomlist, fs);

		for (i = 0; i < temp_roomlist.size(); i++) {
			BuildingRoomList::EURoom* eu_room = temp_roomlist[i];
			temp_roomlist[i]->room_status = RoomStatus::qFullyAssigned;
			temp_roomlist[i]->bed_assigned = fs;
			temp_roomlist[i]->extra = ((enable_extrabeds) ? 1 : 0);
			for (j = 0; j < aregist.size(); j++) {
				aregist[j]->assigned_room = temp_roomlist[i];
				aregist[j]->room = temp_roomlist[i]->room;
			}
			temp_roomlist[i]->persons = (void*)(&m_family_info[party_id]);
			break;
		}
	}

	// male and female are not neighbors


	return 0;
}


std::vector<BuildingRoomList::EURoom*> RoomAssign::queryRoomList(std::vector<BuildingRoomList::EURoom*> &myroomlist, int32_t num, bool enable_extrabed)
{
	int32_t i;
	std::vector<BuildingRoomList::EURoom*> rms;
	int32_t sz = (int32_t)myroomlist.size();
	if (sz == 0)
		return rms;

	for (i = 0; i < sz; i++) {
		BuildingRoomList::EURoom* eur = myroomlist[i];
		if (eur->room_status == RoomStatus::qReservedCH || eur->room_status == RoomStatus::qReservedGE ||
			eur->room_status == RoomStatus::qReservedSK || eur->room_status == RoomStatus::qReservedSP ||
			eur->room_status == RoomStatus::qAvailable  || eur->room_status == RoomStatus::qPartiallyAssigned) {
			int32_t capacity = eur->capacity;
			int32_t beds_assigned = eur->bed_assigned;
			int32_t extra_bed = eur->extra;
			if (enable_extrabed) {
				if ((capacity - beds_assigned + extra_bed) == num) {
					rms.push_back(myroomlist[i]);
				}
			}
			else if ((capacity-beds_assigned) == num) {
				rms.push_back(myroomlist[i]);
			}
		}
	}

	return rms;
}

