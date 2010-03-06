//
// C++ Implementation: PreferencesDialog
//
// Description:
//
//
// Author: cbro <cbro@semperpax.com>, Bart Vanhauwaert (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "Preferences/PreferencesDialog.h"
#include "PaintStyle/MasPaintStyle.h"

#include "MainWindow.h"
#include "Document.h"
#include "Feature.h"
#include "PropertiesDock.h"

#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QPainter>
#include <QStyleFactory>
#include <QNetworkProxy>


static void makeBoundaryIcon(QToolButton* bt, QColor C)
{
    QPixmap pm(36, 18);
    pm.fill(QColor(255, 255, 255));
    QPainter p(&pm);
    p.setPen(C);
    p.setBrush(C);
    p.drawRect(0, 6, 36, 6);
    bt->setIcon(pm);
}

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
#ifdef _MOBILE
    setWindowState(Qt::WindowFullScreen);
#endif

    setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QDir intTemplates(BUILTIN_TEMPLATES_DIR);
    for (int i=0; i < intTemplates.entryList().size(); ++i) {
        cbTemplates->addItem(intTemplates.entryList().at(i));
    }

    QString qVer = QString(qVersion()).replace(".", "");
    int iQVer = qVer.toInt();
    if (iQVer < 451) {
        cbCustomStyle->hide();
        comboCustomStyle->hide();
    }

    resize(1,1);
    QApplication::processEvents();

    loadPrefs();
}

