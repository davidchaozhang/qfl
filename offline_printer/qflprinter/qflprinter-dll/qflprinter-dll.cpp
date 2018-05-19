// qflprinter-dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/* HTML TEMPLATE FIELDS
ID_FIELD		- Individual ID
PARTY_FIELD		- Party ID
NAMECHN_FIELD	- Chinese Name (If applicable)
NAMEENG_FIELD	- English Name
CHURCH_FIELD	- Church Name (Cabriny vs. Other = English vs. Chinese?) Update this
GROUP_FIELD		- Group ID
QR_FIELD		- QR Code Link
*/

typedef struct {
	string ID; //Personal ID
	string partyID; //Party (Family) ID
	string churchNameENG; //Church Name (ENG)
	string contactName;
	string partyType;
	string fname; //First Name (ENG)
	string lname; //Last Name (ENG)
	string nameCHN; //Chinese Name (CHN)
	string room;
	string groupID; //Group ID
	string roomNeeded;
	string ageGroup;
	string gender;
	string grade;
	string regDue;
	string keyFee;
	string roomAssigned;
	string status;
	string isChristian;
	string services;
	string churchNameCHN; //Church Name (CHN)
	string needKey;
	string QRLink; //QR Code Link
} NameTag;

string ReturnCurrTagsReceipt(NameTag *tag, int cnt);
string ReturnCurrTags(NameTag& tag);
bool removeQuotes(string& str);
void fixTagStrings(NameTag& tag);
void fixQRCode(NameTag& tag);
bool fillField(string& templatestr, const string& fieldname, const string& tagfield);
extern "C"
{
	__declspec(dllexport) void OfflinePrint();
	__declspec(dllexport) void PrintTag(char* strCpy, int ID);
	__declspec(dllexport) void PrintReceipt(char* strCpy, int ID);
}

string ReturnCurrTagsReceipt(NameTag *tag, int cnt) {
	string str = "";
	for (int i = 0; i < cnt; i++) {
		if (i > 0)
			str += ",";
		str += (tag[i].ID //21 refs
			+ "," + tag[i].partyID
			+ "," + tag[i].churchNameENG
			+ "," + tag[i].contactName
			+ "," + tag[i].partyType
			+ "," + tag[i].fname
			+ "," + tag[i].lname
			+ "," + tag[i].nameCHN
			+ "," + tag[i].room
			+ "," + tag[i].groupID
			+ "," + tag[i].roomNeeded
			+ "," + tag[i].ageGroup
			+ "," + tag[i].gender
			+ "," + tag[i].grade
			+ "," + tag[i].regDue
			+ "," + tag[i].keyFee
			+ "," + tag[i].roomAssigned
			+ "," + tag[i].status
			+ "," + tag[i].isChristian
			+ "," + tag[i].services
			+ "," + tag[i].churchNameCHN
			+ "," + tag[i].needKey);
			//+ "," + tag[i].QRLink);
	}
	return str;
}

string ReturnCurrTags(NameTag& tag) {
	return (tag.ID
		+ "," + tag.partyID
		+ "," + tag.churchNameENG
		+ "," + tag.contactName
		+ "," + tag.partyType
		+ "," + tag.fname
		+ "," + tag.lname
		+ "," + tag.nameCHN
		+ "," + tag.room
		+ "," + tag.groupID
		+ "," + tag.roomNeeded
		+ "," + tag.ageGroup
		+ "," + tag.gender
		+ "," + tag.grade
		+ "," + tag.regDue
		+ "," + tag.keyFee
		+ "," + tag.roomAssigned
		+ "," + tag.status
		+ "," + tag.churchNameCHN
		+ "," + tag.needKey);
		//+ "," + tag.QRLink);
}

