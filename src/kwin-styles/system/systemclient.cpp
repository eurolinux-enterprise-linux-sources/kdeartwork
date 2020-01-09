/********************************************************************
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "systemclient.h"

#include <qlayout.h>
#include <qdrawutil.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>
#include <QGridLayout>
#include <QEvent>
#include <QBoxLayout>
#include <Q3ValueList>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include <kpixmapeffect.h>
#include <kdrawutil.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>


namespace System {

static unsigned char iconify_bits[] = {
    0x00, 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00};

static unsigned char maximize_bits[] = {
    0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00};

static unsigned char r_minmax_bits[] = {
    0x0c, 0x18, 0x33, 0x67, 0xcf, 0x9f, 0x3f, 0x3f};

static unsigned char l_minmax_bits[] = {
    0x30, 0x18, 0xcc, 0xe6, 0xf3, 0xf9, 0xfc, 0xfc};

static unsigned char unsticky_bits[] = {
    0x00, 0x18, 0x18, 0x7e, 0x7e, 0x18, 0x18, 0x00};

static unsigned char sticky_bits[] = {
    0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00};

static unsigned char question_bits[] = {
    0x3c, 0x66, 0x60, 0x30, 0x18, 0x00, 0x18, 0x18};

static KPixmap *aUpperGradient=0;
static KPixmap *iUpperGradient=0;

static KPixmap *btnPix=0;
static KPixmap *btnPixDown=0;
static KPixmap *iBtnPix=0;
static KPixmap *iBtnPixDown=0;
static QColor *btnForeground;

static bool pixmaps_created = false;

static void drawButtonFrame(KPixmap *pix, const QColorGroup &g)
{
    QPainter p;
    p.begin(pix);
    p.setPen(g.mid());
    p.drawLine(0, 0, 13, 0);
    p.drawLine(0, 0, 0, 13);
    p.setPen(g.light());
    p.drawLine(13, 0, 13, 13);
    p.drawLine(0, 13, 13, 13);
    p.setPen(g.dark());
    p.drawRect(1, 1, 12, 12);
    p.end();
}

static void create_pixmaps()
{
    if(pixmaps_created)
        return;
    pixmaps_created = true;

    if(QPixmap::defaultDepth() > 8){
        // titlebar
        aUpperGradient = new KPixmap;
        aUpperGradient->resize(32, 18);
        iUpperGradient = new KPixmap;
        iUpperGradient->resize(32, 18);
        QColor bgColor = kapp->palette().active().background();
        KPixmapEffect::gradient(*aUpperGradient,
                                KDecoration::options()->color(KDecorationOptions::ColorFrame, true).light(130),
                                bgColor,
                                KPixmapEffect::VerticalGradient);
        KPixmapEffect::gradient(*iUpperGradient,
                                KDecoration::options()->color(KDecorationOptions::ColorFrame, false).light(130),
                                bgColor,
                                KPixmapEffect::VerticalGradient);

        // buttons
        KPixmap aPix;
        aPix.resize(12, 12);
        KPixmap iPix;
        iPix.resize(12, 12);
        KPixmap aInternal;
        aInternal.resize(8, 8);
        KPixmap iInternal;
        iInternal.resize(8, 8);

        QColor hColor(KDecoration::options()->color(KDecorationOptions::ColorButtonBg, false));
        KPixmapEffect::gradient(iInternal,
                                hColor.dark(120),
                                hColor.light(120),
                                KPixmapEffect::DiagonalGradient);
        KPixmapEffect::gradient(iPix,
                                hColor.light(150),
                                hColor.dark(150),
                                KPixmapEffect::DiagonalGradient);

        hColor =KDecoration::options()->color(KDecorationOptions::ColorButtonBg, true);
        KPixmapEffect::gradient(aInternal,
                                hColor.dark(120),
                                hColor.light(120),
                                KPixmapEffect::DiagonalGradient);
        KPixmapEffect::gradient(aPix,
                                hColor.light(150),
                                hColor.dark(150),
                                KPixmapEffect::DiagonalGradient);
        bitBlt(&aPix, 1, 1, &aInternal, 0, 0, 8, 8, Qt::CopyROP, true);
        bitBlt(&iPix, 1, 1, &iInternal, 0, 0, 8, 8, Qt::CopyROP, true);

        // normal buttons
        btnPix = new KPixmap;
        btnPix->resize(14, 14);
        bitBlt(btnPix, 2, 2, &aPix, 0, 0, 10, 10, Qt::CopyROP, true);
        drawButtonFrame(btnPix, KDecoration::options()->colorGroup(KDecorationOptions::ColorFrame, true));

        iBtnPix = new KPixmap;
        iBtnPix->resize(14, 14);
        bitBlt(iBtnPix, 2, 2, &iPix, 0, 0, 10, 10, Qt::CopyROP, true);
        drawButtonFrame(iBtnPix, KDecoration::options()->colorGroup(KDecorationOptions::ColorFrame, false));


        // pressed buttons
        hColor = KDecoration::options()->color(KDecorationOptions::ColorButtonBg, false);
        KPixmapEffect::gradient(iInternal,
                                hColor.light(130),
                                hColor.dark(130),
                                KPixmapEffect::DiagonalGradient);
        KPixmapEffect::gradient(iPix,
                                hColor.light(150),
                                hColor.dark(150),
                                KPixmapEffect::DiagonalGradient);

        hColor =KDecoration::options()->color(KDecorationOptions::ColorButtonBg, true);
        KPixmapEffect::gradient(aInternal,
                                hColor.light(130),
                                hColor.dark(130),
                                KPixmapEffect::DiagonalGradient);
        KPixmapEffect::gradient(aPix,
                                hColor.light(150),
                                hColor.dark(150),
                                KPixmapEffect::DiagonalGradient);
        bitBlt(&aPix, 1, 1, &aInternal, 0, 0, 8, 8, Qt::CopyROP, true);
        bitBlt(&iPix, 1, 1, &iInternal, 0, 0, 8, 8, Qt::CopyROP, true);

        btnPixDown = new KPixmap;
        btnPixDown->resize(14, 14);
        bitBlt(btnPixDown, 2, 2, &aPix, 0, 0, 10, 10, Qt::CopyROP, true);
        drawButtonFrame(btnPixDown, KDecoration::options()->colorGroup(KDecorationOptions::ColorFrame,
                                                        true));

        iBtnPixDown = new KPixmap;
        iBtnPixDown->resize(14, 14);
        bitBlt(iBtnPixDown, 2, 2, &iPix, 0, 0, 10, 10, Qt::CopyROP, true);
        drawButtonFrame(iBtnPixDown, options()->colorGroup(KDecorationOptions::ColorFrame,
                                                         false));
    }
    if(qGray(KDecoration::options()->color(KDecorationOptions::ColorButtonBg, true).rgb()) > 128)
        btnForeground = new QColor(Qt::black);
    else
        btnForeground = new QColor(Qt::white);
}

static void delete_pixmaps()
{
    if(aUpperGradient){
        delete aUpperGradient;
        delete iUpperGradient;
        delete btnPix;
        delete btnPixDown;
        delete iBtnPix;
        delete iBtnPixDown;
        aUpperGradient = 0;
    }
    delete btnForeground;
    pixmaps_created = false;
}

SystemButton::SystemButton(SystemClient *parent, const char *name,
                           const unsigned char *bitmap)
: KCommonDecorationButton(parent->widget(), name)
{
   setBackgroundMode( Qt::NoBackground );
   setCursor(Qt::ArrowCursor);
   resize(14, 14);
   if(bitmap)
      setBitmap(bitmap);
   client = parent;
}


QSize SystemButton::sizeHint() const
{
    return(QSize(14, 14));
}

void SystemButton::reset(unsigned long changed)
{
  if (changed&DecorationReset || changed&ManualReset || changed&SizeChange || changed&StateChange) {
    switch (type() ) {
      case CloseButton:
        setBitmap(close_bits);
        break;
      case HelpButton:
        setBitmap(question_bits);
        break;
      case MinButton:
        setBitmap(iconify_bits);
        break;
      case MaxButton:
        setBitmap( isOn() ? unmaximize_bits : maximize_bits );
        break;
      case OnAllDesktopsButton:
        setBitmap( isOn() ? unsticky_bits : sticky_bits );
        break;
      case ShadeButton:
        setBitmap( isOn() ? shade_on_bits : shade_off_bits );
        break;
      default:
        setBitmap(0);
        break;
    }

    this->update();
  }
#if 0
  static unsigned char iconify_bits[] = {
    0x00, 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00};

static unsigned char maximize_bits[] = {
    0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00};

static unsigned char r_minmax_bits[] = {
    0x0c, 0x18, 0x33, 0x67, 0xcf, 0x9f, 0x3f, 0x3f};

static unsigned char l_minmax_bits[] = {
    0x30, 0x18, 0xcc, 0xe6, 0xf3, 0xf9, 0xfc, 0xfc};

#endif
}

void SystemButton::setBitmap(const unsigned char *bitmap)
{
    deco = QBitmap(8, 8, bitmap, true);
    deco.setMask(deco);
    repaint();
}

void SystemButton::drawButton(QPainter *p)
{
    if(btnPixDown){
        if(client->isActive())
            p->drawPixmap(0, 0, isDown() ? *btnPixDown : *btnPix);
        else
            p->drawPixmap(0, 0, isDown() ? *iBtnPixDown : *iBtnPix);
    }
    else{
        QColorGroup g = KDecoration::options()->color(KDecorationOptions::ColorFrame,
                                            client->isActive());
        int x2 = width()-1;
        int y2 = height()-1;
        // outer frame
        p->setPen(g.mid());
        p->drawLine(0, 0, x2, 0);
        p->drawLine(0, 0, 0, y2);
        p->setPen(g.light());
        p->drawLine(x2, 0, x2, y2);
        p->drawLine(0, x2, y2, x2);
        p->setPen(g.dark());
        p->drawRect(1, 1, width()-2, height()-2);
        // inner frame
        g = KDecoration::options()->colorGroup(KDecorationOptions::ColorButtonBg, client->isActive());
        p->fillRect(3, 3, width()-6, height()-6, g.background());
        p->setPen(isDown() ? g.mid() : g.light());
        p->drawLine(2, 2, x2-2, 2);
        p->drawLine(2, 2, 2, y2-2);
        p->setPen(isDown() ? g.light() : g.mid());
        p->drawLine(x2-2, 2, x2-2, y2-2);
        p->drawLine(2, x2-2, y2-2, x2-2);

    }

    if(!deco.isNull()){
        p->setPen(*btnForeground);
        p->drawPixmap(isDown() ? 4 : 3, isDown() ? 4 : 3, deco);
    }
}




SystemClient::SystemClient(KDecorationBridge* bridge, KDecorationFactory* factory)
    : KCommonDecoration(bridge, factory)
{}

SystemClient::~SystemClient()
{
}

QString SystemClient::visibleName() const
{
    return i18n( "System++" );
}

QString SystemClient::defaultButtonsLeft() const
{
    return "X";
}

QString SystemClient::defaultButtonsRight() const
{
    return "HSIA";
}

KCommonDecorationButton *SystemClient::createButton(ButtonType type)
{
    switch (type) {
        case OnAllDesktopsButton:
            return new SystemButton(this, "sticky", NULL);

        case HelpButton:
            return new SystemButton(this, "help", question_bits);

        case MinButton:
            return new SystemButton(this, "iconify", iconify_bits);

        case MaxButton:
            return new SystemButton(this, "maximize", maximize_bits);

        case CloseButton:
            return new SystemButton(this, "close", NULL);


        default:
            return 0;
    }
}

void SystemClient::init()
{
    //TODO ?????
    KCommonDecoration::init();
}


void SystemClient::reset(unsigned long changed)
{
#if 0
    titleBuffer.resize(0, 0);
    recalcTitleBuffer();
#endif
    KCommonDecoration::reset(changed);
}

void SystemClient::resizeEvent( QResizeEvent* )
{
    //Client::resizeEvent( e );
    recalcTitleBuffer();
    doShape();
    /*
    if ( isVisibleToTLW() && !testWFlags( WStaticContents )) {
        QPainter p( this );
	QRect t = titlebar->geometry();
	t.setTop( 0 );
	QRegion r = rect();
	r = r.subtract( t );
	p.setClipRegion( r );
	p.eraseRect( rect() );
        }*/
}


