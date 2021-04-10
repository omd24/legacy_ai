#pragma once

#pragma once
inline float
sphere (float x [], size_t num_var, int * nfe) {
    float result = 0;
    for (unsigned i = 0; i < num_var; i++) {
        result += x[i] * x[i];
    }
    (*nfe)++;
    return result;
}


