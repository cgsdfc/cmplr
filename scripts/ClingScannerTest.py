"""
This script tests the correctness of cling.scanner.
It recognizes the following tokens:
    o identifier 
    o string literal, with range limitation of the quoted chars
    o char literal, which can only be +, -, /, *, digits and letter
    (including _)
    o punctuation such as () {} [] : , 
    o operator such as < <= >= > == != 

The cling.scanner uses the utillib.token_scanner by providing
three callbacks and gets those `lookahead', `shiftaway' and
`semantic' and `reacheof' for free.

Optically, impl a scanner in python is not hard at all and it
should perform more correctly and cause far less code than in C.
However, I am tired of all those scanner logic and unwilling to
write even one more indeed.

Therefore, this script merely feeds in input to the executable
and expects it return 0 for positive cases and non-zero for 
negative cases.
"""
import sys
import os
import subprocess
import glob

DEVNULL=open(os.devnull)
builddir='cmake-build'
scriptdir=os.getcwd()
exename='cling_scanner'
srcdir=os.path.join(scriptdir, '../src')
execdir=os.path.join(scriptdir, '../', builddir, exename)
datadir=os.path.join(srcdir, 'cling', 'test', 'data', 'scanner')
try:
    assert os.path.exists(execdir),"executable %s should exist" % execdir
    assert os.path.exists(datadir)
except AssertionError:
    print 'Neither executable nor data does not exist'
    print 'Exit'
    sys.exit(1)

goodtest=glob.glob(os.path.join(datadir, 'good', '*.cl'))
try:
    assert goodtest
except AssertionError:
    print 'No good test found'
    print 'Exit'
    sys.exit(2)

count=0
def Test(testinput, expected=0):
    retc=subprocess.call([execdir, testinput], stdout=DEVNULL)
    if retc != expected:
        print 'test failed %s' % testinput
        return False
    else:
        return True

for testinput in goodtest:
    if not Test(testinput):
        count += 1
    
badtest=glob.glob(os.path.join(datadir, 'bad', '*.cl'))
try:
    assert badtest
except AssertionError:
    print 'No bad test found'
    print 'Skips bad tests'
    sys.exit(count)

for testinput in badtest:
    if not Test(testinput):
        count += 1

sys.exit(count)



