#include <liveRead.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static bool readin(const std::string &filename);
static bool writeout(const std::string &filename);

const std::string meals[] = {"SAT-BREAKFAST-", "SAT-LUNCH-", "SAT-DINNER-",
							"SUN-BREAKFAST-", "SUN-LUNCH-", "SUN-DINNER-",
							"MON-BREAKFAST-", "MON-LUNCH-"};

int line_num[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int pos_num[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

int form_start_num = -1;
int form_end_num = -1;

std::vector<std::string> context;
const std::string filename = "D:/users/dzhang/QFL2019/coupons/a_coupon.html";
const std::string fileout = "D:/users/dzhang/QFL2019/coupons/coupons.html";
std::vector<std::string> output;

int32_t main(int32_t argc, char** argv) {
	int i, j;
	char strnum[128];

	// read in template
	readin(filename);

	for (i = 80; i < 101; i++) {
		for (j = 0; j < 16; j++) {
			sprintf(strnum, "%04d", i);
			context[line_num[j]].replace(pos_num[j]+strlen(meals[j/2].c_str()), 4, std::string(strnum));
		}
		for (j = 0; j < context.size()-2; j++)
			output.push_back(context[j]);
	}

	output.push_back(context[context.size() - 2]);
	output.push_back(context[context.size() - 1]);

	// save a html page
	writeout(fileout);

	return 0;
}

bool readin(const std::string &filename)
{
	int i = 0;
	int j = 0;
	std::ifstream fin(filename);
	if (!fin.is_open())
		return false;

	// read in all the data
	while (!fin.eof()) {
		std::string line0;
		std::getline(fin, line0);
		size_t begin = line0.find("format begin");
		if (begin != std::string::npos) {
			form_start_num = j;
		}
		size_t end = line0.find("format end");
		if (end != std::string::npos) {
			form_end_num = j;
		}

		size_t pos = line0.find(meals[i/2]);
		if (pos != std::string::npos) {
			line_num[i] = j;
			pos_num[i] = pos;
			i++;
		}
		context.push_back(line0);
		j++;
	}

	return true;
}

bool writeout(const std::string &filename)
{
	int i;
	std::ofstream fout(filename);
	if (!fout.is_open())
		return false;

	for (i = 0; i < output.size(); i++)
	{
		fout << output[i];
		fout << "\n";
	}

	fout.close();
	return true;
}