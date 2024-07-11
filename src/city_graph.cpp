
#include <vector>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <iomanip>
#include "city_helper.h"

using std::vector;
using std::map;
using std::set;
using std::string;
using std::ifstream;
using std::cout;
using std::stringstream;
using std::pair;

using std::ofstream;
using std::setw;
using std::setprecision;
using std::right;
using std::left;
using std::fixed;


void read_cityinfo(vector<city> &locations, map<string,int> &indeces) {
	//open file
	ifstream fin;
	fin.open("city_list.txt");
	if(!fin.is_open()) { cout << "File Not Found\n"; exit(0); }

	while(!fin.eof()) {
		//read data from file into sstream
		string str;
		getline(fin, str);
		if(str.empty() || str.front() == '#') continue;
		else {
			stringstream ss(str);
			
			//read data from stream into structure
			city c;
			ss >> c;
			locations.push_back(c);

			string name = c.getName();
			int len = name.length();
			name.resize(len-3);

			int index = (int)locations.size() - 1;
			indeces.insert(pair<string,int>(name,index));
		}
	}

	//close file
	fin.close();
}

/* RULES OF ADJACENCY:
 * 1) A REGIONAL city is adjacent to all other REGIONAL cities in the same zone.
 * 2) A REGIONAL city is also ajacent to the nearest GATEWAY city in the same zone.
 * 3) REGIONAL cities in different zones are not connected. Instead, their traffic must go 
 * thru GATEWAY cities
 * 4) A GATEWAY city is adjacent to all other GATEWAY cities in the same zone.
 * 5) A GATEWAY city is also adjacent to the closest GATEWAY city in another zone, if the
 * distance is 5200 miles or less.
 * 6) In order to make the graph unirected, adjacency is bidirectional. thus, if A is 
 * Adjacent to B, then B is also adjacent to A.*/

void create_citygraph(vector<city> &locations, const costtable &table, vector< set<int> > &citygraph) {
	
	//extract index list for each zone
	vector<city>::iterator sublist_begin = locations.begin();
	vector<city>::iterator sublist_end = locations.begin();

	//iterating through zone partitions
	while(sublist_end != locations.end()) {
		while((*sublist_begin).getZone() == (*sublist_end).getZone() && sublist_end!=locations.end()) 
			++sublist_end;
		
		for(vector<city>::iterator iT1=sublist_begin; iT1!=sublist_end; ++iT1) {
			int i = iT1-locations.begin();
			string type1 = (*iT1).getType();
			float closest_gateway_distance = 5200.00;
			int closest_gateway = -1;

			
			for(vector<city>::iterator iT2=sublist_begin; iT2!=sublist_end; ++iT2) {
				if(iT1==iT2) continue;

				int j = iT2-locations.begin();
				string type2 = (*iT2).getType();

				//both regional and both gateway (1) & (4)
				if(type1 == type2) {
					citygraph[i].insert(j);
					citygraph[j].insert(i);
				}
				else if(!type1.compare("REGIONAL")) {
					if(table(false, i, j) <= closest_gateway_distance) {
						closest_gateway = j;
						closest_gateway_distance = table(false, i , j);
					}
				}
				else ; //when type1 == GATEWAY
			}

			if(!type1.compare("REGIONAL") && closest_gateway!=-1) {
				citygraph[i].insert(closest_gateway);
				citygraph[closest_gateway].insert(i);
			}
			else if(!type1.compare("GATEWAY")) {
				//when type1 == GATEWAY
				
				vector<city>::iterator iT2=locations.begin();
				vector<city>::iterator iT3=locations.begin();
				while(iT3!=locations.end()) {
					closest_gateway_distance = 5200.00;
					closest_gateway = -1;
					while((*iT2).getZone()==(*iT3).getZone() && iT3!=locations.end())
						++iT3;

					while(iT2!=iT3) {
						string type2 = (*iT2).getType();
						int zone1 = (*iT1).getZone(), zone2 = (*iT2).getZone();
						if(!type2.compare("GATEWAY") && zone1!=zone2) {
							int j = iT2-locations.begin();

							if(table(false, i, j) <= closest_gateway_distance) {
								closest_gateway = j;
								closest_gateway_distance = table(false, i, j);
							}
						}

						++iT2;
					}
				

					if(closest_gateway!=-1) {
						citygraph[i].insert(closest_gateway);
						citygraph[closest_gateway].insert(i);
					}
				}
			}
			else;
		}

		sublist_begin = sublist_end;
	}
}

void write_citygraph(const vector<city> &locations, const costtable &table,
		const vector< set<int> > &citygraph) {
	ofstream fout;
	fout.open("city_graph.txt");

	fout << "CITY GRAPH: \n\n";

	int N = (int)citygraph.size();
	for(int i=0; i<N; ++i) {
		fout << setw(3) << i << ' ' << locations[i].getName() << '\n';
		
		for(set<int>::iterator iT=citygraph[i].begin(); iT!=citygraph[i].end(); ++iT) {
			int j = *iT;
			fout << setw(6) << j << ' ' << setw(19) << left << locations[j].getName() << right
				 << setw(7) << setprecision(1) << fixed << table(false, i, j) << " miles "
				 << setw(5) << setprecision(1) << fixed << table(true , i, j) << " hours\n";
		}

		fout << '\n';
	}

	fout.close();
}