void PrintReceipt(char* strCpy, int partyID) {
	//constant values
	const char* data = "./qrres/qfl_offline-data.csv"; //input datasheet for all customers
	const char* pagetemplate = "./qrres/receipttemplate.txt"; //input the template html page with css for formatting
	const char* output = "./qrres/receipt_template.html"; //output to this html file

	int pagebreak = 0; //denotes when to break page
	NameTag tag;
	string tempconv;
	int cnt = 0;

	fstream datain(data, ios_base::in); //user data (.csv) data
	fstream dataout(output, ios_base::out); //output file (.html)
	NameTag tags[7]; //array of user data to put into HTML

	//Skip First Line
	getline(datain, tempconv, '\n');

	//Copy CSV into NameTags
	while (datain.good()) {
		getline(datain, tag.ID, ','); //A: Personal ID
		if (tag.ID.length() >= 10)
			break;
		getline(datain, tag.partyID, ','); //C: Party ID
		if (removeQuotes(tag.partyID) || tag.partyID.at(0) == 'P')
			tag.partyID.erase(0, 1);
		if (stoi(tag.partyID) == partyID) {
			getline(datain, tag.churchNameENG, ','); //G: Insert Church Name in English
			getline(datain, tag.contactName, ','); //H: Insert Contact Name
			getline(datain, tag.partyType, ',');
			getline(datain, tag.fname, ','); //E: Insert First Name
			getline(datain, tag.lname, ','); //D: Insert Last Name	
			getline(datain, tag.nameCHN, ','); //F: Insert Chinese Name
			getline(datain, tag.room, ','); //F: Insert Room Name
			getline(datain, tag.groupID, ','); //J: Insert Group ID
			getline(datain, tag.roomNeeded, ',');
			getline(datain, tag.ageGroup, ',');
			getline(datain, tag.gender, ',');
			getline(datain, tag.grade, ','); //age group
			getline(datain, tag.regDue, ',');
			getline(datain, tag.keyFee, ',');
			getline(datain, tag.roomAssigned, ',');
			getline(datain, tag.status, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tag.isChristian, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tag.services, ',');
			getline(datain, tag.needKey, ',');
			getline(datain, tag.churchNameCHN, ','); //G: Insert Church Name in Chinese
			getline(datain, tag.QRLink, '\n'); //K: QR Code Link (from Google API)
			fixTagStrings(tag);
			tags[cnt] = tag; //push extracted NameTag to vector
			++cnt;
		} else {
			getline(datain, tempconv, '\n');
		}
	}
	datain.close();

	if (cnt <= 0) {
		string str = ReturnCurrTagsReceipt(tags, cnt);
		strcpy(strCpy, str.c_str());
		return;
	}

	//COPY FORMATTING OF TEMPLATE FILE
	fstream fheader(pagetemplate, ios_base::in);
	string line;
	while (getline(fheader, line, '\n')) {
		if (line.find("KEYWORD: HEADER END") != string::npos) //loop until program finds this commented keyword to denote end of css and formatting
			break;
		if (line.find("PARTY_FIELD") != string::npos && cnt > 0)
			fillField(line, "PARTY_FIELD", tags[0].partyID); //fill in Party ID Field
		dataout << line << '\n';
	}

	//COPY HTMLTEMPLATE INTO STRING
	string divtemplate = "";
	while (getline(fheader, line, '\n')) {
		if (line.find("KEYWORD: DIV END") != string::npos) //loop until program find this commented keyword to denote end of template
			break;
		line.append("\n"); //add new line to ending
		divtemplate.append(line); //append full line plus new line character to template
	}

	//LOOP BODY
	string modtemplate;
	for (int i = 0; i < cnt; i++) {
		modtemplate = divtemplate; //copy template onto this string
		fillField(modtemplate, "ID_FIELD", tags[i].ID); //fill in ID Field
		fillField(modtemplate, "NAMEENG_FIELD", tags[i].fname + " " + tags[i].lname); //fill in Individual Name Field
		fillField(modtemplate, "ROOM_FIELD", tags[i].room); //fill in Room name
		//fillField(modtemplate, "KEYSTATUS_FIELD", tags[i].needKey); //fill in Key Status Field
		//fillField(modtemplate, "CARDSTATUS_FIELD", tags[i].needKey); //fill in Card Field
		dataout << modtemplate; //output modified template with parameters filled in
	}
	//END OF FILE
	while (getline(fheader, line, '\n')) {
		if (line.find("QR_FIELD") != string::npos)
			fixQRCode(tag);
		fillField(line, "QR_FIELD", tag.QRLink); //fill in Party ID Field
		dataout << line << '\n';
	}
	fheader.close();
	dataout.close();
	string str = ReturnCurrTagsReceipt(tags, cnt);
	strcpy(strCpy, str.c_str());
}

