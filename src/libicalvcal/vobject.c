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
 * src: vobject.c
 * doc: vobject and APIs to construct vobject, APIs pretty print
 * vobject, and convert a vobject into its textual representation.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vobject.h"

#define NAME_OF(o)                      o->id
#define VALUE_TYPE(o)                   o->valType
#define STRINGZ_VALUE_OF(o)             o->val.strs
#define USTRINGZ_VALUE_OF(o)            o->val.ustrs
#define INTEGER_VALUE_OF(o)             o->val.i
#define LONG_VALUE_OF(o)                o->val.l
#define ANY_VALUE_OF(o)                 o->val.any
#define VOBJECT_VALUE_OF(o)             o->val.vobj

typedef union ValueItem {
    const char *strs;
    const wchar_t *ustrs;
    unsigned int i;
    unsigned long l;
    void *any;
    VObject *vobj;
    } ValueItem;

struct VObject {
    VObject *next;
    const char *id;
    VObject *prop;
    unsigned short valType;
    ValueItem val;
    };

typedef struct StrItem StrItem;

struct StrItem {
    StrItem *next;
    const char *s;
    unsigned int refCnt;
    };

const char** fieldedProp;



/*----------------------------------------------------------------------
   The following functions involve with memory allocation:
        newVObject
        deleteVObject
        dupStr
        deleteStr
        newStrItem
        deleteStrItem
   ----------------------------------------------------------------------*/

static VObject* newVObject_(const char *id)
{
    VObject *p = (VObject*)malloc(sizeof(VObject));
    p->next = 0;
    p->id = id;
    p->prop = 0;
    VALUE_TYPE(p) = 0;
    ANY_VALUE_OF(p) = 0;
    return p;
}

VObject* newVObject(const char *id)
{
    return newVObject_(lookupStr(id));
}

void deleteVObject(VObject *p)
{
    if (!p)
        return;
    if (p->id)
        unUseStr(p->id);
    free(p);
}

char* dupStr(const char *s, size_t size)
{
    char *t;
    if  (size == 0) {
        size = strlen(s);
        }
    t = (char*)malloc(size+1);
    if (t) {
        memcpy(t,s,size);
        t[size] = 0;
        return t;
        }
    else {
        return (char*)0;
        }
}

void deleteStr(const char *p)
{
    if (p)
        free((void*)p);
}


static StrItem* newStrItem(const char *s, StrItem *next)
{
    StrItem *p = (StrItem*)malloc(sizeof(StrItem));
    p->next = next;
    p->s = s;
    p->refCnt = 1;
    return p;
}

static void deleteStrItem(StrItem *p)
{
    if (p)
        free((void*)p);
}


/*----------------------------------------------------------------------
  The following function provide accesses to VObject's value.
  ----------------------------------------------------------------------*/

const char* vObjectName(VObject *o)
{
    return NAME_OF(o);
}

void setVObjectName(VObject *o, const char* id)
{
    NAME_OF(o) = id;
}

const char* vObjectStringZValue(VObject *o)
{
    return STRINGZ_VALUE_OF(o);
}

void setVObjectStringZValue(VObject *o, const char *s)
{
    STRINGZ_VALUE_OF(o) = dupStr(s,0);
    VALUE_TYPE(o) = VCVT_STRINGZ;
}

void setVObjectStringZValue_(VObject *o, const char *s)
{
    STRINGZ_VALUE_OF(o) = s;
    VALUE_TYPE(o) = VCVT_STRINGZ;
}

const wchar_t* vObjectUStringZValue(VObject *o)
{
    return USTRINGZ_VALUE_OF(o);
}

void setVObjectUStringZValue(VObject *o, const wchar_t *s)
{
    size_t size = (size_t)((uStrLen(s)+1)*sizeof(wchar_t));
    USTRINGZ_VALUE_OF(o) = (wchar_t*) dupStr((char*)s,size);
    VALUE_TYPE(o) = VCVT_USTRINGZ;
}

void setVObjectUStringZValue_(VObject *o, const wchar_t *s)
{
    USTRINGZ_VALUE_OF(o) = s;
    VALUE_TYPE(o) = VCVT_USTRINGZ;
}

unsigned int vObjectIntegerValue(VObject *o)
{
    return INTEGER_VALUE_OF(o);
}

void setVObjectIntegerValue(VObject *o, unsigned int i)
{
    INTEGER_VALUE_OF(o) = i;
    VALUE_TYPE(o) = VCVT_UINT;
}

unsigned long vObjectLongValue(VObject *o)
{
    return LONG_VALUE_OF(o);
}

void setVObjectLongValue(VObject *o, unsigned long l)
{
    LONG_VALUE_OF(o) = l;
    VALUE_TYPE(o) = VCVT_ULONG;
}

void* vObjectAnyValue(VObject *o)
{
    return ANY_VALUE_OF(o);
}

void setVObjectAnyValue(VObject *o, void *t)
{
    ANY_VALUE_OF(o) = t;
    VALUE_TYPE(o) = VCVT_RAW;
}

VObject* vObjectVObjectValue(VObject *o)
{
    return VOBJECT_VALUE_OF(o);
}

void setVObjectVObjectValue(VObject *o, VObject *p)
{
    VOBJECT_VALUE_OF(o) = p;
    VALUE_TYPE(o) = VCVT_VOBJECT;
}

