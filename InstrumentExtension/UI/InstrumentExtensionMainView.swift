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
                    VStack {
                        HStack {
                            MyKnob(param: parameterTree.global.vca_attack)
                            MyKnob(param: parameterTree.global.vca_decay)
                        }
                        HStack {
                            MyKnob(param: parameterTree.global.vca_sustain)
                            MyKnob(param: parameterTree.global.vca_release)
                        }

                    }
                    VStack {
                        HStack {
                            MyKnob(param: parameterTree.global.vcf_attack)
                            MyKnob(param: parameterTree.global.vcf_decay)
                        }
                        HStack {
                            MyKnob(param: parameterTree.global.vcf_sustain)
                            MyKnob(param: parameterTree.global.vcf_release)
                        }

                    }
                    MyKnob(param: parameterTree.global.vcf_envelope_amount)
                    MyKnob(param: parameterTree.global.vcf_keyboard_tracking_amount)
                    MyKnob(param: parameterTree.global.detune)
                    MyKnob(param: parameterTree.global.cutoff, scale: Knob.Scale.logarithmic)
                    MyKnob(param: parameterTree.global.resonance)
                }
            }
        }
    }
}