void PrintTag(char* strCpy, int ID) {
	//std::string IDstr(ID);
	const char* data = "./qrres/qfl_offline-data.csv"; //input datasheet for all customers
	const char* pagetemplate = "./qrres/webtemplate.txt"; //input the template html page with css for formatting
	const char* output = "./qrres/printtag.html"; //output to this html file

	NameTag tag;
	string tempconv;

	fstream datain(data, ios_base::in); //user data (.csv) data
	fstream dataout(output, ios_base::out); //output file (.html)

	//Skip First Line
	getline(datain, tempconv, '\n');

	//Copy CSV into NameTags
	while (datain.good()) {
		getline(datain, tag.ID, ','); //A: Personal ID
		removeQuotes(tag.ID);
		if (stoi(tag.ID) == ID) {
			getline(datain, tag.partyID, ','); //C: Party ID
			getline(datain, tag.churchNameENG, ','); //G: Insert Church Name in English
			getline(datain, tag.contactName, ','); //H: Insert Contact Name
			getline(datain, tag.partyType, ',');
			getline(datain, tag.fname, ','); //E: Insert First Name
			getline(datain, tag.lname, ','); //D: Insert Last Name	
			getline(datain, tag.nameCHN, ','); //F: Insert Chinese Name
			getline(datain, tag.room, ','); //F: Insert Room Name
			getline(datain, tag.groupID, ','); //J: Insert Group ID
			getline(datain, tag.roomNeeded, ',');
			getline(datain, tag.ageGroup, ',');
			getline(datain, tag.gender, ',');
			getline(datain, tag.grade, ','); //age group
			getline(datain, tag.regDue, ',');
			getline(datain, tag.keyFee, ',');
			getline(datain, tag.roomAssigned, ',');
			getline(datain, tag.status, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tag.isChristian, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tempconv, ',');
			getline(datain, tag.services, ',');
			getline(datain, tag.needKey, ',');
			getline(datain, tag.churchNameCHN, ','); //G: Insert Church Name in Chinese
			getline(datain, tag.QRLink, '\n'); //K: QR Code Link (from Google API)
			fixTagStrings(tag);
			break;
		}
		getline(datain, tempconv, '\n');
	}
	datain.close();
	//COPY FORMATTING OF TEMPLATE FILE
	fstream fheader(pagetemplate, ios_base::in);
	string line;
	while (getline(fheader, line, '\n')) {
		if (line.find("KEYWORD: HEADER END") != string::npos) //loop until program finds this commented keyword to denote end of css and formatting
			break;
		dataout << line << '\n';
	}

	//COPY HTMLTEMPLATE INTO STRING
	string divtemplate = "";
	while (getline(fheader, line, '\n')) {
		if (line.find("KEYWORD: DIV END") != string::npos) //loop until program find this commented keyword to denote end of template
			break;
		line.append("\n"); //add new line to ending
		divtemplate.append(line); //append full line plus new line character to template
	}
	fheader.close();

	//LOOP BODY
	string modtemplate;
	modtemplate = divtemplate; //copy template onto this string
	fillField(modtemplate, "ID_FIELD", "ID: " + tag.ID); //fill in ID Field
	fillField(modtemplate, "PARTY_FIELD", tag.partyID); //fill in Party ID Field
	fillField(modtemplate, "NAMECHN_FIELD", tag.nameCHN); //fill in Chinese Name Field
	fillField(modtemplate, "NAMEENG_FIELD", tag.fname + " " + tag.lname); //fill in Individual Name Field
	if (tag.churchNameCHN.length() > 0)
		fillField(modtemplate, "CHURCH_FIELD", tag.churchNameCHN); //fill in Church Name Field
	else
		fillField(modtemplate, "CHURCH_FIELD", tag.churchNameENG); //if chinese name doesn't exist
	fillField(modtemplate, "GROUP_FIELD", "Group: " + tag.groupID); //fill in Group ID Field
	fillField(modtemplate, "ROOM_FIELD", "Room: " + tag.room); //fill in Group ID Field
	fixQRCode(tag);
	fillField(modtemplate, "QR_FIELD", tag.QRLink); //fill in QR Link Field
	dataout << modtemplate; //output modified template with parameters filled in

	//END OF FILE
	dataout << "</body>\n</html>" << endl; //closing code for html doc
	dataout.close();
	string str = ReturnCurrTags(tag);
	int strSize = str.length();
	strcpy(strCpy, str.c_str());
}

