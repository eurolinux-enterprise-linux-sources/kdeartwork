//-----------------------------------------------------------------------------
//
// kscience - screen saver for KDE
//
// Copyright (c)  Rene Beutler 1998
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>

#include <qpainter.h>
#include <qcolormap.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <QPaintEvent>

#include <kapplication.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <krandomsequence.h>

#include "science.h"
#include "science.moc"

#if defined Q_WS_X11 && !defined K_WS_QTONLY
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#define SCI_DEFAULT_MODE          0
#define SCI_DEFAULT_MOVEX         6
#define SCI_DEFAULT_MOVEY         8
#define SCI_DEFAULT_SIZE         15
#define SCI_DEFAULT_INTENSITY     4
#define SCI_DEFAULT_SPEED        70
#define SCI_DEFAULT_INVERSE   false
#define SCI_DEFAULT_GRAVITY   false
#define SCI_DEFAULT_HIDE      false
#define SCI_MAX_SPEED           100
#define SCI_MAX_MOVE             20

#undef Below


// libkscreensaver interface
class KScienceSaverInterface : public KScreenSaverInterface
{


public:
    virtual KAboutData* aboutData() {
        return new KAboutData( "kscience.kss", "klock", ki18n( "Science Screen Saver" ), "2.2.0", ki18n( "Science Screen Saver" ) );
    }


    virtual KScreenSaver* create( WId id )
    {
        return new KScienceSaver( id );
    }

    virtual QDialog* setup()
    {
        return new KScienceSetup();
    }
};

int main( int argc, char *argv[] )
{
    KScienceSaverInterface kss;
    return kScreenSaverMain( argc, argv, kss );
}

static struct {
	QString name;
	bool inverseEnable;
	} modeInfo[MAX_MODES];

enum { MODE_WHIRL=0, MODE_CURVATURE, MODE_SPHERE, MODE_WAVE, MODE_EXPONENTIAL, MODE_CONTRACTION };

void initModeInfo()
{
	modeInfo[MODE_WHIRL].name = i18n( "Whirl" );
	modeInfo[MODE_WHIRL].inverseEnable = true;

	modeInfo[MODE_SPHERE].name = i18n( "Sphere" );
	modeInfo[MODE_SPHERE].inverseEnable = true;

	modeInfo[MODE_EXPONENTIAL].name = i18n( "Exponential" );
	modeInfo[MODE_EXPONENTIAL].inverseEnable = false;

	modeInfo[MODE_CONTRACTION].name = i18n( "Contraction" );
	modeInfo[MODE_CONTRACTION].inverseEnable = false;

	modeInfo[MODE_WAVE].name = i18n( "Wave" );
	modeInfo[MODE_WAVE].inverseEnable = false;

	modeInfo[MODE_CURVATURE].name = i18n( "Curvature" );
	modeInfo[MODE_CURVATURE].inverseEnable = true;
}

//-----------------------------------------------------------------------------
// KPreviewWidget
//

KPreviewWidget::KPreviewWidget( QWidget *parent ) :
                QWidget ( parent ) { }

void KPreviewWidget::paintEvent( QPaintEvent *event )
{
	if( saver != 0 )
		saver->do_refresh( event->rect() );
}

void KPreviewWidget::notifySaver( KScienceSaver *s )
{
	saver = s;
}

//-----------------------------------------------------------------------------
// Screen Saver
//

struct KScienceData
{
    T32bit     **offset;
    XImage     *buffer;
    XImage     *xRootWin;
    GC         gc;
};

KScienceSaver::KScienceSaver( WId id, bool s, bool gP )
    : KScreenSaver( id )
{
    setAttribute( Qt::WA_NoSystemBackground );

    d = new KScienceData;
    d->gc = XCreateGC(QX11Info::display(), id, 0, 0);
    d->xRootWin = 0;
    d->buffer = 0;

    moveOn = true;
    grabPixmap = gP;
    setup = s;

    vx = vy = 0.0;
    readSettings();

    if( !grabPixmap )
    {
        grabRootWindow();
        initialize();
        do_refresh( QRect ( 0, 0, width(), height() ) );
    }

    connect( &timer, SIGNAL(timeout()), SLOT(slotTimeout()) );
    timer.start( SCI_MAX_SPEED - speed[mode] );
}

KScienceSaver::~KScienceSaver()
{
    timer.stop();
    releaseLens();
    if ( d->xRootWin )
        XDestroyImage( d->xRootWin );
    XFreeGC(QX11Info::display(), d->gc );
    delete d;
}

