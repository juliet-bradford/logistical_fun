#ifndef CITY_HELPER_H
#define CITY_HELPER_H

#include <string>
#include <vector>

using std::string;
using std::vector;
using std::istream;
using std::ostream;

// holds data from a list of cities
class city {
	public:
		city();

		string getName() const { return name; }
		string getType() const { return type; }
		int getZone() const { return zone; }
		int getPop() const { return pop; }
		float getLat() const { return lat; }
		float getLon() const { return lon; }

		friend istream& operator>>(istream&, city&);
		friend ostream& operator<<(ostream&, const city&);

	private:
		string name, type;
		int zone, pop;
		float lat, lon;
};

// holds the distance and time to traverse between any two (directly) connected cities
class costtable{
	public:
		costtable(const vector<city>&);
		float operator()(const bool&, const int&, const int&) const;

	private:
		vector<float> distance_table;
		vector<float> time_table;
};

// writes table information to .txt files, only ran with -graphinfo
void write_cityinfo(const vector<city>&);
void write_distancetable(const vector<city>&, const costtable&);
void write_timetable(const vector<city>&, const costtable&);

#endif