/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
** Configure tool
**
*/

#include "configureapp.h"

QT_BEGIN_NAMESPACE

int runConfigure( int argc, char** argv )
{
    Configure app( argc, argv );
    if (!app.isOk())
        return 3;

    app.parseCmdLine();
    app.validateArgs();
    if (!app.isOk())
        return 3;
    if( app.displayHelp() )
        return 1;

    // Read license now, and exit if it doesn't pass.
    // This lets the user see the command-line options of configure
    // without having to load and parse the license file.
    app.readLicense();
    if (!app.isOk())
        return 3;

    // Source file with path settings. Needed by qmake.
    app.generateQConfigCpp();

    // Bootstrapped includes. Needed by qmake.
    app.generateHeaders();
    if (!app.isOk())
        return 3;

    // Bootstrap qmake. Needed by config tests.
    app.buildQmake();
    if (!app.isOk())
        return 3;

    // Generate qdevice.pri
    app.generateQDevicePri();
    if (!app.isOk())
        return 3;

    // Prepare the config test build directory.
    app.prepareConfigTests();
    if (!app.isOk())
        return 3;

    // Auto-detect modules and settings.
    app.autoDetection();

    // After reading all command-line arguments, and doing all the
    // auto-detection, it's time to do some last minute validation.
    // If the validation fails, we cannot continue.
    if (!app.verifyConfiguration())
        return 3;

    app.generateOutputVars();

    if( !app.isDone() )
        app.generateCachefile();
    if( !app.isDone() )
        app.generateConfigfiles();
    if (!app.isDone())
        app.generateQConfigPri();
    if (!app.isDone())
        app.displayConfig();
    if( !app.isDone() )
        app.generateMakefiles();
    if( !app.isDone() )
        app.showSummary();
    if( !app.isOk() )
        return 2;

    return 0;
}

QT_END_NAMESPACE

int main( int argc, char** argv )
{
    QT_USE_NAMESPACE
    return runConfigure(argc, argv);
}