int vObjectValueType(VObject *o)
{
    return (int)VALUE_TYPE(o);
}


/*----------------------------------------------------------------------
  The following functions can be used to build VObject.
  ----------------------------------------------------------------------*/

VObject* addVObjectProp(VObject *o, VObject *p)
{
    /* circular link list pointed to tail */
    /*
    o {next,id,prop,val}
                V
        pn {next,id,prop,val}
             V
            ...
        p1 {next,id,prop,val}
             V
             pn
    -->
    o {next,id,prop,val}
                V
        pn {next,id,prop,val}
             V
        p {next,id,prop,val}
            ...
        p1 {next,id,prop,val}
             V
             pn
    */

    VObject *tail = o->prop;
    if (tail) {
        p->next = tail->next;
        o->prop = tail->next = p;
        }
    else {
        o->prop = p->next = p;
        }
    return p;
}

VObject* addProp(VObject *o, const char *id)
{
    return addVObjectProp(o,newVObject(id));
}

static VObject* addProp_(VObject *o, const char *id)
{
    return addVObjectProp(o,newVObject_(id));
}

void addList(VObject **o, VObject *p)
{
    p->next = 0;
    if (*o == 0) {
        *o = p;
        }
    else {
        VObject *t = *o;
        while (t->next) {
           t = t->next;
           }
        t->next = p;
        }
}

VObject* nextVObjectInList(VObject *o)
{
    return o->next;
}

VObject* setValueWithSize_(VObject *prop, void *val, unsigned int size)
{
    VObject *sizeProp;
    setVObjectAnyValue(prop, val);
    sizeProp = addProp(prop,VCDataSizeProp);
    setVObjectLongValue(sizeProp, size);
    return prop;
}

VObject* setValueWithSize(VObject *prop, void *val, unsigned int size)
{
    void *p = dupStr((const char *)val,size);
    return setValueWithSize_(prop,p,p?size:0);
}

void initPropIterator(VObjectIterator *i, VObject *o)
{
    i->start = o->prop;
    i->next = 0;
}

void initVObjectIterator(VObjectIterator *i, VObject *o)
{
    i->start = o->next;
    i->next = 0;
}

int moreIteration(VObjectIterator *i)
{
    return (i->start && (i->next==0 || i->next!=i->start));
}

VObject* nextVObject(VObjectIterator *i)
{
    if (i->start && i->next != i->start) {
        if (i->next == 0) {
            i->next = i->start->next;
            return i->next;
            }
        else {
            i->next = i->next->next;
            return i->next;
            }
        }
    else return (VObject*)0;
}

VObject* isAPropertyOf(VObject *o, const char *id)
{
    VObjectIterator i;
    initPropIterator(&i,o);
    while (moreIteration(&i)) {
        VObject *each = nextVObject(&i);
        if (!strcasecmp(id,each->id))
            return each;
        }
    return (VObject*)0;
}

VObject* addGroup(VObject *o, const char *g)
{
    /*
        a.b.c
        -->
        prop(c)
            prop(VCGrouping=b)
                prop(VCGrouping=a)
     */
    char *dot = strrchr(g,'.');
    if (dot) {
        VObject *p, *t;
        char *gs, *n = dot+1;
        gs = dupStr(g,0);       /* so we can write to it. */
        /* used to be
        * t = p = addProp_(o,lookupProp_(n));
        */
        t = p = addProp_(o,lookupProp(n));
        dot = strrchr(gs,'.');
        if (dot) {
            *dot = 0;
            do {
                dot = strrchr(gs,'.');
                if (dot) {
                    n = dot+1;
                    *dot=0;
                }
                else
                    n = gs;
                /* property(VCGroupingProp=n);
                 *  and the value may have VCGrouping property
                 */
                t = addProp(t,VCGroupingProp);
                setVObjectStringZValue(t,lookupProp_(n));
            } while (n != gs);
        } else {
            t = addProp(t,VCGroupingProp);
            setVObjectStringZValue(t,lookupProp_(n));
        }
        deleteStr(gs);
        return p;
        }
    else
        return addProp_(o,lookupProp(g));
}

VObject* addPropValue(VObject *o, const char *p, const char *v)
{
    VObject *prop;
    prop = addProp(o,p);
    setVObjectUStringZValue_(prop, fakeUnicode(v,0));
    return prop;
}

VObject* addPropSizedValue_(VObject *o, const char *p, const char *v,
        unsigned int size)
{
    VObject *prop;
    prop = addProp(o,p);
    (void)setValueWithSize_(prop, (void*)v, size);
    return prop;
}

VObject* addPropSizedValue(VObject *o, const char *p, const char *v,
        unsigned int size)
{
    return addPropSizedValue_(o,p,dupStr(v,size),size);
}



/*----------------------------------------------------------------------
  The following pretty print a VObject
  ----------------------------------------------------------------------*/

static void printVObject_(FILE *fp, VObject *o, int level);

static void indent(FILE *fp, int level)
{
    int i;
    for (i=0;i<level*4;i++) {
        fputc(' ', fp);
        }
}

