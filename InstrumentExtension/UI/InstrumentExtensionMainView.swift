//
//  InstrumentExtensionMainView.swift
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

import SwiftUI

struct InstrumentExtensionMainView: View {
    var parameterTree: ObservableAUParameterGroup
        
    var body: some View {
        return VStack {
            HStack{
                Text("hi")
                Text("you")
                Button(action: {
                    
                }) {
                    Text("hi")
                }
            }
            ParameterSlider(param: parameterTree.global.gain)
            VStack {
                HStack{
                    MyKnob(param: parameterTree.global.attack)
                    MyKnob(param: parameterTree.global.release)
                    MyKnob(param: parameterTree.global.detune)
                    MyKnob(param: parameterTree.global.cutoff, scale: Knob.Scale.logarithmic)
                    MyKnob(param: parameterTree.global.resonance)
                }
            }
        }
    }
}
