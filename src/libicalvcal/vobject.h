/***************************************************************************
(C) Copyright 1996 Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.

For purposes of this license notice, the term Licensors shall mean,
collectively, Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.
The term Licensor shall mean any of the Licensors.

Subject to acceptance of the following conditions, permission is hereby
granted by Licensors without the need for written agreement and without
license or royalty fees, to use, copy, modify and distribute this
software for any purpose.

The above copyright notice and the following four paragraphs must be
reproduced in all copies of this software and any software including
this software.

THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS AND NO LICENSOR SHALL HAVE
ANY OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS OR
MODIFICATIONS.

IN NO EVENT SHALL ANY LICENSOR BE LIABLE TO ANY PARTY FOR DIRECT,
INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOST PROFITS ARISING OUT
OF THE USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

EACH LICENSOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO ANY WARRANTY OF NONINFRINGEMENT OR THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.

The software is provided with RESTRICTED RIGHTS.  Use, duplication, or
disclosure by the government are subject to restrictions set forth in
DFARS 252.227-7013 or 48 CFR 52.227-19, as applicable.

***************************************************************************/

/*

The vCard/vCalendar C interface is implemented in the set
of files as follows:

vcc.y, yacc source, and vcc.c, the yacc output you will use
implements the core parser

vobject.c implements an API that insulates the caller from
the parser and changes in the vCard/vCalendar BNF

vcc.h and vobject.h are header files for their .c counterparts

vcaltmp.h and vcaltmp.c implement vCalendar "macro" functions
which you may find useful.

test.c is a standalone test driver that exercises some of
the features of the APIs provided. Invoke test.exe on a
VCARD/VCALENDAR input text file and you will see the pretty
print output of the internal representation (this pretty print
output should give you a good idea of how the internal
representation looks like -- there is one such output in the
following too). Also, a file with the .out suffix is generated
to show that the internal representation can be written back
in the original text format.

For more information on this API see the readme.txt file
which accompanied this distribution.
*/

#ifndef VOBJECT_H
#define VOBJECT_H

#include "libical_vcal_export.h"
#include <stdlib.h>
#include <stdio.h>