void KScienceSaver::myAssert( bool term, const char *eMsg )
{
	if( !term ) {
		fprintf(stderr, "Error in KScreensaver - mode Science: %s\n", eMsg);
		releaseLens();
		exit(-1);
	}
}

void KScienceSaver::initialize()
{
	KRandomSequence rnd;
	initLens();
	signed int ws = (signed int) (width() -  diam);
	signed int hs = (signed int) (height() - diam);

	x = (ws > 0) ? (rnd.getDouble() * ws ) : 0.0;
	y = (hs > 0) ? (rnd.getDouble() * hs ) : 0.0;

	xcoord = (int) x;
	ycoord = (int) y;

	switch( bpp ) {
		case 1 : applyLens = &KScienceSaver::applyLens8bpp;  break;
		case 2 : applyLens = &KScienceSaver::applyLens16bpp; break;
		case 3 : applyLens = &KScienceSaver::applyLens24bpp; break;
		case 4 : applyLens = &KScienceSaver::applyLens32bpp; break;
		default: myAssert( false, "unsupported colordepth "\
		                   "(only 8, 16, 24, 32 bpp supported)" );
	}
}

void KScienceSaver::initWhirlLens()
{
	double dx, dy, r, phi, intens;
	T32bit *off;
	T32bit xo, yo;

	intens = double( intensity[mode] + 1) / 5.0;
	if( inverse[mode] )
		intens = -intens;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		    if( r < radius )
		    {
			    if ( dx == 0.0 )
				    phi = (dy > 0.0) ? M_PI_2 :-(M_PI_2);
			    else
				    phi = atan2( dy, dx );
			    phi +=  intens * ( radius - r ) / ( r+7.0 );
			    xo = (T32bit) ( origin + r*cos( phi ) - x );
			    yo = (T32bit) ( origin + r*sin( phi ) - y );
			    off[x] = xo*bpp + yo*imgnext;
		    }
		    else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
        }
}

void KScienceSaver::initSphereLens()
{
	double dx, dy, r, xr, yr, phi, intens;
	T32bit *off;
	T32bit xo, yo;

	intens = 1.0 - double( intensity[mode] ) / 20.0;

	if( inverse[mode] )
		intens = -intens;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		if( r < radius )
		{
			xr = (double) radius*cos(asin(dy/radius));
			yr = (double) radius*cos(asin(dx/radius));
			phi = (xr != 0.0) ? asin(dx/xr) : 0.0;
			xo = (T32bit) (origin + intens*2.0*phi*xr / M_PI - x);
			phi = (yr != 0.0) ? asin(dy/yr) : 0.0;
			yo = (T32bit) (origin + intens*2.0*phi*yr / M_PI - y);
			off[x] = xo*bpp + yo*imgnext;
		}
		else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
        }
}

void KScienceSaver::initExponentialLens()
{
	double dx, dy, r, rnew, f, intens;
	T32bit *off;
	T32bit xo, yo;

	if( mode == MODE_EXPONENTIAL )
		intens = - (0.1 + 0.8 * double( intensity[mode] + 2) / 10.0);
	else
		intens = 0.9 - 0.8 * double( intensity[mode] ) / 10.0;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		    if( r < radius )
		    {
			    if( r == 0.0 )
				    f = 0.0;
			    else
			    {
				    rnew = radius*(pow(r, intens) /  pow(radius, intens));
				    f = double ((int)rnew % radius) / r;
			    }
			    xo = (T32bit) ( origin + f*dx - x );
			    yo = (T32bit) ( origin + f*dy - y );
			    off[x] = xo*bpp + yo*imgnext;
		    }
		    else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
        }
}

void KScienceSaver::initCurvatureLens()
{
	double dx, dy, r, f, intens;
	T32bit *off;
	T32bit xo, yo;

	intens = (double) radius*intensity[mode] / 20.0;
	if( inverse[mode] ) intens = -intens;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		    if( r < radius )
		    {
			    if( r == 0.0 )
				    f = 0.0;
			    else
				    f = (r - intens * sin(M_PI * r/(double)radius)) / r;
			    xo = (T32bit) ( origin + f*dx - x );
			    yo = (T32bit) ( origin + f*dy - y );
			    off[x] = xo*bpp + yo*imgnext;
		    }
		    else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
	}
}

