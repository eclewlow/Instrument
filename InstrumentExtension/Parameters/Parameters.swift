//
//  Parameters.swift
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

import Foundation
import AudioToolbox

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
            address: .attack,
            identifier: "attack",
            name: "Attack",
            units: .milliseconds,
            valueRange: 8.0...1000.0,
            defaultValue: 100.0
        );
        ParameterSpec(
            address: .release,
            identifier: "release",
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
            valueRange: 12.0...20_000.0,
            defaultValue: 20_000.0
        );
        ParameterSpec(
            address: .resonance,
            identifier: "resonance",
            name: "Resonance",
            units: .decibels,
            valueRange: -20...20,
            defaultValue: 0.0
        )
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