static void printValue(FILE *fp, VObject *o, int level)
{
    switch (VALUE_TYPE(o)) {
        case VCVT_USTRINGZ: {
            char c;
            char *t,*s;
            s = t = fakeCString(USTRINGZ_VALUE_OF(o));
            fputc('"',fp);
            while (c=*t,c) {
                fputc(c,fp);
                if (c == '\n') indent(fp,level+2);
                t++;
                }
            fputc('"',fp);
            deleteStr(s);
            break;
            }
        case VCVT_STRINGZ: {
            char c;
            const char *s = STRINGZ_VALUE_OF(o);
            fputc('"',fp);
            while (c=*s,c) {
                fputc(c,fp);
                if (c == '\n') indent(fp,level+2);
                s++;
                }
            fputc('"',fp);
            break;
            }
        case VCVT_UINT:
            fprintf(fp,"%u", INTEGER_VALUE_OF(o)); break;
        case VCVT_ULONG:
            fprintf(fp,"%lu", LONG_VALUE_OF(o)); break;
        case VCVT_RAW:
            fprintf(fp,"[raw data]"); break;
        case VCVT_VOBJECT:
            fprintf(fp,"[vobject]\n");
            printVObject_(fp,VOBJECT_VALUE_OF(o),level+1);
            break;
        case 0:
            fprintf(fp,"[none]"); break;
        default:
            fprintf(fp,"[unknown]"); break;
        }
}

static void printNameValue(FILE *fp,VObject *o, int level)
{
    indent(fp,level);
    if (NAME_OF(o)) {
        fprintf(fp,"%s", NAME_OF(o));
        }
    if (VALUE_TYPE(o)) {
        fputc('=',fp);
        printValue(fp,o, level);
        }
    fprintf(fp,"\n");
}

static void printVObject_(FILE *fp, VObject *o, int level)
    {
    VObjectIterator t;
    if (o == 0) {
        fprintf(fp,"[NULL]\n");
        return;
        }
    printNameValue(fp,o,level);
    initPropIterator(&t,o);
    while (moreIteration(&t)) {
        VObject *eachProp = nextVObject(&t);
        printVObject_(fp,eachProp,level+1);
        }
    }

void printVObject(FILE *fp,VObject *o)
{
    printVObject_(fp,o,0);
}

void printVObjectToFile(char *fname,VObject *o)
{
    FILE *fp = fopen(fname,"w");
    if (fp) {
        printVObject(fp,o);
        fclose(fp);
        }
}

void printVObjectsToFile(char *fname,VObject *list)
{
    FILE *fp = fopen(fname,"w");
    if (fp) {
        while (list) {
            printVObject(fp,list);
            list = nextVObjectInList(list);
            }
        fclose(fp);
        }
}

void cleanVObject(VObject *o)
{
    if (o == 0) return;
    if (o->prop) {
        /* destroy time: cannot use the iterator here.
           Have to break the cycle in the circular link
           list and turns it into regular NULL-terminated
           list -- since at some point of destruction,
           the reference entry for the iterator to work
           will not longer be valid.
           */
        VObject *p;
        p = o->prop->next;
        o->prop->next = 0;
        do {
           VObject *t = p->next;
           cleanVObject(p);
           p = t;
           } while (p);
        }
    switch (VALUE_TYPE(o)) {
        case VCVT_USTRINGZ:
        case VCVT_STRINGZ:
        case VCVT_RAW:
                /* assume they are all allocated by malloc. */
            free((char*)STRINGZ_VALUE_OF(o));
            break;
        case VCVT_VOBJECT:
            cleanVObject(VOBJECT_VALUE_OF(o));
            break;
        }
    deleteVObject(o);
}

void cleanVObjects(VObject *list)
{
    while (list) {
        VObject *t = list;
        list = nextVObjectInList(list);
        cleanVObject(t);
        }
}

/*----------------------------------------------------------------------
  The following is a String Table Facilities.
  ----------------------------------------------------------------------*/

#define STRTBLSIZE 255

static StrItem *strTbl[STRTBLSIZE];

static unsigned int hashStr(const char *s)
{
    unsigned int h = 0;
    int i;
    for (i=0;s[i];i++) {
        h += s[i]*i;
        }
    return h % STRTBLSIZE;
}

const char* lookupStr(const char *s)
{
    char *newS;
    StrItem *t;
    unsigned int h = hashStr(s);
    if ((t = strTbl[h]) != 0) {
        do {
            if (strcasecmp(t->s,s) == 0) {
                t->refCnt++;
                return t->s;
                }
            t = t->next;
            } while (t);
        }
    newS = dupStr(s,0);
    strTbl[h] = newStrItem(newS,strTbl[h]);
    return newS;
}

void unUseStr(const char *s)
{
    StrItem *cur, *prev;

    unsigned int h = hashStr(s);
    cur = strTbl[h];
    prev = cur;
    while (cur != 0) {
        if (strcasecmp(cur->s,s) == 0) {
            cur->refCnt--;
            /* if that was the last reference to this string, kill it. */
            if (cur->refCnt == 0) {
                if (cur == strTbl[h]) {
                    strTbl[h] = cur->next;
                    deleteStr(prev->s);
                    deleteStrItem(prev);
                } else {
                    prev->next = cur->next;
                    deleteStr(cur->s);
                    deleteStrItem(cur);
                }
                return;
            }
        }
        prev = cur;
        cur = cur->next;
    }
}