/*
Pre-Printing (Offline) Using Excel
*/
void OfflinePrint() {
	//constant values
	const char* data = "./qrres/qfl_offline-data.csv"; //input datasheet for all customers
	const char* pagetemplate = "./qrres/webtemplate.txt"; //input the template html page with css for formatting
	const char* output = "./qrres/printout.html"; //output to this html file

	int pagebreak = 0; //denotes when to break page
	NameTag tag;
	string tempconv;

	fstream datain(data, ios_base::in); //user data (.csv) data
	fstream dataout(output, ios_base::out); //output file (.html)
	vector<NameTag> tags; //array of user data to put into HTML

						  //Skip First Line
	getline(datain, tempconv, '\n');

	//Copy CSV into NameTags
	while (datain.good()) {
		getline(datain, tag.ID, ','); //A: Personal ID
		getline(datain, tag.partyID, ','); //C: Party ID
		getline(datain, tag.churchNameENG, ','); //G: Insert Church Name in English
		getline(datain, tag.contactName, ','); //H: Insert Contact Name
		getline(datain, tag.partyType, ',');
		getline(datain, tag.fname, ','); //E: Insert First Name
		getline(datain, tag.lname, ','); //D: Insert Last Name	
		getline(datain, tag.nameCHN, ','); //F: Insert Chinese Name
		getline(datain, tag.room, ','); //F: Insert Room Name
		getline(datain, tag.groupID, ','); //J: Insert Group ID
		getline(datain, tag.roomNeeded, ',');
		getline(datain, tag.ageGroup, ',');
		getline(datain, tag.gender, ',');
		getline(datain, tag.grade, ','); //age group
		getline(datain, tag.regDue, ',');
		getline(datain, tag.keyFee, ',');
		getline(datain, tag.roomAssigned, ',');
		getline(datain, tag.status, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tempconv, ',');
		getline(datain, tag.needKey, ',');
		getline(datain, tag.churchNameCHN, ','); //G: Insert Church Name in Chinese
		getline(datain, tag.QRLink, '\n'); //K: QR Code Link (from Google API)		
		fixTagStrings(tag);
		tags.push_back(tag); //push extracted NameTag to vector
	}
	datain.close();

	//COPY FORMATTING OF TEMPLATE FILE
	fstream fheader(pagetemplate, ios_base::in);
	string line;
	while (getline(fheader, line, '\n')) {
		if (line.find("KEYWORD: HEADER END") != string::npos) //loop until program finds this commented keyword to denote end of css and formatting
			break;
		dataout << line << '\n';
	}

	//COPY HTMLTEMPLATE INTO STRING
	string divtemplate = "";
	while (getline(fheader, line, '\n')) {
		if (line.find("KEYWORD: DIV END") != string::npos) //loop until program find this commented keyword to denote end of template
			break;
		line.append("\n"); //add new line to ending
		divtemplate.append(line); //append full line plus new line character to template
	}
	fheader.close();

	//LOOP BODY
	string modtemplate;
	for (NameTag currTag : tags) {
		if (pagebreak % 1 == 0 && pagebreak != 0) {
			dataout << "<div class=\"page-break\"></div>";
		}
		modtemplate = divtemplate; //copy template onto this string
		fillField(modtemplate, "ID_FIELD", "ID: " + currTag.ID); //fill in ID Field
		fillField(modtemplate, "PARTY_FIELD", currTag.partyID); //fill in Party ID Field
		fillField(modtemplate, "NAMECHN_FIELD", currTag.nameCHN); //fill in Chinese Name Field
		fillField(modtemplate, "NAMEENG_FIELD", currTag.fname + " " + currTag.lname); //fill in Individual Name Field
		if (currTag.churchNameCHN.length() > 0)
			fillField(modtemplate, "CHURCH_FIELD", currTag.churchNameCHN); //fill in Church Name Field
		else
			fillField(modtemplate, "CHURCH_FIELD", currTag.churchNameENG); //if chinese name doesn't exist
		fillField(modtemplate, "GROUP_FIELD", "Group: " + currTag.groupID); //fill in Group ID Field
		fixQRCode(currTag);
		fillField(modtemplate, "QR_FIELD", currTag.QRLink); //fill in QR Link Field
		dataout << modtemplate; //output modified template with parameters filled in
		pagebreak++;
	}

	//END OF FILE
	dataout << "</body>\n</html>" << endl; //closing code for html doc
	dataout.close();
}

