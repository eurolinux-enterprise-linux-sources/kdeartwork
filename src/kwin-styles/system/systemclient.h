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

#ifndef __SYSTEMCLIENT_H
#define __SYSTEMCLIENT_H

#include <qvariant.h>
#include <qbitmap.h>
#include <q3button.h>
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>
#include <QEvent>
#include <QBoxLayout>
#include <QLabel>
#include <Q3ValueList>
#include <QShowEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include <kpixmap.h>
#include <kdecorationfactory.h>
#include <kcommondecoration.h>

class QLabel;
class QSpacerItem;
class QBoxLayout;

namespace System {

class SystemButton;


class SystemClient : public KCommonDecoration
{
   Q_OBJECT
   public:
      SystemClient(KDecorationBridge* bridge, KDecorationFactory* factory);
      ~SystemClient();
      virtual Position mousePosition(const QPoint& p) const;
      virtual void resize(const QSize&);
      virtual void init();

      virtual QString visibleName() const ;
      virtual QString defaultButtonsLeft() const ;
      virtual QString defaultButtonsRight() const ;
      virtual KCommonDecorationButton *createButton(ButtonType type);

   protected:
      virtual void shadeChange() {};
      virtual QSize minimumSize() const;
      virtual void borders(int&, int&, int&, int&) const;
      virtual void reset( unsigned long changed );
      void drawRoundFrame(QPainter &p, int x, int y, int w, int h);
      void paintEvent( QPaintEvent* );
      void showEvent( QShowEvent* );
      void mouseDoubleClickEvent( QMouseEvent * );
      void doShape();
      void recalcTitleBuffer();

   private:
      QSpacerItem* titlebar;
      QPixmap titleBuffer;
      QString oldTitle;
};

class SystemButton : public KCommonDecorationButton
{
   public:
      SystemButton(SystemClient *parent=0, const char *name=0,
                   const unsigned char *bitmap=NULL);
	  virtual ~SystemButton() { }
      void setBitmap(const unsigned char *bitmap);
      virtual void reset(unsigned long changed);
      QSize sizeHint() const;
   protected:
      virtual void drawButton(QPainter *p);
      void drawButtonLabel(QPainter *){}
      QBitmap deco;

   private:
      SystemClient* client;
};


class SystemDecoFactory : public QObject, public KDecorationFactory
{
   Q_OBJECT
   public:
      SystemDecoFactory();
      virtual ~SystemDecoFactory();
      virtual KDecoration *createDecoration(KDecorationBridge *);
      virtual bool reset(unsigned long);
      virtual bool supports( Ability ability );
      virtual QList< BorderSize > borderSizes() const;
   private:
      void readConfig();
};



}

#endif