void PreferencesDialog::updateStyles()
{
    cbStyles->clear();
    QDir intStyles(BUILTIN_STYLES_DIR);
    for (int i=0; i < intStyles.entryList().size(); ++i) {
        cbStyles->addItem(intStyles.entryList().at(i) + " (int)", QVariant(intStyles.entryInfoList().at(i).absoluteFilePath()));
    }
    if (!CustomStylesDir->text().isEmpty()) {
        QDir customStyles(CustomStylesDir->text(), "*.mas");
        for (int i=0; i < customStyles.entryList().size(); ++i) {
            cbStyles->addItem(customStyles.entryList().at(i), QVariant(customStyles.entryInfoList().at(i).absoluteFilePath()));
        }
    }

    int idx = cbStyles->findData(M_PREFS->getDefaultStyle());
    if (idx == -1)
        idx = 0;

    cbStyles->setCurrentIndex(idx);
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::on_buttonBox_clicked(QAbstractButton * button)
{
    if ((button == buttonBox->button(QDialogButtonBox::Apply))) {
        savePrefs();
        emit(preferencesChanged());
    } else
        if ((button == buttonBox->button(QDialogButtonBox::Ok))) {
            savePrefs();
            emit(preferencesChanged());
            this->accept();
        }
}

void PreferencesDialog::initLanguages(QComboBox* aBox)
{
    aBox->addItem(tr("English"),"-");
    aBox->addItem(tr("Arabic"),"ar");
    aBox->addItem(tr("Czech"),"cs");
    aBox->addItem(tr("Dutch"),"nl");
    aBox->addItem(tr("German"),"de");
    aBox->addItem(tr("French"),"fr");
    aBox->addItem(tr("Italian"),"it");
    aBox->addItem(tr("Japanase"),"ja");
    aBox->addItem(tr("Polish"),"pl");
    aBox->addItem(tr("Portuguese"),"pt");
    aBox->addItem(tr("Brazilian Portuguese"),"pt_BR");
    aBox->addItem(tr("Russian"),"ru");
    aBox->addItem(tr("Slovak"),"sk");
    aBox->addItem(tr("Spanish"),"es");
    aBox->addItem(tr("Swedish"),"sv");
    aBox->addItem(tr("Ukrainian"),"uk");
}

void PreferencesDialog::loadPrefs()
{
    initLanguages(Language);
    QString CurrentLanguage(getDefaultLanguage());
    int l;
    for (l = 0; l < Language->count(); ++l)
        if (CurrentLanguage == Language->itemData(l))
            break;
    SelectLanguage->setChecked(l < Language->count());
    Language->setEnabled(l < Language->count());
    if (l < Language->count())
        Language->setCurrentIndex(l);
    TranslateTags->setChecked(M_PREFS->getTranslateTags());
    edOsmUrl->setText(M_PREFS->getOsmWebsite());
    edOsmUser->setText(M_PREFS->getOsmUser());
    edOsmPwd->setText(M_PREFS->getOsmPassword());

    edGpsPort->setText(M_PREFS->getGpsPort());
    edGpsdHost->setText(M_PREFS->getGpsdHost());
    sbGpsdPort->setValue(M_PREFS->getGpsdPort());
    if (M_PREFS->getGpsUseGpsd()) {
        rbGpsGpsd->setChecked(true);
        frGpsSerial->setEnabled(false);
    } else {
        rbGpsSerial->setChecked(true);
        frGpsGpsd->setEnabled(false);
    }

    cbGgpsSaveLog->setChecked(M_PREFS->getGpsSaveLog());
    edGpsLogDir->setText(M_PREFS->getGpsLogDir());
    cbGpsSyncTime->setChecked(M_PREFS->getGpsSyncTime());


    sbMaxDistNodes->setValue(M_PREFS->getMaxDistNodes());

    bbUseProxy->setChecked(M_PREFS->getProxyUse());
    edProxyHost->setText(M_PREFS->getProxyHost());
    edProxyPort->setText(QString().setNum(M_PREFS->getProxyPort()));
    edProxyUser->setText(M_PREFS->getProxyUser());
    edProxyPassword->setText(M_PREFS->getProxyPassword());

    //bbUse06Api->setChecked((M_PREFS->apiVersionNum() > 0.5));

    edCacheDir->setText(M_PREFS->getCacheDir());
    sbCacheSize->setValue(M_PREFS->getCacheSize());

    QString s = M_PREFS->getDefaultStyle();
    QString cs = M_PREFS->getCustomStyle();
    if (QFileInfo(cs).isFile())
        cs = QFileInfo(cs).absolutePath();
    CustomStylesDir->setText(cs);
    updateStyles();

    cbDisableStyleForTracks->setChecked(M_PREFS->getDisableStyleForTracks());

    QString t = M_PREFS->getDefaultTemplate();
    QString ct = M_PREFS->getCustomTemplate();
    CustomTemplateName->setText(ct);
    if (t.startsWith(BUILTIN_TEMPLATES_DIR)) {
        TemplateBuiltin->setChecked(true);
        cbTemplates->setEnabled(true);
        cbTemplates->setCurrentIndex(cbTemplates->findText(t.remove(QString(BUILTIN_TEMPLATES_DIR) + "/")));
    } else {
        TemplateCustom->setChecked(true);
        CustomTemplateName->setEnabled(true);
        BrowseTemplate->setEnabled(true);
    }

    sbZoomInPerc->setValue(M_PREFS->getZoomIn());
    sbZoomOutPerc->setValue(M_PREFS->getZoomOut());
    cbBorisZoom->setChecked(M_PREFS->getZoomBoris());
    if (M_PREFS->getZoomBoris()) {
        sbZoomInPerc->setEnabled(false);
        sbZoomOutPerc->setEnabled(false);
    }

    sbAlphaLow->setValue(M_PREFS->getAlpha("Low"));
    sbAlphaHigh->setValue(M_PREFS->getAlpha("High"));

    sbLocalZoom->setValue(1./M_PREFS->getLocalZoom());

    BgColor = M_PREFS->getBgColor();
    cbBackgroundOverwriteStyle->setChecked(M_PREFS->getBackgroundOverwriteStyle());
    FocusColor = M_PREFS->getFocusColor();
    HoverColor = M_PREFS->getHoverColor();
    HighlightColor = M_PREFS->getHighlightColor();
    RelationsColor = M_PREFS->getRelationsColor();
    GpxTrackColor = M_PREFS->getGpxTrackColor();
    makeBoundaryIcon(btBgColor, BgColor);
    makeBoundaryIcon(btHoverColor, HoverColor);
    makeBoundaryIcon(btHighlightColor, HighlightColor);
    makeBoundaryIcon(btFocusColor, FocusColor);
    makeBoundaryIcon(btRelationsColor, RelationsColor);
    makeBoundaryIcon(btGpxTrackColor, GpxTrackColor);
    HoverWidth->setValue(M_PREFS->getHoverWidth());
    HighlightWidth->setValue(M_PREFS->getHighlightWidth());
    FocusWidth->setValue(M_PREFS->getFocusWidth());
    RelationsWidth->setValue(M_PREFS->getRelationsWidth());
    GpxTrackWidth->setValue(M_PREFS->getGpxTrackWidth());
    cbSimpleGpxTrack->setChecked(M_PREFS->getSimpleGpxTrack());

    cbAutoSaveDoc->setChecked(M_PREFS->getAutoSaveDoc());
    cbAutoExtractTracks->setChecked(M_PREFS->getAutoExtractTracks());
    cbReadonlyTracksDefault->setChecked(M_PREFS->getReadonlyTracksDefault());

    ToolList* tl = M_PREFS->getTools();
    ToolListIterator i(*tl);
    while (i.hasNext()) {
        i.next();
        Tool t(i.value().ToolName, i.value().ToolPath);
        theTools.push_back(t);
        lvTools->addItem(t.ToolName);
    }

    cbMouseSingleButton->setChecked(M_PREFS->getMouseSingleButton());
    cbSeparateMoveMode->setChecked(M_PREFS->getSeparateMoveMode());
    cbVirtualNodes->setChecked(M_PREFS->getUseVirtualNodes());
    cbRelationsHiddenSelectable->setChecked(M_PREFS->getRelationsSelectableWhenHidden());

    cbCustomStyle->setChecked(M_PREFS->getMerkaartorStyle());
    comboCustomStyle->addItems(QStyleFactory::keys());
    comboCustomStyle->setCurrentIndex(comboCustomStyle->findText(M_PREFS->getMerkaartorStyleString()));

    cbUseShapefileForBackground->setChecked(M_PREFS->getUseShapefileForBackground());
}

void PreferencesDialog::savePrefs()
{
    if (SelectLanguage->isChecked())
        setDefaultLanguage(Language->itemData(Language->currentIndex()).toString());
    else
        setDefaultLanguage("");

    ((MainWindow*)parent())->updateLanguage();
    retranslateUi(this);

    M_PREFS->setTranslateTags(TranslateTags->isChecked());
    //M_PREFS->setUse06Api(bbUse06Api->isChecked());

    bool OsmDataChanged = false;
    if (edOsmUrl->text() != M_PREFS->getOsmWebsite() ||
        edOsmUser->text() != M_PREFS->getOsmUser() ||
        edOsmPwd->text() != M_PREFS->getOsmPassword())
        OsmDataChanged = true;

    M_PREFS->setOsmWebsite(edOsmUrl->text());
    M_PREFS->setOsmUser(edOsmUser->text());
    M_PREFS->setOsmPassword(edOsmPwd->text());

    M_PREFS->setXapiWebSite(M_PREFS->getXapiWebSite());

    M_PREFS->setGpsPort(edGpsPort->text());
    M_PREFS->setGpsdHost(edGpsdHost->text());
    M_PREFS->setGpsdPort(sbGpsdPort->value());
    if (rbGpsGpsd->isChecked())
        M_PREFS->setGpsUseGpsd(true);
    else
        M_PREFS->setGpsUseGpsd(false);

    M_PREFS->setGpsSaveLog(cbGgpsSaveLog->isChecked());
    M_PREFS->setGpsLogDir(edGpsLogDir->text());
    M_PREFS->setGpsSyncTime(cbGpsSyncTime->isChecked());

    M_PREFS->setMaxDistNodes(sbMaxDistNodes->value());

    M_PREFS->setProxyUse(bbUseProxy->isChecked());
    M_PREFS->setProxyHost(edProxyHost->text());
    M_PREFS->setProxyPort(edProxyPort->text().toInt());
    M_PREFS->setProxyUser(edProxyUser->text());
    M_PREFS->setProxyPassword(edProxyPassword->text());

    M_PREFS->setCacheDir(edCacheDir->text());
    M_PREFS->setCacheSize(sbCacheSize->value());

    M_PREFS->setCustomStyle(CustomStylesDir->text());
    QString NewStyle;
    NewStyle = cbStyles->itemData(cbStyles->currentIndex()).toString();

    bool PainterToInvalidate = false;
    if (NewStyle != M_PREFS->getDefaultStyle())
    {
        M_PREFS->setDefaultStyle(NewStyle);
        M_STYLE->loadPainters(M_PREFS->getDefaultStyle());
        PainterToInvalidate = true;
    }
    if (cbDisableStyleForTracks->isChecked() != M_PREFS->getDisableStyleForTracks()) {
        M_PREFS->setDisableStyleForTracks(cbDisableStyleForTracks->isChecked());
        PainterToInvalidate = true;
    }
    if (cbSimpleGpxTrack->isChecked() != M_PREFS->getSimpleGpxTrack()) {
        M_PREFS->setSimpleGpxTrack(cbSimpleGpxTrack->isChecked());
        PainterToInvalidate = true;
    }
    if (PainterToInvalidate) {
        for (FeatureIterator it(((MainWindow*)parent())->document()); !it.isEnd(); ++it)
        {
            it.get()->invalidatePainter();
        }
    }

    QString NewTemplate;
    if (TemplateBuiltin->isChecked())
        NewTemplate = QString(BUILTIN_TEMPLATES_DIR) + "/" + cbTemplates->currentText();
    else
        NewTemplate = CustomTemplateName->text();

    if (NewTemplate != M_PREFS->getDefaultTemplate())
    {
        M_PREFS->setDefaultTemplate(NewTemplate);
        ((MainWindow*)parent())->properties()->loadTemplates(NewTemplate);
    }

    M_PREFS->setCustomStyle(CustomStylesDir->text());
    M_PREFS->setCustomTemplate(CustomTemplateName->text());

    M_PREFS->setZoomIn(sbZoomInPerc->text().toInt());
    M_PREFS->setZoomOut(sbZoomOutPerc->text().toInt());
    M_PREFS->setZoomBoris(cbBorisZoom->isChecked());

    M_PREFS->getAlphaPtr()->insert("Low", sbAlphaLow->value());
    M_PREFS->getAlphaPtr()->insert("High", sbAlphaHigh->value());

    M_PREFS->setLocalZoom(1./sbLocalZoom->value());

    M_PREFS->setBgColor(BgColor);
    M_PREFS->setBackgroundOverwriteStyle(cbBackgroundOverwriteStyle->isChecked());
    M_PREFS->setFocusColor(FocusColor,FocusWidth->value());
    M_PREFS->setHoverColor(HoverColor,HoverWidth->value());
    M_PREFS->setHighlightColor(HighlightColor,HighlightWidth->value());
    M_PREFS->setRelationsColor(RelationsColor,RelationsWidth->value());
    M_PREFS->setGpxTrackColor(GpxTrackColor,GpxTrackWidth->value());

    M_PREFS->setAutoSaveDoc(cbAutoSaveDoc->isChecked());
    M_PREFS->setAutoExtractTracks(cbAutoExtractTracks->isChecked());
    M_PREFS->setReadonlyTracksDefault(cbReadonlyTracksDefault->isChecked());

    ToolList* tl = M_PREFS->getTools();
    tl->clear();
    for (int i = 0; i < theTools.size(); ++i) {
        Tool t(theTools[i]);
        tl->insert(theTools[i].ToolName, t);
    }

    M_PREFS->setMouseSingleButton(cbMouseSingleButton->isChecked());
    M_PREFS->setSeparateMoveMode(cbSeparateMoveMode->isChecked());
    M_PREFS->setUseVirtualNodes(cbVirtualNodes->isChecked());
    M_PREFS->setRelationsSelectableWhenHidden(cbRelationsHiddenSelectable->isChecked());

    M_PREFS->setMerkaartorStyle(cbCustomStyle->isChecked());
    M_PREFS->setMerkaartorStyleString(comboCustomStyle->currentText());

    M_PREFS->setUseShapefileForBackground(cbUseShapefileForBackground->isChecked());

    M_PREFS->save(OsmDataChanged);
}

void PreferencesDialog::on_BrowseStyle_clicked()
{
    QString s = QFileDialog::getExistingDirectory(this,tr("Custom styles directory"),"");
    if (!s.isNull())
        CustomStylesDir->setText(QDir::toNativeSeparators(s));

    updateStyles();
}

void PreferencesDialog::on_BrowseTemplate_clicked()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Tag Template"),"",tr("Merkaartor tag template (*.mat)"));
    if (!s.isNull())
        CustomTemplateName->setText(QDir::toNativeSeparators(s));
}

