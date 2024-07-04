#include "city_class.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using std::string;
using std::vector;
using std::istream;
using std::ostream;

const float pi = 3.1415927;
const float degrees = 180.00;
const float Earth_r = 3982.00;

city::city() { name = string(); type = string(); zone = 0; pop = 0; lat = 0.000; lon = 0.000; }

istream & operator>>(istream &in, city &c) {

	int t1, t6; 
    string t2, t3; 
    float t4, t5;

	in >> t1 >> t2 >> t3 >> t4 >> t5 >> t6;

	//lat and lon are read in as degrees, need to convert to radians
	c.zone = t1;
	c.name = t2;
	c.type = t3;
	c.lat  = t4*(pi/degrees);
	c.lon  = t5*(pi/degrees);
	c.pop  = t6;

	return in;
}

ostream & operator<<(ostream &out, const city &c) {
	
    //lat and lon are stored as radians, need to convert to degrees
	out << std::left << std::setw(18) << c.name
		<< std::setw(12) << c.type
		<< std::right << std::setw(10) << c.pop
		<< std::setw(2) << c.zone
		<< std::setw(8) << std::fixed << std::setprecision(2) << c.lat*(degrees/pi)
		<< std::setw(8) << c.lon*(degrees/pi);

	return out;
}

costtable::costtable(const vector<city> &locations) {
	int N = (int)locations.size();
	for(int i=0; i<N; ++i) {

		float la1 = locations[i].getLat();
		float lo1 = locations[i].getLon();
		string type1 = locations[i].getType();

		for(int j=0; j<=i; ++j) {
			
			float la2 = locations[j].getLat();
			float lo2 = locations[j].getLon();

			//compute the distance between cities using Great-circle distance

			float central_angle = acos(sin(la1)*sin(la2) + cos(la1)*cos(la2)*cos(fabs(lo1-lo2)));
			float distance = 10.0*round((central_angle*Earth_r)/10.0);
			distance_table.push_back(distance);

			//both regional -> ground
			//regional+gateway -> air
			//both gateway -> air
			//ground = 60 mph, air = 570 mph

			string type2 = locations[j].getType();

			if(!type1.compare("REGIONAL") && !type2.compare("REGIONAL")) 
				time_table.push_back(distance/60.0);
			else 
				time_table.push_back(distance/570.0);
		}
	}
}

float costtable::operator()(const bool &mode, const int &i, const int &j) const {
	if(j<i) {
		if(mode) return time_table[i*(i+1)/2+j];
		else return distance_table[i*(i+1)/2+j];
	}
	else {
		if(mode) return time_table[j*(j+1)/2+i];
		else return distance_table[j*(j+1)/2+i];
	}
}