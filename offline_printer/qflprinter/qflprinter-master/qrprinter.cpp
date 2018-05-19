#include <stdint.h>
#include <stdio.h>
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
	string fname; //First Name (ENG)
	string lname; //Last Name (ENG)
	string nameCHN; //Chinese Name (CHN)
	string contactName;
	string churchNameENG; //Church Name (ENG)
	string churchNameCHN; //Church Name (CHN)
	string QRLink; //QR Code Link
	string attendeeID; //NOT SURE WHAT THIS DOES
	string ID; //Personal ID
	string partyID; //Party (Family) ID
	string groupID; //Group ID
} NameTag;

void removeQuotes(string& str);
void fixTagStrings(NameTag& tag);
void fixQRCode(NameTag& tag);
bool fillField(string& templatestr, const string& fieldname, const string& tagfield);

int main() {
	//constant values
	const char* data = "./qrres/reporttemplate.csv"; //input datasheet for all customers
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
		getline(datain, tag.attendeeID, ','); //B: Attendee ID ?
		getline(datain, tag.partyID, ','); //C: Party ID
		getline(datain, tag.lname, ','); //D: Insert Last Name
		getline(datain, tag.fname, ','); //E: Insert First Name
		getline(datain, tag.nameCHN, ','); //F: Insert Chinese Name
		getline(datain, tag.churchNameENG, ','); //G: Insert Church Name in English
		getline(datain, tag.contactName, ','); //H: Insert Contact Name
		getline(datain, tag.churchNameCHN, ','); //I: Insert Church Name in Chinese
		getline(datain, tag.groupID, ','); //J: Insert Group ID
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
		if (pagebreak % 6 == 0 && pagebreak != 0) {
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

	return 0;
}

/*
Salesforce returns a QRCode that isn't URL-Friendly,
amps; must be removed
outside HTML must be removed
Ex- <img src="https://chart.googleapis.com/chart?chs=150x150&amp;chld=|0&amp;cht=qr&amp;chl=0027,P0019,Rutgers Community Christian Church " alt="QR" border="0"/>
to- https://chart.googleapis.com/chart?chs=150x150&chld=|0&cht=qr&chl=0027,P0019,Rutgers Community Christian Church
*/
void fixQRCode(NameTag& tag) {
	tag.QRLink.erase(0,11); //Remove: '<img src="'
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
	removeQuotes(tag.ID); //A: Personal ID
	removeQuotes(tag.attendeeID); //B: Attendee ID ?
	removeQuotes(tag.partyID); //C: Party ID
	removeQuotes(tag.lname); //D: Insert Last Name
	removeQuotes(tag.fname); //E: Insert First Name
	removeQuotes(tag.nameCHN); //F: Insert Chinese Name
	removeQuotes(tag.churchNameENG); //G: Insert Church Name in English
	removeQuotes(tag.contactName); //H: Insert Contact Name
	removeQuotes(tag.churchNameCHN); //I: Insert Church Name in Chinese
	removeQuotes(tag.groupID); //J: Insert Group ID
	removeQuotes(tag.QRLink); //K: QR Code Link (from Google API)
}

void removeQuotes(string& str) {
	if (str.length() <= 2) {
		str = "";
		return;
	}
	str.erase(0, 1);
	str.erase(str.size() - 1);
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