void KScienceSaver::initWaveLens()
{
	double dx, dy, r, rnew, f, intens, k;
	T32bit *off;
	T32bit xo, yo;

	intens = (double) intensity[mode] + 1.0;
	k = (intensity[mode] % 2) ? -12.0 : 12.0;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		    if( r < radius )
		    {
			    if( r == 0.0 )
				    f = 0.0;
			    else
			    {
				    rnew = r - k * sin( M_PI * intens * r/(double)radius);
				    f = double ((int)rnew % radius) / r;
			    }
			    xo = (T32bit) ( origin + f*dx - x );
			    yo = (T32bit) ( origin + f*dy - y );
			    off[x] = xo*bpp + yo*imgnext;
		    }
		    else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
	}
}

void KScienceSaver::initLens()
{
	int min = (width() < height()) ? width() : height();
	border = 1 + SCI_MAX_MOVE;

	radius = (size[mode] * min) / 100;
	if( radius<<1 == min ) radius--;
	diam = radius << 1;
	myAssert( diam < min, "assertion violated: diam < min" );
	origin = radius + border;
	side  = origin << 1;

	d->buffer = XSubImage( d->xRootWin, 0, 0, side, side );
        myAssert( d->buffer != 0, "cannot allocate pixmap" );

	d->offset = (T32bit **) malloc( sizeof(T32bit *) * side );
	myAssert( d->offset != 0, "too few memory" );

	switch( mode ) {
		case MODE_WHIRL: 	initWhirlLens();  break;
		case MODE_SPHERE: 	initSphereLens(); break;
		case MODE_EXPONENTIAL:
		case MODE_CONTRACTION: 	initExponentialLens(); break;
		case MODE_CURVATURE:    initCurvatureLens(); break;
		case MODE_WAVE: 	initWaveLens(); break;
		default: myAssert( false, "internal error (wrong mode in initLens() )" );
	}
}

void KScienceSaver::releaseLens()
{
	if( d->offset != 0 ) {
		for(int i=0; i<side; i++)
    			if( d->offset[i] != 0 ) free( d->offset[i] );
    		free( d->offset );
		d->offset = 0;
	}
	if( d->buffer != 0 ) {
		XDestroyImage( d->buffer );
		d->buffer = 0;
	}
}

