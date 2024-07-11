#ifndef CITY_GRAPH_H
#define CITY_GRAPH_H

#include <vector>
#include <map>
#include <string>
#include "city_helper.h"

using std::vector;
using std::set;
using std::map;
using std::string;

void read_cityinfo(vector<city>&, map<string,int>&);
void create_citygraph(vector<city>&, const costtable&, vector<set<int>>&);
void write_citygraph(const vector<city>&, const costtable&, const vector<set<int>>&);

#endif