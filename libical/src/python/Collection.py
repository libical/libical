#!/usr/bin/env python 
# -*- Mode: python -*-
#======================================================================
# FILE: Collection.py
# CREATOR: eric 
#
# DESCRIPTION:
#   
#
#  $Id: Collection.py,v 1.1 2001-03-04 18:47:30 plewis Exp $
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

class Collection:
    """A group of components that can be modified somewhat like a list.

    Usage:
        Collection(componet, propSequence)

    component is a Component object
    propSequence is a list or tuple of Property (or subclass of Property)
        of objects already in component
    """

    def __init__(self, component, propSequence):
        self._properties = list(propSequence[:])
        self._component = component

    def __getslice__(self, beg, end):
        return Collection(self._component, self._properties[beg:end])

    def __setslice__(self, beg, end, sequence):
        oldProps = self._properties[beg, end]
        for p in oldProps:
            self._component.removeProperty(p)
        self._properties.__setslice__(beg, end, sequence)
        for p in sequence:
            self._component.addProperty(p)
            
    def __getitem__(self, i):
        return self._properties[i]

    def __setitem__(self, i, prop):
        self._component.removeProperty(self._properties[i])
        self._component.addProperty(prop)
        self._properties[i]=prop

    def __delitem__(self, i):
        self._component.removeProperty(self._properties[i])
        del self._properties[i]

    def __len__(self):
        return len(self._properties)
            
    def append(self, property):
        self._properties.append(property)
        self._component.addProperty(property)

class ComponentCollection:
    
    def __init__(self, parent, componentSequence):
        self._parent = parent
        self._components = list(componentSequence[:])

    def __getslice__(self, beg, end):
        return ComponentCollection(self._parent, self._components[beg:end])

    def __setslice__(self, beg, end, sequence):
        oldComps = self._components[beg:end]
        self._components.__setslice__(beg, end, sequence)
        for c in sequence:
            self._components.addComponent(c)
        for c in oldComps:
            self._parent.remove_component(c)

    def __getitem__(self, i):
        return self._components[i]

    def __setitem__(self, i, prop):
        self._parent.remove_component(self._components[i])
        self._parent.addProperty(prop)
        self._components[i]=prop

    def __delitem__(self, i):
        self._parent.remove_componet(self._components[i])
        del self._components[i]

    def __len__(self):
        return len(self._components)
            
    def append(self, property):
        self._components.append(property)
        self._parent.addComponent(property)
