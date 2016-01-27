#!/usr/bin/env python3
#############################################################################
##
## Copyright (C) 2016 The Qt Company Ltd.
## Contact: https://www.qt.io/licensing/
##
## This file is part of the release tools of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:GPL-EXCEPT$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and The Qt Company. For licensing terms
## and conditions see https://www.qt.io/terms-conditions. For further
## information use the contact form at https://www.qt.io/contact-us.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3 as published by the Free Software
## Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
## included in the packaging of this file. Please review the following
## information to ensure the GNU General Public License requirements will
## be met: https://www.gnu.org/licenses/gpl-3.0.html.
##
## $QT_END_LICENSE$
##
#############################################################################

#regenerate all test's output

import os
import sys
import subprocess
import re

formats = ['xml', 'txt', 'xunitxml', 'lightxml']

qtver = subprocess.check_output(['qmake', '-query', 'QT_VERSION']).strip().decode('utf-8')
rootPath = os.getcwd()

isWindows = sys.platform == 'win32'

replacements = [
    (qtver, r'@INSERT_QT_VERSION_HERE@'),
    (r'Config: Using QtTest library.*', r'Config: Using QtTest library'), # Build string in text logs
    (rootPath.encode('unicode-escape').decode('utf-8'), r''),
    (r'( *)<Duration msecs="[\d\.]+"/>', r'\1<Duration msecs="0"/>'),
    (r'( *)<QtBuild>[^<]+</QtBuild>', r'\1<QtBuild/>'), # Build element in xml, lightxml
    (r'<property value="[^"]+" name="QtBuild"/>', r'<property value="" name="QtBuild"/>') # Build in xunitxml
]

extraArgs = {
    "commandlinedata": "fiveTablePasses fiveTablePasses:fiveTablePasses_data1 -v2",
    "benchlibcallgrind": "-callgrind",
    "benchlibeventcounter": "-eventcounter",
    "benchliboptions": "-eventcounter",
    "benchlibtickcounter": "-tickcounter",
    "badxml": "-eventcounter",
    "benchlibcounting": "-eventcounter",
    "printdatatags": "-datatags",
    "printdatatagswithglobaltags": "-datatags",
    "silent": "-silent",
    "verbose1": "-v1",
    "verbose2": "-v2",
}

# Replace all occurrences of searchExp in one file
def replaceInFile(file):
    import sys
    import fileinput
    for line in fileinput.input(file, inplace=1):
        for searchExp, replaceExp in replacements:
            line = re.sub(searchExp, replaceExp, line)
        sys.stdout.write(line)

def subdirs():
    result = []
    for path in os.listdir('.'):
        if os.path.isdir('./' + path):
            result.append(path)
    return result

def getTestForPath(path):
    if isWindows:
        testpath = path + '\\' + path + '.exe'
    else:
        testpath = path + '/' + path
    return testpath

def generateTestData(testname):
    print("  running " + testname)
    for format in formats:
        cmd = [getTestForPath(testname) + ' -' + format + ' ' + extraArgs.get(testname, '')]
        result = 'expected_' + testname + '.' + format
        data = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True).communicate()[0]
        out = open(result, 'wb')
        out.write(data)
        out.close()
        replaceInFile(result)

if isWindows:
    print("This script does not work on Windows.")
    exit()

tests = sys.argv[1:]
if len(tests) == 0:
   tests = subdirs()
print("Generating " + str(len(tests)) + " test results for: " + qtver + " in: " + rootPath)
for path in tests:
    if os.path.isfile(getTestForPath(path)):
        generateTestData(path)
    else:
        print("Warning: directory " + path + " contains no test executable")
