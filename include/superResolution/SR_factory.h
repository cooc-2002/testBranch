/*
 * SRfactory.h
 *
 *  Created on: 2017. 1. 11.
 *      Author: ysh4568
 */
#include "SR.h"

#ifndef SRFACTORY_H_
#define SRFACTORY_H_

class SR_factory {
public:
	SR* construct(int state);
};

#endif /* SRFACTORY_H_ */
