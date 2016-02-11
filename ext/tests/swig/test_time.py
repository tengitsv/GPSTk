#!/usr/env python

from test_utils import args, run_unit_tests
import unittest
import time
import gpstk

class TestSystemTime(unittest.TestCase):
    def test_SystemTime(self):
        t0 = gpstk.SystemTime()
        time.sleep(0.5)
        t1 = gpstk.SystemTime()
        print t1
        print gpstk.CommonTime(t1)-gpstk.CommonTime(t0)

class TestCommonTime(unittest.TestCase):
    """CommonTime tests"""
    def test_CommonTime(self):
        """CommonTime constructor tests"""
        t=gpstk.CommonTime(1980, 1, 6, 0, 0, 0, gpstk.TimeSystem(gpstk.TimeSystem.GPS))


        systime = gpstk.SystemTime()
        comtime = systime.toCommonTime()

        print systime - comtime

if __name__ == '__main__':
    run_unit_tests(TestSystemTime)