QSize SystemClient::minimumSize() const
{
   return widget()->minimumSize();
}


void SystemClient::recalcTitleBuffer()
{
    if(oldTitle == caption() && width() == titleBuffer.width())
        return;
    QFontMetrics fm(options()->font(true));
    titleBuffer.resize(width(), 18);
    QPainter p;
    p.begin(&titleBuffer);
    if(aUpperGradient)
        p.drawTiledPixmap(0, 0, width(), 18, *aUpperGradient);
    else
        p.fillRect(0, 0, width(), 18,
                   options()->colorGroup(KDecorationOptions::ColorFrame, true).
                   brush(QPalette::Button));

    QRect t = titlebar->geometry();
    t.setTop( 2 );
    t.setLeft( t.left() + 4 );
    t.setRight( t.right() - 2 );

    QRegion r(t.x(), 0, t.width(), 18);
    r -= QRect(t.x()+((t.width()-fm.width(caption()))/2)-4,
               0, fm.width(caption())+8, 18);
    p.setClipRegion(r);
    int i, ly;
    for(i=0, ly=4; i < 4; ++i, ly+=3){
        p.setPen(options()->color(KDecorationOptions::ColorTitleBar, true).light(150));
        p.drawLine(0, ly, width()-1, ly);
        p.setPen(options()->color(KDecorationOptions::ColorTitleBar, true).dark(120));
        p.drawLine(0, ly+1, width()-1, ly+1);
    }
    p.setClipRect(t);
    p.setPen(options()->color(KDecorationOptions::ColorFont, true));
    p.setFont(options()->font(true));

    p.drawText(t.x()+((t.width()-fm.width(caption()))/2)-4,
               0, fm.width(caption())+8, 18, Qt::AlignCenter, caption());
    p.setClipping(false);
    p.end();
    oldTitle = caption();
}


