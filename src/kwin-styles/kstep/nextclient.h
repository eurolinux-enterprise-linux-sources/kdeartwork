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

#ifndef __NEXTCLIENT_H
#define __NEXTCLIENT_H

//#include <QVariant>

#include <QBitmap>
#include <QPixmap>

//Added by qt3to4:
#include <QPaintEvent>
#include <QEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include <qlayout.h>
#include <q3button.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <kcommondecoration.h>

class QLabel;
class QSpacerItem;

namespace KStep {

class NextClient;

class NextButton : public QAbstractButton
{
public:
    NextButton(NextClient *parent=0, 
               const unsigned char *bitmap=NULL, int bw=0, int bh=0,
               const QString& tip=NULL, const int realizeBtns = Qt::LeftButton);
    void setBitmap(const unsigned char *bitmap, int bw, int bh);
    void reset();
    Qt::MouseButton lastButton() { return last_button; }

protected:
    void mousePressEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    virtual void paintEvent(QPaintEvent *e);

    QPixmap aBackground, iBackground;
    QBitmap deco;
    NextClient *client;
    Qt::MouseButton last_button;
    int realizeButtons;
};

class NextClient : public KDecoration
{
    Q_OBJECT
public:
    NextClient(KDecorationBridge *b, KDecorationFactory *f);
    ~NextClient() {;}
    void init();
    virtual bool drawbound(const QRect& geom, bool clear);
protected:
    bool eventFilter(QObject *o, QEvent *e);
    void resizeEvent( QResizeEvent* );
    void paintEvent( QPaintEvent* );
    void showEvent( QShowEvent* );

    void mouseDoubleClickEvent( QMouseEvent * );
    void captionChange();
    void desktopChange();
    void activeChange();
    void shadeChange();
    void iconChange();
    QSize minimumSize() const;
    void resize(const QSize &size);
    void borders(int &left, int &right, int &top, int &bottom) const;
    void reset(unsigned long changed);
    void calcHiddenButtons();
    void updateActiveBuffer();

    Position mousePosition(const QPoint &) const;
    void maximizeChange();

protected slots:
    void slotReset();
    void menuButtonPressed();
    void maximizeButtonClicked();
    void shadeClicked();
    void aboveClicked();
    void belowClicked();
    void resizePressed();

    void keepAboveChange(bool above);
    void keepBelowChange(bool below);

private:
    void initializeButtonsAndTitlebar(QBoxLayout* titleLayout);
    void addButtons(QBoxLayout* titleLayout, const QString& buttons);
    bool mustDrawHandle() const;

    QSpacerItem* titlebar;

    // Helpful constants for buttons in array
    enum { CLOSE_IDX = 0,
           HELP_IDX,
           ICONIFY_IDX,
           MAXIMIZE_IDX,
           MENU_IDX,
           SHADE_IDX,
           ABOVE_IDX,
           BELOW_IDX,
           RESIZE_IDX,
           STICKY_IDX,
           MAX_NUM_BUTTONS = STICKY_IDX + 1 };

    // WARNING: button[i] may be null for any given i.  Make sure you
    // always check for null before doing button[i]->foo().
    NextButton* button[MAX_NUM_BUTTONS];
};

class NextClientFactory: public QObject, public KDecorationFactory
{
public:
    NextClientFactory();
    virtual ~NextClientFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *);
    virtual bool reset(unsigned long changed);
    virtual bool supports(Ability ability) const;

    virtual QList< NextClientFactory::BorderSize > borderSizes() const;

};

}

#endif
