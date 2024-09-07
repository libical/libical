/*======================================================================
 *
 * SPDX-FileCopyrightText: 2024 Contributors to the libical project <git@github.com:libical/libical>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * Based on the original code by Vladyslav in
 * https://github.com/libical/libical/issues/692
 *
 * ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(NDEBUG)
#undef NDEBUG
#endif

#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

#include "libical/ical.h"
#include "vcc.h"
#include "vobject.h"

int main(int argc, char *argv[])
{
    const char *fname;

    if (argc != 2) {
        fname = TEST_DATADIR "/leaktest.vcf";
    } else {
        fname = argv[1];
    }

    std::string content;
    {
        std::ifstream myfile(fname);
        std::stringstream ss;
        ss << myfile.rdbuf();
        content = ss.str();
    }

    using vcardptr = std::unique_ptr<VObject, decltype(&cleanVObject)>;
    vcardptr ptr(Parse_MIME((char *)content.c_str(), static_cast<unsigned long>(content.size())), cleanVObject);

    return 0;
}
