//
//  Parameters.swift
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

import Foundation
import AudioToolbox

enum OscillatorMode:Float {
    case OSCILLATOR_MODE_START = -1
    case OSCILLATOR_MODE_SINE = 0
    case OSCILLATOR_MODE_SAW = 1
    case OSCILLATOR_MODE_SQUARE = 2
    case OSCILLATOR_MODE_TRIANGLE = 3
    case OSCILLATOR_MODE_FM = 4
    case OSCILLATOR_MODE_END = 5
}

let InstrumentExtensionParameterSpecs = ParameterTreeSpec {
    ParameterGroupSpec(identifier: "global", name: "Global") {
        ParameterSpec(
            address: .gain,
            identifier: "gain",
            name: "Output Gain",
            units: .linearGain,
            valueRange: 0.0...1.0,
            defaultValue: 0.25
        );
        ParameterSpec(
            address: .detune,
            identifier: "detune",
            name: "Oscillator 2 Detune",
            units: .cents,
            valueRange: -100...100,
            defaultValue: 0
        );
        ParameterSpec(
            address: .vca_attack,
            identifier: "vca_attack",
            name: "Attack",
            units: .milliseconds,
            valueRange: 8.0...1000.0,
            defaultValue: 100.0
        );
        ParameterSpec(
            address: .vca_decay,
            identifier: "vca_decay",
            name: "Decay",
            units: .milliseconds,
            valueRange: 8.0...1000.0,
            defaultValue: 100.0
        );
        ParameterSpec(
            address: .vca_sustain,
            identifier: "vca_sustain",
            name: "Sustain",
            units: .linearGain,
            valueRange: 0.0...1.0,
            defaultValue: 1.0
        );
        ParameterSpec(
            address: .vca_release,
            identifier: "vca_release",
            name: "Release",
            units: .milliseconds,
            valueRange: 8.0...1000.0,
            defaultValue: 100.0
        );
        ParameterSpec(
            address: .vcf_attack,
            identifier: "vcf_attack",
            name: "Attack",
            units: .milliseconds,
            valueRange: 8.0...1000.0,
            defaultValue: 100.0
        );
        ParameterSpec(
            address: .vcf_decay,
            identifier: "vcf_decay",
            name: "Decay",
            units: .milliseconds,
            valueRange: 8.0...1000.0,
            defaultValue: 100.0
        );
        ParameterSpec(
            address: .vcf_sustain,
            identifier: "vcf_sustain",
            name: "Sustain",
            units: .linearGain,
            valueRange: 0.0...1.0,
            defaultValue: 1.0
        );
        ParameterSpec(
            address: .vcf_release,
            identifier: "vcf_release",
            name: "Release",
            units: .milliseconds,
            valueRange: 8.0...1000.0,
            defaultValue: 100.0
        );
        ParameterSpec(
            address: .cutoff,
            identifier: "cutoff",
            name: "Cutoff",
            units: .hertz,
            valueRange: 12.0...8_500.0,
            defaultValue: 8_500.0
        );
        ParameterSpec(
            address: .resonance,
            identifier: "resonance",
            name: "Resonance",
            units: .decibels,
            valueRange: -8...20,
            defaultValue: -8.0
        );
        ParameterSpec(
            address: .vcf_envelope_amount,
            identifier: "vcf_envelope_amount",
            name: "VCF Envelope Amount",
            units: .linearGain,
            valueRange: 0.0...1.0,
            defaultValue: 0.0
        );
        ParameterSpec(
            address: .vcf_keyboard_tracking_amount,
            identifier: "vcf_keyboard_tracking_amount",
            name: "VCF Keyboard Tracking Amount",
            units: .linearGain,
            valueRange: 0.0...1.0,
            defaultValue: 0.0
        );
        ParameterSpec(
            address: .oscillator_mode,
            identifier: "oscillator_mode",
            name: "Oscillator Mode",
            units: .customUnit,
            valueRange: OscillatorMode.OSCILLATOR_MODE_START.rawValue...OscillatorMode.OSCILLATOR_MODE_END.rawValue,
            defaultValue: OscillatorMode.OSCILLATOR_MODE_SAW.rawValue
        );
        ParameterSpec(
            address: .fm_ratio,
            identifier: "fm_ratio",
            name: "FM Ratio",
            units: .ratio,
            valueRange: 0.03125...32.0,
            defaultValue: 1.0
//            units: .decibels,
//            valueRange: 1...32,
//            defaultValue: 1
        );
        ParameterSpec(
            address: .fm_gain,
            identifier: "fm_gain",
            name: "FM Gain",
            units: .linearGain,
            valueRange: 0.0...1.0,
            defaultValue: 0.0
        );

    }
}

extension ParameterSpec {
    init(
        address: InstrumentExtensionParameterAddress,
        identifier: String,
        name: String,
        units: AudioUnitParameterUnit,
        valueRange: ClosedRange<AUValue>,
        defaultValue: AUValue,
        unitName: String? = nil,
        flags: AudioUnitParameterOptions = [AudioUnitParameterOptions.flag_IsWritable, AudioUnitParameterOptions.flag_IsReadable],
        valueStrings: [String]? = nil,
        dependentParameters: [NSNumber]? = nil
    ) {
        self.init(address: address.rawValue,
                  identifier: identifier,
                  name: name,
                  units: units,
                  valueRange: valueRange,
                  defaultValue: defaultValue,
                  unitName: unitName,
                  flags: flags,
                  valueStrings: valueStrings,
                  dependentParameters: dependentParameters)
    }
}
