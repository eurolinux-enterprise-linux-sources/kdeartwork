// -*- indent-tabs-mode: nil -*-
//////////////////////////////////////////////////////////////////////////////
// phasestyle.cpp
// -------------------
// Qt widget style
// -------------------
// Copyright (c) 2004-2008 David Johnson <david@usermode.org>
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////
//
// Some miscellaneous notes
//
// Menu and toolbars are painted with the background color by default. This
// differs from the Qt default of giving them PaletteButton backgrounds.
// Menubars have normal gradients, toolbars have reverse.
//
// Some toolbars are not part of a QMainWindows, such as in a KDE file dialog.
// In these cases we treat the toolbar as "floating" and paint it flat.
//
//////////////////////////////////////////////////////////////////////////////

#include "phasestyle.h"

#include <QApplication>
#include <QBitmap>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QKeyEvent>
#include <QLinearGradient>
#include <QMenuBar>
#include <QPainter>
#include <QPixmapCache>
#include <QProgressBar>
#include <QStylePlugin>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QSpinBox>
#include <QSplitterHandle>
#include <QToolBar>

#include <limits.h>

#include "bitmaps.h"

// some convenient constants
static const int ARROWMARGIN     = 6;
static const int ITEMFRAME       = 1;
static const int ITEMHMARGIN     = 3;
static const int ITEMVMARGIN     = 0;
static const int MAXGRADIENTSIZE = 128;

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction, Initialization                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PhaseStyle()
// -----------
// Constructor

PhaseStyle::PhaseStyle()
    : QWindowsStyle(), gradients_(QPixmap::defaultDepth() > 8), timerid_(0)
{
    // get phasestyle config
    QSettings settings(QLatin1String( "phasestyle" ));
    if (gradients_) { // don't bother setting if already false
        gradients_ = settings.value(QLatin1String( "/gradients" ), true).toBool();
    }
    highlights_ = settings.value(QLatin1String( "/highlights" ), true).toBool();

    // get contrast from KDE configs
    QSettings kdesettings(QLatin1String( "Trolltech" ));
    contrast_ = 100 + kdesettings.value(QLatin1String( "/Qt/KDE/contrast" ), 5).toInt();

    // create bitmaps
    const QSize arrowsz(6, 6);
    const QSize btnsz(10, 10);
    bitmaps_.insert(UArrow, QBitmap::fromData(arrowsz, uarrow_bits));
    bitmaps_.insert(DArrow, QBitmap::fromData(arrowsz, darrow_bits));
    bitmaps_.insert(LArrow, QBitmap::fromData(arrowsz, larrow_bits));
    bitmaps_.insert(RArrow, QBitmap::fromData(arrowsz, rarrow_bits));
    bitmaps_.insert(PlusSign, QBitmap::fromData(arrowsz, plussign_bits));
    bitmaps_.insert(MinusSign, QBitmap::fromData(arrowsz, minussign_bits));
    bitmaps_.insert(CheckMark, QBitmap::fromData(btnsz, checkmark_bits));
    bitmaps_.insert(TitleClose, QBitmap::fromData(btnsz, title_close_bits));
    bitmaps_.insert(TitleMin, QBitmap::fromData(btnsz, title_min_bits));
    bitmaps_.insert(TitleMax, QBitmap::fromData(btnsz, title_max_bits));
    bitmaps_.insert(TitleNormal, QBitmap::fromData(btnsz, title_normal_bits));
    bitmaps_.insert(TitleHelp, QBitmap::fromData(btnsz, title_help_bits));
}

PhaseStyle::~PhaseStyle() { ; }

//////////////////////////////////////////////////////////////////////////////
// Polishing                                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize application specific

