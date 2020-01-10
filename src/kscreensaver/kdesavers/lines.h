//-----------------------------------------------------------------------------
//
// klines 0.1.1 - Basic screen saver for KDE
// by Dirk Staneker 1997
// based on kpolygon 0.3 by Martin R. Jones 1996
//

#ifndef LINES_H
#define LINES_H

#include <qtimer.h>

#include <kdialog.h>
#include <krandomsequence.h>
#include <kscreensaver.h>

class KColorButton;

class Lines{
	private:
	struct Ln{
		Ln* next;
		int x1, y1, x2, y2;
	};
	Ln *start, *end, *akt;
	int offx1, offy1, offx2, offy2;
	uint numLn;
	public:
	Lines(int);
	~Lines();
	inline void reset();
	inline void getKoord(int&, int&, int&, int&);
	inline void setKoord(const int&, const int&, const int&, const int&);
	inline void next(void);
	void turn(const int&, const int&);
};

class kLinesSaver:public KScreenSaver{
	Q_OBJECT
	public:
	kLinesSaver( WId id );
	virtual ~kLinesSaver();

	void setLines(int len);
	void setSpeed(int spd);
	void setColor(const QColor&, const QColor&, const QColor&);

	private:
	void readSettings();
	void initialiseLines();
	void initialiseColor();

	protected:
        void paintEvent(QPaintEvent *event);

	protected:
	bool cleared;
	KRandomSequence rnd;
	QTimer timer;
	unsigned numLines;
	int speed;
	QColor colors[64];
    QColor colstart, colmid, colend;
	double colscale;
	Lines* lines;
};

class kLinesSetup : public KDialog{
	Q_OBJECT
	public:
	kLinesSetup(QWidget *parent=NULL);
    ~kLinesSetup();

	protected:
	void readSettings();

	private slots:
	void slotLength(int);
	void slotSpeed(int);
	void slotColstart(const QColor &);
	void slotColmid(const QColor &);
	void slotColend(const QColor &);
	void slotOk();
	void slotHelp();

	private:
	KColorButton *colorPush0, *colorPush1, *colorPush2;
	QWidget *preview;
	kLinesSaver *saver;
	int length, speed;
	QColor colstart, colmid, colend;
};

#endif