void cleanStrTbl()
{
    int i;
    for (i=0; i<STRTBLSIZE;i++) {
        StrItem *t = strTbl[i];
        while (t) {
            StrItem *p;
            deleteStr(t->s);
            p = t;
            t = t->next;
            deleteStrItem(p);
            }
        strTbl[i] = 0;
        }
}


struct PreDefProp {
    const char *name;
    const char *alias;
    const char** fields;
    unsigned int flags;
    };

/* flags in PreDefProp */
#define PD_BEGIN        0x1
#define PD_INTERNAL     0x2

static const char *adrFields[] = {
    VCPostalBoxProp,
    VCExtAddressProp,
    VCStreetAddressProp,
    VCCityProp,
    VCRegionProp,
    VCPostalCodeProp,
    VCCountryNameProp,
    0
};

static const char *nameFields[] = {
    VCFamilyNameProp,
    VCGivenNameProp,
    VCAdditionalNamesProp,
    VCNamePrefixesProp,
    VCNameSuffixesProp,
    NULL
    };

static const char *orgFields[] = {
    VCOrgNameProp,
    VCOrgUnitProp,
    VCOrgUnit2Prop,
    VCOrgUnit3Prop,
    VCOrgUnit4Prop,
    NULL
    };

static const char *AAlarmFields[] = {
    VCRunTimeProp,
    VCSnoozeTimeProp,
    VCRepeatCountProp,
    VCAudioContentProp,
    0
    };

/* ExDate -- has unnamed fields */
/* RDate -- has unnamed fields */

static const char *DAlarmFields[] = {
    VCRunTimeProp,
    VCSnoozeTimeProp,
    VCRepeatCountProp,
    VCDisplayStringProp,
    0
    };

static const char *MAlarmFields[] = {
    VCRunTimeProp,
    VCSnoozeTimeProp,
    VCRepeatCountProp,
    VCEmailAddressProp,
    VCNoteProp,
    0
    };

static const char *PAlarmFields[] = {
    VCRunTimeProp,
    VCSnoozeTimeProp,
    VCRepeatCountProp,
    VCProcedureNameProp,
    0
    };

