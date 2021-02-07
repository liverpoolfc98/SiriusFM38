#include "IRProviderConst.h"

#include <stdio.h>
#include <stdexcept>

#define BUFFER_LENGTH 128

namespace siriusFM {

IRProvider<IRModeE::Const>::IRProvider(const char* a_file) {
    if (!a_file) {
        throw std::invalid_argument("empty filename");
    }

    FILE* fp;
    if (!(fp = fopen(a_file, "r"))) {
        throw std::runtime_error("no file");
    }

    memset(m_IRs, 0, sizeof(m_IRs));

    char buffer[BUFFER_LENGTH];

    while (fgets(buffer, BUFFER_LENGTH, fp)) {
        buffer[3] = '\0';
        m_IRs[static_cast<size_t>(Str2CcyE(buffer))] = atof(buffer + 4);
    }

    fclose(fp);
}

}