/*
Salesforce returns a QRCode that isn't URL-Friendly,
amps; must be removed
outside HTML must be removed
Ex- <img src="https://chart.googleapis.com/chart?chs=150x150&amp;chld=|0&amp;cht=qr&amp;chl=0027,P0019,Rutgers Community Christian Church " alt="QR" border="0"/>
to- https://chart.googleapis.com/chart?chs=150x150&chld=|0&cht=qr&chl=0027,P0019,Rutgers Community Christian Church
*/
void fixQRCode(NameTag& tag) {
	tag.QRLink.erase(0, 11); //Remove: '<img src="'
	for (unsigned int i = 0; i < tag.QRLink.length(); i++) { //Linear search is possible because even if future elements are removed, they don't interfere with i and tag.QRLink.length() is updated accordingly (thus no elements are skipped)
		if (tag.QRLink.at(i) == '&')
			tag.QRLink.erase(i + 1, 4); //remove amp;, so 4 characters
		if (tag.QRLink.at(i) == '\"') {
			tag.QRLink.erase(i); //delete the rest of the unused portion
			return;
		}
	}
}

/*
.csv saves each cell with double quotes, they must be removed
*/
void fixTagStrings(NameTag& tag) {
	removeQuotes(tag.ID); //Personal ID
	removeQuotes(tag.partyID); //Party (Family) ID
	removeQuotes(tag.churchNameENG); //Church Name (ENG)
	removeQuotes(tag.contactName);
	removeQuotes(tag.partyType);
	removeQuotes(tag.fname); //First Name (ENG)
	removeQuotes(tag.lname); //Last Name (ENG)
	removeQuotes(tag.nameCHN); //Chinese Name (CHN)
	removeQuotes(tag.room);
	removeQuotes(tag.groupID); //Group ID
	removeQuotes(tag.roomNeeded);
	removeQuotes(tag.ageGroup);
	removeQuotes(tag.gender);
	removeQuotes(tag.grade);
	removeQuotes(tag.regDue);
	removeQuotes(tag.keyFee);
	removeQuotes(tag.roomAssigned);
	removeQuotes(tag.status);
	removeQuotes(tag.isChristian);
	removeQuotes(tag.services);
	removeQuotes(tag.needKey);
	removeQuotes(tag.churchNameCHN); //Church Name (CHN)
	removeQuotes(tag.QRLink); //QR Code Link
}

bool removeQuotes(string& str) {
	if (str.length() == 0 || (str.length() <= 2 && str.at(0) == '\"')) {
		str = "";
		return false;
	}
	else if (str.at(0) != '\"') {
		return false;
	}

	str.erase(0, 1);
	str.erase(str.size() - 1);
	return true;
}

/*
templatestr - Insert the String holding the HTML Div Template in this implicit param
fieldname - Insert the name of the field being replaced, ex - ID_FIELD (Check webtemplate.txt for more details)
tagfield - Insert the variable from the NameTag struct that will be placed in the HTML file
*/
bool fillField(string& templatestr, const string& fieldname, const string& tagfield) {
	size_t start = templatestr.find(fieldname);
	if (start == string::npos)
		return false;
	templatestr.replace(start, fieldname.length(), tagfield);
	return true;
}