static const struct PreDefProp propNames[] = {
    { VC7bitProp, 0, 0, 0 },
    { VC8bitProp, 0, 0, 0 },
    { VCAAlarmProp, 0, AAlarmFields, 0 },
    { VCAdditionalNamesProp, 0, 0, 0 },
    { VCAdrProp, 0, adrFields, 0 },
    { VCAgentProp, 0, 0, 0 },
    { VCAIFFProp, 0, 0, 0 },
    { VCAOLProp, 0, 0, 0 },
    { VCAppleLinkProp, 0, 0, 0 },
    { VCAttachProp, 0, 0, 0 },
    { VCAttendeeProp, 0, 0, 0 },
    { VCATTMailProp, 0, 0, 0 },
    { VCAudioContentProp, 0, 0, 0 },
    { VCAVIProp, 0, 0, 0 },
    { VCBase64Prop, 0, 0, 0 },
    { VCBBSProp, 0, 0, 0 },
    { VCBirthDateProp, 0, 0, 0 },
    { VCBMPProp, 0, 0, 0 },
    { VCBodyProp, 0, 0, 0 },
    { VCBusinessRoleProp, 0, 0, 0 },
    { VCCalProp, 0, 0, PD_BEGIN },
    { VCCaptionProp, 0, 0, 0 },
    { VCCardProp, 0, 0, PD_BEGIN },
    { VCCarProp, 0, 0, 0 },
    { VCCategoriesProp, 0, 0, 0 },
    { VCCellularProp, 0, 0, 0 },
    { VCCGMProp, 0, 0, 0 },
    { VCCharSetProp, 0, 0, 0 },
    { VCCIDProp, VCContentIDProp, 0, 0 },
    { VCCISProp, 0, 0, 0 },
    { VCCityProp, 0, 0, 0 },
    { VCClassProp, 0, 0, 0 },
    { VCCommentProp, 0, 0, 0 },
    { VCCompletedProp, 0, 0, 0 },
    { VCContentIDProp, 0, 0, 0 },
    { VCCountryNameProp, 0, 0, 0 },
    { VCDAlarmProp, 0, DAlarmFields, 0 },
    { VCDataSizeProp, 0, 0, PD_INTERNAL },
    { VCDayLightProp, 0, 0, 0 },
    { VCDCreatedProp, 0, 0, 0 },
    { VCDeliveryLabelProp, 0, 0, 0 },
    { VCDescriptionProp, 0, 0, 0 },
    { VCDIBProp, 0, 0, 0 },
    { VCDisplayStringProp, 0, 0, 0 },
    { VCDomesticProp, 0, 0, 0 },
    { VCDTendProp, 0, 0, 0 },
    { VCDTstartProp, 0, 0, 0 },
    { VCDueProp, 0, 0, 0 },
    { VCEmailAddressProp, 0, 0, 0 },
    { VCEncodingProp, 0, 0, 0 },
    { VCEndProp, 0, 0, 0 },
    { VCEventProp, 0, 0, PD_BEGIN },
    { VCEWorldProp, 0, 0, 0 },
    { VCExNumProp, 0, 0, 0 },
    { VCExpDateProp, 0, 0, 0 },
    { VCExpectProp, 0, 0, 0 },
    { VCExtAddressProp, 0, 0, 0 },
    { VCFamilyNameProp, 0, 0, 0 },
    { VCFaxProp, 0, 0, 0 },
    { VCFullNameProp, 0, 0, 0 },
    { VCGeoLocationProp, 0, 0, 0 },
    { VCGeoProp, 0, 0, 0 },
    { VCGIFProp, 0, 0, 0 },
    { VCGivenNameProp, 0, 0, 0 },
    { VCGroupingProp, 0, 0, 0 },
    { VCHomeProp, 0, 0, 0 },
    { VCIBMMailProp, 0, 0, 0 },
    { VCInlineProp, 0, 0, 0 },
    { VCInternationalProp, 0, 0, 0 },
    { VCInternetProp, 0, 0, 0 },
    { VCISDNProp, 0, 0, 0 },
    { VCJPEGProp, 0, 0, 0 },
    { VCLanguageProp, 0, 0, 0 },
    { VCLastModifiedProp, 0, 0, 0 },
    { VCLastRevisedProp, 0, 0, 0 },
    { VCLocationProp, 0, 0, 0 },
    { VCLogoProp, 0, 0, 0 },
    { VCMailerProp, 0, 0, 0 },
    { VCMAlarmProp, 0, MAlarmFields, 0 },
    { VCMCIMailProp, 0, 0, 0 },
    { VCMessageProp, 0, 0, 0 },
    { VCMETProp, 0, 0, 0 },
    { VCModemProp, 0, 0, 0 },
    { VCMPEG2Prop, 0, 0, 0 },
    { VCMPEGProp, 0, 0, 0 },
    { VCMSNProp, 0, 0, 0 },
    { VCNamePrefixesProp, 0, 0, 0 },
    { VCNameProp, 0, nameFields, 0 },
    { VCNameSuffixesProp, 0, 0, 0 },
    { VCNoteProp, 0, 0, 0 },
    { VCOrgNameProp, 0, 0, 0 },
    { VCOrgProp, 0, orgFields, 0 },
    { VCOrgUnit2Prop, 0, 0, 0 },
    { VCOrgUnit3Prop, 0, 0, 0 },
    { VCOrgUnit4Prop, 0, 0, 0 },
    { VCOrgUnitProp, 0, 0, 0 },
    { VCPagerProp, 0, 0, 0 },
    { VCPAlarmProp, 0, PAlarmFields, 0 },
    { VCParcelProp, 0, 0, 0 },
    { VCPartProp, 0, 0, 0 },
    { VCPCMProp, 0, 0, 0 },
    { VCPDFProp, 0, 0, 0 },
    { VCPGPProp, 0, 0, 0 },
    { VCPhotoProp, 0, 0, 0 },
    { VCPICTProp, 0, 0, 0 },
    { VCPMBProp, 0, 0, 0 },
    { VCPostalBoxProp, 0, 0, 0 },
    { VCPostalCodeProp, 0, 0, 0 },
    { VCPostalProp, 0, 0, 0 },
    { VCPowerShareProp, 0, 0, 0 },
    { VCPreferredProp, 0, 0, 0 },
    { VCPriorityProp, 0, 0, 0 },
    { VCProcedureNameProp, 0, 0, 0 },
    { VCProdIdProp, 0, 0, 0 },
    { VCProdigyProp, 0, 0, 0 },
    { VCPronunciationProp, 0, 0, 0 },
    { VCPSProp, 0, 0, 0 },
    { VCPublicKeyProp, 0, 0, 0 },
    { VCQPProp, VCQuotedPrintableProp, 0, 0 },
    { VCQuickTimeProp, 0, 0, 0 },
    { VCQuotedPrintableProp, 0, 0, 0 },
    { VCRDateProp, 0, 0, 0 },
    { VCRegionProp, 0, 0, 0 },
    { VCRelatedToProp, 0, 0, 0 },
    { VCRepeatCountProp, 0, 0, 0 },
    { VCResourcesProp, 0, 0, 0 },
    { VCRNumProp, 0, 0, 0 },
    { VCRoleProp, 0, 0, 0 },
    { VCRRuleProp, 0, 0, 0 },
    { VCRSVPProp, 0, 0, 0 },
    { VCRunTimeProp, 0, 0, 0 },
    { VCSequenceProp, 0, 0, 0 },
    { VCSnoozeTimeProp, 0, 0, 0 },
    { VCStartProp, 0, 0, 0 },
    { VCStatusProp, 0, 0, 0 },
    { VCStreetAddressProp, 0, 0, 0 },
    { VCSubTypeProp, 0, 0, 0 },
    { VCSummaryProp, 0, 0, 0 },
    { VCTelephoneProp, 0, 0, 0 },
    { VCTIFFProp, 0, 0, 0 },
    { VCTimeZoneProp, 0, 0, 0 },
    { VCTitleProp, 0, 0, 0 },
    { VCTLXProp, 0, 0, 0 },
    { VCTodoProp, 0, 0, PD_BEGIN },
    { VCTranspProp, 0, 0, 0 },
    { VCUniqueStringProp, 0, 0, 0 },
    { VCURLProp, 0, 0, 0 },
    { VCURLValueProp, 0, 0, 0 },
    { VCValueProp, 0, 0, 0 },
    { VCVersionProp, 0, 0, 0 },
    { VCVideoProp, 0, 0, 0 },
    { VCVoiceProp, 0, 0, 0 },
    { VCWAVEProp, 0, 0, 0 },
    { VCWMFProp, 0, 0, 0 },
    { VCWorkProp, 0, 0, 0 },
    { VCX400Prop, 0, 0, 0 },
    { VCX509Prop, 0, 0, 0 },
    { VCXRuleProp, 0, 0, 0 },

    /* Extensions */
    { XRRuleProp, 0, 0, 0 },
    { XDTstartProp, 0, 0, 0 },
    { XDTendProp, 0, 0, 0 },

    { 0,0,0,0 }
    };


