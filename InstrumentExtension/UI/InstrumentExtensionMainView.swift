//
//  InstrumentExtensionMainView.swift
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

import SwiftUI

struct InstrumentExtensionMainView: View {
    var parameterTree: ObservableAUParameterGroup
    @State var volumeValue: Float = 0.25
    @State var volumeManipulating: Bool = false

    @State var delayValue: Float = 0.25
    @State var delayManipulating: Bool = false
    
    let valueFormatter: NumberFormatter = {
      let valueFormatter = NumberFormatter()
      valueFormatter.maximumFractionDigits = 2
      valueFormatter.minimumFractionDigits = 2
      valueFormatter.maximumIntegerDigits = 3
      valueFormatter.minimumIntegerDigits = 1
      return valueFormatter
    }()
    
    var body: some View {
        let trackWidthFactor: CGFloat = 0.08
        let trackColor = Color(red: 0.25, green: 0.25, blue: 0.25)
        let progressWidthFactor: CGFloat = 0.055
        let progressColor = Color(red: 1.0, green: 0.575, blue: 0.0)
        let textColor = Color(red: 0.7, green: 0.5, blue: 0.3)
        
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
                    KnobView2(param: parameterTree.global.attack)
                    MyKnob(param: parameterTree.global.release)
                    KnobView2(param: parameterTree.global.detune)
                    KnobView2(param: parameterTree.global.cutoff, scale: KnobView2.Scale.logarithmic)
                    KnobView2(param: parameterTree.global.resonance)
                }
            }
        }
    }
}
