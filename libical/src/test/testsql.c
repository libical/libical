#include "ical.h"

#include "icalgauge.h"
#include "icalgaugeimpl.h"
#include <stdio.h>



int main()
{

    icalgauge *g;

    g = icalgauge_new_from_sql("SELECT DTSTART,DTEND FROM VEVENT,VTODO WHERE SUMMARY = 'Bongoa' AND SEQUENCE < 5");

    icalgauge_dump(g);

    return 0;
}
