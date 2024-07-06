
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include "city_helper.h"
#include "city_graph.h"

using namespace std;


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
