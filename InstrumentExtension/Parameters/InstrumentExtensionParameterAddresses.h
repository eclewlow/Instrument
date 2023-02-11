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
    gain,
    sendNote,
    vca_attack,
    vca_decay,
    vca_sustain,
    vca_release,
    detune,
    cutoff,
    resonance,
    vcf_attack,
    vcf_decay,
    vcf_sustain,
    vcf_release,
    vcf_amount
};

#ifdef __cplusplus
}
#endif