static const struct PreDefProp* lookupPropInfo(const char* str)
{
    /* brute force for now, could use a hash table here. */
    int i;

    for (i = 0; propNames[i].name; i++)
        if (strcasecmp(str, propNames[i].name) == 0) {
            return &propNames[i];
            }

    return 0;
}


const char* lookupProp_(const char* str)
{
    int i;

    for (i = 0; propNames[i].name; i++)
        if (strcasecmp(str, propNames[i].name) == 0) {
            const char* s;
            s = propNames[i].alias?propNames[i].alias:propNames[i].name;
            return lookupStr(s);
            }
    return lookupStr(str);
}


const char* lookupProp(const char* str)
{
    int i;

    for (i = 0; propNames[i].name; i++)
        if (strcasecmp(str, propNames[i].name) == 0) {
            const char *s;
            fieldedProp = propNames[i].fields;
            s = propNames[i].alias?propNames[i].alias:propNames[i].name;
            return lookupStr(s);
            }
    fieldedProp = 0;
    return lookupStr(str);
}


/*----------------------------------------------------------------------
  APIs to Output text form.
  ----------------------------------------------------------------------*/
#define OFILE_REALLOC_SIZE 256
typedef struct OFile {
    FILE *fp;
    char *s;
    int len;
    int limit;
    int alloc:1;
    unsigned int fail:1;
    } OFile;

#if 0
static void appendsOFile(OFile *fp, const char *s)
{
    int slen;
    if (fp->fail) return;
    slen  = strlen(s);
    if (fp->fp) {
        fwrite(s,1,slen,fp->fp);
        }
    else {
stuff:
        if (fp->len + slen < fp->limit) {
            memcpy(fp->s+fp->len,s,slen);
            fp->len += slen;
            return;
            }
        else if (fp->alloc) {
            fp->limit = fp->limit + OFILE_REALLOC_SIZE;
            if (OFILE_REALLOC_SIZE <= slen) fp->limit += slen;
            fp->s = (char *) realloc(fp->s,(size_t)fp->limit);
            if (fp->s) goto stuff;
            }
        if (fp->alloc)
            free(fp->s);
        fp->s = 0;
        fp->fail = 1;
        }
}

static void appendcOFile(OFile *fp, char c)
{
    if (fp->fail) return;
    if (fp->fp) {
        fputc(c,fp->fp);
        }
    else {
stuff:
        if (fp->len+1 < fp->limit) {
            fp->s[fp->len] = c;
            fp->len++;
            return;
            }
        else if (fp->alloc) {
            fp->limit = fp->limit + OFILE_REALLOC_SIZE;
            fp->s = (char *) realloc(fp->s,fp->limit);
            if (fp->s) goto stuff;
            }
        if (fp->alloc)
            free(fp->s);
        fp->s = 0;
        fp->fail = 1;
        }
}
#else
static void appendcOFile_(OFile *fp, char c)
{
    if (fp->fail) return;
    if (fp->fp) {
        fputc(c,fp->fp);
        }
    else {
stuff:
        if (fp->len+1 < fp->limit) {
            fp->s[fp->len] = c;
            fp->len++;
            return;
            }
        else if (fp->alloc) {
            fp->limit = fp->limit + OFILE_REALLOC_SIZE;
            fp->s = realloc(fp->s,(size_t)fp->limit);
            if (fp->s) goto stuff;
            }
        if (fp->alloc)
            free(fp->s);
        fp->s = 0;
        fp->fail = 1;
        }
}

static void appendcOFile(OFile *fp, char c)
{
    if (c == '\n') {
        /* write out as <CR><LF> */
        appendcOFile_(fp,0xd);
        appendcOFile_(fp,0xa);
        }
    else
        appendcOFile_(fp,c);
}

static void appendsOFile(OFile *fp, const char *s)
{
    size_t i, slen;
    slen  = strlen(s);
    for (i=0; i<slen; i++) {
        appendcOFile(fp,s[i]);
        }
}

#endif

static void initOFile(OFile *fp, FILE *ofp)
{
    fp->fp = ofp;
    fp->s = 0;
    fp->len = 0;
    fp->limit = 0;
    fp->alloc = 0;
    fp->fail = 0;
}

static void initMemOFile(OFile *fp, char *s, int len)
{
    fp->fp = 0;
    fp->s = s;
    fp->len = 0;
    fp->limit = s?len:0;
    fp->alloc = s?0:1;
    fp->fail = 0;
}


