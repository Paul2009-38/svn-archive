#ifndef MERKATOR_INTERACTION_H_
#define MERKATOR_INTERACTION_H_

class Node;
class MainWindow;
class Projection;
class Node;
class Way;

class QMouseEvent;
class QPaintEvent;
class QPainter;

#include "MainWindow.h"
#include "MapView.h"
#include "InfoDock.h"
#include "FeaturesDock.h"
#include "Document.h"
#include "Features.h"

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtGui/QApplication>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QList>

#include <algorithm>

#define XY_TO_COORD(x)  projection().inverse(transform().inverted().map(x))
#define COORD_TO_XY(x)  transform().map(projection().project(x))

class Interaction : public QObject
{
    Q_OBJECT
    public:
        Interaction(MapView* theView);
        virtual ~Interaction();

        virtual void mousePressEvent(QMouseEvent * event);
        virtual void mouseReleaseEvent(QMouseEvent * event);
        virtual void mouseMoveEvent(QMouseEvent* event);
        virtual void wheelEvent(QWheelEvent* ev);

        virtual void paintEvent(QPaintEvent* anEvent, QPainter& thePainter);
        virtual QString toHtml() = 0;

        MapView* view();
        Document* document();
        MainWindow* main();
        const Projection& projection() const;
        const QTransform& transform() const;
        bool panning() const;
    protected:
        MapView* theView;
        bool Panning;
        QPoint FirstPan;
        QPoint LastPan;
    signals:
        void requestCustomContextMenu(const QPoint & pos);

    protected:
        bool Dragging;
        Coord StartDrag;
        Coord EndDrag;
};

class FeatureSnapInteraction : public Interaction
{
public:
    FeatureSnapInteraction(MapView* theView);

    virtual void paintEvent(QPaintEvent* anEvent, QPainter& thePainter);

    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent* event);

    virtual void snapMousePressEvent(QMouseEvent * , Feature*);
    virtual void snapMouseReleaseEvent(QMouseEvent * , Feature*);
    virtual void snapMouseMoveEvent(QMouseEvent* , Feature*);

    void activateSnap(bool b);
    void addToNoSnap(Feature* F);
    void clearNoSnap();
    void clearSnap();
    void clearLastSnap();
    QList<Feature*> snapList();
    void addSnap(Feature* aSnap);
    void setSnap(QList<Feature*> aSnapList);
    void nextSnap();
    void previousSnap();

    void setDontSelectPoints(bool b);
    void setDontSelectRoads(bool b);
    void setDontSelectVirtual(bool b);

#ifndef Q_OS_SYMBIAN
    virtual QCursor cursor() const;
#endif

private:
    void updateSnap(QMouseEvent* event);

    QList<Feature*> NoSnap;
    bool SnapActive;
    bool NoSelectPoints;
    bool NoSelectWays;
    bool NoSelectRoads;
    bool NoSelectVirtuals;

    QCursor handCursor;
    QCursor grabCursor;
    QCursor defaultCursor;

protected:
    Feature* LastSnap;
    QList<Feature*> StackSnap;
    QList<Feature*> SnapList;
    int curStackSnap;
};

#endif


