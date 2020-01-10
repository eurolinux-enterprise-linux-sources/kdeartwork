// -*- indent-tabs-mode: nil -*-
//////////////////////////////////////////////////////////////////////////////
// phasestyle.h
// -------------------
// Qt widget style
// -------------------
// Copyright (c) 2004-2008 David Johnson <david@usermode.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#ifndef PHASESTYLE_H
#define PHASESTYLE_H

#include <QWindowsStyle>

class QStyleOptionTab;
class QProgressBar;

class PhaseStyle : public QWindowsStyle
{
    Q_OBJECT
public:
    PhaseStyle();
    virtual ~PhaseStyle();

    void polish(QApplication* app);
    void polish(QWidget *widget);
    void polish(QPalette &pal);
    void unpolish(QApplication *app);
    void unpolish(QWidget *widget);

    QPalette standardPalette() const;

    void drawPrimitive(PrimitiveElement element,
            const QStyleOption *option,
            QPainter *painter,
            const QWidget *widget = 0) const;

    void drawControl(ControlElement element,
            const QStyleOption *option,
            QPainter *painter,
            const QWidget *widget = 0) const;

    void drawComplexControl(ComplexControl control,
            const QStyleOptionComplex *option,
            QPainter *painter,
            const QWidget *widget = 0) const;

    QPixmap standardPixmap(StandardPixmap pixmap,
            const QStyleOption *option,
            const QWidget *widget) const;

    int pixelMetric(PixelMetric metric,
            const QStyleOption *option = 0,
            const QWidget *widget = 0) const;

    QRect subElementRect(SubElement element,
            const QStyleOption *option,
            const QWidget *widget) const;

    QRect subControlRect(ComplexControl control,
            const QStyleOptionComplex *option,
            SubControl subcontrol,
            const QWidget *widget = 0) const;

    SubControl hitTestComplexControl(ComplexControl control,
            const QStyleOptionComplex *option,
            const QPoint &position,
            const QWidget *widget = 0) const;

    int styleHint(StyleHint hint,
            const QStyleOption *option = 0,
            const QWidget *widget = 0,
            QStyleHintReturn *data = 0) const;

    QSize sizeFromContents(ContentsType contentstype,
            const QStyleOption *option,
            const QSize &contentssize,
            const QWidget *widget) const;

private:
    enum GradientType {
        Horizontal,
        Vertical,
        HorizontalReverse,
        VerticalReverse,
        GradientCount
    };

    enum BitmapType {
        UArrow,
        DArrow,
        LArrow,
        RArrow,
        PlusSign,
        MinusSign,
        CheckMark,
        TitleClose,
        TitleMin,
        TitleMax,
        TitleNormal,
        TitleHelp
    };

    PhaseStyle(const PhaseStyle &);
    PhaseStyle& operator=(const PhaseStyle &);

    void drawPhaseGradient(QPainter *painter,
            const QRect &rect,
            QColor color,
            bool horizontal,
            const QSize &gsize = QSize(),
            bool reverse=false) const;

    void drawPhaseBevel(QPainter *painter,
            QRect rect,
            const QPalette &pal,
	    const QBrush &fill,
            bool sunken=false,
            bool horizontal=true,
            bool reverse=false) const;

    void drawPhaseButton(QPainter *painter,
            QRect rect,
            const QPalette &pal,
	    const QBrush &fill,
            bool sunken=false) const;

    void drawPhasePanel(QPainter *painter,
            const QRect &rect,
            const QPalette &pal,
            const QBrush &fill,
            bool sunken = false) const;

    void drawPhaseDoodads(QPainter *painter,
            const QRect &rect,
            const QPalette &pal,
            bool horizontal) const;

    void drawPhaseTab(QPainter *painter,
            const QPalette &pal,
            const QStyleOptionTab *option) const;

    void timerEvent(QTimerEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    int contrast_;
    bool gradients_;
    bool highlights_;

    QList<QBitmap> bitmaps_;
    QList<QProgressBar*> bars_; // animated progressbars
    int timerid_;
};

#endif // PHASESTYLE_H