static int writeBase64(OFile *fp, unsigned char *s, long len)
{
    long cur = 0;
    int i, numQuads = 0;
    unsigned long trip;
    unsigned char b;
    char quad[5];
#define MAXQUADS 16

    quad[4] = 0;

    while (cur < len) {
            /* collect the triplet of bytes into 'trip' */
        trip = 0;
        for (i = 0; i < 3; i++) {
            b = (cur < len) ? *(s + cur) : 0;
            cur++;
            trip = trip << 8 | b;
            }
        /* fill in 'quad' with the appropriate four characters */
        for (i = 3; i >= 0; i--) {
            b = (unsigned char)(trip & 0x3F);
            trip = trip >> 6;
            if ((3 - i) < (cur - len))
                    quad[i] = '='; /* pad char */
            else if (b < 26) quad[i] = (char)b + 'A';
            else if (b < 52) quad[i] = (char)(b - 26) + 'a';
            else if (b < 62) quad[i] = (char)(b - 52) + '0';
            else if (b == 62) quad[i] = '+';
            else quad[i] = '/';
            }
        /* now output 'quad' with appropriate whitespace and line ending */
        appendsOFile(fp, (numQuads == 0 ? "    " : ""));
        appendsOFile(fp, quad);
        appendsOFile(fp, ((cur >= len)?"\n" :(numQuads==MAXQUADS-1?"\n" : "")));
        numQuads = (numQuads + 1) % MAXQUADS;
        }
    appendcOFile(fp,'\n');

    return 1;
}

static void writeString(OFile *fp, const char *s)
{
    appendsOFile(fp,s);
}

static void writeQPString(OFile *fp, const char *s)
{
    char buf[4];
    int count=0;
    const char *p = s;

    while (*p) {
        /* break up lines biggger than 75 chars */
        if(count >=74){
                count=0;
                appendsOFile(fp,"=\n");
        }

        /* escape any non ASCII characters and '=' as per rfc1521 */
        if (*p<= 0x1f || *p >=0x7f || *p == '=' ) {
                snprintf(buf,sizeof(buf),"=%02X",(unsigned char)*p);
                appendsOFile(fp,buf);
                count+=3;
        } else {
                appendcOFile(fp,*p);
                count++;
        }
        p++;
    }
}



static void writeVObject_(OFile *fp, VObject *o);

static void writeValue(OFile *fp, VObject *o, unsigned long size,int quote)
{
    if (o == 0) return;
    switch (VALUE_TYPE(o)) {
        case VCVT_USTRINGZ: {
            char *s = fakeCString(USTRINGZ_VALUE_OF(o));
            if(quote) writeQPString(fp, s);
            else writeString(fp,s);
            deleteStr(s);
            break;
            }
        case VCVT_STRINGZ: {
            if(quote) writeQPString(fp, STRINGZ_VALUE_OF(o));
            else writeString(fp,STRINGZ_VALUE_OF(o));
            break;
            }
        case VCVT_UINT: {
            char buf[16];
            snprintf(buf,sizeof(buf),"%u", INTEGER_VALUE_OF(o));
            appendsOFile(fp,buf);
            break;
            }
        case VCVT_ULONG: {
            char buf[16];
            snprintf(buf,sizeof(buf),"%lu", LONG_VALUE_OF(o));
            appendsOFile(fp,buf);
            break;
            }
        case VCVT_RAW: {
            appendcOFile(fp,'\n');
            writeBase64(fp,(unsigned char*)(ANY_VALUE_OF(o)),(long)size);
            break;
            }
        case VCVT_VOBJECT:
            appendcOFile(fp,'\n');
            writeVObject_(fp,VOBJECT_VALUE_OF(o));
            break;
        }
}

static void writeAttrValue(OFile *fp, VObject *o)
{
    if (NAME_OF(o)) {
        const struct PreDefProp *pi;
        pi = lookupPropInfo(NAME_OF(o));
        if (pi && ((pi->flags & PD_INTERNAL) != 0)) return;
        appendcOFile(fp,';');
        appendsOFile(fp,NAME_OF(o));
        }
    else
        appendcOFile(fp,';');
    if (VALUE_TYPE(o)) {
        appendcOFile(fp,'=');
        writeValue(fp,o,0,0);
        }
}

static void writeGroup(OFile *fp, VObject *o)
{
    char buf1[256];
    char buf2[256];
    strncpy(buf1,NAME_OF(o),sizeof(buf1)-1);
    buf1[sizeof(buf1)-1]='\0';

    while ((o=isAPropertyOf(o,VCGroupingProp)) != 0) {
        strncpy(buf2,STRINGZ_VALUE_OF(o),sizeof(buf2)-1);
        buf2[sizeof(buf2)-1] = '\0';
        strncat(buf2,".",sizeof(buf2)-strlen(buf2)-1);
        strncat(buf2,buf1,sizeof(buf2)-strlen(buf2)-1);
        strcpy(buf1,buf2);
        }
    appendsOFile(fp,buf1);
}

static int inList(const char **list, const char *s)
{
    if (list == 0) return 0;
    while (*list) {
        if (strcasecmp(*list,s) == 0) return 1;
        list++;
        }
    return 0;
}

