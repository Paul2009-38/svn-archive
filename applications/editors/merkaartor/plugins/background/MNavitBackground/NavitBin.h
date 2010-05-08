//***************************************************************
// CLass: NavitBin
//
// Description:
//
//
// Author: Chris Browet <cbro@semperpax.com> (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//******************************************************************

#ifndef NAVITBIN_H
#define NAVITBIN_H

#include <QPoint>
#include <QPolygon>
#include <QHash>
#include <QStringList>

class NavitZip;

enum attr_type {
#define ATTR2(x,y) attr_##y=x,
#define ATTR(x) attr_##x,
#include "attr_def.h"
#undef ATTR2
#undef ATTR
};

enum item_type {
#define ITEM2(x,y) type_##y=x,
#define ITEM(x) type_##x,
#include "item_def.h"
#undef ITEM2
#undef ITEM
};

class NavitAttribute
{
public:
    NavitAttribute()
        : type(0) {}
    NavitAttribute(qint32 theType, QByteArray theAttribute)
        : type(theType), attribute(theAttribute) {}

public:
    quint32 type;
    QByteArray attribute;
};

class NavitFeature
{
public:
    NavitFeature()
        : type(0) {}

public:
    quint32 type;
    quint16 order;
    QVector<QPoint> coordinates;
    QList<NavitAttribute> attributes;
};

class NavitPointer
{
public:
    QRect box;
    quint32 zipref;
    quint16 orderMin;
    quint16 orderMax;
};

class NavitTile
{
public:
    QList<NavitFeature> features;
    QList<NavitPointer> pointers;
};

class NavitBin
{
public:
    NavitBin();
    ~NavitBin();

    bool setFilename(const QString& filename);
    bool readTile(int index) const;

//    bool getFeatures(const QString& tileRef, QList <NavitFeature>& theFeats) const;
    bool walkTiles(const QRect& box, const NavitTile& t, int order, QList <NavitFeature>& theFeats) const;
    bool getFeatures(const QRect& box, QList <NavitFeature>& theFeats) const;

private:
    NavitZip* zip;

    mutable QHash<int, NavitTile> theTiles;
    NavitTile indexTile;
};

#endif // NAVITBIN_H
