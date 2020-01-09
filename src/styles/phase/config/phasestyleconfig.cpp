//////////////////////////////////////////////////////////////////////////////
// phasestyleconfig.cpp
// -------------------
// Config dialog for Phase widget style
// -------------------
// Copyright (c) 2004-2007 David Johnson <david@usermode.org>
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include "phasestyleconfig.h"

#include <QSettings>
#include <QCheckBox>
#include <klocale.h>
#include <kglobal.h>

//////////////////////////////////////////////////////////////////////////////
// PhaseStyleConfig Class                                                   //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PhaseStyleConfig()
// ----------------
// Constructor

PhaseStyleConfig::PhaseStyleConfig(QWidget* parent) : QWidget(parent)
{
    KGlobal::locale()->insertCatalog("kstyle_phase_config");

    setupUi(this);

    QSettings settings("phasestyle");
    oldgradients =
        settings.value("/gradients", true).toBool();
    gradients->setChecked(oldgradients);
    oldhighlights =
        settings.value("/highlights", true).toBool();
    highlights->setChecked(oldhighlights);

    // connections
    connect(gradients, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(highlights, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
}

//////////////////////////////////////////////////////////////////////////////
// ~PhaseStyleConfig()
// -----------------
// Destructor

PhaseStyleConfig::~PhaseStyleConfig()
{
    KGlobal::locale()->removeCatalog("kstyle_phase_config");
}

//////////////////////////////////////////////////////////////////////////////
// selectionChanged()
// ------------------
// Selection has changed

void PhaseStyleConfig::updateChanged()
{
    bool update = false;

    if ((gradients->isChecked() != oldgradients) ||
        (highlights->isChecked() != oldhighlights)) {
        update = true;
    }

    emit changed(update);
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save the settings

void PhaseStyleConfig::save()
{
    QSettings settings("phasestyle");
    settings.setValue("/gradients", gradients->isChecked());
    settings.setValue("/highlights", highlights->isChecked());
}

//////////////////////////////////////////////////////////////////////////////
// defaults()
// ----------
// Set to the defaults

void PhaseStyleConfig::defaults()
{
    gradients->setChecked(true);
    highlights->setChecked(true);
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    KDE_EXPORT QObject* allocate_kstyle_config(QWidget* parent) {
        return(new PhaseStyleConfig(parent));
    }
}

#include "phasestyleconfig.moc"
