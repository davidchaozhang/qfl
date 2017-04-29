#include "buildings.h"
#include <fstream>
#include <algorithm>

static bool BDbyNumber(const BuildingRoomList::EUBuilding &a, const BuildingRoomList::EUBuilding &b)
{
	return (a.build_no < b.build_no);
}

BuildingRoomList::BuildingRoomList()
{
	m_total_rooms = 0;
	m_total_beds = 0;
}

BuildingRoomList::~BuildingRoomList()
{ }

int BuildingRoomList::readInBuildingList(const char* building_rooms_list)
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

int BuildingRoomList::updateAllSections()
{
	int32_t i, j;
	std::string temp;
	int32_t temp_num;
	EUSection sect;
	bool matched;

	for (i = 1; i < m_room_array.size(); i++){
		matched = false;
		temp_num = std::atoi(m_room_array[i][0].substr(1, m_room_array[i][0].size() - 2).c_str());
		temp = m_room_array[i][1].substr(1, m_room_array[i][1].size() - 2);
		assert(temp.compare(m_eu_buildings[temp_num - 1].building_name) == 0);

		m_eu_buildings[temp_num - 1].building_distance = std::stoi(m_room_array[i][11].substr(1, m_room_array[i][11].size() - 2));
		m_eu_buildings[temp_num - 1].ac = std::stoi(m_room_array[i][14].substr(1, m_room_array[i][14].size() - 2)) > 0;
		m_eu_buildings[temp_num - 1].fridge = std::stoi(m_room_array[i][15].substr(1, m_room_array[i][15].size() - 2)) > 0;
		m_eu_buildings[temp_num - 1].elevator = std::stoi(m_room_array[i][16].substr(1, m_room_array[i][16].size() - 2)) > 0;

		if (m_eu_buildings[temp_num - 1].sects.empty()) {
			sect.direction = m_room_array[i][2];
			sect.level = std::atoi(m_room_array[i][5].substr(1, m_room_array[i][5].size() - 2).c_str());
			m_eu_buildings[temp_num - 1].sects.push_back(sect);
		}
		else {
			for (j = 0; j < m_eu_buildings[temp_num - 1].sects.size(); j++) {
				if (m_room_array[i][2].compare(m_eu_buildings[temp_num - 1].sects[j].direction) == 0) {
					matched = true;
					break;
				}
			}
			if (!matched) {
				sect.direction = m_room_array[i][2];
				sect.level = std::atoi(m_room_array[i][5].substr(1, m_room_array[i][5].size() - 2).c_str());
				m_eu_buildings[temp_num - 1].sects.push_back(sect);
			}
		}
	}

	for (i = 0; i < m_eu_buildings.size(); i++) {
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			std::string temp = m_eu_buildings[i].sects[j].direction.substr(1, m_eu_buildings[i].sects[j].direction.size() - 2);
			m_eu_buildings[i].sects[j].direction = temp;
		}
	}
	return 0;
}


int BuildingRoomList::updateAllRooms()
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
		temp = m_room_array[i][2].substr(1, m_room_array[i][2].size() - 2);
		for (j = 0; j < m_eu_buildings[build_num - 1].sects.size(); j++) {
			if (m_eu_buildings[build_num - 1].sects[j].direction.compare(temp) == 0) {
				aroom.room_id = std::atoi(m_room_array[i][3].substr(1, m_room_array[i][3].size() - 2).c_str());
				aroom.room = m_room_array[i][4].substr(1, m_room_array[i][4].size() - 2).c_str();
				aroom.room_type = m_room_array[i][6].substr(1, m_room_array[i][6].size() - 2).c_str();
				aroom.room_status = m_room_array[i][7].substr(1, m_room_array[i][7].size() - 2).c_str();
				aroom.capacity = std::atoi(m_room_array[i][8].substr(1, m_room_array[i][8].size() - 2).c_str());
				aroom.bed_asigned = std::atoi(m_room_array[i][9].substr(1, m_room_array[i][9].size() - 2).c_str());
				aroom.bath_distance = std::atoi(m_room_array[i][12].substr(1, m_room_array[i][12].size() - 2).c_str());
				aroom.room_conditions = std::atoi(m_room_array[i][17].substr(1, m_room_array[i][17].size() - 2).c_str());
				aroom.score = std::atoi(m_room_array[i][13].substr(1, m_room_array[i][13].size() - 2).c_str());
				aroom.extra = std::atoi(m_room_array[i][10].substr(1, m_room_array[i][10].size() - 2).c_str());
				m_eu_buildings[build_num - 1].sects[j].rooms.push_back(aroom);
				if (aroom.room_status.compare(RoomStatus::qInactive) != 0) {
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

int BuildingRoomList::printRoomStats()
{
	int32_t i, j;

	for (i = 0; i < m_eu_buildings.size(); i++) {
		for (j = 0; j < m_eu_buildings[i].sects.size(); j++) {
			printf("%s %s rooms=%d beds=%d\n", m_eu_buildings[i].building_name.c_str(), m_eu_buildings[i].sects[j].direction.c_str(),
				m_eu_buildings[i].sects[j].sect_rooms, m_eu_buildings[i].sects[j].sect_beds);
		}
		printf("%s rooms=%d beds=%d\n", m_eu_buildings[i].building_name.c_str(), m_eu_buildings[i].building_rooms, m_eu_buildings[i].building_beds);
	}

	printf("total rooms = %d, total beds = %d\n", m_total_rooms, m_total_beds);
	return 0;
}