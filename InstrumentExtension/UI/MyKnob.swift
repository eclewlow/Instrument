//
//  MyKnob.swift
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 2/7/23.
//

import SwiftUI
import Swift
import Foundation
import AudioToolbox


struct MyKnob: View {
    @ObservedObject var param: ObservableAUParameter
    var scale: Knob.Scale
    
    
    init(param: ObservableAUParameter, scale: Knob.Scale=Knob.Scale.linear) {
        self.param = param
        self.scale = scale
        }


    var specifier: String {
        switch param.unit {
        case .midiNoteNumber:
            return "%.0f"
        case .hertz:
            return "%.0fHz"
        case .cents:
            return "%.0f cents"
        case .milliseconds:
            return "%.0f ms"
        case .decibels:
            return "%.0f dB"
        default:
            return "%.2f"
        }
    }
    var body: some View {
        VStack {
            KnobView(param: param, scale: scale).tickStyle(count: 5, offset: 0.0, length: 0.1, width: 5.0, color: Color(red: 0.25, green: 0.25, blue: 0.25))
                .accessibilityIdentifier("delay knob")
                .frame(minWidth: 40, maxWidth: 240, minHeight: 40, maxHeight: 240)
                .aspectRatio(1.0, contentMode: .fit)
                .overlay(Text("\(param.value, specifier: specifier)").foregroundColor(Color(red: 0.25, green: 0.25, blue: 0.25)).disabled(true).allowsHitTesting(false))
            Text("\(param.displayName)").foregroundColor(Color(red: 0.25, green: 0.25, blue: 0.25))
        }
    }
    
}


