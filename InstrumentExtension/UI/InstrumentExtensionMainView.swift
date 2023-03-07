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
            VStack {
                HStack{
                    VStack {
                        HStack {
                            ObservableKnob(param: parameterTree.global.vca_attack)
                            ObservableKnob(param: parameterTree.global.vca_decay)
                        }
                        HStack {
                            ObservableKnob(param: parameterTree.global.vca_sustain)
                            ObservableKnob(param: parameterTree.global.vca_release)
                        }
                        
                    }
                    VStack {
                        HStack {
                            ObservableKnob(param: parameterTree.global.vcf_attack)
                            ObservableKnob(param: parameterTree.global.vcf_decay)
                        }
                        HStack {
                            ObservableKnob(param: parameterTree.global.vcf_sustain)
                            ObservableKnob(param: parameterTree.global.vcf_release)
                        }
                        
                    }
                    ObservableKnob(param: parameterTree.global.vcf_envelope_amount)
                    ObservableKnob(param: parameterTree.global.vcf_keyboard_tracking_amount)
                    ObservableKnob(param: parameterTree.global.detune)
                    ObservableKnob(param: parameterTree.global.cutoff, scale: Knob.Scale.logarithmic)
                    ObservableKnob(param: parameterTree.global.resonance)
                }
            }
        }.background(Color.white)
    }
}
