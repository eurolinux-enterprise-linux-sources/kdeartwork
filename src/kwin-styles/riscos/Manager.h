/*
  RISC OS KWin client

  Copyright 2000
    Rik Hemsley <rik@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef RISC_OS_MANAGER_H
#define RISC_OS_MANAGER_H

#include <q3dict.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QBoxLayout>
#include <Q3PtrList>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class QSpacerItem;
class QVBoxLayout;
class QBoxLayout;

namespace RiscOS
{

class LowerButton;
class CloseButton;
class IconifyButton;
class MaximiseButton;
class StickyButton;
class HelpButton;
class Button;

class Manager : public KDecoration
{
   Q_OBJECT

   public:

      Manager(KDecorationBridge*, KDecorationFactory*);
      ~Manager();
      void init();
      bool eventFilter(QObject*, QEvent*);
      void reset(unsigned long changed);
      void borders(int&, int&, int&, int&) const;
      void resize(const QSize&);
      QSize minimumSize() const;
      void activeChange();
      void captionChange();
      void iconChange();
      void maximizeChange();
      void desktopChange();
      void shadeChange();

   signals:

      void maximizeChanged(bool);
      void stickyChanged(bool);
      void activeChanged(bool);

   public slots:

      void slotAbove();
      void slotLower();
      void slotMaximizeClicked(Qt::MouseButton);
      void slotToggleSticky();

   protected:

      KDecoration::Position mousePosition(const QPoint &) const;
      void paletteChange(const QPalette &);
      void activeChange(bool);
      void stickyChange(bool);
      void paintEvent(QPaintEvent *);
      void resizeEvent(QResizeEvent *);
      void mouseDoubleClickEvent(QMouseEvent *);
      bool animateMinimize(bool);
      void updateButtonVisibility();
      void updateTitleBuffer();

      void createTitle();
      void resetLayout();

   private:

      QVBoxLayout       *topLayout_;
      QBoxLayout        *titleLayout_;
      QSpacerItem       *titleSpacer_;

      QPixmap           titleBuf_;
      Q3PtrList<Button>  leftButtonList_;
      Q3PtrList<Button>  rightButtonList_;
};

class Factory : public QObject, public KDecorationFactory
{
   Q_OBJECT

   public:
      Factory();
      ~Factory();
      virtual bool reset(unsigned long changed);
      virtual KDecoration* createDecoration(KDecorationBridge*);
      virtual bool supports( Ability ability );
};

} // End namespace

#endif

// vim:ts=2:sw=2:tw=78
