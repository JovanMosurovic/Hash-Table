//
// Created by mosur on 21-Dec-23.
//

#ifndef DOMACI_3_ADDRESSFUNCTION_H
#define DOMACI_3_ADDRESSFUNCTION_H


class AddressFunction {
    virtual unsigned int getAddress(int key, unsigned int address, int attempt) = 0;
};


#endif //DOMACI_3_ADDRESSFUNCTION_H
