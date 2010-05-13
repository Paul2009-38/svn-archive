#include "MapView.h"
#include "ImageMapLayer.h"

#include "Document.h"
#include "Preferences/MerkaartorPreferences.h"
#include "Maps/Projection.h"

#include "IMapAdapter.h"
#include "QMapControl/imagemanager.h"
#ifdef USE_WEBKIT
#include "QMapControl/browserimagemanager.h"
#endif
#include "QMapControl/tilemapadapter.h"
#include "QMapControl/wmsmapadapter.h"
#include "QMapControl/WmscMapAdapter.h"

#include <QLocale>
#include <QPainter>

#include "LayerWidget.h"

// ImageMapLayerPrivate

class ImageMapLayerPrivate
{
public:
    QUuid bgType;
    IMapAdapter* theMapAdapter;

    QPixmap pm;
    QPoint theDelta;
    Projection theProjection;
    QString selServer;
    IImageManager* theImageManager;
    TileMapAdapter* tmsa;
    WMSMapAdapter* wmsa;
    WmscMapAdapter* wmsca;
    QRect pr;
    QTransform theTransform;
    CoordBox Viewport;

public:
    ImageMapLayerPrivate()
    {
        theMapAdapter = NULL;
        theImageManager = NULL;
        tmsa = NULL;
        wmsa = NULL;
        wmsca = NULL;
    }
    ~ImageMapLayerPrivate()
    {
        SAFE_DELETE(wmsa)
        SAFE_DELETE(wmsca)
        SAFE_DELETE(tmsa)
        SAFE_DELETE(theImageManager)
    }
};


// ImageMapLayer

ImageMapLayer::ImageMapLayer(const QString & aName)
    : OsbLayer(aName), p(new ImageMapLayerPrivate)
{
    p->bgType = NONE_ADAPTER_UUID;
    setName(tr("Map - None"));
    Layer::setVisible(false);
    setReadonly(true);
}

ImageMapLayer::~ ImageMapLayer()
{
    SAFE_DELETE(p)
}

CoordBox ImageMapLayer::boundingBox()
{
    if (p->bgType == SHAPE_ADAPTER_UUID && isVisible())
        return Layer::boundingBox();
    else
        if (!p->theMapAdapter || p->theMapAdapter->getBoundingbox().isNull())
            return CoordBox();

    QRectF r = p->theMapAdapter->getBoundingbox();
    Coord tl = p->theProjection.inverse(r.topLeft());
    Coord br = p->theProjection.inverse(r.bottomRight());
    return CoordBox(tl, br);
}

int ImageMapLayer::size() const
{
    if (p->bgType == SHAPE_ADAPTER_UUID && isVisible())
        return Layer::size();
    else
        return 0;
}

LayerWidget* ImageMapLayer::newWidget(void)
{
//	delete theWidget;
    theWidget = new ImageLayerWidget(this);
    return theWidget;
}

void ImageMapLayer::updateWidget()
{
    theWidget->initActions();
    setMapAdapter(M_PREFS->getBackgroundPlugin(), M_PREFS->getSelectedServer());
    theWidget->update();
}

void ImageMapLayer::setVisible(bool b)
{
    Layer::setVisible(b);
    if (p->bgType == NONE_ADAPTER_UUID)
        Layer::setVisible(false);
    MerkaartorPreferences::instance()->setBgVisible(isVisible());
}

QString ImageMapLayer::projection() const
{
    if (p->theMapAdapter)
        return p->theMapAdapter->projection();

    return "";
}

IImageManager* ImageMapLayer::getImageManger()
{
    return p->theImageManager;
}

IMapAdapter* ImageMapLayer::getMapAdapter()
{
    return p->theMapAdapter;
}

