//
//  InstrumentExtensionMainView.swift
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

import SwiftUI

struct InstrumentExtensionMainView: View {
    var parameterTree: ObservableAUParameterGroup
    
    @State private var selectedColor = "Red"
    @State private var isOn = true
    @State private var sawSelected = false
    @State private var squareSelected = false
    
    var body: some View {
        //        return VStack {
        //
        //            Text("hi").position(x:100, y:100).foregroundColor(Color.black)
        //        }
        return VStack {
            VStack {
                HStack{
                    Button (action: {
                        print("saw tapped")
                        parameterTree.global.oscillator_mode.value = OscillatorMode.OSCILLATOR_MODE_SAW.rawValue
                    }, label: {
                        Text("Saw")
                    })
                    .buttonStyle(.borderedProminent).tint(.mint)
                    
                    Button(action: {
                        print("square tapped")
                        parameterTree.global.oscillator_mode.value = OscillatorMode.OSCILLATOR_MODE_SQUARE.rawValue
                    }, label: {
                        Text("Square")
                    })
                    .buttonStyle(.borderedProminent).tint(.mint)
                    
                    Button(action: {
                        print("sine tapped")
                        parameterTree.global.oscillator_mode.value = OscillatorMode.OSCILLATOR_MODE_SINE.rawValue
                    }, label: {
                        Text("Sine")
                    })
                    .buttonStyle(.borderedProminent).tint(.mint)
                    Button(action: {
                        print("triangle tapped")
                        parameterTree.global.oscillator_mode.value = OscillatorMode.OSCILLATOR_MODE_TRIANGLE.rawValue
                    }, label: {
                        Text("Triangle")
                    })
                    .buttonStyle(.borderedProminent).tint(.mint)
                    Button(action: {
                        print("FM tapped")
                        parameterTree.global.oscillator_mode.value = OscillatorMode.OSCILLATOR_MODE_FM.rawValue
                    }, label: {
                        Text("FM")
                    })
                    .buttonStyle(.borderedProminent).tint(.mint)
                }
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
                HStack{
                    ObservableKnob(param: parameterTree.global.fm_ratio)
                    ObservableKnob(param: parameterTree.global.fm_gain)
                }
            }
        }
        
        
        
        
        
    }
}
