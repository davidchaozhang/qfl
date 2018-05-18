#include "buildings.h"
#include <fstream>
#include <algorithm>

static bool BDbyNumber(const BuildingRoomList::EUBuilding &a, const BuildingRoomList::EUBuilding &b)
{
	return (a.build_no < b.build_no);
}

const std::string BuildingRoomList::m_buildings[7] = { BuildingNames::qEagleHall, BuildingNames::qGallup, BuildingNames::qGoughHall, BuildingNames::qGuffin,
BuildingNames::qHainer, BuildingNames::qKea, BuildingNames::qSparrowk };

const int32_t BuildingRoomList::m_OpLevel[5] = {4, 0, 1, 2, 3};

BuildingRoomList::BuildingRoomList()
{
	m_total_rooms = 0;
	m_total_beds = 0;
	m_total_inactive_rooms = 0;
	m_room_measure.brBuilding = 10000;
	m_room_measure.brSect = 1000;
	m_room_measure.brLevel = 200;
	m_room_measure.brRoom = 4;
	m_room_measure.brAC = -5;
	m_room_measure.brElevator = 0;

	m_buildingCode[BuildingNames::qEagleHall] = BuildingCode::pEagleHall;
	m_buildingCode[BuildingNames::qGallup] = BuildingCode::pGallup;
	m_buildingCode[BuildingNames::qGoughHall] = BuildingCode::pGoughHall;
	m_buildingCode[BuildingNames::qGuffin] = BuildingCode::pGuffin;
	m_buildingCode[BuildingNames::qHainer] = BuildingCode::pHainer;
	m_buildingCode[BuildingNames::qKea] = BuildingCode::pKea;
	m_buildingCode[BuildingNames::qSparrowk] = BuildingCode::pSparrowk;
}

BuildingRoomList::~BuildingRoomList()
{ }

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryRoomList(RoomState rs)
{
	std::vector<BuildingRoomList::EURoom*> eurlist, my_eurlist;
	if (m_room_list_per_building.size() == 0)
		return (std::vector<BuildingRoomList::EURoom*>)NULL;

	for (int i = 0; i < 7; i++) {
		std::vector<BuildingRoomList::EURoom*> alist;
		std::string abuilding = m_buildings[i];
		alist = queryRoomList(abuilding, rs);
		for (int j = 0; j < alist.size(); j++)
			my_eurlist.push_back(alist[j]);
	}
	return my_eurlist;
}