void ImageMapLayer::setMapAdapter(const QUuid& theAdapterUid, const QString& server)
{
    WmsServerList* wsl;
    TmsServerList* tsl;

    SAFE_DELETE(p->wmsa)
    SAFE_DELETE(p->wmsca)
    SAFE_DELETE(p->tmsa)
    if (p->theImageManager)
        p->theImageManager->abortLoading();
    SAFE_DELETE(p->theImageManager)
    on_loadingFinished();
    p->theMapAdapter = NULL;
    p->pm = QPixmap();

    p->bgType = theAdapterUid;
    MerkaartorPreferences::instance()->setBackgroundPlugin(theAdapterUid);
    if (p->bgType == NONE_ADAPTER_UUID) {
        setName(tr("Map - None"));
        setVisible(false);
    } else
    if (p->bgType == WMS_ADAPTER_UUID) {
        wsl = M_PREFS->getWmsServers();
        p->selServer = server;
        WmsServer theWmsServer(wsl->value(p->selServer));
        switch (theWmsServer.WmsIsTiled) {
        case 0:
            p->wmsa = new WMSMapAdapter(theWmsServer);
            p->theMapAdapter = p->wmsa;
            setName(tr("Map - WMS - %1").arg(p->theMapAdapter->getName()));
            break;
        case 1:
            p->wmsca = new WmscMapAdapter(theWmsServer);
            p->theMapAdapter = p->wmsca;
            setName(tr("Map - WMS-C - %1").arg(p->theMapAdapter->getName()));
            break;
        case 2:
            p->wmsca = new WmscMapAdapter(theWmsServer);
            p->theMapAdapter = p->wmsca;
            setName(tr("Map - WMS-Tiled - %1").arg(p->theMapAdapter->getName()));
            break;
        }
    } else
    if (p->bgType == TMS_ADAPTER_UUID) {
        tsl = M_PREFS->getTmsServers();
        p->selServer = server;
        TmsServer ts = tsl->value(p->selServer);
        p->tmsa = new TileMapAdapter(ts.TmsAdress, ts.TmsPath, ts.TmsTileSize, ts.TmsMinZoom, ts.TmsMaxZoom);
        p->theMapAdapter = p->tmsa;

        setName(tr("Map - TMS - %1").arg(ts.TmsName));
    } else
    if (p->bgType == SHAPE_ADAPTER_UUID) {
        if (!M_PREFS->getUseShapefileForBackground()) {
            p->bgType = NONE_ADAPTER_UUID;
            setName(tr("Map - None"));
            setVisible(false);
        } else {
#if defined(Q_OS_MAC)
            QDir resources = QDir(QCoreApplication::applicationDirPath());
            resources.cdUp();
            resources.cd("Resources");
            QString world_shp = resources.absolutePath() + "/" + STRINGIFY(WORLD_SHP);
            setFilename(world_shp);
#else
            if (QDir::isAbsolutePath(STRINGIFY(WORLD_SHP)))
                setFilename(STRINGIFY(WORLD_SHP));
            else
                setFilename(QCoreApplication::applicationDirPath() + "/" + STRINGIFY(WORLD_SHP));
#endif
        }
            setName(tr("Map - OSB Background"));
            setVisible(true);
    } else
    {
        p->theMapAdapter = M_PREFS->getBackgroundPlugin(p->bgType);
        if (p->theMapAdapter) {
            setName(tr("Map - %1").arg(p->theMapAdapter->getName()));
        } else
            p->bgType = NONE_ADAPTER_UUID;
    }
    if (p->theMapAdapter) {
        p->theProjection.setProjectionType(p->theMapAdapter->projection());
        ImageManager* m;
#ifdef USE_WEBKIT
        BrowserImageManager* b;
#endif
        switch (p->theMapAdapter->getType()) {
            case IMapAdapter::DirectBackground:
                break;
            case IMapAdapter::BrowserBackground :
#ifdef USE_WEBKIT
                b = new BrowserImageManager();
                connect(b, SIGNAL(imageRequested()),
                    this, SLOT(on_imageRequested()), Qt::QueuedConnection);
                connect(b, SIGNAL(imageReceived()),
                    this, SLOT(on_imageReceived()), Qt::QueuedConnection);
                connect(b, SIGNAL(loadingFinished()),
                    this, SLOT(on_loadingFinished()), Qt::QueuedConnection);
                #ifdef BROWSERIMAGEMANAGER_IS_THREADED
                    m->start();
                #endif // BROWSERIMAGEMANAGER_IS_THREADED
                p->theImageManager = b;
                p->theMapAdapter->setImageManager(p->theImageManager);
#endif
                break;
            case IMapAdapter::NetworkBackground :
                m = new ImageManager();
                connect(m, SIGNAL(imageRequested()),
                    this, SLOT(on_imageRequested()), Qt::QueuedConnection);
                connect(m, SIGNAL(imageReceived()),
                    this, SLOT(on_imageReceived()), Qt::QueuedConnection);
                connect(m, SIGNAL(loadingFinished()),
                    this, SLOT(on_loadingFinished()), Qt::QueuedConnection);
                p->theImageManager = m;
                p->theMapAdapter->setImageManager(p->theImageManager);
                break;
        }
        if (p->theImageManager) {
            p->theImageManager->setCacheDir(M_PREFS->getCacheDir());
            p->theImageManager->setCacheMaxSize(M_PREFS->getCacheSize());
        }
    }
}