void PreferencesDialog::on_btBgColor_clicked()
{
    QColor rgb = QColorDialog::getColor(BgColor, this, tr("Select Color"), QColorDialog::ShowAlphaChannel);
    if (rgb.isValid()) {
        BgColor = rgb;
        makeBoundaryIcon(btBgColor, BgColor);
    }
}

void PreferencesDialog::on_btFocusColor_clicked()
{
    QColor rgb = QColorDialog::getColor(FocusColor, this, tr("Select Color"), QColorDialog::ShowAlphaChannel);
    if (rgb.isValid()) {
        FocusColor = rgb;
        makeBoundaryIcon(btFocusColor, FocusColor);
    }
}

void PreferencesDialog::on_btHoverColor_clicked()
{
    QColor rgb = QColorDialog::getColor(HoverColor, this, tr("Select Color"), QColorDialog::ShowAlphaChannel);
    if (rgb.isValid()) {
        HoverColor = rgb;
        makeBoundaryIcon(btHoverColor, HoverColor);
    }
}

void PreferencesDialog::on_btHighlightColor_clicked()
{
    QColor rgb = QColorDialog::getColor(HighlightColor, this, tr("Select Color"), QColorDialog::ShowAlphaChannel);
    if (rgb.isValid()) {
        HighlightColor = rgb;
        makeBoundaryIcon(btHighlightColor, HighlightColor);
    }
}

