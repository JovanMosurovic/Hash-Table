//
// Created by mosur on 21-Dec-23.
//

#ifndef DOMACI_3_DOUBLEHASHING_H
#define DOMACI_3_DOUBLEHASHING_H

#include "AddressFunction.h"

class DoubleHashing: public AddressFunction {
    int p;
    int q;

public:
    DoubleHashing(int p, int q);

    unsigned int getAddress(int key, unsigned int address, int attempt) override;

};


#endif //DOMACI_3_DOUBLEHASHING_H