bool ImageMapLayer::isTiled()
{
    if (!p->theMapAdapter)
        return false;

    return (p->theMapAdapter->isTiled());
}

bool ImageMapLayer::toXML(QDomElement& xParent, QProgressDialog * /* progress */)
{
    bool OK = true;

    QDomElement e = xParent.ownerDocument().createElement(metaObject()->className());
    xParent.appendChild(e);

    e.setAttribute("xml:id", id());
    e.setAttribute("name", name());
    e.setAttribute("alpha", QString::number(getAlpha(),'f',2));
    e.setAttribute("visible", QString((isVisible() ? "true" : "false")));
    e.setAttribute("selected", QString((isSelected() ? "true" : "false")));
    e.setAttribute("enabled", QString((isEnabled() ? "true" : "false")));

    e.setAttribute("bgtype", p->bgType.toString());

    QDomElement c;
    WmsServer ws;
    TmsServer ts;

    if (p->bgType == WMS_ADAPTER_UUID) {
        c = e.ownerDocument().createElement("WmsServer");
        e.appendChild(c);

        c.setAttribute("name", p->selServer);
    } else
    if (p->bgType == TMS_ADAPTER_UUID) {
        c = e.ownerDocument().createElement("TmsServer");
        e.appendChild(c);

        c.setAttribute("name", p->selServer);
    }

    return OK;
}

ImageMapLayer * ImageMapLayer::fromXML(Document* d, const QDomElement& e, QProgressDialog * /*progress*/)
{
    ImageMapLayer* l = new ImageMapLayer(e.attribute("name"));
    l->blockIndexing(true);
    d->addImageLayer(l);
    l->setId(e.attribute("xml:id"));

    QDomElement c = e.firstChildElement();

    QString server;
    if (c.tagName() == "WmsServer") {
        server = c.attribute("name");
    } else
    if (c.tagName() == "TmsServer") {
        server = c.attribute("name");
    }
    l->setMapAdapter(QUuid(e.attribute("bgtype")), server);

    l->setAlpha(e.attribute("alpha").toDouble());
    l->setVisible((e.attribute("visible") == "true" ? true : false));
    l->setSelected((e.attribute("selected") == "true" ? true : false));
    l->setEnabled((e.attribute("enabled") == "false" ? false : true));

    l->blockIndexing(false);
    l->reIndex();
    return l;
}

void ImageMapLayer::drawImage(QPixmap& thePix)
{
    if (!p->theMapAdapter)
        return;
    // Do not draw if saved pixmap is null as a copy of a null pixmap seems to crash on Mac (fixes #2262)
    if (p->pm.isNull())
        return;

    const QSize ps = p->pr.size();
    const QSize pmSize = p->pm.size();
    const qreal ratio = qMax<const qreal>((qreal)pmSize.width()/ps.width()*1.0, (qreal)pmSize.height()/ps.height()*1.0);
    qDebug() << "Bg image ratio " << ratio;
    QPixmap pms;
    if (ratio >= 1.0) {
        qDebug() << "Bg image scale 1 " << ps << " : " << p->pm.size();
        pms = p->pm.scaled(ps);
    } else {
        const QSizeF drawingSize = pmSize * ratio;
        const QSizeF originSize = pmSize/2 - drawingSize/2;
        const QPointF drawingOrigin = QPointF(originSize.width(), originSize.height());
        const QRect drawingRect = QRect(drawingOrigin.toPoint(), drawingSize.toSize());

        qDebug() << "Bg image scale 2 " << ps << " : " << p->pm.size();
        if (ps*ratio != drawingRect.size())
            pms = p->pm.copy(drawingRect).scaled(ps*ratio);
        else
            pms = p->pm.copy(drawingRect);
    }

    QPainter P(&thePix);
    P.setOpacity(getAlpha());
    if (p->theMapAdapter->isTiled())
        P.drawPixmap((pmSize.width()-pms.width())/2, (pmSize.height()-pms.height())/2, pms);
    else
        P.drawPixmap(QPoint((pmSize.width()-pms.width())/2, (pmSize.height()-pms.height())/2) + p->theDelta, pms);
}