void PhaseStyle::polish(QApplication* app)
{
    QWindowsStyle::polish(app);
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the appearance of widget

void PhaseStyle::polish(QWidget *widget)
{
    if (highlights_ &&
               (qobject_cast<QPushButton*>(widget) ||
                qobject_cast<QComboBox*>(widget) ||
                qobject_cast<QAbstractSpinBox*>(widget) ||
                qobject_cast<QCheckBox*>(widget) ||
                qobject_cast<QRadioButton*>(widget) ||
                qobject_cast<QSplitterHandle*>(widget) ||
                qobject_cast<QSlider*>(widget) ||
                qobject_cast<QTabBar*>(widget))) {
        // mouseover highlighting
        widget->setAttribute(Qt::WA_Hover);
    }
    if (widget->inherits("QDockSeparator")
        || widget->inherits("QDockWidgetSeparator")) {
        widget->setAttribute(Qt::WA_Hover);
    }
    if (qobject_cast<QProgressBar*>(widget)) {
        // potentially animate progressbars
        widget->installEventFilter(this);
    }
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the palette

void PhaseStyle::polish(QPalette &pal)
{
    // clear out gradients on a color change
    QPixmapCache::clear();

    // adjust bevel colors to have better contrast (KDE like)
    QColor background;

    int highlightval = 100 + (2*(contrast_-100)+4)*16/10;
    int lowlightval = 100 + (2*(contrast_-100)+4)*10;

    background = pal.color(QPalette::Active, QPalette::Window);
    pal.setColor(QPalette::Active, QPalette::Light,
                 background.lighter(highlightval));
    pal.setColor(QPalette::Active, QPalette::Dark,
                 background.darker(lowlightval));
    pal.setColor(QPalette::Active, QPalette::Mid,
                 background.darker(120));
    pal.setColor(QPalette::Active, QPalette::Midlight,
                 background.lighter(110));

    background = pal.color(QPalette::Inactive, QPalette::Window);
    pal.setColor(QPalette::Inactive, QPalette::Light,
                 background.lighter(highlightval));
    pal.setColor(QPalette::Inactive, QPalette::Dark,
                 background.darker(lowlightval));
    pal.setColor(QPalette::Inactive, QPalette::Mid,
                 background.darker(120));
    pal.setColor(QPalette::Inactive, QPalette::Midlight,
                 background.lighter(110));

    background = pal.color(QPalette::Disabled, QPalette::Window);
    pal.setColor(QPalette::Disabled, QPalette::Light,
                 background.lighter(highlightval));
    pal.setColor(QPalette::Disabled, QPalette::Dark,
                 background.darker(lowlightval));
    pal.setColor(QPalette::Disabled, QPalette::Mid,
                 background.darker(120));
    pal.setColor(QPalette::Disabled, QPalette::Midlight,
                 background.lighter(110));
}

//////////////////////////////////////////////////////////////////////////////
// unPolish()
// ----------
// Undo the application polish

void PhaseStyle::unpolish(QApplication *app)
{
    QWindowsStyle::unpolish(app);
}

//////////////////////////////////////////////////////////////////////////////
// unPolish()
// ----------
// Undo the initialization of a widget appearance

void PhaseStyle::unpolish(QWidget *widget)
{
    if (highlights_ &&
               (qobject_cast<QPushButton*>(widget) ||
                qobject_cast<QComboBox*>(widget) ||
                qobject_cast<QAbstractSpinBox*>(widget) ||
                qobject_cast<QCheckBox*>(widget) ||
                qobject_cast<QRadioButton*>(widget) ||
                qobject_cast<QSplitterHandle*>(widget) ||
                qobject_cast<QSlider*>(widget) ||
                qobject_cast<QTabBar*>(widget))) {
        // turn off mouseover highlighting
        widget->setAttribute(Qt::WA_Hover, false);
    }
    if (widget->inherits("QDockSeparator")
        || widget->inherits("QDockWidgetSeparator")) {
        widget->setAttribute(Qt::WA_Hover, false);
    }
    if (qobject_cast<QProgressBar*>(widget)) {
        widget->removeEventFilter(this);
    }
}

//////////////////////////////////////////////////////////////////////////////
// standardPalette()
// -----------------
// Return a standard palette

QPalette PhaseStyle::standardPalette() const
{
    QColor window(0xee, 0xee, 0xee);
    QColor button(0xdd, 0xdd, 0xe3);
    QColor highlight(0x60, 0x90, 0xc0);
    QPalette pal(window, window);

    pal.setBrush(QPalette::Button, button);
    pal.setBrush(QPalette::Highlight, highlight);

    pal.setBrush(QPalette::Disabled, QPalette::Button, window);
    pal.setBrush(QPalette::Disabled, QPalette::Foreground, window.darker());
    pal.setBrush(QPalette::Disabled, QPalette::Text, window.darker());
    pal.setBrush(QPalette::Disabled, QPalette::ButtonText, window.darker());

    return pal;
}

//////////////////////////////////////////////////////////////////////////////
// Drawing                                                                  //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// drawPhaseGradient()
// ------------------
// Draw a gradient

void PhaseStyle::drawPhaseGradient(QPainter *painter,
                                   const QRect &rect,
                                   QColor color,
                                   bool horizontal,
                                   const QSize &gsize,
                                   bool reverse) const
{
    if (!gradients_) {
        painter->fillRect(rect, color);
        return;
    }

    int size = (horizontal) ? gsize.width() :  gsize.height();

    if (size > MAXGRADIENTSIZE) { // keep it sensible
        painter->fillRect(rect, color);
        return;
    }

    GradientType type;
    QString name;
    QPixmap pixmap;

    if (horizontal) type = (reverse) ? HorizontalReverse : Horizontal;
    else            type = (reverse) ? VerticalReverse : Vertical;

    name = QString(QLatin1String( "%1.%2.%3" )).arg(color.name()).arg(size).arg(type);
    if (!QPixmapCache::find(name, pixmap)) {
        QPainter cachepainter;

        switch (type) {
          case Horizontal: {
              pixmap = QPixmap(size, 16);
              QLinearGradient gradient(0, 0, size, 0);
              gradient.setColorAt(0, color.lighter(contrast_));
              gradient.setColorAt(1, color.darker(contrast_));
              cachepainter.begin(&pixmap);
              cachepainter.fillRect(pixmap.rect(), gradient);
              cachepainter.end();
              break;
          }
          case HorizontalReverse: {
              pixmap = QPixmap(size, 16);
              QLinearGradient gradient(0, 0, size, 0);
              gradient.setColorAt(0, color.darker(contrast_));
              gradient.setColorAt(1, color.lighter(contrast_));
              cachepainter.begin(&pixmap);
              cachepainter.fillRect(pixmap.rect(), gradient);
              cachepainter.end();
              break;
          }
          case Vertical: {
              pixmap = QPixmap(16, size);
              QLinearGradient gradient(0, 0, 0, size);
              gradient.setColorAt(0, color.lighter(contrast_));
              gradient.setColorAt(1, color.darker(contrast_));
              cachepainter.begin(&pixmap);
              cachepainter.fillRect(pixmap.rect(), gradient);
              cachepainter.end();
              break;
          }
          case VerticalReverse: {
              pixmap = QPixmap(16, size);
              QLinearGradient gradient(0, 0, 0, size);
              gradient.setColorAt(0, color.darker(contrast_));
              gradient.setColorAt(1, color.lighter(contrast_));
              cachepainter.begin(&pixmap);
              cachepainter.fillRect(pixmap.rect(), gradient);
              cachepainter.end();
              break;
          }
          default:
              break;
        }
        QPixmapCache::insert(name, pixmap);
    }

    painter->drawTiledPixmap(rect, pixmap);
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseBevel()
// ----------------
// Draw the basic Phase bevel

void PhaseStyle::drawPhaseBevel(QPainter *painter,
                                QRect rect,
                                const QPalette &pal,
                                const QBrush &fill,
                                bool sunken,
                                bool horizontal,
                                bool reverse) const
{
    int x, y, w, h;
    rect.getRect(&x, &y, &w, &h);
    int x2 = rect.right();
    int y2 = rect.bottom();

    QPen pen = painter->pen();

    painter->setPen(pal.dark().color());
    painter->drawRect(rect.adjusted(0, 0, -1, -1));

    painter->setPen(sunken ? pal.mid().color() : pal.midlight().color());
    painter->drawLine(x+1, y+1, x2-2, y+1);
    painter->drawLine(x+1, y+2, x+1, y2-2);

    painter->setPen(sunken ? pal.midlight().color() : pal.mid().color());
    painter->drawLine(x+2, y2-1, x2-1, y2-1);
    painter->drawLine(x2-1, y+2, x2-1, y2-2);

    painter->setPen(pal.button().color());
    painter->drawPoint(x+1, y2-1);
    painter->drawPoint(x2-1, y+1);

    if (sunken) {
        // sunken bevels don't get gradients
        painter->fillRect(rect.adjusted(2, 2, -2, -2), fill);
    } else {
        drawPhaseGradient(painter, rect.adjusted(2, 2, -2, -2), fill.color(),
                          horizontal, QSize(w-4, h-4), reverse);
    }
    painter->setPen(pen);
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseButton()
// ----------------
// Draw the basic Phase button

void PhaseStyle::drawPhaseButton(QPainter *painter,
                                 QRect rect,
                                 const QPalette &pal,
                                 const QBrush &fill,
                                 bool sunken) const
{
    int x, y, w, h;
    rect.getRect(&x, &y, &w, &h);
    int x2 = rect.right();
    int y2 = rect.bottom();

    QPen pen = painter->pen();

    painter->setPen(pal.midlight().color());
    painter->drawLine(x+1, y2, x2, y2);
    painter->drawLine(x2, y+1, x2, y2-1);

    painter->setPen(pal.mid().color());
    painter->drawLine(x, y,  x2-1, y);
    painter->drawLine(x, y+1, x, y2-1);

    painter->setPen(pal.window().color());
    painter->drawPoint(x, y2);
    painter->drawPoint(x2, y);

    painter->setPen(pen);
    drawPhaseBevel(painter, rect.adjusted(1, 1, -1, -1), pal, fill,
                   sunken, false, false);
}

//////////////////////////////////////////////////////////////////////////////
// drawPhasePanel()
// ----------------
// Draw the basic Phase panel

void PhaseStyle::drawPhasePanel(QPainter *painter,
                                const QRect &rect,
                                const QPalette &pal,
                                const QBrush &fill,
                                bool sunken) const
{
    int x, y, w, h;
    rect.getRect(&x, &y, &w, &h);
    int x2 = rect.right();
    int y2 = rect.bottom();

    QPen pen = painter->pen();

    if (sunken) {
        painter->setPen(pal.dark().color());
        painter->drawRect(rect.adjusted(1, 1, -2, -2));
        painter->setPen(pal.midlight().color());
        painter->drawLine(x+1, y2, x2, y2);
        painter->drawLine(x2, y+1, x2, y2-1);
        painter->setPen(pal.mid().color());
        painter->drawLine(x, y, x, y2-1);
        painter->drawLine(x+1, y, x2-1, y);
        painter->setPen(pal.window().color());
        painter->drawPoint(x, y2);
        painter->drawPoint(x2, y);
    } else {
        painter->setPen(pal.dark().color());
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        painter->setPen(pal.midlight().color());
        painter->drawLine(x+1, y+1, x2-2, y+1);
        painter->drawLine(x+1, y+2, x+1, y2-2);
        painter->setPen(pal.mid().color());
        painter->drawLine(x+2, y2-1, x2-1, y2-1);
        painter->drawLine(x2-1, y+2, x2-1, y2-2);
        painter->setPen(pal.window().color());
        painter->drawPoint(x+1, y2-1);
        painter->drawPoint(x2-1, y+1);
    }

    painter->fillRect(rect.adjusted(2, 2, -2, -2), fill);
    painter->setPen(pen);
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseDoodads()
// ------------------
// Draw three doodads in rect

void PhaseStyle::drawPhaseDoodads(QPainter *painter,
                                  const QRect& rect,
                                  const QPalette &pal,
                                  bool horizontal) const
{
    int cx = rect.center().x();
    int cy = rect.center().y();

    QPen pen = painter->pen();
    if (horizontal && (rect.width() >= 20)) {
        for (int n = -5; n <= 5; n += 5) {
            painter->setPen(pal.mid().color());
            painter->drawLine(cx-1+n, cy+1, cx-1+n, cy-1);
            painter->drawLine(cx+n, cy-1, cx+1+n, cy-1);
            painter->setPen(pal.light().color());
            painter->drawLine(cx+2+n, cy, cx+2+n, cy+2);
            painter->drawLine(cx+1+n, cy+2, cx+n, cy+2);
        }
    } else if (!horizontal && (rect.height() >= 20)) {
        for (int n = -5; n <= 5; n += 5) {
            painter->setPen(pal.mid().color());
            painter->drawLine(cx-1, cy+1+n, cx-1, cy-1+n);
            painter->drawLine(cx, cy-1+n, cx+1, cy-1+n);
            painter->setPen(pal.light().color());
            painter->drawLine(cx+2, cy+n, cx+2, cy+2+n);
            painter->drawLine(cx+1, cy+2+n, cx, cy+2+n);
        }
    }
    painter->setPen(pen);
}

///////////////////////////////////////////////////////////////////////////////
// drawPhaseTab()
// -------------
// Draw a Phase style tab

void PhaseStyle::drawPhaseTab(QPainter *painter,
                              const QPalette &pal,
                              const QStyleOptionTab *tab) const
{
    const State &flags = tab->state;
    const QStyleOptionTab::TabPosition &tabpos = tab->position;
    const QStyleOptionTab::SelectedPosition &selpos = tab->selectedPosition;

    bool selected = (flags & State_Selected);
    bool mouseover = (flags & State_MouseOver) && !selected;
    bool vertical = (tab->shape == QTabBar::RoundedEast) ||
                    (tab->shape == QTabBar::RoundedWest);
    bool reverse = (tab->direction == Qt::RightToLeft);
    bool corner = (tab->cornerWidgets & QStyleOptionTab::LeftCornerWidget);
    bool first = (tabpos == QStyleOptionTab::Beginning);
    bool rlast = (tabpos == QStyleOptionTab::End) && reverse;
    bool only = (tabpos == QStyleOptionTab::OnlyOneTab);
    bool edge = (first && !corner) || only;
    bool leftedge = vertical ? edge : (edge && !reverse);
    bool rightedge = vertical ? edge : (edge && reverse);
    bool prevselected = (selpos == QStyleOptionTab::PreviousIsSelected);
    bool nextselected = (selpos == QStyleOptionTab::NextIsSelected);
    if (reverse && !vertical) {
        qSwap(prevselected, nextselected);
    }

    // get rectangle
    QRect rect = tab->rect;
    if (!selected) {
        switch (tab->shape) {
          case QTabBar::RoundedNorth:
          default:
              rect.adjust(0, 2, 0, 0);
              break;

          case QTabBar::RoundedSouth:
              rect.adjust(0, 0, 0, -2);
              break;

          case QTabBar::RoundedWest:
              rect.adjust(2, 0, 0, 0);
              break;

          case QTabBar::RoundedEast:
              rect.adjust(0, 0, -2, 0);
              break;
        }
    }

    int x, y, w, h;
    rect.getRect(&x, &y, &w, &h);
    const int x2 = rect.right();
    const int y2 = rect.bottom();

    painter->save();

    // draw tab
    switch (tab->shape) {
      case QTabBar::RoundedNorth:
      default:
          // draw fill
          if (selected) {
              painter->fillRect(rect.adjusted(0, 1, 0, 0), pal.window());
          } else {
              drawPhaseGradient(painter, rect.adjusted(0, 1, 0, -2),
                                mouseover ?
                                pal.window().color() :
                                pal.window().color().darker(contrast_),
                                false, QSize(w, h), false);
          }

          // top frame
          painter->setPen(pal.dark().color());
          painter->drawLine(x, y, x2, y);

          painter->setPen(pal.midlight().color());
          if (nextselected)      painter->drawLine(x+1, y+1, x2,   y+1);
          else if (prevselected) painter->drawLine(x,   y+1, x2-2, y+1);
          else                   painter->drawLine(x+1, y+1, x2-2, y+1);

          // left frame
          painter->setPen(pal.dark().color());
          if (leftedge && selected)  painter->drawLine(x, y, x, y2);
          if (leftedge && !selected) painter->drawLine(x, y, x, y2-2);
          if (!leftedge && selected) painter->drawLine(x, y, x, y2-1);
          if (!selected && rlast)    painter->drawLine(x, y, x, y2-2);
          painter->setPen(pal.midlight().color());
          if (selected)           painter->drawLine(x+1, y+1, x+1, y2);
          else if (leftedge)      painter->drawLine(x+1, y+1, x+1, y2-2);
          else if (rlast)         painter->drawLine(x+1, y+1, x+1, y2-2);
          else if (!prevselected) painter->drawLine(x,   y+1, x,   y2-2);
          if (selected && !leftedge) painter->drawPoint(x, y2);

          // right frame
          if (!nextselected) {
              painter->setPen(pal.dark().color());
              if (rightedge && selected) painter->drawLine(x2, y+1, x2, y2);
              if (rightedge)             painter->drawLine(x2, y+1, x2, y2+1);
              else                       painter->drawLine(x2, y+1, x2, y2-1);
              painter->setPen(pal.mid().color());
              if (rightedge && selected) painter->drawLine(x2-1, y+2, x2-1, y2);
              else if (selected) painter->drawLine(x2-1, y+2, x2-1, y2-1);
              else               painter->drawLine(x2-1, y+2, x2-1, y2-2);

              if (selected && !rightedge) {
                  painter->setPen(pal.midlight().color());
                  painter->drawPoint(x2, y2);
              }
          }

          break;

      case QTabBar::RoundedSouth:
          // draw fill
          if (selected) {
              painter->fillRect(rect.adjusted(0, 0, 0, -1), pal.window());
          } else {
              drawPhaseGradient(painter, rect.adjusted(0, 2, 0, -1),
                                mouseover ?
                                pal.window().color() :
                                pal.window().color().darker(contrast_),
                                false, QSize(w, h), false);
          }

          // bottom frame
          painter->setPen(pal.dark().color());
          painter->drawLine(x, y2, x2, y2);

          painter->setPen(pal.mid().color());
          if (nextselected)      painter->drawLine(x, y2-1, x2,   y2-1);
          else                   painter->drawLine(x, y2-1, x2-1, y2-1);

          painter->setPen(pal.window().color());
          if (selected || first)  painter->drawPoint(x+1, y2-1);
          else if (!prevselected) painter->drawPoint(x,   y2-1);

          // left frame
          painter->setPen(pal.dark().color());
          if (selected && leftedge) painter->drawLine(x, y,   x, y2);
          else if (selected || leftedge || rlast) painter->drawLine(x, y+1, x, y2);

          painter->setPen(pal.midlight().color());
          if (!prevselected) {
              if (leftedge && selected)       painter->drawLine(x+1, y,   x+1, y2-2);
              else if (leftedge && !selected) painter->drawLine(x+1, y+2, x+1, y2-2);
              else if (selected)              painter->drawLine(x+1, y+1, x+1, y2-2);
              else if (rlast)                 painter->drawLine(x+1, y+2, x+1, y2-2);
              else                            painter->drawLine(x,   y+2, x,   y2-2);
          }

          if (selected && !leftedge) {
              painter->setPen(pal.mid().color());
              painter->drawPoint(x, y);
          }

          // right frame
          if (!nextselected) {
              painter->setPen(pal.dark().color());
              if (rightedge && selected) painter->drawLine(x2, y, x2, y2-1);
              else                       painter->drawLine(x2, y+1, x2, y2-1);

              painter->setPen(pal.mid().color());
              if (selected) {
                  painter->drawLine(x2-1, y,   x2-1, y2-2);
                  if (!rightedge) painter->drawPoint(x2, y);
              } else {
                  painter->drawLine(x2-1, y+2, x2-1, y2-2);
              }
          }
          break;

      case QTabBar::RoundedWest:
          // draw fill
          if (selected) {
              painter->fillRect(rect.adjusted(1, 0, 0, 0), pal.window());
          } else {
              drawPhaseGradient(painter, rect.adjusted(1, 0, -2, 0),
                                mouseover ?
                                pal.window().color() :
                                pal.window().color().darker(contrast_),
                                true, QSize(w, h), false);
          }

          // left frame
          painter->setPen(pal.dark().color());
          painter->drawLine(x, y, x, y2);

          painter->setPen(pal.midlight().color());
          if (nextselected)      painter->drawLine(x+1, y+1, x+1, y2);
          else if (prevselected) painter->drawLine(x+1, y,   x+1, y2-2);
          else                   painter->drawLine(x+1, y+1, x+1, y2-2);

          // top frame
          painter->setPen(pal.dark().color());
          if (leftedge || selected) painter->drawLine(x, y, x2-1, y);
          if (leftedge && selected) painter->drawPoint(x2, y);

          painter->setPen(pal.midlight().color());
          if (leftedge && selected)       painter->drawLine(x+1, y+1, x2,   y+1);
          else if (leftedge && !selected) painter->drawLine(x+1, y+1, x2-2, y+1);
          else if (selected)              painter->drawLine(x+1, y+1, x2,   y+1);
          else if (!prevselected)         painter->drawLine(x+1, y,   x2-2, y);
          if (!leftedge && selected)      painter->drawPoint(x2, y);

          // bottom frame
          if (!nextselected) {
              painter->setPen(pal.dark().color());
              if (rightedge && selected) painter->drawLine(x, y2, x2, y2);
              else if (selected)        painter->drawLine(x, y2, x2-1, y2);
              else                       painter->drawLine(x, y2, x2-2, y2);

              painter->setPen(pal.mid().color());
              if (selected) painter->drawLine(x+2, y2-1, x2-1, y2-1);
              else          painter->drawLine(x+2, y2-1, x2-2, y2-1);

              if (selected) {
                  painter->setPen(pal.midlight().color());
                  if (!rightedge) painter->drawPoint(x2, y2);
              }
          }
          break;

      case QTabBar::RoundedEast:
          // draw fill
          if (selected) {
              painter->fillRect(rect.adjusted(0, 0, -1, 0), pal.window());
          } else {
              drawPhaseGradient(painter, rect.adjusted(2, 0, -1, 0),
                                mouseover ?
                                pal.window().color() :
                                pal.window().color().darker(contrast_),
                                true, QSize(w, h), false);
          }

          // right frame
          painter->setPen(pal.dark().color());
          painter->drawLine(x2, y, x2, y2);

          painter->setPen(pal.mid().color());
          if (selected)          painter->drawLine(x2-1, y+2, x2-1, y2-1);
          else if (leftedge)     painter->drawLine(x2-1, y+2, x2-1, y2);
          else if (nextselected) painter->drawLine(x2-1, y+1, x2-1, y2);
          else if (prevselected) painter->drawLine(x2-1, y,   x2-1, y2-1);
          else                   painter->drawLine(x2-1, y+1, x2-1, y2-1);

          // top frame
          painter->setPen(pal.dark().color());
          if (leftedge || selected) painter->drawLine(x+1, y, x2, y);
          if (leftedge && selected) painter->drawPoint(x, y);

          painter->setPen(pal.midlight().color());
          if (leftedge && selected)       painter->drawLine(x,   y+1, x2-2, y+1);
          else if (leftedge && !selected) painter->drawLine(x+2, y+1, x2-2, y+1);
          else if (selected)              painter->drawLine(x+1, y+1, x2-2, y+1);
          else if (!prevselected)         painter->drawLine(x+2, y,   x2-2, y);
          if (!leftedge && selected)      painter->drawPoint(x+1, y+1);

          if (!leftedge && selected) {
              painter->setPen(pal.mid().color());
              painter->drawPoint(x, y);
          }

          // bottom frame
          if (!nextselected) {
              painter->setPen(pal.dark().color());
              if (rightedge && selected) painter->drawLine(x, y2, x2, y2);
              else if (selected)         painter->drawLine(x+1, y2, x2, y2);
              else                       painter->drawLine(x+1, y2, x2, y2);

              painter->setPen(pal.mid().color());
              if (selected) {
                  painter->drawLine(x,   y2-1, x2-2, y2-1);
                  if (!rightedge) painter->drawPoint(x, y2);
              } else {
                  painter->drawLine(x+2, y2-1, x2-2, y2-1);
              }
          }
          break;
    }

    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawPrimitive()
// ---------------
// Draw a primitive element

void PhaseStyle::drawPrimitive(PrimitiveElement element,
                               const QStyleOption *option,
                               QPainter *painter,
                               const QWidget *widget) const
{
    // shorthand
    const State &flags = option->state;
    const QPalette &pal = option->palette;
    const QRect &rect = option->rect;

    // common states
    bool sunken    = flags & State_Sunken;
    bool on        = flags & State_On;
    bool depress   = (sunken || on);
    bool enabled   = flags & State_Enabled;
    bool horiz     = flags & State_Horizontal;
    bool mouseover = highlights_ && (flags & State_MouseOver) && enabled;

    int x, y, w, h;
    rect.getRect(&x, &y, &w, &h);
    int x2 = rect.right();
    int y2 = rect.bottom();
    int cx = rect.center().x();
    int cy = rect.center().y();

    QPolygon polygon;

    painter->save();

    switch (element) {
      case PE_PanelButtonCommand:       // command button (QPushbutton)
          drawPhaseButton(painter, rect, pal, mouseover ?
                          pal.button().color().lighter(contrast_) :
                          pal.button(), depress);
          break;

      case PE_FrameButtonBevel:         // panel frame for a button bevel
      case PE_FrameButtonTool:          // panel frame for a tool button
          drawPhaseBevel(painter, rect, pal, Qt::NoBrush,
                         depress, false, false);
          break;

      case PE_PanelButtonBevel:         // generic panel with a button bevel
      case PE_IndicatorButtonDropDown:  // indicator for a drop down button
          drawPhaseBevel(painter, rect, pal, mouseover ?
                         pal.button().color().lighter(contrast_) :
                         pal.button(),
                         depress, false, false);
          break;

      case PE_PanelButtonTool:          // panel for a tool button
          if (widget && widget->inherits("QDockWidgetTitleButton")) {
              bool floating = (widget->parentWidget() && widget->parentWidget()->isWindow());

              if (mouseover || floating) {
                  drawPhasePanel(painter, rect, pal,
                                 pal.window().color(), depress);
              }
              break;
          }

          drawPhaseBevel(painter, rect, pal, mouseover ?
                         pal.button().color().lighter(contrast_) :
                         pal.button(),
                         depress, false, true);
          break;

      case PE_FrameDefaultButton:       // the frame around a default button
          drawPhasePanel(painter, rect, pal, pal.mid(), true);
          break;

      case PE_Frame:                    // generic styled frame
      case PE_FrameLineEdit:            // frame around line edit
      case PE_FrameMenu:                // frame for popup windows/menus
      case PE_FrameDockWidget:          // frame for dock windows and toolbars
      case PE_FrameTabWidget:           // frame around tab widgets
          drawPhasePanel(painter, rect, pal, Qt::NoBrush, sunken);
          break;

      case PE_FrameTabBarBase:          // frame for base of a tab bar
          // don't draw anything
          break;

      case PE_FrameWindow:              // frame for MDI or docking window
          drawPhasePanel(painter, rect, pal, Qt::NoBrush, sunken);
          // needs a black border
          painter->setPen(pal.shadow().color());
          painter->drawRect(rect.adjusted(0, 0, -1, -1));
          break;

      case PE_FrameGroupBox:            // frame around a group box
          painter->setPen(pal.dark().color());
          painter->drawRect(rect.adjusted(0, 0, -1, -1));
          break;

      case PE_FrameFocusRect: {         // generic focus indicator
          QColor focus(pal.highlight().color());
          focus.setAlphaF(0.66);
          painter->setPen(focus);
          painter->drawRect(rect.adjusted(0, 0, -1, -1));
          break;
      }

      case PE_IndicatorCheckBox:        // on/off indicator for check box
          drawPhasePanel(painter, rect.adjusted(1, 1, -1, -1),
                         pal, enabled ? pal.base() : pal.window(), true);

          if (mouseover) {
              painter->setPen(pal.highlight().color().darker(contrast_));
          } else if (on || (flags & State_NoChange)) {
              painter->setPen(pal.dark().color());
          } else {
              painter->setPen(Qt::NoPen);
          }
          if (on) {
              painter->setBrush(pal.highlight());
          } else if (flags & State_NoChange) {
              painter->setBrush(pal.mid());
          }

          painter->drawRect(rect.adjusted(4, 4, -5, -5));
          break;

      case PE_IndicatorRadioButton:     // on/off indicator for radio button
          painter->setBrush(enabled ? pal.base() : pal.window());

          painter->setPen(pal.dark().color());
          polygon.setPoints(8, x+1,cy+1, x+1,cy,    cx,y+1,    cx+1,y+1,
                               x2-1,cy,  x2-1,cy+1, cx+1,y2-1, cx,y2-1);
          painter->drawConvexPolygon(polygon);

          painter->setPen(pal.mid().color());
          polygon.setPoints(4, x,cy, cx,y, cx+1,y, x2,cy);
          painter->drawPolyline(polygon);
          painter->setPen(pal.midlight().color());
          polygon.setPoints(4, x2,cy+1, cx+1,y2, cx,y2, x,cy+1);
          painter->drawPolyline(polygon);

          if (on) {
              painter->setBrush(pal.highlight());
              painter->setPen(mouseover
                              ? pal.highlight().color().darker(contrast_)
                              : pal.dark().color());
              polygon.setPoints(8, x+4,cy+1, x+4,cy,    cx,y+4,    cx+1,y+4,
                                   x2-4,cy,  x2-4,cy+1, cx+1,y2-4, cx,y2-4);
              painter->drawConvexPolygon(polygon);
          } else if (mouseover) {
              painter->setPen(pal.highlight().color().darker(contrast_));
              polygon.setPoints(9, x+4,cy+1, x+4,cy,    cx,y+4,    cx+1,y+4,
                                   x2-4,cy,  x2-4,cy+1, cx+1,y2-4, cx,y2-4,
                                   x+4,cy+1);
              painter->drawPolyline(polygon);
          }
          break;

      case PE_IndicatorHeaderArrow: {   // sort arrow on view header
          const QStyleOptionHeader *header;
          header = qstyleoption_cast<const QStyleOptionHeader *>(option);
          if (header) {
              if (header->sortIndicator & QStyleOptionHeader::SortUp) {
                  drawPrimitive(PE_IndicatorArrowUp, option, painter, widget);
              } else {
                  drawPrimitive(PE_IndicatorArrowDown, option, painter, widget);
              }
          }
          break;
      }

      case PE_PanelMenuBar:             // panel for menu bars
      case PE_PanelToolBar:             // panel for a toolbar
          // adjust rect so we can use bevel
          drawPhaseBevel(painter, rect.adjusted(-1, -1, 0, 0),
                         pal, pal.window(), false, (w < h),
                         (element==PE_PanelToolBar) ? true : false);
          break;

      case PE_FrameStatusBar:           // frame for a section of a status bar
          painter->setPen(pal.mid().color());
          painter->drawLine(x, y,  x2-1, y);
          painter->drawLine(x, y+1, x, y2-1);
          painter->setPen(pal.midlight().color());
          painter->drawLine(x+1, y2, x2, y2);
          painter->drawLine(x2, y+1, x2, y2-1);
          break;

      case PE_IndicatorDockWidgetResizeHandle: // resize handle for docks
          painter->fillRect(rect,
                            (mouseover) ?
                            pal.window().color().lighter(contrast_) :
                            pal.window());
          drawPhaseDoodads(painter, rect, pal, horiz);
          break;


      case PE_IndicatorMenuCheckMark:   // check mark used in a menu
          if (on) {
              painter->setBrush(pal.highlightedText());
          } else {
              painter->setBrush(sunken ? pal.dark() : pal.text());
          }
          painter->setPen(painter->brush().color());
          painter->drawPixmap(cx-4, cy-4, bitmaps_[CheckMark]);
          break;

      case PE_IndicatorViewItemCheck:   // on/off indicator for a view item
      case PE_Q3CheckListIndicator: {   // Q3 checkbox of a list view item
          painter->setPen(pal.text().color());
          // draw box
          QRect box(0, 0, 13, 13);
          box.moveCenter(rect.center());
          painter->drawRect(box);
          painter->drawRect(box.adjusted(1,1,-1,-1));
          // draw check
          if (flags & State_On) {
              painter->setBrush(pal.text());
              painter->setPen(painter->brush().color());
              painter->drawPixmap(cx-4, cy-4, bitmaps_[CheckMark]);
          }
          break;
      }

      case PE_IndicatorBranch: {        // branch lines of a tree view
          if (pal.color(QPalette::Active, QPalette::Base).value() < 192) {
              painter->setPen(pal.text().color());
              painter->setBrush(pal.text());
          } else {
              painter->setPen(pal.mid().color());
              painter->setBrush(pal.mid());
          }

          int spacer = 0;
          // draw expander
          if (flags & State_Children) {
              QPolygon poly;
              if (flags & State_Open) {
                  poly.setPoints(3, -4,-2, 4,-2, 0,2);
              } else {
                  poly.setPoints(3, -2,-4, 2,0, -2,4);
              }
              poly.translate(cx, cy);
              painter->drawPolygon(poly);

              spacer = 6;
          }

          // draw lines
          if (flags & State_Item) {
              if (option->direction == Qt::RightToLeft)
                  painter->drawLine(x, cy, cx-spacer, cy);
              else
                  painter->drawLine(cx+spacer, cy, x2, cy);
          }
          if (flags & State_Sibling) {
              painter->drawLine(cx, cy+spacer, cx, y2);
          }
          if (flags & (State_Item|State_Sibling)) {
              painter->drawLine(cx, cy-spacer, cx, y);
          }
          break;
      }

      case PE_IndicatorToolBarHandle:   // toolbar handle
          drawPhaseGradient(painter, rect, pal.window().color(),
                            !horiz, rect.size(), true);
          drawPhaseDoodads(painter, rect, pal, !horiz);
          break;

      case PE_Q3DockWindowSeparator:     // Q3 dock/toolbar separator
      case PE_IndicatorToolBarSeparator: // toolbar separator
          // TODO: lines don't go to edge of bar, need to fix
          if (horiz) {
              painter->setPen(pal.mid().color());
              painter->drawLine(cx, 0, cx, y2);
              painter->setPen(pal.midlight().color());
              painter->drawLine(cx+1, 0, cx+1, y2);
          } else {
              painter->setPen(pal.mid().color());
              painter->drawLine(0, cy, x2, cy);
              painter->setPen(pal.midlight().color());
              painter->drawLine(0, cy+1, x2, cy+1);
          }
          break;

      case PE_IndicatorArrowUp:         // generic up arrow
      case PE_IndicatorSpinUp:          // spin up arrow
          painter->setBrush(enabled ? pal.dark() : pal.mid());
          painter->setPen(painter->brush().color());
          if (sunken) painter->translate(pixelMetric(PM_ButtonShiftHorizontal),
                                         pixelMetric(PM_ButtonShiftVertical));
          painter->drawPixmap(cx-2, cy-2, bitmaps_[UArrow]);
          break;

      case PE_IndicatorArrowDown:       // generic down arrow
      case PE_IndicatorSpinDown:        // spin down arrow
          painter->setBrush(enabled ? pal.dark() : pal.mid());
          painter->setPen(painter->brush().color());
          if (sunken) painter->translate(pixelMetric(PM_ButtonShiftHorizontal),
                                         pixelMetric(PM_ButtonShiftVertical));
          painter->drawPixmap(cx-2, cy-2, bitmaps_[DArrow]);
          break;

      case PE_IndicatorArrowLeft:       // generic left arrow
          painter->setBrush(enabled ? pal.dark() : pal.mid());
          painter->setPen(painter->brush().color());
          if (sunken) painter->translate(pixelMetric(PM_ButtonShiftHorizontal),
                                         pixelMetric(PM_ButtonShiftVertical));
          painter->drawPixmap(cx-2, cy-2, bitmaps_[LArrow]);
          break;

      case PE_IndicatorArrowRight:      // generic right arrow
          painter->setBrush(enabled ? pal.dark() : pal.mid());
          painter->setPen(painter->brush().color());
          if (sunken) painter->translate(pixelMetric(PM_ButtonShiftHorizontal),
                                         pixelMetric(PM_ButtonShiftVertical));
          painter->drawPixmap(cx-2, cy-2, bitmaps_[RArrow]);
          break;

      case PE_IndicatorSpinPlus:        // spin plus sign
          painter->setBrush(enabled ? pal.dark() : pal.mid());
          painter->setPen(painter->brush().color());
          if (sunken) painter->translate(pixelMetric(PM_ButtonShiftHorizontal),
                                         pixelMetric(PM_ButtonShiftVertical));
          painter->drawPixmap(cx-2, cy-2, bitmaps_[PlusSign]);
          break;

      case PE_IndicatorSpinMinus:       // spin minus sign
          painter->setBrush(enabled ? pal.dark() : pal.mid());
          painter->setPen(painter->brush().color());
          if (sunken) painter->translate(pixelMetric(PM_ButtonShiftHorizontal),
                                         pixelMetric(PM_ButtonShiftVertical));
          painter->drawPixmap(cx-2, cy-2, bitmaps_[MinusSign]);
          break;

      // not drawing these elements, as default is sufficient
      // case PE_PanelLineEdit:            // panel for a line edit
      // case PE_Q3Separator:              // Q3 generic separator
      // case PE_IndicatorTabTear:         // jaggy torn tab indicator
      // case PE_IndicatorProgressChunk:   // section of progress bar
      // case PE_Q3CheckListController:    // Q3 controller of a list view item
      // case PE_Q3CheckListExclusiveIndicator: // Q3 radio of a list view item
      // case PE_PanelTipLabel:            // panel for a tip

      default:
          QWindowsStyle::drawPrimitive(element, option, painter, widget);
          break;
    }

    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawControl()
// -------------
// Draw a control

void PhaseStyle::drawControl(ControlElement element,
                             const QStyleOption *option,
                             QPainter *painter,
                             const QWidget *widget) const
{
    const QRect &rect = option->rect;
    const State &flags = option->state;
    const QPalette &pal = option->palette;
    bool depress = flags & (State_Sunken | State_On);
    bool enabled = flags & State_Enabled;
    bool horizontal = flags & State_Horizontal;
    bool mouseover = highlights_ && (flags & State_MouseOver) && enabled;

    int x, y, w, h;
    rect.getRect(&x, &y, &w, &h);
    int x2 = rect.right();
    int y2 = rect.bottom();
    //int cx = rect.center().x();
    int cy = rect.center().y();

    switch (element) {
      case CE_PushButtonBevel: { // QPushButton bevel and default indicator
          const QStyleOptionButton *button;
          button = qstyleoption_cast<const QStyleOptionButton *>(option);
          if (!button) break;

          if (!depress &&
              (button->features & QStyleOptionButton::DefaultButton)) {
              // draw default frame
              drawPrimitive(PE_FrameDefaultButton,
                            option, painter, widget);
              // adjust size for bevel
              int dbi = pixelMetric(PM_ButtonDefaultIndicator,
                                    button, widget);
              QStyleOptionButton tempopt = *button;
              tempopt.rect.adjust(dbi, dbi, -dbi, -dbi);
              // draw bevel
              drawPrimitive(PE_PanelButtonBevel,
                            &tempopt, painter, widget);
          } else if ((button->features & QStyleOptionButton::Flat)
                     && !depress) {
              // flat button, don't draw anything
          } else {
              // draw normal button
              drawPrimitive(PE_PanelButtonCommand,
                            button, painter, widget);
          }

          if (button->features & QStyleOptionButton::HasMenu) {
              // get arrow rect
              int mbi = pixelMetric(PM_MenuButtonIndicator,
                                    button, widget);
              QStyleOptionButton tempopt = *button;
              tempopt.rect = QRect(rect.right()- mbi, rect.height() - 20,
                                   mbi, rect.height() - 4);
              // draw arrow
              drawPrimitive(PE_IndicatorArrowDown,
                            &tempopt, painter, widget);
          }
          break;
      }

      case CE_DockWidgetTitle: {        // dock window title
          const QStyleOptionDockWidget *doption;
          doption = qstyleoption_cast<const QStyleOptionDockWidget*>(option);
          if (!doption) break;

          const QStyleOptionDockWidgetV2 *v2;
          v2 = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(option);
          bool vertical = (v2 == 0) ? false : v2->verticalTitleBar;

          bool floating = false;

          if (doption->movable) {
              const QDockWidget *dwidget;
              dwidget = qobject_cast<const QDockWidget*>(widget);
              floating = (dwidget && dwidget->isFloating());
              if (floating) {
                  drawPhaseGradient(painter, rect,
                                    pal.highlight().color(),
                                    vertical, rect.size(), false);
              } else {
                  painter->setPen(pal.midlight().color());
                  painter->drawLine(x, y, x2-1, y);
                  painter->drawLine(x, y, x, y2-1);
                  painter->setPen(pal.mid().color());
                  painter->drawLine(x+1, y2, x2, y2);
                  painter->drawLine(x2, y+1, x2, y2);
                  drawPhaseGradient(painter, rect.adjusted(1, 1, -1, -1),
                                    pal.window().color(),
                                    vertical, QSize(w-2, h-2), false);
              }
          }

          if (!doption->title.isEmpty()) {
              painter->save();

              QRect trect = subElementRect(SE_DockWidgetTitleBarText,
                                           option, widget);

              if (vertical) {
                  painter->translate(trect.center().x(), trect.center().y());
                  painter->rotate(-90);
                  painter->translate(-trect.center().x(), -trect.center().y());
                  trect = painter->worldMatrix().inverted().mapRect(trect);
              }

              if (floating) {
                  QFont font = painter->font();
                  font.setBold(true);
                  painter->setFont(font);
              }

              drawItemText(painter,
                           trect.adjusted(4, 0, -4, -1),
                           Qt::AlignCenter | Qt::TextShowMnemonic,
                           pal, enabled, doption->title,
                           floating ? QPalette::HighlightedText : QPalette::WindowText);

              painter->restore();
          }
          break;
      }

      case CE_Splitter:              // splitter handle
          painter->fillRect(rect, mouseover ?
                            pal.window().color().lighter(contrast_) :
                            pal.window());
          drawPhaseDoodads(painter, rect, pal, !horizontal);
          break;

      case CE_TabBarTabShape: {      // tab shape within a tabbar
          const QStyleOptionTab *tab;
          tab = qstyleoption_cast<const QStyleOptionTab*>(option);
          if (!tab) break;

          // use default for triangular tabs
          if (tab->shape != QTabBar::RoundedNorth &&
              tab->shape != QTabBar::RoundedWest &&
              tab->shape != QTabBar::RoundedSouth &&
              tab->shape != QTabBar::RoundedEast) {
              QWindowsStyle::drawControl(element, option, painter, widget);
              break;
          }
          // this guy can get complicated, we we do it elsewhere
          drawPhaseTab(painter, pal, tab);
          break;
      }

      case CE_ProgressBarGroove:     // groove of progress bar
          drawPhasePanel(painter, rect, pal, pal.base(), true);
          break;

      case CE_ProgressBarContents: { // indicator of progress bar
          const QStyleOptionProgressBar *pbar;
          pbar = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
          if (!pbar) break;

          bool vertical = false;
          bool inverted = false;

          // Get v2 options
          const QStyleOptionProgressBarV2 *pb2;
          pb2 = qstyleoption_cast<const QStyleOptionProgressBarV2*>(option);
          if (pb2) {
              vertical = (pb2->orientation == Qt::Vertical);
              inverted = pb2->invertedAppearance;
          }

          if (vertical) {
              QMatrix matrix;
              qSwap(h, w); // flip width and height
              matrix.translate(h+5, 0.0);
              matrix.rotate(90.0);
              painter->setMatrix(matrix);
          }

          bool reverse = (vertical ||
                          (!vertical && (pbar->direction==Qt::RightToLeft)));
          if (inverted) reverse = !reverse;

          painter->save();

          painter->setBrush(pal.highlight());
          painter->setPen(pal.dark().color());

          if (pbar->minimum == 0 && pbar->maximum == 0) {
              // busy indicator
              int bar = pixelMetric(PM_ProgressBarChunkWidth, pbar, widget);
              int progress = pbar->progress % ((w-bar) * 2);
              if (progress > (w-bar)) progress = 2 * (w-bar) - progress;
              painter->drawRect(x+progress, y, bar-1, h-1);
          } else {
              double progress = (double)pbar->progress / (double)pbar->maximum;
              int dx = (int)(w * progress);
              if (dx > 2) {
                  if (reverse) x += w - dx;
                  painter->drawRect(x, y, dx-1, h-1);
              }
          }
          painter->restore();
          break;
      }

      case CE_ProgressBarLabel: {    // label of progress bar
          const QStyleOptionProgressBar *pbar;
          pbar = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
          if (!pbar) break;
          if (pbar->minimum == 0 && pbar->maximum == 0) break;

          painter->save();

          bool vert = false;
          bool invert = false;
          bool btt = false; // bottom to top text orientation
          const QStyleOptionProgressBarV2 *pbar2;
          pbar2 = qstyleoption_cast<const QStyleOptionProgressBarV2*>(option);
          if (pbar2) {
              vert = (pbar2->orientation == Qt::Vertical);
              invert = pbar2->invertedAppearance;
              btt = pbar2->bottomToTop;
          }

          if (vert) {
              QMatrix matrix;
              qSwap(w, h); // flip width and height
              if (btt) {
                  matrix.translate(0.0, w);
                  matrix.rotate(-90.0);
              } else {
                  matrix.translate(h, 0.0);
                  matrix.rotate(90.0);
              }
              painter->setMatrix(matrix);
          }

          QRect left;
          int ipos = int(((pbar->progress - pbar->minimum)
                          / double(pbar->maximum - pbar->minimum)) * w);
          bool rtl = (pbar->direction == Qt::RightToLeft);
          bool flip = ((!vert && ((rtl && !invert) || (!rtl && invert)))
                       || (vert && ((!invert && !btt) || (invert && btt))));
          if (flip) {
              ipos = w - ipos;
              if (ipos >= 0 && ipos <= w) {
                  left = QRect(x, y, ipos, h);
              }
          } else {
              if (ipos >= 0 && ipos <= w) {
                  left = QRect(x, y, ipos, h);
              }
          }

          // QFont font = painter->font();
          // font.setBold(true);
          // painter->setFont(font);

          painter->setPen(flip
                          ? pbar->palette.base().color()
                          : pbar->palette.text().color());
          painter->drawText(x, y, w, h, Qt::AlignCenter, pbar->text);
          if (!left.isNull()) {
              painter->setPen(flip
                              ? pbar->palette.text().color()
                              : pbar->palette.base().color());
              painter->setClipRect(left, Qt::IntersectClip);
              painter->drawText(x, y, w, h, Qt::AlignCenter, pbar->text);
          }
          painter->restore();
          break;
      }

      case CE_MenuBarItem: {            // menu item in a QMenuBar
          const QStyleOptionMenuItem *mbi;
          mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
          if (!mbi) break;

          if ((flags & State_Selected) && (flags & State_HasFocus)) {
              if (flags & State_Sunken) {
                  drawPhasePanel(painter, rect, pal,  pal.window(), true);
              } else {
                  drawPhaseBevel(painter, rect, pal, pal.window(),
                                 false, false, false);
              }
          } else {
              drawPhaseGradient(painter, rect, pal.window().color(), false,
                                rect.size(), false);
          }
          QCommonStyle::drawControl(element, mbi, painter, widget);
          break;
      }

      case CE_MenuBarEmptyArea:         // empty area of a QMenuBar
          drawPhaseGradient(painter, rect, pal.window().color(), false,
                            rect.size(), false);
          break;

      case CE_MenuItem: {            // menu item in a QMenu
          const QStyleOptionMenuItem *mi;
          mi = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
          if (!mi) break;

          int checkwidth = qMax(mi->maxIconWidth, 20);
          bool active = (flags & State_Selected);
          int checked = (mi->checkType != QStyleOptionMenuItem::NotCheckable)
              ? mi->checked : false;
          QRect vrect;

          // draw background
          if (active && enabled) {
              painter->fillRect(rect, pal.highlight());
          } else {
              painter->fillRect(rect, pal.window());
          }

          // draw separator
          if (mi->menuItemType == QStyleOptionMenuItem::Separator) {
              painter->setPen(pal.mid().color());
              painter->drawLine(x+checkwidth, cy-1, x2-checkwidth-1, cy-1);
              painter->setPen(pal.dark().color());
              painter->drawLine(x+checkwidth+1, cy, x2-checkwidth-1, cy);
              painter->drawPoint(x+checkwidth, cy);
              painter->setPen(pal.midlight().color());
              painter->drawLine(x+checkwidth+1, cy+1, x2-checkwidth, cy+1);
              painter->drawPoint(x2-checkwidth, cy+1);
              break;
          }

          // set pen for text and icons
          if (enabled) {
              painter->setPen(active ? pal.highlightedText().color() :
                              pal.windowText().color());
          } else {
              painter->setPen(pal.mid().color());
          }

          // draw icon
          if (!mi->icon.isNull() && !checked) {
              QIcon::Mode mode;
              if (active)
                  mode = enabled ? QIcon::Active : QIcon::Disabled;
              else
                  mode = enabled ? QIcon::Normal : QIcon::Disabled;

              QPixmap pixmap = mi->icon.pixmap(pixelMetric(PM_SmallIconSize),
                                               mode);
              vrect = visualRect(mi->direction, rect,
                                 QRect(x, y, checkwidth, h));
              QRect pmrect(0, 0, pixmap.width(), pixmap.height());
              pmrect.moveCenter(vrect.center());
              painter->drawPixmap(pmrect.topLeft(), pixmap);
          }

          // draw check
          if (checked) {
              QStyleOptionMenuItem newmi = *mi;
              newmi.state = State_None;
              if (enabled) newmi.state |= State_Enabled;
              if (active) newmi.state |= State_On;
              newmi.rect = visualRect(mi->direction, rect,
                                      QRect(x, y, checkwidth, h));
              drawPrimitive(PE_IndicatorMenuCheckMark, &newmi, painter, widget);
          }

          // draw text
          int xm = ITEMFRAME + checkwidth + ITEMHMARGIN;
          int xp = x + xm;
          int tw = w - xm - mi->tabWidth - ARROWMARGIN - ITEMHMARGIN * 3
              - ITEMFRAME + 1;
          QString text = mi->text;
          QRect trect(xp, y+ITEMVMARGIN, tw, h - 2 * ITEMVMARGIN);
          vrect = visualRect(option->direction, rect, trect);

          if (!text.isEmpty()) { // draw label
              painter->save();

              if (mi->menuItemType == QStyleOptionMenuItem::DefaultItem) {
                  QFont font = mi->font;
                  font.setBold(true);
                  painter->setFont(font);
              }

              int t = text.indexOf(QLatin1Char( '\t' ));
              int tflags = Qt::AlignVCenter | Qt::AlignLeft |
                           Qt::TextDontClip | Qt::TextSingleLine;
              if (styleHint(SH_UnderlineShortcut, mi, widget))
                  tflags |= Qt::TextShowMnemonic;
              else
                  tflags |= Qt::TextHideMnemonic;

              if (t >= 0) { // draw right label (accelerator)
                  QRect tabrect = visualRect(option->direction, rect,
                                  QRect(trect.topRight(),
                                        QPoint(rect.right(), trect.bottom())));
                  painter->drawText(tabrect, tflags, text.mid(t+1));
                  text = text.left(t);
              }

              // draw left label
              painter->drawText(vrect, tflags, text.left(t));
              painter->restore();
          }

          // draw submenu arrow
          if (mi->menuItemType == QStyleOptionMenuItem::SubMenu) {
              PrimitiveElement arrow = (option->direction == Qt::RightToLeft)
                  ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
              int dim = (h-2*ITEMFRAME) / 2;
              vrect = visualRect(option->direction, rect,
                                 QRect(x + w - ARROWMARGIN - ITEMFRAME - dim,
                                       y + h / 2 - dim / 2, dim, dim));

              QStyleOptionMenuItem newmi = *mi;
              newmi.rect = vrect;
              newmi.state = enabled ? State_Enabled : State_None;
              if (active) {
                    newmi.palette.setColor(QPalette::Dark,
                        newmi.palette.highlightedText().color());
              }
              drawPrimitive(arrow, &newmi, painter, widget);
          }
          break;
      }

      case CE_MenuTearoff:           // tearoff area in menu
          if (flags & State_Selected)
              painter->fillRect(rect, pal.brush(QPalette::Highlight));
          else
              painter->fillRect(rect, pal.brush(QPalette::Window));
          painter->setPen(QPen(pal.mid().color(), 1, Qt::DotLine));
          painter->drawLine(x+6, cy-1, x2-6, cy-1);
          painter->setPen(QPen(pal.dark().color(), 1, Qt::DotLine));
          painter->drawLine(x+6, cy, x2-6, cy);
          painter->setPen(QPen(pal.midlight().color(), 1, Qt::DotLine));
          painter->drawLine(x+6, cy+1, x2-6, cy+1);
          break;

      case CE_ToolBoxTab: {          // tab area of toolbox
          // TODO: account for reverse layout
          // TODO: Qt broken - palette isn't constant from tab to tab
	  //           should be fixed using the widget palette

	  // some const
	  const bool enabled(flags & State_Enabled);
	  const bool selected(flags & State_Selected);
	  const bool mouseover(enabled && !selected && (flags & State_MouseOver));

          const QStyleOptionToolBox *box;
          box = qstyleoption_cast<const QStyleOptionToolBox*>(option);
          if (!box) break;
	  // try the v2
	  const QStyleOptionToolBox *boxv2;
	  boxv2 = qstyleoption_cast<const QStyleOptionToolBoxV2*>(option);

	  // dont trust option palette, as oxygen does
	  QColor windowc( widget->palette().color(QPalette::Window) );
	  QColor windowcdark( windowc.darker(contrast_) );
	  QColor windowclight( windowc.lighter(contrast_) );

          const int rx = x2 - 20;
          const int cx = rx - h + 1;

          QPolygon polygon;
          polygon.setPoints(6,
                            x-1,y, cx,y, rx-2,y2-2, x2+1,y2-2,
                            x2+1,y2+1, x-1,y2+1);

          painter->save();

          if (selected) {
              painter->setPen(pal.dark().color());
              painter->setBrush(windowcdark);
              painter->drawConvexPolygon(polygon);
          } else {
              painter->setClipRegion(polygon);
              drawPhaseGradient(painter, rect,
				mouseover ? windowclight : windowc,
				false, QSize(w, h), false);
              painter->setClipping(false);
              painter->drawPolyline(polygon);
          }

          polygon.setPoints(4, x,y+1, cx,y+1, rx-2,y2-1, x2,y2-1);
          painter->setPen(pal.midlight().color());
          painter->drawPolyline(polygon);

	  // set pen for text and icons
	  QRect vrect;
          if (enabled) {
              painter->setPen(selected ? pal.highlightedText().color() :
                              pal.windowText().color());
          } else {
              painter->setPen(pal.mid().color());
          }
          // draw icon
          if (!box->icon.isNull()) {
              QIcon::Mode mode;
              if (selected)
                  mode = enabled ? QIcon::Active : QIcon::Disabled;
              else
                  mode = enabled ? QIcon::Normal : QIcon::Disabled;

              QPixmap pixmap = box->icon.pixmap(pixelMetric(PM_SmallIconSize),
                                               mode);
              vrect = visualRect(option->direction, rect,
                                 QRect(x, y, h, h)); //asume square icons
              QRect pmrect(0, 0, pixmap.width(), pixmap.height());
              pmrect.moveCenter(vrect.center());
              painter->drawPixmap(pmrect.topLeft(), pixmap);
	  }

          QRect trect(x+h+4, y+1, rx-2, y2-1);
          vrect = visualRect(option->direction, rect, trect);
          QString text = box->text;
          int tflags = Qt::AlignVCenter | Qt::AlignLeft |
                       Qt::TextDontClip | Qt::TextSingleLine;
          if (styleHint(SH_UnderlineShortcut, box, widget))
              tflags |= Qt::TextShowMnemonic;
          else
              tflags |= Qt::TextHideMnemonic;
          painter->drawText(vrect, tflags, text);
          painter->restore();
          break;
      }

      case CE_SizeGrip: {            // window resize handle
          int sw = qMin(h, w) - 1;
          y = y2 - sw;

          QPen pen = painter->pen();
          if (option->direction == Qt::RightToLeft) {
              x2 = x + sw;
              for (int n = 0; n < 4; ++n) {
                  painter->setPen(pal.mid().color());
                  painter->drawLine(x, y, x2, y2);
                  painter->setPen(pal.midlight().color());
                  painter->drawLine(x, y+1, x2-1, y2);
                  y += 3;
                  x2 -= 3;
              }
          } else {
              x = x2 - sw;
              for (int n = 0; n < 4; ++n) {
                  painter->setPen(pal.mid().color());
                  painter->drawLine(x, y2, x2, y);
                  painter->setPen(pal.midlight().color());
                  painter->drawLine(x+1, y2, x2, y+1);
                  x += 3;
                  y += 3;
              }
          }
          painter->setPen(pen);
          break;
      }

      case CE_HeaderSection: {          // header bevel
          const QStyleOptionHeader *header;
          header = qstyleoption_cast<const QStyleOptionHeader *>(option);
          if (!header) break;

          horizontal = (header->orientation == Qt::Horizontal);
          // adjust rect so headers overlap by one pixel
          QRect arect = rect.adjusted(-1, -1, 0, 0);
          if (depress) {
              painter->save();
              painter->setPen(pal.dark().color());
              painter->setBrush(pal.mid());
              painter->drawRect(arect.adjusted(0, 0, -1, -1));
              painter->restore();
          }
          else {
              drawPhaseBevel(painter, arect, pal,
                             pal.window(), false, !horizontal, true);
          }
          break;
      }

      case CE_ScrollBarAddLine: {     // scrollbar scroll down
          PrimitiveElement arrow = (horizontal)
              ? PE_IndicatorArrowRight : PE_IndicatorArrowDown;

          drawPhaseBevel(painter, rect, pal, pal.button(),
                         depress, !horizontal, true);
          drawPrimitive(arrow, option, painter, widget);
          break;
      }
      case CE_ScrollBarSubLine: {    // scrollbar scroll up
          const QStyleOptionSlider *sb;
          sb = qstyleoption_cast<const QStyleOptionSlider *>(option);
          if (!sb) break;

          int extent = pixelMetric(PM_ScrollBarExtent, sb, widget);

          QRect button1, button2;
          PrimitiveElement arrow;

          if (horizontal) {
              button1.setRect(x, y, extent, extent);
              button2.setRect(x2 - extent + 1, y, extent, extent);
              arrow = PE_IndicatorArrowLeft;
          } else {
              button1.setRect(x, y, extent, extent);
              button2.setRect(x, y2 - extent + 1, extent, extent);
              arrow = PE_IndicatorArrowUp;
          }

          // draw buttons
          drawPhaseBevel(painter, button1, pal, pal.button(),
                         depress, !horizontal, true);
          drawPhaseBevel(painter, button2, pal, pal.button(),
                         depress, !horizontal, true);

          QStyleOptionSlider newoption = *sb;
          newoption.rect = button1;
          drawPrimitive(arrow, &newoption, painter, widget);
          newoption.rect = button2;
          drawPrimitive(arrow, &newoption, painter, widget);

          break;
      }

      case CE_ScrollBarAddPage:      // scrollbar page down
      case CE_ScrollBarSubPage:      // scrollbar page up
          if (h) { // has a height, thus visible
              QPen pen = painter->pen();
              painter->fillRect(rect, pal.mid());
              painter->setPen(pal.dark().color());
              if (horizontal) { // vertical
                  painter->drawLine(x, y, x2, y);
                  painter->drawLine(x, y2, x2, y2);
              } else { // horizontal
                  painter->drawLine(x, y, x, y2);
                  painter->drawLine(x2, y, x2, y2);
              }
              painter->setPen(pen);
          }
          break;

      case CE_ScrollBarSlider:       // scrollbar slider/thumb
          drawPhaseBevel(painter, rect, pal, mouseover ?
                         pal.button().color().lighter(contrast_) : pal.button(),
                         false, !horizontal, true);
          drawPhaseDoodads(painter, rect, pal, horizontal);
          break;

      case CE_RubberBand: {          // rubberband (such as for iconview)
          const QStyleOptionRubberBand *rb;
          rb = qstyleoption_cast<const QStyleOptionRubberBand*>(option);
          if (rb) {
              painter->save();
              QColor color = pal.highlight().color();
              if (!rb->opaque) color.setAlpha(127);
              painter->setPen(color);
              if (!rb->opaque) color.setAlpha(31);
              painter->setBrush(color);
              QStyleHintReturnMask mask;
              painter->drawRect(rect.adjusted(0, 0, -1, -1));
              painter->restore();
          }
          break;
      }

      case CE_ToolBar: {             // QToolBar
          drawPrimitive(PE_PanelToolBar, option, painter, widget);
          QRect grect = rect.adjusted(2, 2, -2, -2);
          drawPhaseGradient(painter, grect, pal.window().color(),
                            !horizontal, grect.size(), true);
          break;
      }

      // not drawing these controls, as default is sufficient
      // case CE_Q3DockWindowEmptyArea: // empty area of dock widget
      // case CE_PushButtonLabel:
      // case CE_FocusFrame:
      // case CE_CheckBox:
      // case CE_CheckBoxLabel:
      // case CE_ComboBoxLabel:
      // case CE_Header:
      // case CE_HeaderLabel:
      // case CE_MenuScroller:
      // case CE_RadioButton:
      // case CE_RadioButtonLabel:
      // case CE_ScrollBarFirst:
      // case CE_ScrollBarLast:
      // case CE_TabBarTab:
      // case CE_TabBarTabLabel:
      // case CE_ToolButtonLabel:

      default:
          QWindowsStyle::drawControl(element, option, painter, widget);
          break;
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawComplexControl()
// --------------------
// Draw complex control

void PhaseStyle::drawComplexControl(ComplexControl control,
                                    const QStyleOptionComplex *option,
                                    QPainter *painter,
                                    const QWidget *widget) const
{
    const QRect &rect = option->rect;
    const State &flags = option->state;
    const QPalette &pal = option->palette;
    bool enabled = flags & State_Enabled;
    bool sunken = flags & State_Sunken;
    bool mouseover = highlights_ && (flags & State_MouseOver) && enabled;
    int x, y, w, h;
    rect.getRect(&x, &y, &w, &h);
    QRect subrect;

    switch (control) {
      case CC_SpinBox: {                // QSpinBox
          const QStyleOptionSpinBox *box;
          box = qstyleoption_cast<const QStyleOptionSpinBox*>(option);
          if (!box) break;

          QStyleOptionSpinBox copybox = *box;
          PrimitiveElement element;

          // draw frame
          if (box->frame && (box->subControls & SC_SpinBoxFrame)) {
              subrect = subControlRect(CC_SpinBox, box,
                                       SC_SpinBoxFrame, widget);
              drawPhasePanel(painter, subrect, pal, Qt::NoBrush, true);
          }

          // draw button field
          subrect = subControlRect(CC_SpinBox, box,
                                   SC_SpinBoxFrame, widget);
          subrect.adjust(1, 1, -1, -1);
          int left = subControlRect(CC_SpinBox, box,
                                    SC_SpinBoxUp, widget).left();
          subrect.setLeft(left);
          drawPhaseBevel(painter, subrect, pal, mouseover ?
                         pal.button().color().lighter(contrast_) :
                         pal.button(),
                         false, false, false);

          // draw up arrow
          if (box->subControls & SC_SpinBoxUp) {
              subrect = subControlRect(CC_SpinBox, box, SC_SpinBoxUp, widget);
              copybox.subControls = SC_SpinBoxUp;
              copybox.rect = subControlRect(CC_SpinBox, box,
                                            SC_SpinBoxUp, widget);

              if (box->buttonSymbols == QAbstractSpinBox::PlusMinus)
                  element = PE_IndicatorSpinPlus;
              else
                  element = PE_IndicatorSpinUp;

              if (box->activeSubControls == SC_SpinBoxUp && sunken) {
                  copybox.state |= State_On;
                  copybox.state |= State_Sunken;
              } else {
                  copybox.state |= State_Raised;
                  copybox.state &= ~State_Sunken;
              }
              drawPrimitive(element, &copybox, painter, widget);
          }

          // draw down arrow
          if (box->subControls & SC_SpinBoxDown) {
              subrect = subControlRect(CC_SpinBox, box, SC_SpinBoxDown, widget);
              copybox.subControls = SC_SpinBoxDown;
              copybox.rect = subControlRect(CC_SpinBox, box,
                                            SC_SpinBoxDown, widget);

              if (box->buttonSymbols == QAbstractSpinBox::PlusMinus)
                  element = PE_IndicatorSpinMinus;
              else
                  element = PE_IndicatorSpinDown;

              if (box->activeSubControls == SC_SpinBoxDown && sunken) {
                  copybox.state |= State_On;
                  copybox.state |= State_Sunken;
              } else {
                  copybox.state |= State_Raised;
                  copybox.state &= ~State_Sunken;
              }
              drawPrimitive(element, &copybox, painter, widget);
          }
          break;
      }

      case CC_ComboBox: {               // QComboBox
          const QStyleOptionComboBox *combo;
          combo = qstyleoption_cast<const QStyleOptionComboBox*>(option);
          if (!combo) break;

          subrect = subControlRect(CC_ComboBox, combo,
                                   SC_ComboBoxArrow, widget);

          if (combo->editable) {
              // draw frame
              drawPhasePanel(painter, rect, pal, Qt::NoBrush, true);
              // draw arrow box
              drawPhaseBevel(painter, subrect.adjusted(-1, -1, 1, 1),
                             pal, mouseover
                             ? pal.button().color().lighter(contrast_)
                             : pal.button().color(), sunken, false);
          } else {
              // draw bevel
              drawPhaseButton(painter, rect, pal, mouseover
                              ? pal.button().color().lighter(contrast_)
                              : pal.button().color(), sunken);
          }

          if (combo->subControls & SC_ComboBoxArrow) {
              // draw slot
              int slot = qMax(h/4, 6);
              slot = qMin(slot, 12) + (h%2);
              subrect.adjust(3, 0, -3, 0);
              subrect.setTop(subrect.top() + subrect.height()/2 - slot/2);
              subrect.setHeight(slot);
              drawPhasePanel(painter, subrect,
                             pal, sunken ? pal.midlight() : pal.mid(),
                             true);
          }

          if ((flags & State_HasFocus) && !combo->editable) {
              QStyleOptionFocusRect focus;
              focus.QStyleOption::operator=(*combo);
              subrect = subElementRect(SE_ComboBoxFocusRect,
                                          combo, widget);
              focus.rect = visualRect(combo->direction, rect, subrect);
              drawPrimitive(PE_FrameFocusRect, &focus, painter, widget);
          }
          break;
      }

      case CC_Slider: {                 // QSlider
          const QStyleOptionSlider *slider;
          slider = qstyleoption_cast<const QStyleOptionSlider*>(option);
          if (!slider) break;

          if (slider->subControls & SC_SliderGroove) {
              subrect = subControlRect(CC_Slider, slider,
                                       SC_SliderGroove, widget);
              if (subrect.isValid()) {
                  if (slider->orientation == Qt::Horizontal) {
                      subrect.setTop(subrect.top()+subrect.height()/2-3);
                      subrect.setHeight(7);
                  } else {
                      subrect.setLeft(subrect.left()+subrect.width()/2-3);
                      subrect.setWidth(7);
                  }
                  drawPhasePanel(painter, subrect, pal, pal.mid(), true);
              }
          }

          if (slider->subControls & SC_SliderHandle) {
              subrect = subControlRect(CC_Slider, slider,
                                       SC_SliderHandle, widget);
              QColor color = mouseover
                  ? pal.button().color().lighter(contrast_)
                  : pal.button().color();

              if (slider->orientation == Qt::Horizontal) {
                  subrect.setWidth(6);
                  drawPhaseBevel(painter, subrect, pal, color,
                                 false, false, false);
                  subrect.moveLeft(subrect.left()+5);
                  drawPhaseBevel(painter, subrect, pal, color,
                                 false, false, false);
              } else {
                  subrect.setHeight(6);
                  drawPhaseBevel(painter, subrect, pal, color,
                                 false, true, false);
                  subrect.moveTop(subrect.top()+5);
                  drawPhaseBevel(painter, subrect, pal, color,
                                 false, true, false);
              }
          }

          if (slider->subControls & SC_SliderTickmarks) {
              bool ticksabove = slider->tickPosition & QSlider::TicksAbove;
              bool ticksbelow = slider->tickPosition & QSlider::TicksBelow;
              bool horizontal = (slider->orientation == Qt::Horizontal);

              int spaceavail = pixelMetric(PM_SliderSpaceAvailable,
                                           slider, widget);
              int interval = slider->tickInterval;
              if (interval==0) {
                  interval = slider->singleStep;
                  if (QStyle::sliderPositionFromValue(slider->minimum,
                                                      slider->maximum,
                                                      interval, spaceavail)
                      - QStyle::sliderPositionFromValue(slider->minimum,
                                                        slider->maximum,
                                                        0, spaceavail) < 3)
                      interval = slider->pageStep;
              }
              if (interval < 2) interval = 2;

              QRect handle = subControlRect(CC_Slider, slider,
                                            SC_SliderHandle, widget);
              int pos, offset, span, ticksize;
              if (horizontal) {
                  offset = handle.width() / 2;
                  span = w - handle.width();
                  ticksize = (h - handle.height()) / 2 - 1;
              } else {
                  offset = handle.height() / 2;
                  span = h - handle.height();
                  ticksize = (w - handle.width()) / 2 - 1;
              }

              QPen oldpen = painter->pen();
              painter->setPen(pal.dark().color());

              for (int n=slider->minimum; n<=slider->maximum; n+=interval) {
                  pos = sliderPositionFromValue(slider->minimum,
                                                slider->maximum,
                                                n,  span,
                                                slider->upsideDown);
                  pos += offset;

                  if (horizontal) {
                      if (ticksabove) {
                          painter->drawLine(pos, y, pos, y + ticksize);
                      }
                      if (ticksbelow) {
                          painter->drawLine(pos, rect.bottom(),
                                            pos, rect.bottom() - ticksize);
                      }
                  } else {
                      if (ticksabove) {
                          painter->drawLine(x,  pos, x + ticksize, pos);
                      }
                      if (ticksbelow) {
                          painter->drawLine(rect.right(), pos,
                                            rect.right() - ticksize, pos);
                      }
                  }

                  painter->setPen(oldpen);
              }
          }
          break;
      }

      case CC_Dial: {                   // QDial
          const QStyleOptionSlider *dial;
          dial = qstyleoption_cast<const QStyleOptionSlider*>(option);
          if (!dial) break;

          // avoid aliasing
          QPainter::RenderHints oldhints = painter->renderHints();
          painter->setRenderHint(QPainter::Antialiasing, true);

          qreal cx = rect.center().x();
          qreal cy = rect.center().y();
          qreal radius = (qMin(w, h) / 2.0) - 2.0;
          qreal tick = qMax(radius / 6, static_cast<qreal>(4.0));

          if (dial->subControls & SC_DialGroove) {
              QRectF groove = QRectF(cx-radius+tick, cy-radius+tick,
                                     radius*2-tick*2, radius*2-tick*2);

              // Note: State_MouseOver doesn't work well with QDial
              QLinearGradient gradient(0, groove.top(), 0, groove.bottom());
              gradient.setColorAt(0, pal.button().color().darker(contrast_));
              gradient.setColorAt(1, pal.button().color().lighter(contrast_));

              painter->setPen(QPen(pal.dark().color(), 1.5));
              painter->setBrush(gradient);
              painter->drawEllipse(groove);
              painter->setBrush(Qt::NoBrush);

              groove.adjust(1.0, 1.0, -1.0, -1.0);
              painter->setPen(pal.midlight().color());
              painter->drawArc(groove, 60*16, 150*16);
              painter->setPen(pal.button().color());
              painter->drawArc(groove, 30*16, 30*16);
              painter->drawArc(groove, 210*16, 30*16);
              painter->setPen(pal.mid().color());
              painter->drawArc(groove, 240*16, 150*16);
          }

          if (dial->subControls & SC_DialHandle) {
              painter->save();
              qreal angle;
              qreal percent = (double)(dial->sliderValue - dial->minimum)
                            / (double)(dial->maximum - dial->minimum);

              if (dial->maximum == dial->minimum) {
                  angle = 0.0;
              } else if (dial->dialWrapping) {
                  angle = percent * 360.0;
              } else {
                  angle = percent * 315.0 + 22.5;
              }

              painter->translate(cx, cy);
              painter->rotate(angle);

              bool ul = (angle > 135.0 && angle < 315.0);
              painter->setPen(ul ? pal.midlight().color() : pal.mid().color());
              painter->drawLine(QLineF(-1, radius-tick-1, -1, radius-tick*4));
              painter->setPen(pal.dark().color());
              painter->drawLine(QLineF(0, radius-tick, 0, radius-tick*4));
              painter->setPen(ul ? pal.mid().color() : pal.midlight().color());
              painter->drawLine(QLineF(1, radius-tick-1, 1, radius-tick*4));

              painter->restore();
          }

          if (dial->subControls & QStyle::SC_DialTickmarks) {
              painter->save();
              painter->setPen(pal.dark().color());

              int ti = dial->tickInterval;
              int notches = (dial->maximum - dial->minimum + ti - 1) / ti;

              if (notches > 0) {
                  qreal start, increment;
                  if (dial->dialWrapping) {
                      start = 0.0;
                      increment = 360.0 / notches;
                  } else {
                      start = 22.5;
                      increment = 315.0 / notches;
                  }

                  painter->translate(cx, cy);
                  painter->rotate(start);
                  for (int n=0; n<=notches; ++n) {
                      painter->drawLine(QLineF(0.0, radius,
                                               0.0, radius - tick / 2.0));;
                      painter->rotate(increment);
                  }
              }
              painter->restore();
          }

          painter->setRenderHints(oldhints);
          break;
      }

      case CC_TitleBar:  {               // QWorkspace titlebar
          // TODO: sync this look up with dock window titles ?
          const QStyleOptionTitleBar *title;
          title = qstyleoption_cast<const QStyleOptionTitleBar*>(option);
          if (!title) break;

          bool shaded = (title->titleBarState & Qt::WindowMinimized);

          int x2 = rect.right();
          int y2 = rect.bottom();

          // draw titlebar frame
          drawPhaseGradient(painter, rect.adjusted(0,0,0,-3),
                            pal.highlight().color(),
                            false, rect.size(), false);

          painter->setPen(pal.shadow().color());
          painter->drawLine(x, y, x, y2);
          painter->drawLine(x, y, x2, y);
          painter->drawLine(x2, y, x2, y2);
          painter->setPen(pal.midlight().color());
          painter->drawLine(x+1, y+1, x2-2, y+1);
          painter->drawLine(x+1, y+2, x+1, y2);
          painter->setPen(pal.mid().color());
          painter->drawLine(x2-1, y+2, x2-1, y2);
          painter->setPen(pal.window().color());
          painter->drawLine(x+2, y+2, x2-2, y+2);
          painter->drawLine(x+2, y+2, x+2, y2);
          painter->drawLine(x2-2, y+2, x2-2, y2);
          painter->drawPoint(x2-1, y+1);
          if (shaded) {
              painter->setPen(pal.shadow().color());
              painter->drawLine(x, y2, x2, y2);
              painter->setPen(pal.mid().color());
              painter->drawLine(x+2, y2-1, x2-1, y2-1);
              painter->setPen(pal.window().color());
              painter->drawLine(x+2, y2-2, x2-2, y2-2);
              painter->drawPoint(x+1, y2-1);
          }

          // draw label
          if (title->subControls & SC_TitleBarLabel) {
              subrect = subControlRect(CC_TitleBar, title,
                                       SC_TitleBarLabel, widget);

              QFont font = painter->font();
              font.setBold(true);
              painter->setFont(font);

              drawItemText(painter, subrect,
                           Qt::AlignCenter |
                           Qt::TextShowMnemonic |
                           Qt::TextSingleLine,
                           pal, enabled, title->text,
                           QPalette::HighlightedText);
          }

          // draw buttons
          QPixmap pix;
          bool down;
          QStyleOption tool(*title);

          painter->setPen(pal.windowText().color());

          // menu/icon button
          if ((title->subControls & SC_TitleBarSysMenu) &&
              (title->titleBarFlags & Qt::WindowSystemMenuHint)) {
              subrect = subControlRect(CC_TitleBar, title,
                                       SC_TitleBarSysMenu, widget);
                if (!title->icon.isNull()) {
                    title->icon.paint(painter, subrect);
                } else {
                    down = ((title->activeSubControls & SC_TitleBarCloseButton)
                            && (flags & State_Sunken));
                    pix = standardPixmap(SP_TitleBarMenuButton, &tool, widget);
                    tool.rect = subrect;
                    tool.state = down ? State_Sunken : State_Raised;
                    drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);

                    if (down) painter->translate(1, 1);
                    drawItemPixmap(painter, subrect, Qt::AlignCenter, pix);
                    if (down) painter->translate(-1, -1);
                }
          }

          if (title->subControls & SC_TitleBarCloseButton) {
              subrect = subControlRect(CC_TitleBar, title,
                                       SC_TitleBarCloseButton, widget);
              if (!subrect.isNull()) {
                  down = ((title->activeSubControls & SC_TitleBarCloseButton)
                          && (flags & State_Sunken));
                  pix = standardPixmap(SP_TitleBarCloseButton, &tool, widget);
                  tool.rect = subrect;
                  tool.state = down ? State_Sunken : State_Raised;
                  drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);

                  if (down) painter->translate(1, 1);
                  drawItemPixmap(painter, subrect, Qt::AlignCenter, pix);
                  if (down) painter->translate(-1, -1);
              }
          }

          if (title->subControls & SC_TitleBarMinButton) {
              subrect = subControlRect(CC_TitleBar, title,
                                       SC_TitleBarMinButton, widget);
              if (!subrect.isNull()) {
                  down = ((title->activeSubControls & SC_TitleBarMinButton)
                          && (flags & State_Sunken));
                  pix = standardPixmap(SP_TitleBarMinButton, &tool, widget);
                  tool.rect = subrect;
                  tool.state = down ? State_Sunken : State_Raised;
                  drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);

                  if (down) painter->translate(1, 1);
                  drawItemPixmap(painter, subrect, Qt::AlignCenter, pix);
                  if (down) painter->translate(-1, -1);
              }
          }

          if ((title->subControls & SC_TitleBarMaxButton) &&
              (title->titleBarFlags & Qt::WindowMaximizeButtonHint)) {
              subrect = subControlRect(CC_TitleBar, title,
                                       SC_TitleBarMaxButton, widget);
              if (!subrect.isNull()) {
                  down = ((title->activeSubControls & SC_TitleBarMaxButton) &&
                          (flags & State_Sunken));
                  pix = standardPixmap(SP_TitleBarMaxButton, &tool, widget);
                  tool.rect = subrect;
                  tool.state = down ? State_Sunken : State_Raised;
                  drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);

                  if (down) painter->translate(1, 1);
                  drawItemPixmap(painter, subrect, Qt::AlignCenter, pix);
                  if (down) painter->translate(-1, -1);
              }
          }

          if ((title->subControls & SC_TitleBarNormalButton) &&
              (((title->titleBarFlags & Qt::WindowMinimizeButtonHint) &&
                (title->titleBarState & Qt::WindowMinimized)) ||
               ((title->titleBarFlags & Qt::WindowMaximizeButtonHint) &&
                (title->titleBarState & Qt::WindowMaximized)))) {
              subrect = subControlRect(CC_TitleBar, title,
                                       SC_TitleBarNormalButton, widget);
              if (!subrect.isNull()) {
                  down = ((title->activeSubControls & SC_TitleBarNormalButton)
                          && (flags & State_Sunken));
                  pix = standardPixmap(SP_TitleBarNormalButton, &tool, widget);
                  tool.rect = subrect;
                  tool.state = down ? State_Sunken : State_Raised;
                  drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);

                  if (down) painter->translate(1, 1);
                  drawItemPixmap(painter, subrect, Qt::AlignCenter, pix);
              }
          }

          if ((title->subControls & SC_TitleBarContextHelpButton) &&
              (title->titleBarFlags & Qt::WindowContextHelpButtonHint)) {
              subrect = subControlRect(CC_TitleBar, title,
                                       SC_TitleBarContextHelpButton, widget);
              if (!subrect.isNull()) {
                  down = ((title->activeSubControls & SC_TitleBarContextHelpButton)
                          && (flags & State_Sunken));
                  pix = standardPixmap(SP_TitleBarContextHelpButton,
                                       &tool, widget);
                  tool.rect = subrect;
                  tool.state = down ? State_Sunken : State_Raised;
                  drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);

                  if (down) painter->translate(1, 1);
                  drawItemPixmap(painter, subrect, Qt::AlignCenter, pix);
                  if (down) painter->translate(-1, -1);
              }
          }

          break;
      }

      // not drawing these controls, as default is sufficient
      // case CC_ScrollBar:
      // case CC_ToolButton:
      // case CC_GroupBox:

      default:
          QWindowsStyle::drawComplexControl(control, option, painter, widget);
          break;
    }
}

//////////////////////////////////////////////////////////////////////////////
// standardPixmap()
// ----------------
// Get pixmap for style

QPixmap PhaseStyle::standardPixmap(StandardPixmap pixmap,
                       const QStyleOption *option,
                       const QWidget *widget) const
{
    switch (pixmap) {
      case SP_TitleBarMenuButton:
          return QPixmap(title_menu_xpm);

      case SP_DockWidgetCloseButton:
      case SP_TitleBarCloseButton:
          return bitmaps_[TitleClose];

      case SP_TitleBarMinButton:
          return bitmaps_[TitleMin];

      case SP_TitleBarMaxButton:
          return bitmaps_[TitleMax];

      case SP_TitleBarNormalButton:
          return bitmaps_[TitleNormal];

      case SP_TitleBarContextHelpButton:
          return bitmaps_[TitleHelp];

      default:
          return QWindowsStyle::standardPixmap(pixmap, option, widget);

    }
}

//////////////////////////////////////////////////////////////////////////////
// Metrics and Rects                                                        //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// pixelMetric()
// -------------
// Get the pixel metric for metric

int PhaseStyle::pixelMetric(PixelMetric metric,
                            const QStyleOption *option,
                            const QWidget *widget) const
{
    int ex = qMax(QApplication::fontMetrics().xHeight(), 17);

    switch (metric) {
      case PM_ButtonMargin:
         return 6;

      case PM_ButtonDefaultIndicator:   // size of default button frame
          return 3; // half button margin

      case PM_IndicatorWidth:
      case PM_IndicatorHeight:
      case PM_ExclusiveIndicatorWidth:
      case PM_ExclusiveIndicatorHeight:
      case PM_CheckListButtonSize:
          return ex & 0xfffe; // even size

      case PM_MenuBarPanelWidth:
          return 2;

      case PM_DockWidgetTitleMargin:
          return 2;

      case PM_DockWidgetFrameWidth:
          return 3;

      case PM_ScrollBarExtent:          // base width of a vertical scrollbar
          return ex & 0xfffe;

      case PM_ScrollBarSliderMin:       // minimum length of slider
          return  ex * 2;

      case PM_TabBarTabHSpace:          // extra tab spacing
          return 24;

      case PM_TabBarTabShiftVertical:
          return 2;

      case PM_TabBarTabVSpace: {
          const QStyleOptionTab *tab;
          tab = qstyleoption_cast<const QStyleOptionTab *>(option);
          if (tab) {
              if (tab->shape == QTabBar::RoundedNorth) {
                  return 10;
              } else {
                  return 6;
              }
          }
          return 0;
      }

      case PM_ProgressBarChunkWidth: {
          int breadth = qMax(option->rect.width(), option->rect.height());
          return qMax(breadth / 10, 10);
      }

      case PM_TitleBarHeight:
          return qMax(widget ? widget->fontMetrics().lineSpacing() :
                      (option ? option->fontMetrics.lineSpacing() : 0), 22);

      default:
          return QWindowsStyle::pixelMetric(metric, option, widget);
    }
}

//////////////////////////////////////////////////////////////////////////////
// subElementRect()
// ----------------
// Return rect of subelement

QRect PhaseStyle::subElementRect(SubElement element,
                                 const QStyleOption *option,
                                 const QWidget *widget) const
{
    QRect rect;
    const int fw = 2;
    int x, y, w, h;
    option->rect.getRect(&x, &y, &w, &h);

    switch (element) {
      case SE_ComboBoxFocusRect: {
          int bw = qMin(h, 32); // position between edit and arrow
          rect.setRect(fw*2, fw*2, w-bw-fw-1, h-(fw*4));
          break;
      }

      case SE_ProgressBarContents:
          return option->rect.adjusted(3, 3, -3, -3);

      case SE_ProgressBarGroove:
      case SE_ProgressBarLabel:
          return option->rect;

      case SE_HeaderArrow: {
          int margin = pixelMetric(PM_HeaderMargin, option, widget);
          rect.setSize(QSize(h - margin*2, h - margin*2));
          if (option->state & State_Horizontal) {
              rect.moveTopLeft(QPoint(x + w - h, margin));
          } else {
              rect.moveTopLeft(QPoint(margin, margin));
          }
          rect = visualRect(option->direction, option->rect, rect);
          break;
      }

      case SE_ToolBoxTabContents:
          rect = visualRect(option->direction, option->rect, option->rect);
          break;

      case SE_DockWidgetTitleBarText: {
          rect = QWindowsStyle::subElementRect(element, option, widget);

          const QStyleOptionDockWidgetV2 *v2
              = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(option);
          bool vertical = (v2 == 0) ? false : v2->verticalTitleBar;

          if (vertical) {
              rect.adjust(0, -3, 0, -3);
          } else {
              rect.adjust(-3, 0, 3, 0);
          }
          break;
      }

      case SE_DockWidgetFloatButton:
      case SE_DockWidgetCloseButton: {
          rect = QWindowsStyle::subElementRect(element, option, widget);

          const QStyleOptionDockWidgetV2 *v2
              = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(option);
          bool vertical = (v2 == 0) ? false : v2->verticalTitleBar;

          if (vertical) {
              rect.moveTop(rect.top() + 3);
          } else {
              if (option->direction == Qt::LeftToRight) {
                  rect.moveLeft(rect.left() - 3);
              } else {
                  rect.moveLeft(rect.left() + 3);
              }
          }
          break;
      }

      default:
          rect = QWindowsStyle::subElementRect(element, option, widget);
    }

    return rect;
}

//////////////////////////////////////////////////////////////////////////////
// subControlRect()
// ----------------
// Return rect of control

QRect PhaseStyle::subControlRect(ComplexControl control,
                                 const QStyleOptionComplex *option,
                                 SubControl subcontrol,
                                 const QWidget *widget) const
{
    QRect rect;
    QRect ctlrect = option->rect;
    int x, y, w, h, x2, y2;
    const int fw = 2;
    ctlrect.getRect(&x, &y, &w, &h);
    x2 = ctlrect.right(); y2 = ctlrect.bottom();

    switch (control) {
      case CC_SpinBox: {
          const QStyleOptionSpinBox *box;
          box = qstyleoption_cast<const QStyleOptionSpinBox*>(option);
          if (!box) break;

          bool odd = widget ? widget->height() % 2 : 0;
          int bw = (h*3/4) - odd + 1; // width of button box
          switch (subcontrol) {
            case SC_SpinBoxUp:
                rect.setRect(w-bw, (h/2)-(odd ? 6 : 7), bw-1, 6);
                break;
            case SC_SpinBoxDown:
                rect.setRect(w-bw, (h/2)+1, bw-1, odd ? 7 : 6);
                break;
            case SC_SpinBoxEditField:
                rect.setRect(fw, fw, w-bw-fw, h-(fw*2));
                break;
            case SC_SpinBoxFrame:
                rect = ctlrect;
                break;
            default:
                break;
          }
          rect = visualRect(box->direction, ctlrect, rect);
          break;
      }

      case CC_ComboBox: {
          const QStyleOptionComboBox *combo;
          combo = qstyleoption_cast<const QStyleOptionComboBox*>(option);
          if (!combo) break;

          int bw = qMin(h, 32); // position between edit and arrow
          switch (subcontrol) {
            case SC_ComboBoxFrame: // total combobox area
                rect = ctlrect;
                break;

            case SC_ComboBoxArrow: // the right side
                rect.setRect(w-bw, fw, bw-fw, h-(fw*2));
                break;

            case SC_ComboBoxEditField: // the left side
                rect.setRect(fw, fw, w-bw-fw-1, h-(fw*2));
                if (!combo->editable) {
                    // give extra margin
                    rect.adjust(pixelMetric(PM_ButtonMargin),
                                0, 0, 0);
                }
                break;

            case SC_ComboBoxListBoxPopup: // the list popup box
                rect = ctlrect;
                break;

            default:
                break;
          }
          rect = visualRect(combo->direction, ctlrect, rect);
          break;
      }

      case CC_ScrollBar: {
          // three button scrollbar
          const QStyleOptionSlider *sb;
          sb = qstyleoption_cast<const QStyleOptionSlider *>(option);
          if (!sb) break;

          bool horizontal = (sb->orientation == Qt::Horizontal);
          int extent = pixelMetric(PM_ScrollBarExtent, sb, widget);
          int slidermax = ((horizontal) ?  ctlrect.width() : ctlrect.height())
              - (extent * 3);
          int slidermin = pixelMetric(PM_ScrollBarSliderMin, sb, widget);
          int sliderlen, sliderstart;

          // calculate slider length
          if (sb->maximum != sb->minimum) {
              int range = sb->maximum - sb->minimum;
              sliderlen = (sb->pageStep * slidermax) / (range + sb->pageStep);
              if ((sliderlen < slidermin) || (range > INT_MAX / 2))
                  sliderlen = slidermin;
              if (sliderlen > slidermax)
                  sliderlen = slidermax;
          } else {
              sliderlen = slidermax;
          }

          sliderstart = extent + sliderPositionFromValue(sb->minimum,
                                                         sb->maximum,
                                                         sb->sliderPosition,
                                                         slidermax - sliderlen,
                                                         sb->upsideDown);

          switch (subcontrol) {
            case SC_ScrollBarAddLine:
                if (horizontal) {
                    rect.setRect(x2 - extent + 1, y, extent, extent);
                } else {
                    rect.setRect(x, y2 - extent + 1, extent, extent);
                }
                break;

            case SC_ScrollBarSubLine:
                // rect that covers *both* subline buttons
                if (horizontal) {
                    rect.setRect(x, y, w - extent + 1, extent);
                } else {
                    rect.setRect(x, y, extent, h - extent + 1);
                }
                break;

            case SC_ScrollBarAddPage:
                if (horizontal)
                    rect.setRect(sliderstart + sliderlen, y,
                             slidermax - sliderstart - sliderlen + extent + 1,
                                 extent);
                else
                    rect.setRect(x, sliderstart + sliderlen, extent,
                             slidermax - sliderstart - sliderlen + extent + 1);
                break;

            case SC_ScrollBarSubPage:
                if (horizontal) {
                    rect.setRect(x + extent, y,
                                 sliderstart - (x + extent), extent);
                } else {
                    rect.setRect(x, y + extent, extent,
                                 sliderstart - (x + extent));
                }
                break;

            case SC_ScrollBarSlider:
                if (horizontal) {
                    rect.setRect(sliderstart - 1, y, sliderlen + 2 + 1, extent);
                } else {
                    rect.setRect(x, sliderstart - 1, extent, sliderlen + 2 + 1);
                }
                break;

            case SC_ScrollBarGroove:
                if (horizontal) {
                    rect = ctlrect.adjusted(extent, 0, -(extent*2), 0);
                } else {
                    rect = ctlrect.adjusted(0, extent, 0, -(extent*2));
                }
                break;

            default:
                break;
          }
          rect = visualRect(sb->direction, ctlrect, rect);
          break;
      }

      case CC_TitleBar: {
          const QStyleOptionTitleBar *title;
          title = qstyleoption_cast<const QStyleOptionTitleBar*>(option);
          if (!title) break;

          const int wfw = 3; // window frame width
          const int button = h - 2*wfw;
          const int delta = button + fw;
          int offset = 0;

          bool minimized = title->titleBarState & Qt::WindowMinimized;
          bool maximized = title->titleBarState & Qt::WindowMaximized;

          switch (subcontrol) {
            case SC_TitleBarLabel:
                rect = ctlrect;
                if (title->titleBarFlags
                    & (Qt::WindowTitleHint | Qt::WindowSystemMenuHint)) {
                    if (title->titleBarFlags & Qt::WindowSystemMenuHint)
                        rect.adjust(delta, 0, 0, 0);
                    if (title->titleBarFlags & Qt::WindowMinimizeButtonHint)
                        rect.adjust(0, 0, -delta, 0);
                    if (title->titleBarFlags & Qt::WindowMaximizeButtonHint)
                        rect.adjust(0, 0, -delta, 0);
                    if (title->titleBarFlags & Qt::WindowContextHelpButtonHint)
                      rect.adjust(0, 0, -delta, 0);
                    rect.adjust(wfw, wfw, -wfw, -wfw);
                }
                break;

            // right side buttons all fall through
            case SC_TitleBarContextHelpButton:
                if (title->titleBarFlags & Qt::WindowContextHelpButtonHint)
                    offset += delta;

            case SC_TitleBarMinButton:
                if (!minimized &&
                    (title->titleBarFlags & Qt::WindowMinimizeButtonHint))
                    offset += delta;
                else if (subcontrol == SC_TitleBarMinButton)
                    break;

            case SC_TitleBarNormalButton:
                if (minimized &&
                    (title->titleBarFlags & Qt::WindowMinimizeButtonHint))
                    offset += delta;
                else if (maximized  &&
                         (title->titleBarFlags & Qt::WindowMaximizeButtonHint))
                    offset += delta;
                else if (subcontrol == SC_TitleBarNormalButton)
                    break;

            case SC_TitleBarMaxButton:
                if (!maximized &&
                    (title->titleBarFlags & Qt::WindowMaximizeButtonHint))
                    offset += delta;
                else if (subcontrol == SC_TitleBarMaxButton)
                    break;

            case SC_TitleBarCloseButton:
                if (title->titleBarFlags & Qt::WindowSystemMenuHint)
                    offset += delta;
                else if (subcontrol == SC_TitleBarCloseButton)
                    break;
                rect.setRect(x2 - offset, y + wfw, button, button);
                break;

            // left side buttons
            case SC_TitleBarSysMenu:
                if (title->titleBarFlags & Qt::WindowSystemMenuHint) {
                    rect.setRect(x + wfw, y + wfw, button, button);
                }
                break;

            default:
                break;
          }

          rect = visualRect(title->direction, ctlrect, rect);
          break;
        }

      default:
          rect = QWindowsStyle::subControlRect(control, option,
                                              subcontrol, widget);
    }

    return rect;
}


//////////////////////////////////////////////////////////////////////////////
// hitTestComplexControl()
// -----------------------
// Return subcontrol of position

QStyle::SubControl PhaseStyle::hitTestComplexControl(ComplexControl control,
                                             const QStyleOptionComplex *option,
                                             const QPoint &position,
                                             const QWidget *widget) const
{
    SubControl subcontrol = SC_None;
    QRect rect;

    switch (control) {
      case CC_ScrollBar: {
          const QStyleOptionSlider *sb;
          sb = qstyleoption_cast<const QStyleOptionSlider *>(option);
          if (!sb) break;

          // these cases are order dependent
          rect = subControlRect(control, sb, SC_ScrollBarSlider, widget);
          if (rect.contains(position)) {
              subcontrol = SC_ScrollBarSlider;
              break;
          }

          rect = subControlRect(control, sb, SC_ScrollBarAddPage, widget);
          if (rect.contains(position)) {
              subcontrol = SC_ScrollBarAddPage;
              break;
          }

          rect = subControlRect(control, sb, SC_ScrollBarSubPage, widget);
          if (rect.contains(position)) {
              subcontrol = SC_ScrollBarSubPage;
              break;
          }

          rect = subControlRect(control, sb, SC_ScrollBarAddLine, widget);
          if (rect.contains(position)) {
              subcontrol = SC_ScrollBarAddLine;
              break;
          }

          rect = subControlRect(control, sb, SC_ScrollBarSubLine, widget);
          if (rect.contains(position)) {
              subcontrol = SC_ScrollBarSubLine;
              break;
          }

          break;
      }

      default:
          subcontrol = QWindowsStyle::hitTestComplexControl(control, option,
                                                           position, widget);
          break;
    }

    return subcontrol;
}

//////////////////////////////////////////////////////////////////////////////
// sizeFromContents()
// ------------------
// Return size of the element based on content

QSize PhaseStyle::sizeFromContents(ContentsType contentstype,
                                   const QStyleOption *option,
                                   const QSize &contentssize,
                                   const QWidget *widget) const
{
    switch (contentstype) {
      case CT_PushButton: {
          const QStyleOptionButton *button;
          button = qstyleoption_cast<const QStyleOptionButton *>(option);
          QSize size = contentssize;
          if (button) {
              int bm = pixelMetric(PM_ButtonMargin, button, widget);
              int fw = pixelMetric(PM_DefaultFrameWidth, button, widget) * 2;
              size.rwidth() += bm + fw;
              size.rheight() += bm + fw;

              if (button->text.isEmpty()) {
                  size = size.expandedTo(QSize(23, 23));
              } else {
                  size = size.expandedTo(QSize(75, 23));
              }

              if (button->features & QStyleOptionButton::AutoDefaultButton) {
                  int dbi = pixelMetric(PM_ButtonDefaultIndicator,
                                        button,
                                        widget);
                  size.rwidth() += dbi*2;
                  // Note: don't expand height
             }

          }
          return size;
      }

      default:
          return QWindowsStyle::sizeFromContents(contentstype,
                                                 option,
                                                 contentssize,
                                                 widget);
    }

    return QSize();
}

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous stuff                                                      //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// styleHint()
// -----------
// "feel" hints for the GUI

int PhaseStyle::styleHint(StyleHint hint,
                          const QStyleOption *option,
                          const QWidget *widget,
                          QStyleHintReturn *data) const
{
    switch (hint) {
      case SH_Menu_SpaceActivatesItem:
      case SH_TitleBar_NoBorder:
        return 1;

      case SH_ScrollBar_MiddleClickAbsolutePosition:
          return 1;

      case SH_MainWindow_SpaceBelowMenuBar:
          return 0;

      case SH_UnderlineShortcut:
          if (QApplication::keyboardModifiers() & Qt::AltModifier) {
              return 1;
          }
          return 0;

    case SH_DialogButtonLayout:
        return QDialogButtonBox::KdeLayout;

// TODO: investigate other hints, including:
//    SH_ItemView_ShowDecorationSelected
//    SH_ToolBox_SelectedPageTitleBold
//    SH_ScrollView_FrameOnlyAroundContents

      default:
          return QWindowsStyle::styleHint(hint, option, widget, data);
    }
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Handle events for animating progress bars

bool PhaseStyle::eventFilter(QObject *object, QEvent *event)
{
    if (!object->isWidgetType())
        return QObject::eventFilter(object, event);

    switch (event->type()) {
      case QEvent::KeyRelease:
      case QEvent::KeyPress:
          if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Alt) {
              // find top level window
              QWidget *widget = qobject_cast<QWidget*>(object);
              widget = widget->window();
              if (widget->parentWidget()) {
                  widget = widget->parentWidget()->window();
              }

              // update all visible enabled children widgets
              QList<QWidget*> wlist = qFindChildren<QWidget *>(widget);
              for (int n=0 ; n<wlist.size(); n++) {
                  if (wlist[n]->isEnabled() && wlist[n]->isVisible()) {
                      wlist[n]->update();
                  }
              }
          }
          break;

      case QEvent::StyleChange:
      case QEvent::Show:
          if (QProgressBar *bar = qobject_cast<QProgressBar*>(object)) {
            bars_ << bar;
            if (bars_.size() == 1) {
                timerid_ = startTimer(25);
            }
          }
          break;

      case QEvent::Destroy:
          bars_.removeAll(reinterpret_cast<QProgressBar*>(object));
          break;

      case QEvent::Hide:
          if (QProgressBar *bar = qobject_cast<QProgressBar*>(object)) {
              bars_.removeAll(bar);
              if (bars_.isEmpty() && timerid_) {
                  killTimer(timerid_);
                  timerid_ = 0;
              }
          }
      default:
          break;
    }

    return QWindowsStyle::eventFilter(object, event);
}

//////////////////////////////////////////////////////////////////////////////
// timerEvent()
// ------------
// Internal timer fired

void PhaseStyle::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerid_) {
        foreach (QProgressBar *bar, bars_) {
            if ((bar->minimum() == 0 && bar->maximum() == 0)) {
                bar->setValue(bar->value()+1);
                bar->update();
            }
        }
    }
    event->ignore();
}

//////////////////////////////////////////////////////////////////////////////
// Plugin stuff                                                            //
//////////////////////////////////////////////////////////////////////////////

class PhaseStylePlugin : public QStylePlugin
{
public:
    PhaseStylePlugin() { ; }
    QStringList keys() const {
        return QStringList() << QLatin1String( "Phase" );
    }
    QStyle *create(const QString &key) {
        if (key.toLower() == QLatin1String( "phase" )) {
            return new PhaseStyle();
        }
        return 0;
    }
};

Q_EXPORT_PLUGIN(PhaseStylePlugin)