/*
room_status:
"Available", "Inactive", "Reserved-GE", "Reserved-SK", "Reserved-SP",
"Reserved-CH","Reserved-CC",
"Fully Assigned", "Partially Assigned"

room_type:
"Family Private", "Family Male", "Family Female", "Male", "Female"
*/
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryRoomList(const std::string &building_name, RoomState rs)
{
	std::vector<BuildingRoomList::EURoom*> eurlist, my_eurlist;
	if (m_room_list_per_building.size() == 0)
		return (std::vector<BuildingRoomList::EURoom*>)NULL;

	eurlist = m_room_list_per_building[building_name];
	if (eurlist.size() == 0)
		return (std::vector<BuildingRoomList::EURoom*>)NULL;
	else {
		for (int i = 0; i < eurlist.size(); i++) {
		std::string str = getStringNames(rs);
		if (str.compare(eurlist[i]->room_status) == 0)
			my_eurlist.push_back(eurlist[i]);
		else if (str.compare(eurlist[i]->room_type) == 0)
			my_eurlist.push_back(eurlist[i]);
		}
	}
	return my_eurlist;
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryAvailableRooms()
{
	RoomState rs = RoomState::tAvailable;
	return queryRoomList(rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryAvailableRooms(std::string building_name)
{
	RoomState rs = RoomState::tAvailable;
	return queryRoomList(building_name, rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryAssignedRooms()
{
	RoomState rs = RoomState::tFullyAssigned;
	return queryRoomList(rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryAssignedRooms(std::string building_name)
{
	RoomState rs = RoomState::tFullyAssigned;
	return queryRoomList(building_name, rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryPartialAssignedRooms()
{
	RoomState rs = RoomState::tPartiallyAssigned;
	return queryRoomList(rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryPartialAssignedRooms(std::string building_name)
{
	RoomState rs = RoomState::tPartiallyAssigned;
	return queryRoomList(building_name, rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryBathSharedRooms()
{
	std::vector<BuildingRoomList::EURoom*> alist;
	if (m_eu_buildings.size() == 0)
		return std::vector<BuildingRoomList::EURoom*>(NULL);

	for (int i = 0; i < m_eu_buildings.size(); i++) {
		EUBuilding eub = m_eu_buildings[i];
		for (int j = 0; j < eub.sects.size(); j++) {
			EUSection eus = eub.sects[j];
			for (int k = 0; k < eus.rooms.size(); k++) {
				EURoom eur = eus.rooms[k];
				if (eur.neighbors.size() > 0)
					alist.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
			}
		}
	}
	return alist;
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryBathSharedRooms(std::string building_name)
{
	std::vector<EURoom*> eurlist = queryRoomList(building_name, RoomState::tFamily_Private);
	std::vector<EURoom*> mylist;

	for (int i = 0; i < eurlist.size(); i++) {
		EURoom *eur = eurlist[i];
		if (eur->neighbors.size() > 0)
			mylist.push_back(eur);
	}
	return mylist;
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryFamilyPrivateRooms()
{
	RoomState rs = RoomState::tFamily_Private;
	return queryRoomList(rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryFamilyPrivateRooms(std::string building_name)
{
	RoomState rs = RoomState::tFamily_Private;
	return queryRoomList(building_name, rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryFamilyMaleRooms()
{
	RoomState rs = RoomState::tFamily_Male;
	return queryRoomList(rs);
}


std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryFamilyMaleRooms(std::string building_name) 
{
	RoomState rs = RoomState::tFamily_Male;
	return queryRoomList(building_name, rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryFamilyFemaleRooms()
{
	RoomState rs = RoomState::tFamily_Female;
	return queryRoomList(rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryFamilyFemaleRooms(std::string building_name)
{
	RoomState rs = RoomState::tFamily_Female;
	return queryRoomList(building_name, rs);
}


std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryMaleRooms()
{
	RoomState rs = RoomState::tMale;
	return queryRoomList(rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryMaleRooms(std::string building_name)
{
	RoomState rs = RoomState::tMale;
	return queryRoomList(building_name, rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryFemaleRooms()
{
	RoomState rs = RoomState::tFemale;
	return queryRoomList(rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryFemaleRooms(std::string building_name)
{
	RoomState rs = RoomState::tFemale;
	return queryRoomList(building_name, rs);
}

std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedGERooms()
{
	RoomState rs = RoomState::tReservedGE;
	return queryRoomList(rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedGERooms(std::string building_name)
{
	RoomState rs = RoomState::tReservedGE;
	return queryRoomList(building_name, rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedSKRooms()
{
	RoomState rs = RoomState::tReservedSK;
	return queryRoomList(rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedSKRooms(std::string building_name)
{
	RoomState rs = RoomState::tReservedSK;
	return queryRoomList(building_name, rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedSPRooms()
{
	RoomState rs = RoomState::tReservedSP;
	return queryRoomList(rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedSPRooms(std::string building_name)
{
	RoomState rs = RoomState::tReservedSP;
	return queryRoomList(building_name, rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedCHRooms()
{
	RoomState rs = RoomState::tReservedCH;
	return queryRoomList(rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedCHRooms(std::string building_name)
{
	RoomState rs = RoomState::tReservedCH;
	return queryRoomList(building_name, rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedCCRooms()
{
	RoomState rs = RoomState::tReservedCC;
	return queryRoomList(rs);
}
std::vector<BuildingRoomList::EURoom*> BuildingRoomList::queryReservedCCRooms(std::string building_name)
{
	RoomState rs = RoomState::tReservedCC;
	return queryRoomList(building_name, rs);
}

int BuildingRoomList::readInBuildingLists(const char * building_rooms_list, char dataformat)
{
	if (dataformat == ',') {
		readInBuildingList_commas(building_rooms_list);
		updateAllSections(dataformat);
		updateAllRooms_commas();
	}
	else if (dataformat == '"') {
		readInBuildingList_quotes(building_rooms_list);
		updateAllSections(dataformat);
		updateAllRooms_quotes();
	}
	else {
		printf("Invalid data format... not supported\n");
		return -1;
	}
	return 0;
}

int BuildingRoomList::readInBuildingList_quotes(const char* building_rooms_list)
{
	int32_t i;
	if (building_rooms_list == NULL)
		return -1;

	std::ifstream fin(building_rooms_list);
	if (!fin.is_open())
		return -2;

	std::vector<std::string> tmp;
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
		m_room_array.push_back(tmp);
	}

	std::vector<std::string> building_names;
	std::vector<int32_t> buiding_no;
	std::string temp;
	int32_t temp_num;
	std::vector<std::string> aroom = m_room_array[0];
	bool matched;
	size_t n = aroom.size();

	for (y = 1; y < m_room_array.size(); y++){
		matched = false;
		aroom = m_room_array[y];
		if (aroom.size() < n) {
			m_room_array.erase(m_room_array.begin() + y);
			y--;
			continue;
		}
		if (building_names.empty()) {
			temp_num = std::atoi(m_room_array[y][0].substr(1, m_room_array[y][0].size() - 2).c_str());
			building_names.push_back(m_room_array[y][1]);
			buiding_no.push_back(temp_num);
		}
		else {
			for (i = 0; i < building_names.size(); i++) {
				if (m_room_array[y][1].compare(building_names[i]) == 0) {
					matched = true;
					break;
				}
			}
			if (!matched) {
				temp_num = std::atoi(m_room_array[y][0].substr(1, m_room_array[y][0].size() - 2).c_str());
				building_names.push_back(m_room_array[y][1]);
				buiding_no.push_back(temp_num);
			}
		}
	}

	m_eu_buildings = std::vector<EUBuilding>(building_names.size());

	for (i = 0; i < building_names.size(); i++) {
		m_eu_buildings[i].build_no = buiding_no[i];
		temp = building_names[i].substr(1, building_names[i].size() - 2);
		m_eu_buildings[i].building_name = temp;
	}

	std::sort(m_eu_buildings.begin(), m_eu_buildings.end(), BDbyNumber);

	return 0;
}

int BuildingRoomList::readInBuildingList_commas(const char * building_rooms_list)
{
	int32_t i;
	if (building_rooms_list == NULL)
		return -1;

	std::ifstream fin(building_rooms_list);
	if (!fin.is_open())
		return -2;

	std::vector<std::string> tmp;
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
			tmp.push_back(line0.substr(0, pos));
			//erase the val including ,
			line0.erase(0, pos + 1);
		}
		tmp.push_back(line0);
		line0.erase();
		m_room_array.push_back(tmp);
	}

	std::vector<std::string> building_names;
	std::vector<int32_t> buiding_no;
	std::string temp;
	int32_t temp_num;
	std::vector<std::string> aroom = m_room_array[0];
	bool matched;
	size_t n = aroom.size();

	for (y = 1; y < m_room_array.size(); y++){
		matched = false;
		aroom = m_room_array[y];
		if (aroom.size() < n) {
			m_room_array.erase(m_room_array.begin() + y);
			y--;
			continue;
		}
		if (building_names.empty()) {
			temp_num = std::atoi(m_room_array[y][0].c_str());
			building_names.push_back(m_room_array[y][1]);
			buiding_no.push_back(temp_num);
		}
		else {
			for (i = 0; i < building_names.size(); i++) {
				if (m_room_array[y][1].compare(building_names[i]) == 0) {
					matched = true;
					break;
				}
			}
			if (!matched) {
				temp_num = std::atoi(m_room_array[y][0].c_str());
				building_names.push_back(m_room_array[y][1]);
				buiding_no.push_back(temp_num);
			}
		}
	}

	m_eu_buildings = std::vector<EUBuilding>(building_names.size());

	for (i = 0; i < building_names.size(); i++) {
		m_eu_buildings[i].build_no = buiding_no[i];
		m_eu_buildings[i].building_name = building_names[i];
		m_eu_buildings[i].building_beds = 0;
		m_eu_buildings[i].building_rooms = 0;
	}

	std::sort(m_eu_buildings.begin(), m_eu_buildings.end(), BDbyNumber);
	return 0;
}

int BuildingRoomList::updateAllSections(char dataformat)
{
	int32_t i, j, k;
	std::string temp;
	int32_t temp_num, bed_num;
	EUSection sect;
	bool matched;

	for (i = 1; i < m_room_array.size(); i++){
		matched = false;
		if (dataformat == ',') {
			temp_num = std::atoi(m_room_array[i][0].c_str());
			temp = m_room_array[i][1];
			assert(temp.compare(m_eu_buildings[temp_num - 1].building_name) == 0);
			m_eu_buildings[temp_num - 1].building_distance = std::stoi(m_room_array[i][12]);
			m_eu_buildings[temp_num - 1].ac = std::stoi(m_room_array[i][15]) > 0;
			m_eu_buildings[temp_num - 1].fridge = std::stoi(m_room_array[i][16]) > 0;
			m_eu_buildings[temp_num - 1].elevator = std::stoi(m_room_array[i][17]) > 0;
			bed_num = std::atoi(m_room_array[i][9].c_str());
			m_eu_buildings[temp_num - 1].building_rooms++;
			m_eu_buildings[temp_num - 1].building_beds += bed_num;
		}
		else {
			temp_num = std::atoi(m_room_array[i][0].substr(1, m_room_array[i][0].size() - 2).c_str());
			temp = m_room_array[i][1].substr(1, m_room_array[i][1].size() - 2);
			assert(temp.compare(m_eu_buildings[temp_num - 1].building_name) == 0);
			m_eu_buildings[temp_num - 1].building_distance = std::stoi(m_room_array[i][12].substr(1, m_room_array[i][11].size() - 2));
			m_eu_buildings[temp_num - 1].ac = std::stoi(m_room_array[i][14].substr(1, m_room_array[i][15].size() - 2)) > 0;
			m_eu_buildings[temp_num - 1].fridge = std::stoi(m_room_array[i][15].substr(1, m_room_array[i][16].size() - 2)) > 0;
			m_eu_buildings[temp_num - 1].elevator = std::stoi(m_room_array[i][16].substr(1, m_room_array[i][17].size() - 2)) > 0;
			bed_num = std::atoi(m_room_array[i][9].substr(1, m_room_array[i][9].size() - 2).c_str());
			m_eu_buildings[temp_num - 1].building_rooms++;
			m_eu_buildings[temp_num - 1].building_beds += bed_num;
		}

		if (m_eu_buildings[temp_num - 1].sects.empty()) {
			sect.name = m_room_array[i][2] + "_" + m_room_array[i][5];
			sect.direction = m_room_array[i][2];
			sect.level = std::atoi(m_room_array[i][5].c_str());
			sect.building = (void *) (&m_eu_buildings[temp_num - 1]);
			m_eu_buildings[temp_num - 1].sects.push_back(sect);
		}
		else {
			for (j = 0; j < m_eu_buildings[temp_num - 1].sects.size(); j++) {
				std::string temp_name = m_room_array[i][2] + "_" + m_room_array[i][5];
				if (temp_name.compare(m_eu_buildings[temp_num - 1].sects[j].name) == 0) {
					matched = true;
					break;
				}
			}
			if (!matched) {
				sect.name = m_room_array[i][2] + "_" + m_room_array[i][5];
				sect.direction = m_room_array[i][2];
				sect.level = std::atoi(m_room_array[i][5].c_str());
				sect.building = (void *)(&m_eu_buildings[temp_num - 1]);
				m_eu_buildings[temp_num - 1].sects.push_back(sect);
			}
		}
	}

	for (i = 0; i < m_eu_buildings.size(); i++) {
		EUBuilding eub = m_eu_buildings[i];
		std::vector<std::string> sect_list;
		sect_list.empty();
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			EUSection eus = eub.sects[j];
			std::string sect_name = eus.direction;
			if (sect_list.size() == 0)
				sect_list.push_back(sect_name);
			else {
				bool matched = false;
				for (k = 0; k < sect_list.size(); k++) {
					if (sect_list[k].compare(sect_name) == 0) {
						matched = true;
						break;
					}
				}
				if(!matched)
					sect_list.push_back(sect_name);
			}
		}
		std::sort(sect_list.begin(), sect_list.end());
		std::map<std::string, int32_t> sect_codec;
		for (j = 0; j < sect_list.size(); j++)
			sect_codec[sect_list[j]] = j;
		for (k = 0; k < m_eu_buildings[i].sects.size(); k++) {
			EUSection eus = eub.sects[k];
			m_eu_buildings[i].sects[k].sect_number = sect_codec[eus.direction];
		}
	}

	return 0;
}


int BuildingRoomList::updateAllRooms_quotes()
{
	int32_t i, j;
	std::string temp;
	int32_t build_num;
	EUSection sect;
	EURoom aroom;
	bool matched;

	for (i = 0; i < m_eu_buildings.size(); i++) {
		m_eu_buildings[i].building_rooms = 0;
		m_eu_buildings[i].building_beds = 0;
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			m_eu_buildings[i].sects[j].sect_rooms = 0;
			m_eu_buildings[i].sects[j].sect_beds = 0;
		}
	}

	for (i = 1; i < m_room_array.size(); i++){
		matched = false;
		build_num = std::atoi(m_room_array[i][0].substr(1, m_room_array[i][0].size() - 2).c_str());
		temp = m_room_array[i][1].substr(1, m_room_array[i][1].size() - 2);
		assert(temp.compare(m_eu_buildings[build_num - 1].building_name) == 0);
		temp = m_room_array[i][2].substr(1, m_room_array[i][2].size() - 2) + "_" + m_room_array[i][5].substr(1, m_room_array[i][5].size() - 2);
		for (j = 0; j < m_eu_buildings[build_num - 1].sects.size(); j++) {
			if (m_eu_buildings[build_num - 1].sects[j].name.compare(temp) == 0) {
				aroom.room_id = std::atoi(m_room_array[i][3].substr(1, m_room_array[i][3].size() - 2).c_str());
				aroom.room = m_room_array[i][4].substr(1, m_room_array[i][4].size() - 2).c_str();
				aroom.room_type = m_room_array[i][6].substr(1, m_room_array[i][6].size() - 2).c_str();
				aroom.bathroom = m_room_array[i][7].substr(1, m_room_array[i][7].size() - 2).c_str();
				aroom.room_status = m_room_array[i][8].substr(1, m_room_array[i][8].size() - 2).c_str();
				aroom.capacity = std::atoi(m_room_array[i][9].substr(1, m_room_array[i][9].size() - 2).c_str());
				aroom.bed_assigned = std::atoi(m_room_array[i][10].substr(1, m_room_array[i][10].size() - 2).c_str());
				aroom.bath_distance = std::atoi(m_room_array[i][13].substr(1, m_room_array[i][13].size() - 2).c_str());
				aroom.room_conditions = std::atoi(m_room_array[i][18].substr(1, m_room_array[i][18].size() - 2).c_str());
				aroom.score = std::atoi(m_room_array[i][14].substr(1, m_room_array[i][14].size() - 2).c_str());
				aroom.extra = std::atoi(m_room_array[i][11].substr(1, m_room_array[i][11].size() - 2).c_str());
				aroom.section = (void*)(&m_eu_buildings[build_num - 1].sects[j]);
				aroom.building = (void*)(&m_eu_buildings[build_num - 1]);
				aroom.stype = SexType::eNull;
				aroom.persons.clear();
				m_eu_buildings[build_num - 1].sects[j].rooms.push_back(aroom);
				if (aroom.room_status.compare(RoomStatus::qInactive) == 0)
					m_total_inactive_rooms++;

				//if (aroom.room_status.compare(RoomStatus::qInactive) != 0) 
				{
					m_eu_buildings[build_num - 1].sects[j].sect_rooms++;
					m_eu_buildings[build_num - 1].sects[j].sect_beds += aroom.capacity;
				}
			}
		}
	}

	for (i = 0; i < m_eu_buildings.size(); i++) {
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			m_eu_buildings[i].building_rooms += m_eu_buildings[i].sects[j].sect_rooms;
			m_eu_buildings[i].building_beds += m_eu_buildings[i].sects[j].sect_beds;
		}
		m_total_rooms += m_eu_buildings[i].building_rooms;
		m_total_beds += m_eu_buildings[i].building_beds;
	}

	return 0;
}

int BuildingRoomList::updateAllRooms_commas()
{
	int32_t i, j;
	std::string temp;
	int32_t build_num;
	EUSection sect;
	EURoom aroom;
	bool matched;

	for (i = 0; i < m_eu_buildings.size(); i++) {
		m_eu_buildings[i].building_rooms = 0;
		m_eu_buildings[i].building_beds = 0;
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			m_eu_buildings[i].sects[j].sect_rooms = 0;
			m_eu_buildings[i].sects[j].sect_beds = 0;
		}
	}

	for (i = 1; i < m_room_array.size(); i++){
		matched = false;
		build_num = std::atoi(m_room_array[i][0].c_str());
		temp = m_room_array[i][1];
		assert(temp.compare(m_eu_buildings[build_num - 1].building_name) == 0);
		temp = m_room_array[i][2] + "_" + m_room_array[i][5];
		for (j = 0; j < m_eu_buildings[build_num - 1].sects.size(); j++) {
			if (m_eu_buildings[build_num - 1].sects[j].name.compare(temp) == 0) {
				aroom.room_id = std::atoi(m_room_array[i][3].c_str());
				aroom.room = m_room_array[i][4].c_str();
				aroom.room_type = m_room_array[i][6].c_str();
				aroom.bathroom = m_room_array[i][7].c_str();
				aroom.room_status = m_room_array[i][8].c_str();
				aroom.capacity = std::atoi(m_room_array[i][9].c_str());
				aroom.bed_assigned = std::atoi(m_room_array[i][10].c_str());
				aroom.bath_distance = std::atoi(m_room_array[i][13].c_str());
				aroom.room_conditions = std::atoi(m_room_array[i][18].c_str());
				aroom.score = std::atoi(m_room_array[i][14].c_str());
				aroom.extra = std::atoi(m_room_array[i][11].c_str());
				aroom.section = (void*)(&m_eu_buildings[build_num - 1].sects[j]);
				aroom.building = (void*)(&m_eu_buildings[build_num - 1]);
				aroom.stype = SexType::eNull;
				aroom.persons.clear();
				m_eu_buildings[build_num - 1].sects[j].rooms.push_back(aroom);
				if (aroom.room_status.compare(RoomStatus::qInactive) == 0)
					m_total_inactive_rooms++;
				//if (aroom.room_status.compare(RoomStatus::qInactive) != 0) 
				{
					m_eu_buildings[build_num - 1].sects[j].sect_rooms++;
					m_eu_buildings[build_num - 1].sects[j].sect_beds += aroom.capacity;
				}
			}
		}
	}

	for (i = 0; i < m_eu_buildings.size(); i++) {
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			m_eu_buildings[i].building_rooms += m_eu_buildings[i].sects[j].sect_rooms;
			m_eu_buildings[i].building_beds += m_eu_buildings[i].sects[j].sect_beds;
		}
		m_total_rooms += m_eu_buildings[i].building_rooms;
		m_total_beds += m_eu_buildings[i].building_beds;
	}

	return 0;
}

int BuildingRoomList::accumulateRoomInfo()
{
	if (m_eu_buildings.size() == 0)
		return -1;
	
	findNeighbors();
	computeRoomStats();
	makeRoomListPerBuilding();
	scoreRooms();
	return 0;
}

void BuildingRoomList::findNeighbors()
{
	int m;
	if (m_eu_buildings.size() == 0)
		return;
	std::string rm_pre, rm_num, room_a, room_b;
	for (int i = 0; i < m_eu_buildings.size(); i++) {
		EUBuilding eub = m_eu_buildings[i];
		for (int j = 0; j < eub.sects.size(); j++) {
			EUSection eus = eub.sects[j];
			for (int k = 0; k < eus.rooms.size(); k++) {
				EURoom eur = eus.rooms[k];
				std::string room_name = eur.room;
				std::string bath_name = eur.bathroom;
				if (!bath_name.empty() && bath_name.compare(" ")!=0) {
					size_t pos0 = room_name.find("-");
					rm_pre = room_name.substr(0, pos0);
					rm_num = room_name.substr(pos0 + 1);
					size_t pos1 = bath_name.find("-");
					room_a.clear();
					room_b.clear();
					if (pos1 != std::string::npos) {
						room_a = bath_name.substr(0, pos1);
						room_b = bath_name.substr(pos1 + 1);
						size_t pos2 = room_b.find(" ");
						if (pos2 != std::string::npos)
							room_b = room_b.substr(0, pos2);
					}
					if (rm_num.compare(room_a) == 0) {
						rm_pre = rm_pre + "-" + room_b;
						for (m = 0; m < eus.rooms.size(); m++) {
							EURoom aeur = eus.rooms[m];
							if (aeur.room.compare(rm_pre) == 0)
								break;
						}
						if (m != eus.rooms.size()) {
							EURoom *euroom = &m_eu_buildings[i].sects[j].rooms[m];
							m_eu_buildings[i].sects[j].rooms[k].neighbors.push_back(euroom);
						}
					}
					else if (rm_num.compare(room_b) == 0) {
						rm_pre = rm_pre + "-" + room_a;
						for (m = 0; m < eus.rooms.size(); m++) {
							EURoom aeur = eus.rooms[m];
							if (aeur.room.compare(rm_pre) == 0)
								break;
						}
						if (m != eus.rooms.size()) {
							EURoom *euroom = &m_eu_buildings[i].sects[j].rooms[m];
							m_eu_buildings[i].sects[j].rooms[k].neighbors.push_back(euroom);
						}
					}
				}
			}
		}
	}
	return;
}

void BuildingRoomList::computeRoomStats()
{
	if (m_eu_buildings.size() == 0)
		return;

	for (int i = 0; i < m_eu_buildings.size(); i++) {
		EUBuilding eub = m_eu_buildings[i];
		RoomStats rs;
		resetStats(rs);
		rs.total_beds = eub.building_beds;
		rs.total_rooms = eub.building_rooms;
		for (int j = 0; j < eub.sects.size(); j++) {
			EUSection eus = eub.sects[j];
			for (int k = 0; k < eus.rooms.size(); k++) {
				EURoom eur = eus.rooms[k];
				int32_t reserved = checkRoomStatus(eur.room_status, RoomStatus::qReservedCH)
					+ checkRoomStatus(eur.room_status, RoomStatus::qReservedGE)
					+ checkRoomStatus(eur.room_status, RoomStatus::qReservedSK)
					+ checkRoomStatus(eur.room_status, RoomStatus::qReservedSP);

				bool inactive_flag = (checkRoomStatus(eur.room_status, RoomStatus::qInactive) == 1);
				if (inactive_flag) {
					rs.total_inactive_rooms++;
					rs.total_inactive_beds += eur.capacity;
				}
				else {

					m_room_list_by_id[eur.room_id] = &m_eu_buildings[i].sects[j].rooms[k];
					m_room_list_by_roomname[eur.room] = &m_eu_buildings[i].sects[j].rooms[k];

					if (eur.neighbors.size() > 0)
						rs.total_shared_family_beds++;

					if (checkRoomType(eur.room_type, RoomTypes::qFamily_Private)) {
						rs.total_family_rooms++;
						rs.total_family_beds += eur.capacity;
					}
					if (checkRoomType(eur.room_type, RoomTypes::qMale)) {
						rs.total_male_rooms++;
						rs.total_male_beds += eur.capacity;
					}

					if (checkRoomType(eur.room_type, RoomTypes::qFemale)) {
						rs.total_female_rooms++;
						rs.total_female_beds += eur.capacity;
					}

					if (checkRoomStatus(eur.room_status, RoomStatus::qAvailable)) {
						rs.total_available_rooms++;
						rs.total_available_beds += eur.capacity;
					}

					if (checkRoomStatus(eur.room_status, RoomStatus::qFullyAssigned)) {
						rs.total_assigned_rooms++;
						rs.total_assigned_beds += eur.bed_assigned;
					}

					if (checkRoomStatus(eur.room_status, RoomStatus::qPartiallyAssigned)) {
						rs.total_partial_assigned_rooms++;
					}

					if (reserved) {
						rs.total_reserved_rooms += reserved;
						rs.total_reserved_beds += eur.capacity;
					}
					rs.total_extra_beds += eur.extra; 

					rs.total_assigned_rooms += int32_t(eur.bed_assigned > 0);
					rs.total_assigned_beds += eur.bed_assigned;
					rs.total_partial_assigned_rooms += int32_t((eur.bed_assigned > 0) && (eur.bed_assigned < eur.capacity));
				}
			}
		}
		m_room_stats[eub.building_name] = rs;
	}

	return;
}


void BuildingRoomList::makeRoomListPerBuilding()
{
	if (m_eu_buildings.size() == 0)
		return;

	for (int i = 0; i < m_eu_buildings.size(); i++) {
		EUBuilding eub = m_eu_buildings[i];
		std::vector<EURoom*> room_list;
		std::vector<EURoom*> family_room_list;
		std::vector<EURoom*> male_room_list;
		std::vector<EURoom*> female_room_list;
		std::vector<EURoom*> reserved_room_list;
		std::vector<EURoom*> available_room_list;
		std::vector<EURoom*> ReservedGE_room_list;
		std::vector<EURoom*> ReservedSK_room_list;
		std::vector<EURoom*> ReservedSP_room_list;
		std::vector<EURoom*> ReservedCH_room_list;
		std::vector<EURoom*> fully_assigned_room_list;
		std::vector<EURoom*> partially_assigned_room_list;
		std::vector<EURoom*> extra_bed_room_list;

		std::vector<EURoom*> room_1person_list;
		std::vector<EURoom*> room_2person_list;
		std::vector<EURoom*> room_3person_list;
		std::vector<EURoom*> room_4person_list;
		std::vector<EURoom*> room_5person_list;

		for (int j = 0; j < eub.sects.size(); j++) {
			EUSection eus = eub.sects[j];
			for (int k = 0; k < eus.rooms.size(); k++) {
				EURoom eur = eus.rooms[k];
				int32_t reserved = checkRoomStatus(eur.room_status, RoomStatus::qReservedCH)
					+ checkRoomStatus(eur.room_status, RoomStatus::qReservedGE)
					+ checkRoomStatus(eur.room_status, RoomStatus::qReservedSK)
					+ checkRoomStatus(eur.room_status, RoomStatus::qReservedSP);
				bool inactive_flag = (checkRoomStatus(eur.room_status, RoomStatus::qInactive) == 1);
				if (!inactive_flag) {

					room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);

					if (eur.capacity == 1)
						room_1person_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					else if (eur.capacity == 2)
						room_2person_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					else if (eur.capacity == 3)
						room_3person_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					else if (eur.capacity == 4)
						room_4person_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					else if (eur.capacity == 5)
						room_5person_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);

					if (checkRoomType(eur.room_type, RoomTypes::qFamily_Private)) {
						family_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}
					if (checkRoomType(eur.room_type, RoomTypes::qMale)) {
						male_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}

					if (checkRoomType(eur.room_type, RoomTypes::qFemale)) {
						female_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}

					if (checkRoomStatus(eur.room_status, RoomStatus::qAvailable)) {
						available_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}

					if (checkRoomStatus(eur.room_status, RoomStatus::qFullyAssigned)) {
						fully_assigned_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}

					if (checkRoomStatus(eur.room_status, RoomStatus::qPartiallyAssigned)) {
						partially_assigned_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}

					if (checkRoomStatus(eur.room_status, RoomStatus::qReservedCH)) {
						ReservedCH_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}
					if (checkRoomStatus(eur.room_status, RoomStatus::qReservedGE)) {
						ReservedGE_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}
					if (checkRoomStatus(eur.room_status, RoomStatus::qReservedSK)) {
						ReservedSK_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}
					if (checkRoomStatus(eur.room_status, RoomStatus::qReservedSP)) {
						ReservedSP_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
					}
					if (reserved)
						reserved_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);

					if (eur.extra > 0)
						extra_bed_room_list.push_back(&m_eu_buildings[i].sects[j].rooms[k]);
				}
			}
		}
		m_room_list_per_building[eub.building_name] = room_list;
		m_family_rooms_array[eub.building_name] = family_room_list;
		m_male_rooms_array[eub.building_name] = male_room_list;
		m_female_rooms_array[eub.building_name] = female_room_list;
		m_reserved_rooms_array[eub.building_name] = reserved_room_list;
		m_available_rooms_array[eub.building_name] = available_room_list;

		m_ReservedGE_rooms[eub.building_name] = ReservedGE_room_list;
		m_ReservedSK_rooms[eub.building_name] = ReservedSK_room_list;
		m_ReservedSP_rooms[eub.building_name] = ReservedSP_room_list;
		m_ReservedCH_rooms[eub.building_name] = ReservedCH_room_list;
		m_FullyAssigned_rooms[eub.building_name] = fully_assigned_room_list;
		m_PartiallyAssigned[eub.building_name] = partially_assigned_room_list;
		m_ExtraBeds[eub.building_name] = extra_bed_room_list;

		m_1person_rooms[eub.building_name] = room_1person_list;
		m_2person_rooms[eub.building_name] = room_2person_list;
		m_3person_rooms[eub.building_name] = room_3person_list;
		m_4person_rooms[eub.building_name] = room_4person_list;
		m_5person_rooms[eub.building_name] = room_5person_list;
	}
	return;
}


int BuildingRoomList::scoreRooms()
{
	int32_t score[3] = { 0,0,0 };
	if (m_eu_buildings.size() == 0)
		return -1;

	for (int i = 0; i < m_eu_buildings.size(); i++) {
		EUBuilding eub = m_eu_buildings[i];
		score[0] = m_buildingCode[eub.building_name] * m_room_measure.brBuilding;
		for (int j = 0; j < eub.sects.size(); j++) {
			EUSection eus = eub.sects[j];
			int32_t level = eus.level;
			score[1]= eus.sect_number*m_room_measure.brSect + m_OpLevel[level] * m_room_measure.brLevel; // *(1 - (int32_t)eub.elevator);
			for (int k = 0; k < eus.rooms.size(); k++) {
				size_t pos = std::string::npos;
				EURoom eur = eus.rooms[k];
				pos = eur.room.find_first_of("-");
				std::string room_num = eur.room.substr(pos + 1, -1);
				int value;
				char c;
				std::stringstream ss(room_num);
				if (eub.building_name.compare(BuildingNames::qGallup) == 0)
					ss >> c >> value;
				else
					ss >> value >> c;
				value = value % 100;
				score[2]= m_room_measure.brRoom*value;
				if (eur.neighbors.size() > 0) {
					pos = eur.neighbors[0]->room.find_first_of("-");
					room_num = eur.neighbors[0]->room.substr(pos + 1, -1);
					std::stringstream ss_2(room_num);
					if (eub.building_name.compare(BuildingNames::qGallup) == 0)
						ss_2 >> c >> value;
					else
						ss_2 >> value >> c;
					value = value % 100;
					int32_t score_neighbor = m_room_measure.brRoom * value;
					score[2] = (score[2] + score_neighbor) / 2 + 1;
				}
				int32_t final_score = score[0] + score[1] + score[2];
#if 0
				printf("%s %d\n", eur.room.c_str(), final_score);
#endif
				m_eu_buildings[i].sects[j].rooms[k].score = final_score;
				m_room_list_by_score[final_score].push_back(&m_eu_buildings[i].sects[j].rooms[k]);
			}
		}
	}

	return 0;
}

void BuildingRoomList::resetStats(RoomStats &rs)
{
	rs.total_assigned_beds = 0;
	rs.total_assigned_rooms = 0;
	rs.total_available_beds = 0;
	rs.total_available_rooms = 0;
	rs.total_beds = 0;
	rs.total_extra_beds = 0;
	rs.total_family_beds = 0; 
	rs.total_family_rooms = 0;
	rs.total_female_beds = 0;
	rs.total_female_rooms = 0;
	rs.total_male_beds = 0;
	rs.total_male_rooms = 0;
	rs.total_partial_assigned_rooms = 0;
	rs.total_reserved_beds = 0;
	rs.total_reserved_rooms = 0;
	rs.total_rooms = 0;
	rs.total_inactive_rooms = 0;
	rs.total_inactive_beds = 0;
	rs.total_shared_family_beds = 0;
}

int BuildingRoomList::checkRoomType(const std::string rt, const std::string art)
{
	if (rt.compare(art) == 0)
		return 1;
	else
		return 0;
}

int BuildingRoomList::checkRoomStatus(const std::string rs, const std::string ars)
{
	if (rs.compare(ars) == 0)
		return 1;
	else
		return 0;
}

BuildingRoomList::EURoom *BuildingRoomList::checkRoomState(EURoom *eur, RoomState rs)
{
	std::string nm = getStringNames(rs);

	if (nm.compare(eur->room_status) == 0) {
		return eur;
	}
	else if (nm.compare(eur->room_type) == 0) {
		return eur;
	}

	return NULL;
}

std::string BuildingRoomList::getStringNames(BuildingRoomList::RoomState rs)
{
	std::string str;
	switch (rs) {
	case BuildingRoomList::tFamily_Private:
		str = RoomTypes::qFamily_Private;
		break;
	case BuildingRoomList::tFamily_Male:
		str = RoomTypes::qFamily_Male;
		break;
	case BuildingRoomList::tFamily_Female:
		str = RoomTypes::qFamily_Female;
		break;
	case BuildingRoomList::tMale:
		str = RoomTypes::qMale;
		break;
	case BuildingRoomList::tFemale:
		str = RoomTypes::qFemale;
		break;
	case BuildingRoomList::tAvailable:
		str = RoomStatus::qAvailable;
		break;
	case BuildingRoomList::tReservedGE:
		str = RoomStatus::qReservedGE;
		break;
	case BuildingRoomList::tReservedSK:
		str = RoomStatus::qReservedSK;
		break;
	case BuildingRoomList::tReservedSP:
		str = RoomStatus::qReservedSP;
		break;
	case BuildingRoomList::tReservedCH:
		str = RoomStatus::qReservedCH;
		break;
	case BuildingRoomList::tReservedCC:
		str = RoomStatus::qReservedCC;
		break;
	case BuildingRoomList::tFullyAssigned:
		str = RoomStatus::qFullyAssigned;
		break;
	case BuildingRoomList::tPartiallyAssigned:
		str = RoomStatus::qPartiallyAssigned;
		break;
	}
	return str;
}

int BuildingRoomList::printRoomStats()
{
	int32_t i, j;

	for (i = 0; i < m_eu_buildings.size(); i++) {
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			printf("%s %s sect=%d level=%d rooms=%d beds=%d\n", m_eu_buildings[i].building_name.c_str(), m_eu_buildings[i].sects[j].direction.c_str(),
				m_eu_buildings[i].sects[j].sect_number, m_eu_buildings[i].sects[j].level, m_eu_buildings[i].sects[j].sect_rooms, m_eu_buildings[i].sects[j].sect_beds);
		}
		printf("== %s rooms=%d beds=%d ==\n", m_eu_buildings[i].building_name.c_str(), m_eu_buildings[i].building_rooms, m_eu_buildings[i].building_beds);
	}

	printf("=== Total rooms = %d, Total beds = %d\n", m_total_rooms, m_total_beds);
	return 0;
}

int BuildingRoomList::writeUpdatedBuildingRoomDoc(const char* filename)
{
	int32_t i, j, k;
	if (filename == NULL)
		return -1;

	std::ofstream fout(filename);
	if (!fout.is_open())
		return -2;

	fout << "Building No," << "Building Name," << "Direction," << "Room_ID," << "Room," << "Level,"
		<< "Room Type," << "Bathroom," << "Room Status," << "Capacity," << "Bed Assigned," << "Extra,"
		<< "Building Distance," << "Bath Distance," << "Score," << "AC," << "Fridge," << "Elevator,"
		<< "Room Conditions," << "Sect," << "Note" << std::endl;

	for (i = 0; i < m_eu_buildings.size(); i++) {
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			for (k = 0; k < m_eu_buildings[i].sects[j].rooms.size(); k++) {
				BuildingRoomList::EURoom room = m_eu_buildings[i].sects[j].rooms[k];
				int32_t building_no = m_eu_buildings[i].build_no;
				std::string building_name = m_eu_buildings[i].building_name;
				std::string direction = m_eu_buildings[i].sects[j].direction;
				int32_t sect_num = m_eu_buildings[i].sects[j].sect_number;
				int32_t id = m_eu_buildings[i].sects[j].rooms[k].room_id;
				std::string rm = m_eu_buildings[i].sects[j].rooms[k].room;
				int32_t level = m_eu_buildings[i].sects[j].level;
				std::string rtype = m_eu_buildings[i].sects[j].rooms[k].room_type;
				std::string bath = m_eu_buildings[i].sects[j].rooms[k].bathroom;
				std::string status = m_eu_buildings[i].sects[j].rooms[k].room_status;
				int32_t capacity = m_eu_buildings[i].sects[j].rooms[k].capacity;
				int32_t num_assigned_beds = m_eu_buildings[i].sects[j].rooms[k].bed_assigned;
				int32_t extra_bed = m_eu_buildings[i].sects[j].rooms[k].extra;
				int32_t bath_distance = m_eu_buildings[i].sects[j].rooms[k].bath_distance;
				int32_t building_distance = m_eu_buildings[i].building_distance;
				int32_t score = m_eu_buildings[i].sects[j].rooms[k].score;
				bool ac = m_eu_buildings[i].ac;
				bool fridge = m_eu_buildings[i].fridge;
				bool elevator = m_eu_buildings[i].elevator;
				int32_t room_conditions = m_eu_buildings[i].sects[j].rooms[k].room_conditions;

				fout << building_no <<"," << building_name <<"," << direction << "," << id << "," << rm <<"," 
					<< level << "," << rtype << "," << bath+" " << "," << status <<"," << capacity << "," << num_assigned_beds <<"," 
					<< extra_bed <<"," << building_distance <<"," << bath_distance <<"," << score <<"," << ac <<"," << fridge <<"," 
					<< elevator <<"," << room_conditions <<"," << sect_num << "," << " " << std::endl;
			}
		}
	}

	fout.close();
	return 0;
}