using namespace ggl;

void ImageMapLayer::zoom(double zoom, const QPoint& pos, const QRect& rect)
{
    if (!p->theMapAdapter)
        return;

    if (p->theMapAdapter->getImageManager())
        p->theMapAdapter->getImageManager()->abortLoading();

    QPixmap tpm = p->pm.scaled(rect.size() * zoom, Qt::KeepAspectRatio);
    p->pm.fill(Qt::transparent);
    QPainter P(&p->pm);
    P.drawPixmap(pos - (pos * zoom), tpm);
}

void ImageMapLayer::zoom_in()
{
    if (!isTiled())
        return;

    p->theMapAdapter->zoom_in();
}

void ImageMapLayer::zoom_out()
{
    if (!isTiled())
        return;

    p->theMapAdapter->zoom_out();
}

int ImageMapLayer::getCurrentZoom()
{
    if (!isTiled())
        return -1;

    return p->theMapAdapter->getAdaptedZoom();
}

void ImageMapLayer::setCurrentZoom(const CoordBox& viewport, const QRect& rect)
{
    QRectF vp = p->theProjection.getProjectedViewport(viewport, rect);

    qreal tileWidth, tileHeight;
    int maxZoom = p->theMapAdapter->getAdaptedMaxZoom();
    int tilesize = p->theMapAdapter->getTileSize();
    QPointF mapmiddle_px = vp.center();

    // Set zoom level to 0.
    while (p->theMapAdapter->getAdaptedZoom()) {
        p->theMapAdapter->zoom_out();
    }

    tileWidth = p->theMapAdapter->getBoundingbox().width() / p->theMapAdapter->getTilesWE(p->theMapAdapter->getAdaptedZoom());
    tileHeight = p->theMapAdapter->getBoundingbox().height() / p->theMapAdapter->getTilesNS(p->theMapAdapter->getAdaptedZoom());
    qreal w = ((qreal)rect.width() / tilesize) * tileWidth;
    qreal h = ((qreal)rect.height() / tilesize) * tileHeight;
    QPointF upperLeft = QPointF(mapmiddle_px.x() - w/2, mapmiddle_px.y() + h/2);
    QPointF lowerRight = QPointF(mapmiddle_px.x() + w/2, mapmiddle_px.y() - h/2);
    QRectF vlm = QRectF(upperLeft, lowerRight);

    while ((!vp.contains(vlm)) && (p->theMapAdapter->getAdaptedZoom() < maxZoom)) {
        p->theMapAdapter->zoom_in();

        tileWidth = p->theMapAdapter->getBoundingbox().width() / p->theMapAdapter->getTilesWE(p->theMapAdapter->getAdaptedZoom());
        tileHeight = p->theMapAdapter->getBoundingbox().height() / p->theMapAdapter->getTilesNS(p->theMapAdapter->getAdaptedZoom());
        w = ((qreal)rect.width() / tilesize) * tileWidth;
        h = ((qreal)rect.height() / tilesize) * tileHeight;
        upperLeft = QPointF(mapmiddle_px.x() - w/2, mapmiddle_px.y() + h/2);
        lowerRight = QPointF(mapmiddle_px.x() + w/2, mapmiddle_px.y() - h/2);
        vlm = QRectF(upperLeft, lowerRight);
    }
    if (p->theMapAdapter->getAdaptedZoom()  && vp != vlm)
        p->theMapAdapter->zoom_out();
}

#define EQUATORIAL_CIRCUMFERENCE 40074982.83566
qreal ImageMapLayer::pixelPerM()
{
    if (!isTiled())
        return -1.;

    return (p->theMapAdapter->getTileSize() * p->theMapAdapter->getTilesWE(p->theMapAdapter->getAdaptedZoom())) / EQUATORIAL_CIRCUMFERENCE;
}

void ImageMapLayer::forceRedraw(MapView& theView, QRect Screen, QPoint delta)
{
    if (!p->theMapAdapter)
        return;

    if (p->pm.size() != Screen.size()) {
        p->pm = QPixmap(Screen.size());
        p->pm.fill(Qt::transparent);
    }

    MapView::transformCalc(p->theTransform, p->theProjection, theView.viewport(), Screen);

//    QRectF fScreen(Screen);
//    p->Viewport =
//        CoordBox(p->theProjection.inverse(p->theTransform.inverted().map(fScreen.bottomLeft())),
//             p->theProjection.inverse(p->theTransform.inverted().map(fScreen.topRight())));
    p->Viewport = theView.viewport();

    p->theDelta = delta;
    if (p->theMapAdapter->getImageManager())
        p->theMapAdapter->getImageManager()->abortLoading();
    draw(theView, Screen);
}

