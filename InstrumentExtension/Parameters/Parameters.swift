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
            valueRange: 0.0...1000.0,
            defaultValue: 100.0
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
