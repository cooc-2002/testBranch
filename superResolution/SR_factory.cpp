/*
 * SRfactory.cpp
 *
 *  Created on: 2017. 1. 11.
 *      Author: ysh4568
 */
#include "superResolution/SR_factory.h"
#include "superResolution/SR_GPU2.h"
#include "superResolution/SR_CPU.h"

SR* SR_factory::construct(int state){
	switch (state){
		case 0 :
			return new SR_GPU2;
		case 1 :
			return new SR_CPU;
	}
}