void ImageMapLayer::draw(MapView& theView, QRect& rect)
{
    if (!p->theMapAdapter)
        return;

    if (p->theMapAdapter->isTiled())
        p->pr = drawTiled(theView, rect);
    else
        p->pr = drawFull(theView, rect);
}

QRect ImageMapLayer::drawFull(MapView& theView, QRect& rect) const
{
    QRectF vp = p->theProjection.getProjectedViewport(p->Viewport, rect);
    QRectF wgs84vp = QRectF(QPointF(intToAng(p->Viewport.bottomLeft().lon()), intToAng(p->Viewport.bottomLeft().lat()))
                        , QPointF(intToAng(p->Viewport.topRight().lon()), intToAng(p->Viewport.topRight().lat())));
    if (p->theMapAdapter->getType() == IMapAdapter::DirectBackground) {
        QPixmap pm = p->theMapAdapter->getPixmap(wgs84vp, vp, rect);
        if (!pm.isNull() && pm.rect() != rect)
            p->pm = pm.scaled(rect.size(), Qt::IgnoreAspectRatio);
        else
            p->pm = pm;
        p->theDelta = QPoint();
    } else {
        QRectF fScreen(rect);
        CoordBox Viewport(p->theProjection.inverse(p->theTransform.inverted().map(fScreen.bottomLeft())),
                         p->theProjection.inverse(p->theTransform.inverted().map(fScreen.topRight())));
        QRectF vp = p->theProjection.getProjectedViewport(Viewport, rect);
        QRectF wgs84vp = QRectF(QPointF(intToAng(Viewport.bottomLeft().lon()), intToAng(Viewport.bottomLeft().lat()))
                            , QPointF(intToAng(Viewport.topRight().lon()), intToAng(Viewport.topRight().lat())));
        QString url (p->theMapAdapter->getQuery(wgs84vp, vp, rect));
        if (!url.isEmpty()) {

            qDebug() << "ImageMapLayer::drawFull: getting: " << url;

            QPixmap pm = p->theMapAdapter->getImageManager()->getImage(p->theMapAdapter,url);
            if (!pm.isNull())  {
                p->pm = pm.scaled(rect.size(), Qt::IgnoreAspectRatio);
                p->theDelta = QPoint();
            }
        }
        const QPointF bl = theView.toView(Viewport.bottomLeft());
        const QPointF tr = theView.toView(Viewport.topRight());

        return QRectF(bl.x(), tr.y(), tr.x() - bl.x(), bl.y() - tr.y()).toRect();
    }

    const QPointF bl = theView.toView(p->Viewport.bottomLeft());
    const QPointF tr = theView.toView(p->Viewport.topRight());

    return QRectF(bl.x(), tr.y(), tr.x() - bl.x(), bl.y() - tr.y()).toRect();
}