void PreferencesDialog::on_btRelationsColor_clicked()
{
    QColor rgb = QColorDialog::getColor(RelationsColor, this, tr("Select Color"), QColorDialog::ShowAlphaChannel);
    if (rgb.isValid()) {
        RelationsColor = rgb;
        makeBoundaryIcon(btRelationsColor, RelationsColor);
    }
}
void PreferencesDialog::on_btGpxTrackColor_clicked()
{
    QColor rgb = QColorDialog::getColor(GpxTrackColor, this, tr("Select Color"), QColorDialog::ShowAlphaChannel);
    if (rgb.isValid()) {
        GpxTrackColor = rgb;
        makeBoundaryIcon(btGpxTrackColor, GpxTrackColor);
    }
}


/* Tools */
void PreferencesDialog::on_btAddTool_clicked(void)
{
    for (int i=0; i<theTools.size(); ++i) {
        if (theTools[i].ToolName == edToolName->text()) {
        QMessageBox::critical(this, tr("Tool already exists"),
            tr("A tool of this name already exists.\nPlease select another name or click the <Apply> button if you want to modify the existing one"), QMessageBox::Ok);
        return;
        }
    }
    Tool t(edToolName->text(), edToolPath->text());
    theTools.push_back(t);
    lvTools->addItem(t.ToolName);

    lvTools->setCurrentRow(theTools.size() - 1);
    on_lvTools_itemSelectionChanged();
}

