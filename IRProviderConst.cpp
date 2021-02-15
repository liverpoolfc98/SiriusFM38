#include "IRProviderConst.h"

#include <stdio.h>
#include <stdexcept>

namespace siriusFM {

constexpr size_t BUFFER_LENGTH = 128;

IRProvider<IRModeE::Const>::IRProvider(const char* a_file) {
    memset(m_IRs, 0, sizeof(m_IRs));

    if (!a_file) {
        return;
    }

    FILE* fp;
    if (!(fp = fopen(a_file, "r"))) {
        return;
    }

    char buffer[BUFFER_LENGTH];

    while (fgets(buffer, BUFFER_LENGTH, fp)) {
        if (!buffer || buffer == "\n" || buffer[0] == '/') {
            continue;
        }
        buffer[3] = '\0';
        m_IRs[static_cast<size_t>(Str2CcyE(buffer))] = atof(buffer + 4);
    }

    fclose(fp);
}

}