QRect ImageMapLayer::drawTiled(MapView& theView, QRect& rect) const
{
    QRectF vp = p->theProjection.getProjectedViewport(p->Viewport, rect);

    qreal tileWidth, tileHeight;
    int maxZoom = p->theMapAdapter->getAdaptedMaxZoom();
    int tilesize = p->theMapAdapter->getTileSize();
    QPointF screenmiddle = QPointF(vp.width()/2, -vp.height()/2);
    QPointF mapmiddle_px = vp.center();

    if (!M_PREFS->getZoomBoris()) {
        // Set zoom level to 0.
        while (p->theMapAdapter->getAdaptedZoom()) {
            p->theMapAdapter->zoom_out();
        }
    }

    tileWidth = p->theMapAdapter->getBoundingbox().width() / p->theMapAdapter->getTilesWE(p->theMapAdapter->getAdaptedZoom());
    tileHeight = p->theMapAdapter->getBoundingbox().height() / p->theMapAdapter->getTilesNS(p->theMapAdapter->getAdaptedZoom());
    qreal w = ((qreal)rect.width() / tilesize) * tileWidth;
    qreal h = ((qreal)rect.height() / tilesize) * tileHeight;
    QPointF upperLeft = QPointF(mapmiddle_px.x() - w/2, mapmiddle_px.y() + h/2);
    QPointF lowerRight = QPointF(mapmiddle_px.x() + w/2, mapmiddle_px.y() - h/2);
    QRectF vlm = QRectF(upperLeft, lowerRight);

    if (!M_PREFS->getZoomBoris()) {
        while ((!vp.contains(vlm)) && (p->theMapAdapter->getAdaptedZoom() < maxZoom)) {
            p->theMapAdapter->zoom_in();

            tileWidth = p->theMapAdapter->getBoundingbox().width() / p->theMapAdapter->getTilesWE(p->theMapAdapter->getZoom());
            tileHeight = p->theMapAdapter->getBoundingbox().height() / p->theMapAdapter->getTilesNS(p->theMapAdapter->getZoom());
            w = ((qreal)rect.width() / tilesize) * tileWidth;
            h = ((qreal)rect.height() / tilesize) * tileHeight;
            upperLeft = QPointF(mapmiddle_px.x() - w/2, mapmiddle_px.y() + h/2);
            lowerRight = QPointF(mapmiddle_px.x() + w/2, mapmiddle_px.y() - h/2);
            vlm = QRectF(upperLeft, lowerRight);
        }
        if (p->theMapAdapter->getAdaptedZoom() && vp.contains(vlm)) {
            p->theMapAdapter->zoom_out();
            tileWidth = p->theMapAdapter->getBoundingbox().width() / p->theMapAdapter->getTilesWE(p->theMapAdapter->getZoom());
            tileHeight = p->theMapAdapter->getBoundingbox().height() / p->theMapAdapter->getTilesNS(p->theMapAdapter->getZoom());
            w = ((qreal)rect.width() / tilesize) * tileWidth;
            h = ((qreal)rect.height() / tilesize) * tileHeight;
            upperLeft = QPointF(mapmiddle_px.x() - w/2, mapmiddle_px.y() + h/2);
            lowerRight = QPointF(mapmiddle_px.x() + w/2, mapmiddle_px.y() - h/2);
            vlm = QRectF(upperLeft, lowerRight);
        }
    }

    p->pm = QPixmap(rect.size());
    p->pm.fill(Qt::transparent);
    QPainter painter(&p->pm);

    // Actual drawing
    int i, j;
    QPointF mapmiddle_px0 = QPointF(mapmiddle_px.x()-p->theMapAdapter->getBoundingbox().left(), p->theMapAdapter->getBoundingbox().bottom()-mapmiddle_px.y());
    int mapmiddle_tile_x = mapmiddle_px0.x()/tileWidth;
    int mapmiddle_tile_y = mapmiddle_px0.y()/tileHeight;
    qDebug() << "z: " << p->theMapAdapter->getAdaptedZoom() << "; t_x: " << mapmiddle_tile_x << "; t_y: " << mapmiddle_tile_y ;

    qreal cross_x = mapmiddle_px0.x() - int(mapmiddle_px0.x()/tileWidth)*tileWidth;		// position on middle tile
    qreal cross_y = mapmiddle_px0.y() - int(mapmiddle_px0.y()/tileHeight)*tileHeight;
    qDebug() << "cross_x: " << cross_x << "; cross_y: " << cross_y;

        // calculate how many surrounding tiles have to be drawn to fill the display
    qreal space_left = screenmiddle.x() - cross_x;
    int tiles_left = space_left/tileWidth;
    if (space_left>0)
        tiles_left+=1;

    qreal space_above = screenmiddle.y() - cross_y;
    int tiles_above = space_above/tileHeight;
    if (space_above>0)
        tiles_above+=1;

    qreal space_right = screenmiddle.x() - (tileWidth-cross_x);
    int tiles_right = space_right/tileWidth;
    if (space_right>0)
        tiles_right+=1;

    qreal space_bottom = screenmiddle.y() - (tileHeight-cross_y);
    int tiles_bottom = space_bottom/tileHeight;
    if (space_bottom>0)
        tiles_bottom+=1;

    QList<Tile> tiles;
    int cross_scr_x = cross_x * tilesize / tileWidth;
    int cross_scr_y = cross_y * tilesize / tileHeight;

    for (i=-tiles_left+mapmiddle_tile_x; i<=tiles_right+mapmiddle_tile_x; i++)
    {
        for (j=-tiles_above+mapmiddle_tile_y; j<=tiles_bottom+mapmiddle_tile_y; j++)
        {
#ifdef Q_CC_MSVC
            double priority = _hypot(i - mapmiddle_tile_x, j - mapmiddle_tile_y);
#else
            double priority = hypot(i - mapmiddle_tile_x, j - mapmiddle_tile_y);
#endif
            tiles.append(Tile(i, j, priority));
        }
    }

    qSort(tiles);

    for (QList<Tile>::const_iterator tile = tiles.begin(); tile != tiles.end(); ++tile)
    {
        if (p->theMapAdapter->isValid(tile->i, tile->j, p->theMapAdapter->getZoom()))
        {
            QPixmap pm = p->theMapAdapter->getImageManager()->getImage(p->theMapAdapter, tile->i, tile->j, p->theMapAdapter->getZoom());
            if (!pm.isNull())
                painter.drawPixmap(((tile->i-mapmiddle_tile_x)*tilesize)+rect.width()/2 -cross_scr_x,
                            ((tile->j-mapmiddle_tile_y)*tilesize)+rect.height()/2-cross_scr_y,
                                                    pm);

            if (MerkaartorPreferences::instance()->getDrawTileBoundary()) {
                painter.drawRect(((tile->i-mapmiddle_tile_x)*tilesize)-cross_scr_x+rect.width()/2,
                          ((tile->j-mapmiddle_tile_y)*tilesize)-cross_scr_y-rect.height()/2,
                                            tilesize, tilesize);
            }
        }
    }
    painter.end();

    Coord ulCoord = p->theProjection.inverse(vlm.topLeft());
    Coord lrCoord = p->theProjection.inverse(vlm.bottomRight());

    const QPointF tl = theView.transform().map(theView.projection().project(ulCoord));
    const QPointF br = theView.transform().map(theView.projection().project(lrCoord));

    qDebug() << "tl: " << tl << "; br: " << br;
    return QRectF(tl, br).toRect();
}

