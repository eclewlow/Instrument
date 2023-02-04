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
        VStack {
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
                    KnobView(param: parameterTree.global.attack)
                    KnobView(param: parameterTree.global.release)
                    KnobView(param: parameterTree.global.detune)
                }
            }
        }
    }
}
