//
//  InstrumentExtensionParameterAddresses.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <AudioToolbox/AUParameters.h>

#ifdef __cplusplus
namespace InstrumentExtensionParameterAddress {
#endif

typedef NS_ENUM(AUParameterAddress, InstrumentExtensionParameterAddress) {
    gain = 0,
    sendNote = 1,
    attack=2,
    detune=3
};

#ifdef __cplusplus
}
#endif