void SystemClient::drawRoundFrame(QPainter &p, int x, int y, int w, int h)
{
   kDrawRoundButton(&p, x, y, w, h,
                    options()->colorGroup(KDecorationOptions::ColorFrame, isActive()), false);

}

void SystemClient::paintEvent( QPaintEvent* )
{
    QPainter p(widget());
    QRect t = titlebar->geometry();

    QBrush fillBrush(widget()->palette().brush(QPalette::Background).pixmap() ?
                     widget()->palette().brush(QPalette::Background) :
                     options()->color(KDecorationOptions::ColorFrame, isActive()).
                     brush(QPalette::Button));

    p.fillRect(1, 18, width()-2, height()-19, fillBrush);

    t.setTop( 2 );
    t.setLeft( t.left() + 4 );
    t.setRight( t.right() - 2 );

    if(isActive())
        p.drawPixmap(0, 0, titleBuffer);
    else{
        if(iUpperGradient)
            p.drawTiledPixmap(0, 0, width(), 18, *iUpperGradient);
        else
            p.fillRect(0, 0, width(), 18, fillBrush);
        p.setPen(options()->color(KDecorationOptions::ColorFont, isActive()));
        p.setFont(options()->font(isActive()));
        p.drawText(t, Qt::AlignCenter, caption() );
    }

    p.setPen(options()->color(KDecorationOptions::ColorFrame, isActive()).light());
    p.drawLine(width()-20, height()-7, width()-10,  height()-7);
    p.drawLine(width()-20, height()-5, width()-10,  height()-5);
    p.setPen(options()->color(KDecorationOptions::ColorFrame, isActive()).dark());
    p.drawLine(width()-20, height()-6, width()-10,  height()-6);
    p.drawLine(width()-20, height()-4, width()-10,  height()-4);

    drawRoundFrame(p, 0, 0, width(), height());
}

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