//QRect ImageMapLayer::drawTiled(MapView& theView, QRect& rect) const
//{
//    int tilesize = p->theMapAdapter->getTileSize();
//    QRectF vp = QRectF(QPointF(intToAng(p->Viewport.bottomLeft().lon()), intToAng(p->Viewport.bottomLeft().lat()))
//                        , QPointF(intToAng(p->Viewport.topRight().lon()), intToAng(p->Viewport.topRight().lat())));
//
//    // Set zoom level to 0.
//    while (p->theMapAdapter->getAdaptedZoom()) {
//        p->theMapAdapter->zoom_out();
//    }
//
//    // Find zoom level where tilesize < viewport wdth
//    QPoint mapmiddle_px = p->theMapAdapter->coordinateToDisplay(vp.center());
//    QPoint screenmiddle = rect.center();
//    QRectF vlm = QRectF(QPointF(-180., -90.), QSizeF(360., 180.));
//    int maxZoom = p->theMapAdapter->getAdaptedMaxZoom();
//    while ((!vp.contains(vlm)) && (p->theMapAdapter->getAdaptedZoom() < maxZoom)) {
//        p->theMapAdapter->zoom_in();
//
//        mapmiddle_px = p->theMapAdapter->coordinateToDisplay(vp.center());
//
//        QPoint upperLeft = QPoint(mapmiddle_px.x()-screenmiddle.x(), mapmiddle_px.y()+screenmiddle.y());
//        QPoint lowerRight = QPoint(mapmiddle_px.x()+screenmiddle.x(), mapmiddle_px.y()-screenmiddle.y());
//
//        QPointF ulCoord = p->theMapAdapter->displayToCoordinate(upperLeft);
//        QPointF lrCoord = p->theMapAdapter->displayToCoordinate(lowerRight);
//
//        vlm = QRectF(ulCoord, QSizeF( (lrCoord-ulCoord).x(), (lrCoord-ulCoord).y()));
//    }
//
//    if (p->theMapAdapter->getAdaptedZoom() && vp.contains(vlm))
//        p->theMapAdapter->zoom_out();
//
//    mapmiddle_px = p->theMapAdapter->coordinateToDisplay(vp.center());
//
//    QPoint upperLeft = QPoint(mapmiddle_px.x()-screenmiddle.x(), mapmiddle_px.y()+screenmiddle.y());
//    QPoint lowerRight = QPoint(mapmiddle_px.x()+screenmiddle.x(), mapmiddle_px.y()-screenmiddle.y());
//
//    QPointF ulCoord = p->theMapAdapter->displayToCoordinate(upperLeft);
//    QPointF lrCoord = p->theMapAdapter->displayToCoordinate(lowerRight);
//
//    vlm = QRectF(ulCoord, QSizeF( (lrCoord-ulCoord).x(), (lrCoord-ulCoord).y()));
//
//    p->pm = QPixmap(rect.size());
//    QPainter painter(&p->pm);
//
//    // Actual drawing
//    int i, j;
//
//    int cross_x = int(mapmiddle_px.x())%tilesize;		// position on middle tile
//    int cross_y = int(mapmiddle_px.y())%tilesize;
//
//        // calculate how many surrounding tiles have to be drawn to fill the display
//    int space_left = screenmiddle.x() - cross_x;
//    int tiles_left = space_left/tilesize;
//    if (space_left>0)
//        tiles_left+=1;
//
//    int space_above = screenmiddle.y() - cross_y;
//    int tiles_above = space_above/tilesize;
//    if (space_above>0)
//        tiles_above+=1;
//
//    int space_right = screenmiddle.x() - (tilesize-cross_x);
//    int tiles_right = space_right/tilesize;
//    if (space_right>0)
//        tiles_right+=1;
//
//    int space_bottom = screenmiddle.y() - (tilesize-cross_y);
//    int tiles_bottom = space_bottom/tilesize;
//    if (space_bottom>0)
//        tiles_bottom+=1;
//
//// 	int tiles_displayed = 0;
//    int mapmiddle_tile_x = mapmiddle_px.x()/tilesize;
//    int mapmiddle_tile_y = mapmiddle_px.y()/tilesize;
//
//    QList<Tile> tiles;
//
//    for (i=-tiles_left+mapmiddle_tile_x; i<=tiles_right+mapmiddle_tile_x; i++)
//    {
//        for (j=-tiles_above+mapmiddle_tile_y; j<=tiles_bottom+mapmiddle_tile_y; j++)
//        {
//#ifdef Q_CC_MSVC
//            double priority = _hypot(i - mapmiddle_tile_x, j - mapmiddle_tile_y);
//#else
//            double priority = hypot(i - mapmiddle_tile_x, j - mapmiddle_tile_y);
//#endif
//            tiles.append(Tile(i, j, priority));
//        }
//    }
//
//    qSort(tiles);
//
//    for (QList<Tile>::const_iterator tile = tiles.begin(); tile != tiles.end(); ++tile)
//    {
//        if (p->theMapAdapter->isValid(tile->i, tile->j, p->theMapAdapter->getZoom()))
//        {
//            QPixmap pm = p->theMapAdapter->getImageManager()->getImage(p->theMapAdapter, tile->i, tile->j, p->theMapAdapter->getZoom());
//            if (!pm.isNull())
//                painter.drawPixmap(((tile->i-mapmiddle_tile_x)*tilesize)-cross_x+rect.width()/2,
//                            ((tile->j-mapmiddle_tile_y)*tilesize)-cross_y+rect.height()/2,
//                                                    pm);
//
//            if (MerkaartorPreferences::instance()->getDrawTileBoundary()) {
//                painter.drawRect(((tile->i-mapmiddle_tile_x)*tilesize)-cross_x+rect.width()/2,
//                          ((tile->j-mapmiddle_tile_y)*tilesize)-cross_y+rect.height()/2,
//                                            tilesize, tilesize);
//            }
//        }
//    }
//    painter.end();
//
//    const Coord ctl = Coord(angToInt(vlm.bottomLeft().y()), angToInt(vlm.bottomLeft().x()));
//    const Coord cbr = Coord(angToInt(vlm.topRight().y()), angToInt(vlm.topRight().x()));
//
//    const QPointF tl = theView.transform().map(theView.projection().project(ctl));
//    const QPointF br = theView.transform().map(theView.projection().project(cbr));
//
//    return QRectF(tl, br).toRect();
//}
//
void ImageMapLayer::on_imageRequested()
{
    emit imageRequested(this);
}

void ImageMapLayer::on_imageReceived()
{
    p->theDelta = QPoint();
    emit imageReceived(this);
}

void ImageMapLayer::on_loadingFinished()
{
    emit loadingFinished(this);
}

