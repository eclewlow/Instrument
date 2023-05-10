//
//  InstrumentExtensionMainView.swift
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

import SwiftUI

struct InstrumentExtensionMainView: View {
    var parameterTree: ObservableAUParameterGroup
    
//    @State private var hardSyncOn: Bool { parameterTree.global.hard_sync }

//    @State private var binding: Bool {
//        get { return parameterTree.global.hard_sync.value == AUValue(1) }
//        set { if !newValue { parameterTree.global.hard_sync.value = AUValue(1) }else {
//            parameterTree.global.hard_sync.value = AUValue(0)
//        } }
//    }
    
    var body: some View {
        //        return VStack {
        //
        //            Text("hi").position(x:100, y:100).foregroundColor(Color.black)
        //        }
//        let binding = Binding<Bool>(get: { parameterTree.global.hard_sync.value == AUValue(1) }, set: {
//            if $0 {
//                parameterTree.global.hard_sync.value = AUValue(1)
//            } else {
//                parameterTree.global.hard_sync.value = AUValue(0)
//            }
//        })

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
                    ObservableToggle(param: parameterTree.global.hard_sync)
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
                    ObservableKnob(param: parameterTree.global.fine_tune)
                    ObservableKnob(param: parameterTree.global.coarse_tune)
                    ObservableKnob(param: parameterTree.global.cutoff, scale: Knob.Scale.logarithmic)
                    ObservableKnob(param: parameterTree.global.resonance)
                }
                HStack{
                    ObservableKnob(param: parameterTree.global.fm_ratio)
                    ObservableKnob(param: parameterTree.global.fm_gain)
                    ObservableKnob(param: parameterTree.global.fm_feedback)
                }
                HStack{
                    ObservableKnob(param: parameterTree.global.pulse_width)
                    ObservableKnob(param: parameterTree.global.bitcrush_rate, scale: Knob.Scale.logarithmic)
                }
            }
        }
        
        
        
        
        
    }
}