#if defined(__CPLUSPLUS__) || defined(__cplusplus)
extern "C"
{
#endif

#define VC7bitProp                      "7BIT"
#define VC8bitProp                      "8BIT"
#define VCAAlarmProp                    "AALARM"
#define VCAdditionalNamesProp           "ADDN"
#define VCAdrProp                       "ADR"
#define VCAgentProp                     "AGENT"
#define VCAIFFProp                      "AIFF"
#define VCAOLProp                       "AOL"
#define VCAppleLinkProp                 "APPLELINK"
#define VCAttachProp                    "ATTACH"
#define VCAttendeeProp                  "ATTENDEE"
#define VCATTMailProp                   "ATTMAIL"
#define VCAudioContentProp              "AUDIOCONTENT"
#define VCAVIProp                       "AVI"
#define VCBase64Prop                    "BASE64"
#define VCBBSProp                       "BBS"
#define VCBirthDateProp                 "BDAY"
#define VCBMPProp                       "BMP"
#define VCBodyProp                      "BODY"
#define VCBusinessRoleProp              "ROLE"
#define VCCalProp                       "VCALENDAR"
#define VCCaptionProp                   "CAP"
#define VCCardProp                      "VCARD"
#define VCCarProp                       "CAR"
#define VCCategoriesProp                "CATEGORIES"
#define VCCellularProp                  "CELL"
#define VCCGMProp                       "CGM"
#define VCCharSetProp                   "CS"
#define VCCIDProp                       "CID"
#define VCCISProp                       "CIS"
#define VCCityProp                      "L"
#define VCClassProp                     "CLASS"
#define VCCommentProp                   "NOTE"
#define VCCompletedProp                 "COMPLETED"
#define VCContentIDProp                 "CONTENT-ID"
#define VCCountryNameProp               "C"
#define VCDAlarmProp                    "DALARM"
#define VCDataSizeProp                  "DATASIZE"
#define VCDayLightProp                  "DAYLIGHT"
#define VCDCreatedProp                  "DCREATED"
#define VCDeliveryLabelProp             "LABEL"
#define VCDescriptionProp               "DESCRIPTION"
#define VCDIBProp                       "DIB"
#define VCDisplayStringProp             "DISPLAYSTRING"
#define VCDomesticProp                  "DOM"
#define VCDTendProp                     "DTEND"
#define VCDTstartProp                   "DTSTART"
#define VCDueProp                       "DUE"
#define VCEmailAddressProp              "EMAIL"
#define VCEncodingProp                  "ENCODING"
#define VCEndProp                       "END"
#define VCEventProp                     "VEVENT"
#define VCEWorldProp                    "EWORLD"
#define VCExNumProp                     "EXNUM"
#define VCExpDateProp                   "EXDATE"
#define VCExpectProp                    "EXPECT"
#define VCExtAddressProp                "EXT ADD"
#define VCFamilyNameProp                "F"
#define VCFaxProp                       "FAX"
#define VCFullNameProp                  "FN"
#define VCGeoProp                       "GEO"
#define VCGeoLocationProp               "GEO"
#define VCGIFProp                       "GIF"
#define VCGivenNameProp                 "G"
#define VCGroupingProp                  "Grouping"
#define VCHomeProp                      "HOME"
#define VCIBMMailProp                   "IBMMail"
#define VCInlineProp                    "INLINE"
#define VCInternationalProp             "INTL"
#define VCInternetProp                  "INTERNET"
#define VCISDNProp                      "ISDN"
#define VCJPEGProp                      "JPEG"
#define VCLanguageProp                  "LANG"
#define VCLastModifiedProp              "LAST-MODIFIED"
#define VCLastRevisedProp               "REV"
#define VCLocationProp                  "LOCATION"
#define VCLogoProp                      "LOGO"
#define VCMailerProp                    "MAILER"
#define VCMAlarmProp                    "MALARM"
#define VCMCIMailProp                   "MCIMAIL"
#define VCMessageProp                   "MSG"
#define VCMETProp                       "MET"
#define VCModemProp                     "MODEM"
#define VCMPEG2Prop                     "MPEG2"
#define VCMPEGProp                      "MPEG"
#define VCMSNProp                       "MSN"
#define VCNamePrefixesProp              "NPRE"
#define VCNameProp                      "N"
#define VCNameSuffixesProp              "NSUF"
#define VCNoteProp                      "NOTE"
#define VCOrgNameProp                   "ORGNAME"
#define VCOrgProp                       "ORG"
#define VCOrgUnit2Prop                  "OUN2"
#define VCOrgUnit3Prop                  "OUN3"
#define VCOrgUnit4Prop                  "OUN4"
#define VCOrgUnitProp                   "OUN"
#define VCPagerProp                     "PAGER"
#define VCPAlarmProp                    "PALARM"
#define VCParcelProp                    "PARCEL"
#define VCPartProp                      "PART"
#define VCPCMProp                       "PCM"
#define VCPDFProp                       "PDF"
#define VCPGPProp                       "PGP"
#define VCPhotoProp                     "PHOTO"
#define VCPICTProp                      "PICT"
#define VCPMBProp                       "PMB"
#define VCPostalBoxProp                 "BOX"
#define VCPostalCodeProp                "PC"
#define VCPostalProp                    "POSTAL"
#define VCPowerShareProp                "POWERSHARE"
#define VCPreferredProp                 "PREF"
#define VCPriorityProp                  "PRIORITY"
#define VCProcedureNameProp             "PROCEDURENAME"
#define VCProdIdProp                    "PRODID"
#define VCProdigyProp                   "PRODIGY"
#define VCPronunciationProp             "SOUND"
#define VCPSProp                        "PS"
#define VCPublicKeyProp                 "KEY"
#define VCQPProp                        "QP"
#define VCQuickTimeProp                 "QTIME"
#define VCQuotedPrintableProp           "QUOTED-PRINTABLE"
#define VCRDateProp                     "RDATE"
#define VCRegionProp                    "R"
#define VCRelatedToProp                 "RELATED-TO"
#define VCRepeatCountProp               "REPEATCOUNT"
#define VCResourcesProp                 "RESOURCES"
#define VCRNumProp                      "RNUM"
#define VCRoleProp                      "ROLE"
#define VCRRuleProp                     "RRULE"
#define VCRSVPProp                      "RSVP"
#define VCRunTimeProp                   "RUNTIME"
#define VCSequenceProp                  "SEQUENCE"
#define VCSnoozeTimeProp                "SNOOZETIME"
#define VCStartProp                     "START"
#define VCStatusProp                    "STATUS"
#define VCStreetAddressProp             "STREET"
#define VCSubTypeProp                   "SUBTYPE"
#define VCSummaryProp                   "SUMMARY"
#define VCTelephoneProp                 "TEL"
#define VCTIFFProp                      "TIFF"
#define VCTimeZoneProp                  "TZ"
#define VCTitleProp                     "TITLE"
#define VCTLXProp                       "TLX"
#define VCTodoProp                      "VTODO"
#define VCTranspProp                    "TRANSP"
#define VCUniqueStringProp              "UID"
#define VCURLProp                       "URL"
#define VCURLValueProp                  "URLVAL"
#define VCValueProp                     "VALUE"
#define VCVersionProp                   "VERSION"
#define VCVideoProp                     "VIDEO"
#define VCVoiceProp                     "VOICE"
#define VCWAVEProp                      "WAVE"
#define VCWMFProp                       "WMF"
#define VCWorkProp                      "WORK"
#define VCX400Prop                      "X400"
#define VCX509Prop                      "X509"
#define VCXRuleProp                     "XRULE"

/* Extensions */

#define XPilotIdProp                    "X-PILOTID"
#define XPilotStatusProp                "X-PILOTSTAT"
#define XRRuleProp                      "X-RRULE"
#define XDTstartProp                    "X-DTSTART"
#define XDTendProp                      "X-DTEND"

    typedef struct VObject VObject;

    typedef struct VObjectIterator
    {
        VObject *start;
        VObject *next;
    } VObjectIterator;

    LIBICAL_VCAL_EXPORT VObject *newVObject(const char *id);
    LIBICAL_VCAL_EXPORT void deleteVObject(VObject *p);
    LIBICAL_VCAL_EXPORT char *dupStr(const char *s, size_t size);
    LIBICAL_VCAL_EXPORT void deleteStr(const char *p);
    LIBICAL_VCAL_EXPORT void unUseStr(const char *s);

    LIBICAL_VCAL_EXPORT void setVObjectName(VObject *o, const char *id);
    LIBICAL_VCAL_EXPORT void setVObjectStringZValue(VObject *o, const char *s);
    LIBICAL_VCAL_EXPORT void setVObjectStringZValue_(VObject *o, const char *s);
    LIBICAL_VCAL_EXPORT void setVObjectUStringZValue(VObject *o, const wchar_t *s);
    LIBICAL_VCAL_EXPORT void setVObjectUStringZValue_(VObject *o, const wchar_t *s);
    LIBICAL_VCAL_EXPORT void setVObjectIntegerValue(VObject *o, unsigned int i);
    LIBICAL_VCAL_EXPORT void setVObjectLongValue(VObject *o, unsigned long l);
    LIBICAL_VCAL_EXPORT void setVObjectAnyValue(VObject *o, void *t);
    LIBICAL_VCAL_EXPORT VObject *setValueWithSize(VObject *prop, void *val, unsigned int size);
    LIBICAL_VCAL_EXPORT VObject *setValueWithSize_(VObject *prop, void *val, unsigned int size);

    LIBICAL_VCAL_EXPORT const char *vObjectName(VObject *o);
    LIBICAL_VCAL_EXPORT const char *vObjectStringZValue(VObject *o);
    LIBICAL_VCAL_EXPORT const wchar_t *vObjectUStringZValue(VObject *o);
    LIBICAL_VCAL_EXPORT unsigned int vObjectIntegerValue(VObject *o);
    LIBICAL_VCAL_EXPORT unsigned long vObjectLongValue(VObject *o);
    LIBICAL_VCAL_EXPORT void *vObjectAnyValue(VObject *o);
    LIBICAL_VCAL_EXPORT VObject *vObjectVObjectValue(VObject *o);
    LIBICAL_VCAL_EXPORT void setVObjectVObjectValue(VObject *o, VObject *p);

    LIBICAL_VCAL_EXPORT VObject *addVObjectProp(VObject *o, VObject *p);
    LIBICAL_VCAL_EXPORT VObject *addProp(VObject *o, const char *id);
    LIBICAL_VCAL_EXPORT VObject *addPropValue(VObject *o, const char *p, const char *v);
    LIBICAL_VCAL_EXPORT VObject *addPropSizedValue_(VObject *o, const char *p, const char *v,
                                                    unsigned int size);
    LIBICAL_VCAL_EXPORT VObject *addPropSizedValue(VObject *o, const char *p, const char *v,
                                                   unsigned int size);
    LIBICAL_VCAL_EXPORT VObject *addGroup(VObject *o, const char *g);
    LIBICAL_VCAL_EXPORT void addList(VObject ** o, VObject *p);

    LIBICAL_VCAL_EXPORT VObject *isAPropertyOf(VObject *o, const char *id);

    LIBICAL_VCAL_EXPORT VObject *nextVObjectInList(VObject *o);
    LIBICAL_VCAL_EXPORT void initPropIterator(VObjectIterator * i, VObject *o);
    LIBICAL_VCAL_EXPORT int moreIteration(VObjectIterator * i);
    LIBICAL_VCAL_EXPORT VObject *nextVObject(VObjectIterator * i);

    LIBICAL_VCAL_EXPORT char *writeMemVObject(char *s, int *len, VObject *o);
    LIBICAL_VCAL_EXPORT char *writeMemVObjects(char *s, int *len, VObject *list);

    LIBICAL_VCAL_EXPORT const char *lookupStr(const char *s);
    LIBICAL_VCAL_EXPORT void cleanStrTbl();

    LIBICAL_VCAL_EXPORT void cleanVObject(VObject *o);
    LIBICAL_VCAL_EXPORT void cleanVObjects(VObject *list);

    LIBICAL_VCAL_EXPORT const char *lookupProp(const char *str);
    LIBICAL_VCAL_EXPORT const char *lookupProp_(const char *str);

    LIBICAL_VCAL_EXPORT wchar_t *fakeUnicode(const char *ps, size_t *bytes);
    LIBICAL_VCAL_EXPORT int uStrLen(const wchar_t *u);
    LIBICAL_VCAL_EXPORT char *fakeCString(const wchar_t *u);

    LIBICAL_VCAL_EXPORT void printVObjectToFile(char *fname, VObject *o);
    LIBICAL_VCAL_EXPORT void printVObjectsToFile(char *fname, VObject *list);
    LIBICAL_VCAL_EXPORT void writeVObjectToFile(char *fname, VObject *o);
    LIBICAL_VCAL_EXPORT void writeVObjectsToFile(char *fname, VObject *list);

    LIBICAL_VCAL_EXPORT int vObjectValueType(VObject *o);

/* return type of vObjectValueType: */
#define VCVT_NOVALUE    0
    /* if the VObject has no value associated with it. */
#define VCVT_STRINGZ    1
    /* if the VObject has value set by setVObjectStringZValue. */
#define VCVT_USTRINGZ   2
    /* if the VObject has value set by setVObjectUStringZValue. */
#define VCVT_UINT               3
    /* if the VObject has value set by setVObjectIntegerValue. */
#define VCVT_ULONG              4
    /* if the VObject has value set by setVObjectLongValue. */
#define VCVT_RAW                5
    /* if the VObject has value set by setVObjectAnyValue. */
#define VCVT_VOBJECT    6
    /* if the VObject has value set by setVObjectVObjectValue. */

    extern const char **fieldedProp;

/* NOTE regarding printVObject and writeVObject

The functions below are not exported from the DLL because they
take a FILE* as a parameter, which cannot be passed across a DLL
interface (at least that is my experience). Instead you can use
their companion functions which take file names or pointers
to memory. However, if you are linking this code into
your build directly then you may find them a more convenient API
and you can go ahead and use them. If you try to use them with
the DLL LIB you will get a link error.
*/
    extern void printVObject(FILE *fp, VObject *o);
    extern void writeVObject(FILE *fp, VObject *o);

#if defined(__CPLUSPLUS__) || defined(__cplusplus)
}

#endif

#endif /* VOBJECT_H */
