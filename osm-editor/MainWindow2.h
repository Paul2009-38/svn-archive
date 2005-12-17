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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "functions.h"
#include "GPSDevice2.h"
#include "Components2.h"
#include "Map.h"
#include "LandsatManager2.h"
#include "SRTMGeneral.h"
#include <map>
#include <vector>

#include <qmainwindow.h>
#include <qfont.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qpen.h>
#include <qevent.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>


using std::vector;

// Mouse action modes
// N_ACTIONS should always be the last
enum { ACTION_NODE, ACTION_MOVE_NODE, ACTION_SEL_SEG, ACTION_NEW_SEG, 
		N_ACTIONS };

namespace OpenStreetMap 
{


struct ImgLabelData 
{
	QFont font;
	QColor colour;

	ImgLabelData(const char* f, int s,QColor c) :font(f,s){ colour=c; }
};

struct SegData
{
	QString segtype;
	QPen pen;

	SegData(){}
	SegData(const QString &st, const QPen& p) { segtype=st; pen=p; }
};

struct PolyData
{
	QString polytype;
	QColor colour;

	PolyData(){}
	PolyData(const QString& pt,	const QColor& c) { polytype=pt; colour=c; }
};

class WaypointRep
{
private:
	QPixmap image;	
	ImgLabelData * labelData; // pointer so we can use NULL for no label

public:
	WaypointRep(const QString& imagefile) : image(imagefile)
		{  labelData=NULL;  }
	WaypointRep(const QString& imagefile,const char* f,int s,
					QColor c) :
			image(imagefile)
	    { labelData=new ImgLabelData(f,s,c);  }
	WaypointRep(const char* f,int s,QColor c)
		{  labelData=new ImgLabelData(f,s,c);  }
	~WaypointRep() 
		{ if(labelData)delete labelData; }
	void draw(QPainter&,int x,int y,const QString& label=""); 
	const QPixmap& getImage(){ return image; }
};

class MainWindow2 : public QMainWindow, public DrawSurface
{

Q_OBJECT

private:
	//GridRef topleft;
	//double scale;
	Map map;
	
	// key data
	Components2 * components;

	// on-screen data representations 
	std::map<QString,WaypointRep*> nodeReps; 
	std::map<QString,QPen> segpens;

	// currently selected trackpoints

	// selected segment
	Segment *selSeg;

	// current mouse action mode
	int actionMode;

	// other stuff
	QString curSegType; 
	bool trackpoints, contours;
	QString curFilename; 
	bool mouseDown;

	QToolButton* modeButtons[N_ACTIONS]; 

	QComboBox * modes;

	QPixmap landsatPixmap;


	void saveFile(const QString&);

	LandsatManager2 landsatManager;

	Node *pts[2];
	vector<Node*> ptsv[2];
	int nSelectedPoints;

	QPainter *curPainter;

	bool wptSaved;

	double nameAngle;
	bool doingName;
	ScreenPos namePos, curNamePos;
	QString trackName;


	QString username, password;
	bool liveUpdate;

public:
	MainWindow2 (double=51.0,double=-1.0,double=4000,
			 		double=640,double=480);
	~MainWindow2();
	Components2 * doOpen(const QString&);


	void paintEvent(QPaintEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void resizeEvent(QResizeEvent * ev);
	void keyPressEvent(QKeyEvent*);
	void drawLandsat(QPainter&);
	void drawContours();
	void rescale(double);

	void updateWithLandsatCheck();
	Map getMap() { return map; }

	void drawContour(int,int,int,int,int,int,int);
	void drawAngleText(int,double,int,int,int,int,int,char*);
	void heightShading(int x1,int y1,int x2,int y2,int x3,int y3,
							int x4,int y4,int r,int g,int b);

	void doDrawAngleText(QPainter *p,int originX,int originY,int x,int y,
					double angle, const char * text);
	void showPosition()
	{
		QString msg; 
		msg.sprintf("Lat %lf Long %lf",
						map.getBottomLeft().y, map.getBottomLeft().x);
		statusBar()->message(msg);
	}

	void drawSegments(QPainter&);
	void drawNodes(QPainter&);
	void drawNode(QPainter&,Node*);
	void editNode(int,int,int);
	void nameTrackOn();

public slots:
	void open();
	void save();
	void saveAs();
	void readGPS();
	void quit();
	void toggleLandsat();
	void toggleContours();
	void undo();
	void setMode(int);
	void setSegType(const QString&);
	void toggleNodes();
	void grabLandsat();
	void up();
	void down();
	void left();
	void right();
	void magnify();
	void screenUp();
	void screenDown();
	void screenLeft();
	void screenRight();
	void shrink();
	void loginToLiveUpdate();
	void grabOSMFromNet();
	void uploadOSM();
	void logoutFromLiveUpdate();
	void removeTrackPoints();
};

}
#endif // MAINWINDOW_H
