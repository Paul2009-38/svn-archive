/***************************************************************************
 *   Copyright (C) 2007 by Kai Winter   *
 *   kaiwinter@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "tilemapadapter.h"

// from wikipedia
#define EQUATORIALRADIUS 6378137.0
#define POLARRADIUS      6356752.0
#define EQUATORIALMETERCIRCUMFERENCE  40075016.68
#define EQUATORIALMETERHALFCIRCUMFERENCE  20037508.34
#define EQUATORIALMETERPERDEGREE    111319490.79

TileMapAdapter::TileMapAdapter(const QString& host, const QString& serverPath, const QString& projection, int theTilesize, int minZoom, int maxZoom, bool blOrigin)
    :MapAdapter(host, serverPath, projection, minZoom, maxZoom)
    , tilesize(theTilesize)
    , BlOrigin(blOrigin)
{
    name = "tiles";

// 	qDebug() << "creating adapter: min, max, current: " << minZoom << ", " << maxZoom << ", " << current_zoom << ", " << (min_zoom < max_zoom);

    /*
        Initialize the "substring replace engine". First the string replacement
        in getQuery was made by QString().arg() but this was very slow. So this
        splits the servers path into substrings and when calling getQuery the
        substrings get merged with the parameters of the URL.
        Pretty complicated, but fast.
    */
    param1 = serverPath.indexOf("%1");
    param2 = serverPath.indexOf("%2");
    param3 = serverPath.indexOf("%3");

    int min = param1 < param2 ? param1 : param2;
    min = param3 < min ? param3 : min;

    int max = param1 > param2 ? param1 : param2;
    max = param3 > max ? param3 : max;

    int middle = param1+param2+param3-min-max;

    order[0][0] = min;
    if (min == param1)
        order[0][1] = 0;
    else if (min == param2)
        order[0][1] = 1;
    else
        order[0][1] = 2;

    order[1][0] = middle;
    if (middle == param1)
        order[1][1] = 0;
    else if (middle == param2)
        order[1][1] = 1;
    else
        order[1][1] = 2;

    order[2][0] = max;
    if (max == param1)
        order[2][1] = 0;
    else if(max == param2)
        order[2][1] = 1;
    else
        order[2][1] = 2;

    isProj4326 = (Projection.contains(":4326"));
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
}

TileMapAdapter::~TileMapAdapter()
{
}

QUuid TileMapAdapter::getId() const
{
    return QUuid("{CA8A07EC-A466-462b-929F-3805BC9DEC95}");
}

IMapAdapter::Type TileMapAdapter::getType() const
{
    return IMapAdapter::NetworkBackground;
}

QRectF TileMapAdapter::getBoundingbox() const
{
    if (isProj4326)
        return QRectF(QPointF(-180.00, -90.00), QPointF(180.00, 90.00));
    else
        return QRectF(QPointF(-EQUATORIALMETERHALFCIRCUMFERENCE, -EQUATORIALMETERHALFCIRCUMFERENCE), QPointF(EQUATORIALMETERHALFCIRCUMFERENCE, EQUATORIALMETERHALFCIRCUMFERENCE));
}

int TileMapAdapter::getTileSize() const
{
    return tilesize;
}

//TODO: rausziehen? ->MapAdapter?
void TileMapAdapter::zoom_in()
{
    if (min_zoom > max_zoom)
        current_zoom = current_zoom > max_zoom ? current_zoom-1 : max_zoom;
    else if (min_zoom < max_zoom)
        current_zoom = current_zoom < max_zoom ? current_zoom+1 : max_zoom;
}
void TileMapAdapter::zoom_out()
{
    if (min_zoom > max_zoom)
        current_zoom = current_zoom < min_zoom ? current_zoom+1 : min_zoom;
    else if (min_zoom < max_zoom)
        current_zoom = current_zoom > min_zoom ? current_zoom-1 : min_zoom;
}

QString TileMapAdapter::getQuery		(int x, int y, int z) const
{
    if (BlOrigin)
        y = getTilesNS(current_zoom) - y;
    int a[3] = {z, x, y};
    return QString(serverPath).replace(order[2][0],2, loc.toString(a[order[2][1]]))
                                      .replace(order[1][0],2, loc.toString(a[order[1][1]]))
                                      .replace(order[0][0],2, loc.toString(a[order[0][1]]));

}

bool TileMapAdapter::isValid(int x, int y, int z) const
{
    if ((x<0) || (x>getTilesWE(z)) ||
            (y<0) || (y>getTilesNS(z)))
    {
        return false;
    }
    return true;
}

int TileMapAdapter::getTilesWE(int zoomlevel) const
{
    int t = int(pow(2, zoomlevel+0.0));
    if (isProj4326)
        t *= 2;

    return t;
}

int TileMapAdapter::getTilesNS(int zoomlevel) const
{
    return int(pow(2, zoomlevel+0.0));
}