void KScienceSaver::setMode( int m )
{
	timer.stop();

	releaseLens();
	int old = mode;
	mode = m;
	vx = copysign( moveX[mode], vx );
	vy = copysign( moveY[mode], vy );
	int dm = diam;
	initLens();
	if( hideBG[old] ^ hideBG[m] )
		do_refresh( QRect( 0, 0, width(), height() ) );
	else
		if( diam < dm )
		{
			do_refresh( QRect( (int) x+diam, (int) y,      dm-diam, diam    ) );
			do_refresh( QRect( (int) x,      (int) y+diam, dm,      dm-diam ) );
		}

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setMoveX( int s )
{
	timer.stop();

	moveX[mode] = s;
	vx = copysign( moveX[mode], vx );

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setMoveY( int s )
{
	timer.stop();

	moveY[mode] = s;
	vy = copysign( moveY[mode], vy );

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setMove( bool s )
{
	moveOn = s;
}

void KScienceSaver::setSize( int s )
{
	timer.stop();

	releaseLens();
	int dm = diam;
	size[mode] = s;
	initLens();
	if( diam < dm )
	{
		do_refresh( QRect( (int) x+diam, (int) y,      dm-diam, diam    ) );
		do_refresh( QRect( (int) x,      (int) y+diam, dm,      dm-diam ) );
	}

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setSpeed( int s )
{
	speed[mode] = s;

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setIntensity( int i )
{
	timer.stop();

	releaseLens();
	intensity[mode] = i;
	initLens();

	timer.start( SCI_MAX_SPEED - speed[mode]);
}

void KScienceSaver::setInverse( bool b )
{
	timer.stop();

	releaseLens();
	inverse[mode] = b;
	initLens();

	timer.start( SCI_MAX_SPEED - speed[mode]);
}

void KScienceSaver::setGravity( bool b )
{
	timer.stop();

	releaseLens();
	gravity[mode] = b;
	vy = copysign( moveY[mode], vy );
	initLens();

	timer.start( SCI_MAX_SPEED - speed[mode]);
}

void KScienceSaver::setHideBG( bool b )
{
	timer.stop();

	releaseLens();
	hideBG[mode] = b;
	initLens();
	do_refresh( QRect( 0, 0, width(), height() ) );

	timer.start( SCI_MAX_SPEED - speed[mode]);
}

void KScienceSaver::readSettings()
{
    KConfigGroup group = KGlobal::config()->group("Settings");
        QString sMode;

	mode = group.readEntry( "ModeNr", SCI_DEFAULT_MODE );

	for(int i=0; i < MAX_MODES; i++)
	{
		sMode.setNum( i );
		group = KGlobal::config()->group( QLatin1String( "Mode" ) + sMode );
		moveX[i]     = group.readEntry(  "MoveX",     SCI_DEFAULT_MOVEX);
		moveY[i]     = group.readEntry(  "MoveY",     SCI_DEFAULT_MOVEY);
		size[i]      = group.readEntry(  "Size",      SCI_DEFAULT_SIZE);
		speed[i]     = group.readEntry(  "Speed",     SCI_DEFAULT_SPEED);
		intensity[i] = group.readEntry(  "Intensity", SCI_DEFAULT_INTENSITY);
		inverse[i]   = group.readEntry( "Inverse",   SCI_DEFAULT_INVERSE);
		gravity[i]   = group.readEntry( "Gravity",   SCI_DEFAULT_GRAVITY);
		hideBG[i]    = group.readEntry( "HideBG",    SCI_DEFAULT_HIDE);
	}

	vx = copysign( moveX[mode], vx );
	vy = copysign( moveY[mode], vy );
}

void KScienceSaver::do_refresh( const QRect & origRect )
{
	if( grabPixmap )
		return;
        QRect rect(origRect.normalized());

	if( hideBG[mode] )
	{
		XSetWindowBackground( QX11Info::display(), winId(), QColormap::instance().pixel(Qt::black) );
		XClearArea( QX11Info::display(), winId(), rect.left(), rect.top(),
                            rect.width(), rect.height(), false );
	}
	else
	{
		myAssert( d->xRootWin != 0, "root window not grabbed" );
		XPutImage( QX11Info::display(), winId(), d->gc, d->xRootWin,
		           rect.left(), rect.top(),
                           rect.left(), rect.top(),
                           rect.width(), rect.height() );
	}
}

void KScienceSaver::slotTimeout()
{
	if( grabPixmap ) {
		if( !QWidget::find(winId())->isActiveWindow() )
			return;
		grabPreviewWidget();
		grabPixmap = false;
		initialize();
		if( hideBG[mode] )
			do_refresh( QRect ( 0, 0, width(), height() ) );
	}

	signed int oldx = xcoord, oldy = ycoord;

	if( gravity[mode] ) {
		double h = double(y+1.0) / double(height()-diam);
		if( h > 1.0 ) h = 1.0;
		vy = sqrt( h ) * ( (vy > 0.0) ? moveY[mode] : -moveY[mode] );
	}
	myAssert( abs((int)rint(vy)) <= border, "assertion violated: vy <= border" );

	if( moveOn )
	{
		x += vx;
		y += vy;
	}

	if( x <= 0.0 ) {
		vx = -vx;
		x = 0.0;
	}
	if( int(x) + diam >= width()) {
		vx = -vx;
		myAssert( width()-diam > 0, "assertion violated: width-diam > 0" );
		x = (double) (width() - diam - 1);
	}
	if( y <= 0.0 ) {
		vy = -vy;
		y = 0.0;
	}
	if( int(y) + diam >= height() ) {
		vy = -vy;
		myAssert( height() - diam > 0, "assertion violated: height-diam > 0" );
		y = (double) (height() - diam - 1);
	}

	xcoord = (int) x ;
	ycoord = (int) y ;
	signed int dx = (signed int) xcoord - oldx;
	signed int dy = (signed int) ycoord - oldy;
	signed int xs, ys, xd, yd, w, h;

	if( dx > 0 ) {
		w = diam+dx;
		xd = oldx;
		xs = border-dx;
		if( dy > 0 ) {
			h = diam+dy;
			yd = oldy;
			ys = border-dy;
		}
		else {
			h = diam-dy;
			yd = ycoord;
			ys = border;
		}
	}
	else {
		w = diam-dx;
		xd = xcoord;
		xs = border;
		if( dy > 0 ) {
			h = diam+dy;
			yd = oldy;
			ys = border-dy;
		} else {
			h = diam-dy;
			yd = ycoord;
			ys = border;
		}
	}

	if(  xd + w >= width()  ) w = width()  - xd - 1;
	if(  yd + h >= height() ) h = height() - yd - 1;

//printf("%d: (dx: %3d, dy: %3d), diam: %3d, (xc: %3d, yc: %3d), (xs: %3d, ys: %3d), (xd: %3d, yd: %3d), (w: %3d, h: %3d)\n", mode, dx, dy, diam, xcoord, ycoord, xs, ys, xd, yd, w, h);
	myAssert( dx <= border && dy <=border, "assertion violated: dx or dy <= border");
	myAssert( xcoord >= 0 && ycoord >= 0, "assertion violated: xcoord, ycoord >= 0 ");
	myAssert( xd+w < width(), "assertion violated: xd+w < width" );
	myAssert( yd+h < height(), "assertion violated: yd+h < height" );

	if( hideBG[mode] )
		blackPixel( xcoord, ycoord );
	(this->*applyLens)(xs, ys, xd, yd, w, h);
	XPutImage( QX11Info::display(), winId(), d->gc, d->buffer, 0, 0, xd, yd, w, h );
	if( hideBG[mode] )
		blackPixelUndo( xcoord, ycoord );
}

void KScienceSaver::grabRootWindow()
{
	Display *dsp = QX11Info::display();
	Window rootwin = RootWindow( dsp, QX11Info::appScreen() );

	// grab contents of root window
	if( d->xRootWin )
		XDestroyImage( d->xRootWin );

	d->xRootWin = XGetImage( dsp, rootwin, 0, 0, width(),
	                      height(), AllPlanes, ZPixmap);
	myAssert( d->xRootWin, "unable to grab root window\n" );

	imgnext = d->xRootWin->bytes_per_line;
	bpp = ( d->xRootWin->bits_per_pixel ) >> 3;
}

void KScienceSaver::grabPreviewWidget()
{
	myAssert( QWidget::find(winId())->isActiveWindow(), "cannot grab preview widget: dialog not active()" );

	if( d->xRootWin )
		XDestroyImage( d->xRootWin );

	Display *dsp = QX11Info::display();
	d->xRootWin = XGetImage( dsp, winId(), 0, 0, width(), height(), AllPlanes, ZPixmap);
	myAssert( d->xRootWin, "unable to grab preview window\n" );

	imgnext = d->xRootWin->bytes_per_line;
	bpp = ( d->xRootWin->bits_per_pixel ) >> 3;
}

void KScienceSaver::blackPixel( int x, int y )
{
	unsigned char black = (char) BlackPixel( QX11Info::display(), QX11Info::appScreen() );
	unsigned int adr = x*bpp + y*imgnext;

	for(int i=0; i<bpp; i++) {
		blackRestore[i] = d->xRootWin->data[adr];
		d->xRootWin->data[adr++] = black;
	}
}

void KScienceSaver::blackPixelUndo( int x, int y )
{
	unsigned int adr = x*bpp + y*imgnext;
	for(int i=0; i<bpp; i++)
		d->xRootWin->data[adr++] = blackRestore[i];
}

// hm....

void KScienceSaver::applyLens8bpp(int xs, int ys, int xd, int yd, int w, int h)
{
	T32bit *off;
	char *img1, *img2, *data;
	signed int ix, iy, datanext = d->buffer->bytes_per_line - w;

	img1 = d->xRootWin->data + xd + yd*imgnext;
	data = d->buffer->data;
	for(iy = ys; iy < ys+h; iy++)
	{
		off = d->offset[iy] + xs;
		img2 = img1;
		for(ix = w; ix > 0; ix--)
			*data++ = img2++[*off++];
		img1 += imgnext;
		data += datanext;
	}

}

void KScienceSaver::applyLens16bpp(int xs, int ys, int xd, int yd, int w, int h)
{
	T32bit *off;
	char *img1, *img2, *data;
	int ix, iy, datanext = d->buffer->bytes_per_line - (w << 1);

	img1 = d->xRootWin->data + (xd << 1) + yd*imgnext;
	data = d->buffer->data;
	for(iy = ys; iy < ys+h; iy++)
	{
		off = d->offset[iy] + xs;
		img2 = img1;
		for(ix = w; ix > 0; ix--) {
			*data++ = img2++[*off];
			*data++ = img2++[*off++];
		}
		img1 += imgnext;
		data += datanext;
	}
}

void KScienceSaver::applyLens24bpp(int xs, int ys, int xd, int yd, int w, int h)
{
	T32bit *off;
	char *img1, *img2, *data;
	signed int ix, iy, datanext = d->buffer->bytes_per_line - 3*w;

	img1 = d->xRootWin->data + 3*xd + yd*imgnext;
	data = d->buffer->data;
	for(iy = ys; iy < ys+h; iy++)
	{
		off = d->offset[iy] + xs;
		img2 = img1;
		for(ix = w; ix > 0; ix--) {
			*data++ = img2++[*off];
			*data++ = img2++[*off];
			*data++ = img2++[*off++];
		}
		img1 += imgnext;
		data += datanext;
	}
}

void KScienceSaver::applyLens32bpp(int xs, int ys, int xd, int yd, int w, int h)
{
	T32bit *off;
	char *img1, *img2, *data;
	signed int ix, iy, datanext = d->buffer->bytes_per_line - (w << 2);

	img1 = d->xRootWin->data + (xd << 2) + yd*imgnext;
	data = d->buffer->data;
	for(iy = ys; iy < ys+h; iy++)
	{
		off = d->offset[iy] + xs;
		img2 = img1;
		for(ix = w; ix > 0; ix--) {
			*data++ = img2++[*off];
			*data++ = img2++[*off];
			*data++ = img2++[*off];
			*data++ = img2++[*off++];
		}
		img1 += imgnext;
		data += datanext;
	}
}


//-----------------------------------------------------------------------------

KScienceSetup::KScienceSetup(  QWidget *parent )
	: KDialog( parent)
	  , saver( 0 )
{
	setCaption(i18n( "Setup Science Screen Saver" ));
	setModal(true);
	setButtons(Ok|Cancel|Help);
	setDefaultButton(Ok);
	showButtonSeparator(true);
	readSettings();
	initModeInfo();
       setButtonText( Help, i18n( "A&bout" ) );
	QWidget *main = new QWidget(this);
	setMainWidget(main);

	QHBoxLayout *lt  = new QHBoxLayout( main );
        lt->setSpacing( spacingHint() );
	QVBoxLayout *ltm = new QVBoxLayout;
	lt->addLayout( ltm );
	QVBoxLayout *ltc = new QVBoxLayout;
	lt->addLayout( ltc );

	// mode
	QLabel *label = new QLabel( i18n("Mode:"), main );
	ltm->addWidget( label );

	QListWidget *c = new QListWidget( main );
	for(int i = 0; i<MAX_MODES; i++)
		c->addItem( modeInfo[i].name );
	c->setCurrentRow( mode );
	c->setFixedHeight( 5 * c->fontMetrics().height() );
	connect( c, SIGNAL(currentRowChanged(int)), SLOT(slotMode(int)) );
	ltm->addWidget( c );

	// inverse
	QCheckBox *cbox = checkInverse = new QCheckBox( i18n("Inverse"), main );
	cbox->setEnabled( modeInfo[mode].inverseEnable );
	cbox->setChecked( inverse[mode] );
	connect( cbox, SIGNAL(clicked()), SLOT(slotInverse()) );
	ltm->addWidget( cbox );

	// gravity
	cbox = checkGravity = new QCheckBox( i18n("Gravity"), main );
	cbox->setChecked( gravity[mode] );
	connect( cbox, SIGNAL(clicked()), SLOT(slotGravity()) );
	ltm->addWidget( cbox );

	// hide background
	cbox = checkHideBG = new QCheckBox( i18n("Hide background"), main );
	cbox->setChecked( hideBG[mode] );
	connect( cbox, SIGNAL(clicked()), SLOT(slotHideBG()) );
	ltm->addWidget( cbox );
	ltm->addStretch();

	// size
	label = new QLabel( i18n("Size:"), main );
	ltc->addWidget( label );

	slideSize = new QSlider(Qt::Horizontal, main );
	slideSize->setMinimum(9);
	slideSize->setMaximum(50);
	slideSize->setPageStep(5);
	slideSize->setValue(size[mode]);
	slideSize->setMinimumSize( 90, 20 );
	slideSize->setTickPosition(QSlider::TicksBelow);
	slideSize->setTickInterval(5);
	connect( slideSize, SIGNAL(sliderMoved(int)),
		SLOT(slotSize(int)) );
	connect( slideSize, SIGNAL(sliderPressed()),
		SLOT(slotSliderPressed()) );
	connect( slideSize, SIGNAL(sliderReleased()),
		SLOT(slotSliderReleased()) );

	ltc->addWidget( slideSize );

	// intensity
	label = new QLabel( i18n("Intensity:"), main );
	ltc->addWidget( label );

	slideIntensity = new QSlider(Qt::Horizontal, main );
	slideIntensity->setMinimum(0);
	slideIntensity->setMaximum(10);
	slideIntensity->setPageStep(1);
	slideIntensity->setValue(intensity[mode]);
	slideIntensity->setMinimumSize( 90, 20 );
	slideIntensity->setTickPosition(QSlider::TicksBelow);
	slideIntensity->setTickInterval(1);
	connect( slideIntensity, SIGNAL(sliderMoved(int)),
		SLOT(slotIntensity(int)) );
	connect( slideIntensity, SIGNAL(sliderPressed()),
		SLOT(slotSliderPressed()) );
	connect( slideIntensity, SIGNAL(sliderReleased()),
		SLOT(slotSliderReleased()) );
	ltc->addWidget( slideIntensity );

	// speed
	label = new QLabel( i18n("Speed:"), main );
	ltc->addWidget( label );

	slideSpeed = new QSlider(Qt::Horizontal, main );
	slideSpeed->setMinimum(0);
	slideSpeed->setMaximum(SCI_MAX_SPEED);
	slideSpeed->setPageStep(10);
	slideSpeed->setValue(speed[mode]);
	slideSpeed->setMinimumSize( 90, 20 );
	slideSpeed->setTickPosition(QSlider::TicksBelow);
	slideSpeed->setTickInterval(10);
	connect( slideSpeed, SIGNAL(sliderMoved(int)),
		SLOT(slotSpeed(int)) );
	ltc->addWidget( slideSpeed );

	// motion
	label = new QLabel( i18n("Motion:"), main );
	ltc->addWidget( label );

	QHBoxLayout *ltcm = new QHBoxLayout;
	ltc->addLayout( ltcm );

	slideMoveX = new QSlider(Qt::Horizontal, main );
	slideMoveX->setMinimum(0);
	slideMoveX->setMaximum(SCI_MAX_MOVE);
	slideMoveX->setPageStep(5);
	slideMoveX->setValue(moveX[mode]);
	slideMoveX->setMinimumSize( 40, 20 );
	slideMoveX->setTickPosition(QSlider::TicksBelow);
	slideMoveX->setTickInterval(5);
	connect( slideMoveX, SIGNAL(sliderMoved(int)),
		SLOT(slotMoveX(int)) );
	ltcm->addWidget( slideMoveX );

	slideMoveY = new QSlider(Qt::Horizontal, main );
	slideMoveY->setMinimum(0);
	slideMoveY->setMaximum(SCI_MAX_MOVE);
	slideMoveY->setPageStep(5);
	slideMoveY->setValue(moveY[mode]);
	slideMoveY->setMinimumSize( 40, 20 );
	slideMoveY->setTickPosition(QSlider::TicksBelow);
	slideMoveY->setTickInterval(5);
	connect( slideMoveY, SIGNAL(sliderMoved(int)),
		SLOT(slotMoveY(int)) );
	ltcm->addWidget( slideMoveY );

	ltc->addStretch();

	// preview
	preview = new KPreviewWidget( main );
	preview->setFixedSize( 220, 170 );
	QPixmap p( KStandardDirs::locate("data", QLatin1String( "kscreensaver/pics/kscience.png" )) );
	QPalette palette;
	if( p.isNull() ) {
		palette.setColor( preview->backgroundRole(), Qt::black );
	} else {
		palette.setBrush( preview->backgroundRole(), QBrush( p ) );
	}
	preview->setPalette( palette );
	preview->setAutoFillBackground(true);
	preview->show();	// otherwise saver does not get correct size
	lt->addWidget( preview );

	// let the preview window display before creating the saver
	kapp->processEvents();

	saver = new KScienceSaver( preview->winId(), true, !p.isNull() );
	preview->notifySaver( saver );
	connect(this,SIGNAL(okClicked()),SLOT(slotOk()));
	connect(this,SIGNAL(helpClicked()),SLOT(slotHelp()));
}

KScienceSetup::~KScienceSetup()
{
	delete saver;		// be sure to delete this first
}

void KScienceSetup::updateSettings()
{
	// update dialog
	slideMoveX    ->setValue(   moveX[mode]     );
	slideMoveY    ->setValue(   moveY[mode]     );
	slideSize     ->setValue(   size[mode]      );
	slideSpeed    ->setValue(   speed[mode]     );
	slideIntensity->setValue(   intensity[mode] );
	checkInverse  ->setEnabled( modeInfo[mode].inverseEnable );
	checkInverse  ->setChecked( inverse[mode]   );
	checkGravity  ->setChecked( gravity[mode]   );
	checkHideBG   ->setChecked( hideBG[mode]    );
}

// read settings from config file
void KScienceSetup::readSettings()
{
    KConfigGroup group = KGlobal::config()->group("Settings");
        QString sMode;

	mode = group.readEntry( "ModeNr", SCI_DEFAULT_MODE );

	for(int i=0; i < MAX_MODES; i++)
	{
		sMode.setNum( i );
		group = KGlobal::config()->group( QLatin1String( "Mode" ) + sMode );
		moveX[i]     = group.readEntry(  "MoveX",     SCI_DEFAULT_MOVEX);
		moveY[i]     = group.readEntry(  "MoveY",     SCI_DEFAULT_MOVEY);
		size[i]      = group.readEntry(  "Size",      SCI_DEFAULT_SIZE);
		speed[i]     = group.readEntry(  "Speed",     SCI_DEFAULT_SPEED);
		intensity[i] = group.readEntry(  "Intensity", SCI_DEFAULT_INTENSITY);
		inverse[i]   = group.readEntry( "Inverse",   SCI_DEFAULT_INVERSE);
		gravity[i]   = group.readEntry( "Gravity",   SCI_DEFAULT_GRAVITY);
		hideBG[i]    = group.readEntry( "HideBG",    SCI_DEFAULT_HIDE);
	}
}

void KScienceSetup::slotMode( int m )
{
	mode = m;

	if( saver )
		saver->setMode( mode );

	updateSettings();
}

void KScienceSetup::slotInverse( )
{
	inverse[mode] = checkInverse->isChecked();

	if( saver )
		saver->setInverse( inverse[mode] );
}

void KScienceSetup::slotGravity( )
{
	gravity[mode] = checkGravity->isChecked();

	if( saver )
		saver->setGravity( gravity[mode] );
}

void KScienceSetup::slotHideBG( )
{
	hideBG[mode] = checkHideBG->isChecked();

	if( saver )
		saver->setHideBG( hideBG[mode] );
}

void KScienceSetup::slotMoveX( int x )
{
	moveX[mode] = x;

	if( saver )
		saver->setMoveX( x );
}

void KScienceSetup::slotMoveY( int y )
{
	moveY[mode] = y;

	if( saver )
		saver->setMoveY( y );
}

void KScienceSetup::slotSize( int s )
{
	size[mode] = s;

	if( saver )
		saver->setSize( s );
}

void KScienceSetup::slotSpeed( int s )
{
	speed[mode] = s;

	if( saver )
		saver->setSpeed( s );
}

void KScienceSetup::slotIntensity( int i )
{
	intensity[mode] = i;

	if( saver )
		saver->setIntensity( i );
}

void KScienceSetup::slotSliderPressed()
{
	if( saver )
		saver->setMove( false );
}

void KScienceSetup::slotSliderReleased()
{
	if( saver )
		saver->setMove( true );
}

// Ok pressed - save settings and exit
void KScienceSetup::slotOk()
{
    KConfigGroup group = KGlobal::config()->group("Settings");
	QString sSize, sSpeed, sIntensity, sMode;

	group.writeEntry( "ModeNr", mode );

	for(int i=0; i<MAX_MODES; i++)
	{
		sMode.setNum( i );
		group = KGlobal::config()->group(QLatin1String( "Mode" ) + sMode );
		group.writeEntry( "MoveX",     moveX[i]     );
		group.writeEntry( "MoveY",     moveY[i]     );
		group.writeEntry( "Size",      size[i]      );
		group.writeEntry( "Speed",     speed[i]     );
		group.writeEntry( "Intensity", intensity[i] );
		group.writeEntry( "Inverse",   inverse[i]   );
		group.writeEntry( "Gravity",   gravity[i]   );
		group.writeEntry( "HideBG",    hideBG[i]    );
	}

	group.sync();

	accept();
}

void KScienceSetup::slotHelp()
{
	QString about = i18n("Science Version 0.26.5\n\nWritten by Rene Beutler (1998)\nrbeutler@g26.ethz.ch");
	KMessageBox::about(this,
	                      about);
}
