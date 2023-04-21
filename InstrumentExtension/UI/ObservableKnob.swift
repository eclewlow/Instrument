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


struct ObservableKnob: View {
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
        case .percent:
            return "%.0f %%"
        default:
            return "%.2f"
        }
    }
    var body: some View {
//        var c:Color = Color(red: 0.25, green: 0.25, blue: 0.25)
        var foregroundColor:Color = Color(red: 1, green: 1, blue: 1)
        var tickColor:Color = Color(red: 0.25, green: 0.25, blue: 0.25)
        return VStack {
            KnobView(param: param, scale: scale).tickStyle(count: 5, offset: 0.0, length: 0.1, width: 5.0, color: tickColor)
                .accessibilityIdentifier("delay knob")
                .frame(minWidth: 40, maxWidth: 240, minHeight: 40, maxHeight: 240)
                .aspectRatio(1.0, contentMode: .fit)
                .overlay(Text("\(param.value, specifier: specifier)")
                    .foregroundColor(foregroundColor)
                    .disabled(true).allowsHitTesting(false))
            Text("\(param.displayName)")
                .foregroundColor(foregroundColor)
        }
    }
    
}