void PreferencesDialog::on_btDelTool_clicked(void)
{
    int idx = static_cast<int>(lvTools->currentRow());
    if (idx >= theTools.size())
        return;

    if (theTools[idx].ToolName == "Inkscape") {
        QMessageBox::critical(this, tr("Cannot delete preset tool"),
            tr("Cannot delete preset tool \"%1\"").arg(theTools[idx].ToolName), QMessageBox::Ok);
        return;
    }
    theTools.erase(theTools.begin() + idx);
    delete lvTools->takeItem(idx);
    if (idx && (idx >= theTools.size()))
        --idx;
    lvTools->setCurrentRow(idx);
    on_lvTools_itemSelectionChanged();
}

void PreferencesDialog::on_btApplyTool_clicked(void)
{
    int idx = static_cast<int>(lvTools->currentRow());
    if (idx >= theTools.size())
        return;

    if ((theTools[idx].ToolName == "Inkscape") && edToolName->text() != theTools[idx].ToolName) {
        QMessageBox::critical(this, tr("Cannot modify preset tool name"),
            tr("Cannot modify preset tool \"%1\"'s name").arg(theTools[idx].ToolName), QMessageBox::Ok);
        return;
    }
    Tool& t(theTools[idx]);
    t.ToolName = edToolName->text();
    t.ToolPath = edToolPath->text();

    lvTools->item(lvTools->currentRow())->setText(edToolName->text());
}

void PreferencesDialog::on_lvTools_itemSelectionChanged()
{
    QListWidgetItem* it = lvTools->item(lvTools->currentRow());

    int idx = static_cast<int>(lvTools->row(it));
    if (idx >= theTools.size())
        return;

    Tool& t(theTools[idx]);
    edToolName->setText(t.ToolName);
    edToolPath->setText(t.ToolPath);
}

void PreferencesDialog::on_btBrowse_clicked()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select tool executable"));
    if (!s.isNull()) {
        edToolPath->setText(s);
    }
}

void PreferencesDialog::on_btGpsLogDirBrowse_clicked()
{
    QString s = QFileDialog::getExistingDirectory(this,tr("Select Log directory"));
    if (!s.isNull()) {
        edGpsLogDir->setText(s);
    }
}

void PreferencesDialog::changeEvent(QEvent * event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi(this);
    QDialog::changeEvent(event);
}