static void writeProp(OFile *fp, VObject *o)
{
    int isQuoted=0;
    if (NAME_OF(o)) {
        const struct PreDefProp *pi;
        VObjectIterator t;
        const char **fields_ = 0;
        pi = lookupPropInfo(NAME_OF(o));
        if (pi && ((pi->flags & PD_BEGIN) != 0)) {
            writeVObject_(fp,o);
            return;
            }
        if (isAPropertyOf(o,VCGroupingProp))
            writeGroup(fp,o);
        else
            appendsOFile(fp,NAME_OF(o));
        if (pi) fields_ = pi->fields;
        initPropIterator(&t,o);
        while (moreIteration(&t)) {
            const char *s;
            VObject *eachProp = nextVObject(&t);
            s = NAME_OF(eachProp);
            if (strcasecmp(VCGroupingProp,s) && !inList(fields_,s))
                writeAttrValue(fp,eachProp);
            if (strcasecmp(VCQPProp,s)==0 || strcasecmp(VCQuotedPrintableProp,s)==0)
                 isQuoted=1;
            }
        if (fields_) {
            int i = 0, n = 0;
            const char** fields = fields_;
            /* output prop as fields */
            appendcOFile(fp,':');
            while (*fields) {
                VObject *tl = isAPropertyOf(o,*fields);
                i++;
                if (tl) n = i;
                fields++;
                }
            fields = fields_;
            for (i=0;i<n;i++) {
                writeValue(fp,isAPropertyOf(o,*fields),0,isQuoted);
                fields++;
                if (i<(n-1)) appendcOFile(fp,';');
                }
            }
        }

    if (VALUE_TYPE(o)) {
        unsigned long size = 0;
        VObject *p = isAPropertyOf(o,VCDataSizeProp);
        if (p) size = LONG_VALUE_OF(p);
        appendcOFile(fp,':');
        writeValue(fp,o,size,isQuoted);
        }

    appendcOFile(fp,'\n');
}

static void writeVObject_(OFile *fp, VObject *o)
{
    if (NAME_OF(o)) {
        const struct PreDefProp *pi;
        pi = lookupPropInfo(NAME_OF(o));

        if (pi && ((pi->flags & PD_BEGIN) != 0)) {
            VObjectIterator t;
            const char *begin = NAME_OF(o);
            appendsOFile(fp,"BEGIN:");
            appendsOFile(fp,begin);
            appendcOFile(fp,'\n');
            initPropIterator(&t,o);
            while (moreIteration(&t)) {
                VObject *eachProp = nextVObject(&t);
                writeProp(fp, eachProp);
                }
            appendsOFile(fp,"END:");
            appendsOFile(fp,begin);
            appendsOFile(fp,"\n\n");
            }
        }
}

void writeVObject(FILE *fp, VObject *o)
{
    OFile ofp;
    initOFile(&ofp,fp);
    writeVObject_(&ofp,o);
}

void writeVObjectToFile(char *fname, VObject *o)
{
    FILE *fp = fopen(fname,"w");
    if (fp) {
        writeVObject(fp,o);
        fclose(fp);
        }
}

void writeVObjectsToFile(char *fname, VObject *list)
{
    FILE *fp = fopen(fname,"w");
    if (fp) {
        while (list) {
            writeVObject(fp,list);
            list = nextVObjectInList(list);
            }
        fclose(fp);
        }
}

char* writeMemVObject(char *s, int *len, VObject *o)
{
    OFile ofp;
    initMemOFile(&ofp,s,len?*len:0);
    writeVObject_(&ofp,o);
    if (len) *len = ofp.len;
    appendcOFile(&ofp,0);
    return ofp.s;
}

char* writeMemVObjects(char *s, int *len, VObject *list)
{
    OFile ofp;
    initMemOFile(&ofp,s,len?*len:0);
    while (list) {
        writeVObject_(&ofp,list);
        list = nextVObjectInList(list);
        }
    if (len) *len = ofp.len;
    appendcOFile(&ofp,0);
    return ofp.s;
}

/*----------------------------------------------------------------------
  APIs to do fake Unicode stuff.
  ----------------------------------------------------------------------*/
wchar_t* fakeUnicode(const char *ps, size_t *bytes)
{
    wchar_t *r, *pw;
    size_t len = strlen(ps)+1;

    pw = r = (wchar_t*)malloc(sizeof(wchar_t)*len);
    if (bytes)
        *bytes = len * sizeof(wchar_t);

    while (*ps) {
        if (*ps == '\n')
            *pw = (wchar_t)0x2028;
        else if (*ps == '\r')
            *pw = (wchar_t)0x2029;
        else
            *pw = (wchar_t)(unsigned char)*ps;
        ps++; pw++;
        }
    *pw = (wchar_t)0;

    return r;
}

int uStrLen(const wchar_t *u)
{
    int i;

    if (u == NULL)
        return 0;

    i = 0;
    while (*u != (wchar_t)0) { u++; i++; }
    return i;
}

char* fakeCString(const wchar_t *u)
{
    char *s, *t;
    size_t len;

    if (u == NULL)
        return NULL;

    len = (size_t)(uStrLen(u) + 1);
    t = s = (char*)malloc(len);
    while (*u) {
        if (*u == (wchar_t)0x2028)
            *t = '\n';
        else if (*u == (wchar_t)0x2029)
            *t = '\r';
        else
            *t = (char)*u;
        u++; t++;
        }
    *t = 0;
    return s;
}

/* end of source file vobject.c */
