# qfl
# Quest for Life repository for RCCC
# start 
env_v14.bat

room score calculation:

building code = building distance: 
Gough Hall = 0
Gallup = 1
SparrowK = 2
Eagle Hall = 3
Hainer = 4 
Kea = 5
Guffin = 6  

brBuilding = 10000
brSect = 1000
brLevel = 200
brRoom = 4
brElevator = 0
brAC = -5

sect_number: sort "Direction" and assign index

score[0] = m_buildingCode[eub.building_name] * m_room_measure.brBuilding;
score[1]= eus.sect_number*m_room_measure.brSect + m_OpLevel[level] * m_room_measure.brLevel;
score[2]= m_room_measure.brRoom * (room_number%100);

if(neighbor)
   score_neighbor = m_room_measure.brRoom * (room_number%100);
score[2] = (score[2] + score_neighbor) / 2 + 1;

final_score = score[0] + score[1] + score[2];


- How to run:
roomassign.exe -c config/churchlist_20180430.csv -b config/buildingAndRoom-0509_2018.csv -r data/report1526266918506.csv -s results/sorted_report.csv -n results/report_new_assignment.csv -d results/report_church_per_building.csv