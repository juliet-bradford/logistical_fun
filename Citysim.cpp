
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include "city_class.h"
using namespace std;


//needs a file to read from and a structure to write to(&)
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

//three functions outputting costtable data to files, all run with "-graphinfo"
void write_cityinfo(const vector<city> &locations) { 
	ofstream fout;
	fout.open("city_info.txt");
	
	fout << "CITY INFO: (N=" << locations.size() << "):\n\n";
	
	for(int i=0; i<(int)locations.size(); ++i)
		fout << setw(3) << i << ' ' << locations[i] << '\n';

	fout.close();
}

void write_distancetable(const vector<city> &locations, const costtable &table) {
	ofstream fout;
	fout.open("city_distancetable.txt");

	fout << "DISTANCE TABLE: \n";
	
	for(int i=0; i<(int)locations.size(); ++i) {
		string name1 = locations[i].getName();

		for(int j=0; j<i; ++j) {
			string flight = name1 + " to " + locations[j].getName() + ' ';

			fout << setw(3) << i << ' ' << setw(38) << setfill('.') << left << flight << right
				 << setfill(' ') << setw(7) << setprecision(1) << fixed << table(false, i, j) 
				 << " miles\n";
		}

		fout << '\n';
	}

	fout.close();
}

void write_timetable(const vector<city> &locations, const costtable &table) {
	ofstream fout;
	fout.open("city_timetable.txt");

	fout << "TIME TABLE: \n";
	
	for(int i=0; i<(int)locations.size(); ++i) {
		string name1 = locations[i].getName();

		for(int j=0; j<i; ++j) {
			string flight = name1 + " to " + locations[j].getName() + ' ';

			fout << setw(3) << i << ' ' << setw(38) << setfill('.') << left << flight << right
				 << setfill(' ') << setw(5) << setprecision(1) << fixed << table(true, i, j)
				 << " hours\n";
		}

		fout << '\n';
	}

	fout.close();
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

//city vector not const because of iterators
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
				else; //when type1 == GATEWAY
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


void dijkstra_route(const vector<city> &locations, const costtable &table,
		const vector< set<int> > &citygraph, const bool &mode, const int &source, 
		const int &sink) { 
	vector<bool> vcolor(locations.size(), 0);

	vector<float> vdist(locations.size(), numeric_limits<float>::max());
	vdist[source] = 0;

	vector<int> vlink(locations.size(), -1);
	vlink[source] = source;

	while (1) {
		int next_i = -1;
		float mindist = numeric_limits<float>::max();

		for(int i=0; i<(int)vcolor.size(); ++i) {
			if(vcolor[i] == 0 && mindist > vdist[i]) {
				next_i = i;
				mindist = vdist[i];
			}
		}

		int i = next_i;
		if (i == -1)
			return;

		vcolor[i] = 1;

		if (i == sink)
			break;

		for(set<int>::iterator iT=citygraph[i].begin(); iT!=citygraph[i].end(); ++iT) {
			int j = *iT;
			float wij = table(mode,i,j);
			if (vcolor[j] == 0) {
				if(vdist[j] > vdist[i] + wij) {
					vdist[j] = vdist[i] + wij;
					vlink[j] = i;
				}
			}
		}
	}

	stack<int> path;
	for(int i = sink; i!=source; i=vlink[i])
		path.push(i);
	path.push(source);

	int i = path.top(), j = path.top();
	float cumdist = 0.0, cumtime = 0.0;
	while(1) {

		cumdist+=table(0,i,j); cumtime+=table(1,i,j);

		cout << setw(7) << setprecision(1) << fixed << cumdist << " miles " 
			 << setw(5) << setprecision(1) << fixed << cumtime << " hours :"
			 << setw(3) << j << ' '
			 << setw(18) << left << locations[j].getName() << right;

		if(i!=j)
			cout << setw(9) << setprecision(1) << fixed << table(0,i,j) << " miles " 
				 << setw(5) << setprecision(1) << fixed << table(1,i,j) << " hours";

		cout << '\n';

			i = j;
			path.pop();

			if(path.empty()) break;
			else j = path.top();
	}

}

int main(int argc, char *argv[])
{

	if(argc!=2) {
		cout << "Usage: ./Citysim -graphinfo|distance|time\n";
		return -1;
	}
	//commandline option decoding
	string arg((char *)&argv[1][1]);
	bool mode;
	if(!arg.compare("distance")) mode = 0;
	else mode = 1;


	//city graph declarations
	vector<city> locations;
	map<string, int> indeces;

	read_cityinfo(locations, indeces);

	//set up costtables
	costtable table(locations);

	int N = locations.size();
	vector< set<int> > citygraph(N, set<int>());
	create_citygraph(locations, table, citygraph);

	if (!arg.compare("graphinfo")) {
	    write_cityinfo(locations);
	
	    write_distancetable(locations, table);
	    write_timetable(locations, table);

		write_citygraph(locations, table, citygraph);

		return 0;
	}

	cout << "Usage: [source city] [destination city]\n";

	string from, to;
	while (cin >> from >> to) {
		map<string,int>::iterator iT1 = indeces.find(from), iT2 = indeces.find(to);
		if(iT1==indeces.end() || iT2==indeces.end()) {
			cout << "Invalid Cities\n";
		}
		else {
			int source = iT1->second, sink = iT2->second;
			dijkstra_route(locations, table, citygraph, mode, source, sink);
		}
	}

}
