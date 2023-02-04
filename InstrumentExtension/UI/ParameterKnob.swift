//
//  ParameterSlider.swift
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

import SwiftUI

/// A SwiftUI Slider container which is bound to an ObservableAUParameter
///
/// This view wraps a SwiftUI Slider, and provides it relevant data from the Parameter, like the minimum and maximum values.
struct ParameterKnob: View {
    @ObservedObject var param: ObservableAUParameter
    
    var specifier: String {
        switch param.unit {
        case .midiNoteNumber:
            return "%.0f"
        default:
            return "%.2f"
        }
    }
    
    @State var value : Double = 0.0
    @State private var startDragValue : Double = -1.0
    var body: some View {
        Text("Knob \(value)")
            .gesture(DragGesture(minimumDistance: 0)
            .onEnded({ _ in
                startDragValue = -1.0
            })
            .onChanged { dragValue in
                let diff =  dragValue.startLocation.y - dragValue.location.y
                if startDragValue == -1 {
                    startDragValue = value
                }
                let newValue = startDragValue + Double(diff)
                value = newValue < 0 ? 0 : newValue > 100 ? 100 : newValue
            })
    }
    
//    var body: some View {
//        VStack {
//            Slider(
//                value: $param.value,
//                in: param.min...param.max,
//                onEditingChanged: param.onEditingChanged,
//                minimumValueLabel: Text("\(param.min, specifier: specifier)"),
//                maximumValueLabel: Text("\(param.max, specifier: specifier)")
//            ) {
//                EmptyView()
//            }
//            Text("\(param.displayName): \(param.value, specifier: specifier)")
//        }
//        .padding()
//    }
}
