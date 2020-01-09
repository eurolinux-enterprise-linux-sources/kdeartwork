/*
 *	Distributed under the terms of the BSD license.
 */

#ifndef __KDE_CDECONFIG_H
#define __KDE_CDECONFIG_H

#include <qcheckbox.h>
#include <q3groupbox.h>
#include <q3buttongroup.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <q3hbox.h>
#include <kconfig.h>

class QCheckBox;
class Q3GroupBox;
class KVBox;
class QLabel;
class QRadioButton;

class CdeConfig: public QObject
{
	Q_OBJECT

	public:
		CdeConfig( KConfig* conf, QWidget* parent );
		~CdeConfig();

	// These public signals/slots work similar to KCM modules
	signals:
		void changed();

	public slots:
		void load( KConfig* conf );	
		void save( KConfig* conf );
		void defaults();

	protected slots:
		void slotSelectionChanged();	// Internal use
		void slotSelectionChanged( int );
		
	private:
		KConfig*   	cdeConfig;
		QCheckBox* 	cbColorBorder;
//		QCheckBox* 	cbTitlebarButton;
		KHBox* 	        groupBox;
		Q3GroupBox* 	gbSlider;
		Q3ButtonGroup*	bgAlign;
};


#endif

// vim: ts=4
