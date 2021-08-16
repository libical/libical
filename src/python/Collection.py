#!/usr/bin/env python
#======================================================================
# FILE: Collection.py
# CREATOR: eric
#
# (C) COPYRIGHT 2001, Eric Busboom <eric@civicknowledge.com>
# (C) COPYRIGHT 2001, Patrick Lewis <plewis@inetarena.com>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of either:
#
#   The LGPL as published by the Free Software Foundation, version
#   2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.txt
#
# Or:
#
#   The Mozilla Public License Version 2.0. You may obtain a copy of
#   the License at https://www.mozilla.org/MPL/
#======================================================================

from types import *

class Collection:
    """A group of components that can be modified somewhat like a list.

    Usage:
        Collection(component, propSequence)

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

        if not isinstance(sequence, ListType):
            raise TypeError, "must assign list (not instance) to slice"

        oldProps = self._properties[beg:end]

        for p in oldProps:
            self._component.remove_property(p)

        self._properties[beg:end] = sequence
        for p in sequence:
            self._component.add_property(p)

    def __getitem__(self, i):
        return self._properties[i]

    def __setitem__(self, i, prop):
        self._component.remove_property(self._properties[i])
        self._component.add_property(prop)
        self._properties[i] = prop

    def __delitem__(self, i):
        self._component.remove_property(self._properties[i])
        del self._properties[i]

    def __len__(self):
        return len(self._properties)

    def __str__(self):
        s = "[ "
        if self._properties:
            s = s + str(self._properties[0])
            for p in self._properties[1:]:
                s = "%s, %s" % (s, p)
        s = s + " ]"
        return s

    def append(self, property):
        self._properties.append(property)
        self._component.add_property(property)


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
            self._parent.add_component(c)
        for c in oldComps:
            self._parent.remove_component(c)

    def __getitem__(self, i):
        return self._components[i]

    def __setitem__(self, i, prop):
        self._parent.remove_component(self._components[i])
        self._parent.add_property(prop)
        self._components[i] = prop

    def __delitem__(self, i):
        self._parent.remove_component(self._components[i])
        del self._components[i]

    def __len__(self):
        return len(self._components)

    def __add__(self, iterable):
        for i in iterable:
            self.append(i)

    def append(self, property):
        self._components.append(property)
        self._parent.add_component(property)
