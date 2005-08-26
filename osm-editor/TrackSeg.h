#ifndef TRACKSEG_H
#define TRACKSEG_H

/*
    Copyright (C) 2005 Nick Whitelegg, Hogweed Software, nick@hogweed.org 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

 */


#include <qstring.h>
#include <vector>
#include <fstream>
#include "functions.h"
#include "EarthPoint.h"

#include <iostream>

using std::vector;
using std::ostream;

#include <cmath>

namespace OpenStreetMap
{

struct TrackPoint
{
	// 10/04/05 now storing the timestamp as the standard GPX format
	QString timestamp;
	double lat, lon;

	TrackPoint(){lat=lon=0; timestamp=""; }
	TrackPoint(const QString& t, double lt, double ln)
		{ timestamp=t; lat=lt; lon=ln; }
	void toGPX(ostream&);
	bool connected(const TrackPoint& pt, double speed);
	bool operator==(const TrackPoint& tp)
		{ return (fabs(lat-tp.lat)<0.000001) && (fabs(lon-tp.lon)<0.000001); }
};

class RetrievedTrackPoint;

class TrackSeg
{
private:
	QString id, type;
	vector<TrackPoint> points;



public:
	TrackSeg() 
		{ id=""; type="track";  }

	 TrackSeg(const QString& i,const QString& t)
		{ id=i; type=t;  }

	 void setID(const QString& i) { id=i; }
	 QString getID() { return id; }
	 QString getType() { return type; }

	
	void setType(const QString& t) { type=t; }

	void addPoint(const QString& ts,double lat,double lon) 
		{ points.push_back(TrackPoint(ts,lat,lon)); }

	void addPoint(const TrackPoint& pt)
		{ points.push_back(pt); }

	bool addPoint(const TrackPoint& pt, int pos);
	
	
	void toGPX(ostream&);
	int findNearestTrackpoint(const EarthPoint& p,double limit,double* = NULL);
	bool deletePoints(int start, int end);

	int nPoints() { return points.size(); }
	TrackPoint getPoint(int i) throw(QString); 

	void deleteExcessPoints (double angle, double distance);

};

struct PointInfo
{
	TrackSeg *seg;
	int	point;

	PointInfo(TrackSeg* s, int p ) { seg=s; point=p; }
};

class RetrievedTrackPoint
{
private:
	vector<PointInfo> segs;
public:
	int size() const { return segs.size(); }
	void add(TrackSeg* seg,int p) { segs.push_back(PointInfo(seg,p)); }
	int getPointIdx(int i) const { return segs[i].point; }
	TrackSeg *getSeg(int i) const { return segs[i].seg; }
	TrackPoint getPoint (int i) const
		{ return segs[i].seg->getPoint(segs[i].point); }
	bool operator==(const TrackPoint& tp) const;
	void print(){for(int count=0; count<size(); count++) 
					cout<<segs[count].point<<endl; }
};

}
#endif /* not TRACKSEG_H */
