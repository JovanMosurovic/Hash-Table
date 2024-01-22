//
// Created by mosur on 21-Dec-23.
//
#include <iostream>
#include "DoubleHashing.h"

unsigned int DoubleHashing::getAddress(int key, unsigned int address, int attempt) {
    return address + attempt * (q + (key % p));
}

DoubleHashing::DoubleHashing(int p, int q) : p(p), q(q) {}
