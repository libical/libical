#!/usr/bin/env python 
# -*- Mode: python -*-
#======================================================================
# FILE: Store.py
# CREATOR: eric 
#
# DESCRIPTION:
#   
#
#  $Id: Store.py,v 1.1 2001-03-04 18:47:30 plewis Exp $
#  $Locker:  $
#
# (C) COPYRIGHT 2001, Eric Busboom <eric@softwarestudio.org>
# (C) COPYRIGHT 2001, Patrick Lewis <plewis@inetarena.com>  
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of either: 
#
#    The LGPL as published by the Free Software Foundation, version
#    2.1, available at: http://www.fsf.org/copyleft/lesser.html
#
#  Or:
#
#    The Mozilla Public License Version 1.0. You may obtain a copy of
#    the License at http://www.mozilla.org/MPL/
#======================================================================

class Store:
    """ 
    Base class for several component storage methods 
    """

    def __init__(self):
        pass

    def path(self):
        pass
    
    def mark(self):
        pass
    
    def commit(self): 
        pass
    
    def addComponent(self, comp):
        pass
    
    def removeComponent(self, comp):
        pass
    
    def countComponents(self, kind):
        pass
    
    def select(self, gauge):
        pass
    
    def clearSelect(self):
        pass
    
    def fetch(self, uid):
        pass
    
    def fetchMatch(self, comp):
        pass
    
    def modify(self, oldc, newc):
        pass
    
    def currentComponent(self):
        pass
    
    def firstComponent(self):
        pass
    
    def nextComponent(self):
        pass


class FileStore(Store):

    def __init__(self):
        pass

    def path(self):
        pass

    def mark(self):
        pass

    def commit(self): 
        pass

    def addComponent(self, comp):
        pass

    def removeComponent(self, comp):
        pass

    def countComponents(self, kind):
        pass

    def select(self, gauge):
        pass

    def clearSelect(self):
        pass

    def fetch(self, uid):
        pass

    def fetchMatch(self, comp):
        pass

    def modify(self, oldc, newc):
        pass

    def currentComponent(self):
        pass

    def firstComponent(self):
        pass

    def nextComponent(self):
        pass
