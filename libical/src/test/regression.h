
/* regression-component.c */
icalcomponent* create_new_component();
icalcomponent* create_new_component_with_va_args();
void test_span();

/* regression-utils.c */
const char* ical_timet_string(const time_t t);
const char* ictt_as_string(struct icaltimetype t);
