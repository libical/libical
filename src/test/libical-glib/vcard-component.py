#!/usr/bin/env python3
# GI_TYPELIB_PATH=$PREFIX/lib/girepository-1.0/ ./vcard-component.py

###############################################################################
# SPDX-FileCopyrightText: 2025 Red Hat <www.redhat.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
###############################################################################

"""Test Python bindings for libicalvcard component"""

import gi

gi.require_version('ICalGLib', '4.0')
from gi.repository import ICalGLib  # noqa E402


def test_creation():
    note = 'note 1\nnote 2\nnote 3'

    card = ICalGLib.VcardComponent.new(ICalGLib.VcardComponentKind.VCARD_COMPONENT)
    assert card.isa() == ICalGLib.VcardComponentKind.VCARD_COMPONENT
    assert card.count_properties(ICalGLib.VcardPropertyKind.ANY_PROPERTY, False) == 0

    prop = ICalGLib.VcardProperty.new_uid('123')
    card.add_property(prop)
    prop = ICalGLib.VcardProperty.new_note(note)
    card.add_property(prop)
    assert card.count_properties(ICalGLib.VcardPropertyKind.ANY_PROPERTY, False) == 2

    prop = card.get_first_property(ICalGLib.VcardPropertyKind.UID_PROPERTY)
    assert prop.get_uid() == '123'
    assert card.get_uid() == '123'

    prop = card.get_first_property(ICalGLib.VcardPropertyKind.NOTE_PROPERTY)
    assert prop.get_note() == note


def test_restriction():
    str = (
        'BEGIN:VCARD\r\n'
        'VERSION:3.0\r\n'
        'UID:foo-bar\r\n'
        'FN:Mickey Mouse\r\n'
        'N:Perreault;Simon;;;ing. jr,M.Sc.\r\n'
        'PHOTO;ENCODING=B;TYPE=JPEG:ABCDEF\r\n'
        'END:VCARD\r\n'
    )
    expected = (
        'BEGIN:VCARD\r\n'
        'VERSION:3.0\r\n'
        'UID:foo-bar\r\n'
        'FN:Mickey Mouse\r\n'
        'N:Perreault;Simon;;;ing. jr,M.Sc.\r\n'
        'END:VCARD\r\n'
    )
    card = ICalGLib.VcardComponent.new_from_string(str)
    prop = card.get_first_property(ICalGLib.VcardPropertyKind.PHOTO_PROPERTY)
    card.remove_property(prop)

    card.transform(ICalGLib.VcardPropertyVersion.V30)

    ICalGLib.VcardComponent.check_restrictions(card)
    assert card.as_vcard_string() == expected


def main():
    test_creation()
    test_restriction()


if __name__ == '__main__':
    main()