void SystemClient::doShape()
{
    // using a bunch of QRect lines seems much more efficent than bitmaps or
    // point arrays

    QRegion mask;
    kRoundMaskRegion(mask, 0, 0, width(), height());
    setMask(mask);
}

void SystemClient::showEvent(QShowEvent *)
{
//    Client::showEvent(ev);
    doShape();
    widget()->show();
//    widget()->repaint();
}

/*void SystemClient::windowWrapperShowEvent( QShowEvent* )
{
    doShape();
}*/

void SystemClient::mouseDoubleClickEvent( QMouseEvent * e )
{
   if ( e->button() == Qt::LeftButton && titlebar->geometry().contains( e->pos() ) )
      titlebarDblClickOperation();
}




/*void SystemClient::stickyChange(bool on)
{
}*/

KDecoration::Position SystemClient::mousePosition(const QPoint &p) const
{
   return KDecoration::mousePosition(p);
}

void SystemClient::borders(int& left, int& right, int& top, int& bottom) const
{
    left = 4;
    right = 4;
    top =  18;
    bottom = 8;

/*    if ((maximizeMode()==MaximizeFull) && !options()->moveResizeMaximizedWindows()) {
        left = right = bottom = 0;
        top = 1 + titleHeight + (borderSize-1);
    }*/
}

SystemDecoFactory::SystemDecoFactory()
{
   create_pixmaps();
}

SystemDecoFactory::~SystemDecoFactory()
{
   delete_pixmaps();
}

KDecoration *SystemDecoFactory::createDecoration( KDecorationBridge *b )
{
   return new SystemClient(b, this);
}

bool SystemDecoFactory::reset( unsigned long changed )
{
   System::delete_pixmaps();
   System::create_pixmaps();
   // Ensure changes in tooltip state get applied
   resetDecorations(changed);
   return true;
}

bool SystemDecoFactory::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonHelp:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
            return true;
        default:
            return false;
    };
}

QList<KDecorationFactory::BorderSize> SystemDecoFactory::borderSizes() const
{ // the list must be sorted
   return Q3ValueList< BorderSize >() << BorderNormal;
}

}

extern "C" KDE_EXPORT KDecorationFactory *create_factory()
{
   return new System::SystemDecoFactory();
}

#include "systemclient.moc"
