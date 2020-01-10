//-----------------------------------------------------------------------------
//
// kwave - Partical Wave Screen Saver for KDE 2
//
// Copyright (c)  Ian Reinhart Geiser 2001
//
/////
//NOTE:
// The base particle engine did not come from me, it was designed by Jeff Molofee <nehe@connect.ab.ca>
// I did some extensive modifications to make it work with QT's OpenGL but the base principal is about
// the same.
////

#ifndef WAVE_H
#define WAVE_H

#include <qdialog.h>
#include <qgl.h>
#ifdef Q_WS_MACX
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <kscreensaver.h>
#include <qtimer.h>
#include <qimage.h>
#include "ui_wavecfg.h"


class Wave : public QGLWidget
{
Q_OBJECT

public:
	Wave( QWidget * parent=0 );
	~Wave();

protected:
	/** paint the GL view */
	void paintGL ();
	/** resize the gl view */
	void resizeGL ( int w, int h );
	/** setup the GL environment */
	void initializeGL ();


private:
//
	GLUnurbsObj *pNurb;
	GLint nNumPoints;
//	float ctrlPoints[4][4][3];
//	float knots[8];
	int index;
	bool LoadGLTextures();
	GLuint  texture[1];
	QImage  tex;

};

class KWaveSaver : public KScreenSaver
{
Q_OBJECT
public:
	KWaveSaver( WId drawable );
	virtual ~KWaveSaver();
	void readSettings();
public slots:
	void blank();

private:
	Wave *wave;
	QTimer  *timer;
};

class WaveWidget : public QWidget, public Ui::WaveWidget
{
public:
    WaveWidget( QWidget *parent ) : QWidget( parent ) {
        setupUi( parent );
    }
};

class KWaveSetup : public KDialog
{
	Q_OBJECT
public:
	KWaveSetup( QWidget *parent = 0L );
    ~KWaveSetup( );
protected:
	void readSettings();

private slots:
	void slotOkPressed();
	void aboutPressed();
private:
	KWaveSaver *saver;
	float	size;
	float	stars;
    WaveWidget *cfg;
};

#endif


