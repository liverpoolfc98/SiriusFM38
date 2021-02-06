#include "IRProviderConst.h"

#include <stdio.h>
#include <stdexcept>

namespace siriusFM {

IRProvider<IRModeE::Const>::IRProvider(const char* a_file) {
    FILE* fp;
    if (!(fp = fopen(a_file, "r"))) {
        throw std::runtime_error("no file");
    }

    memset(m_IRs, 0, sizeof(m_IRs));

    char* line = nullptr;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1) {
        line[3] = '\0';
        m_IRs[static_cast<size_t>(Str2CcyE(line))] = atof(line + 4);
    }

    free(line);
    fclose(fp);
}

}
