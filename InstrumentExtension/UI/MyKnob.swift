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
    
    enum Scale {
        case logarithmic, linear
    }
    
    var bounds: ClosedRange<AUValue> = 0...1
    var range: Double = 0
    var lowerBound: Double = 0
    var upperBound: Double = 0
    
    var minpos: Double = -135.0
    var maxpos: Double = 135.0

    var minlval: Double = 0.0
    var maxlval: Double = 1000.0

    var logscale: Double = 100.0
    
    var scale: Scale
    
    @State private var rotation: Double = 0.0
    @State private var startRotation: Double = 0.0
    
    @State var value : Float = 0.0
    @State private var startDragValue : Double = -1.0
    @State var manipulating: Bool = false
    
    init(param: ObservableAUParameter, scale: Scale=Scale.linear) {
        self.param = param
//            self.bounds = bounds!
        self.scale = scale

        self.bounds = param.min...param.max
        self.range = Double(param.max - param.min)
    
        self.upperBound = Double(param.max)
        self.lowerBound = Double(param.min)
                
        self.minpos = -135;
        self.maxpos = 135;
//

        if scale == Scale.logarithmic {
            self.minlval = log(self.lowerBound);
            self.maxlval = log(self.upperBound);
            self.logscale = (self.maxlval - self.minlval) / (self.maxpos - self.minpos);
        } else {
            self.minlval = 0
            self.maxlval = 0
            self.logscale = 0
        }
//                self.minlval = 2.99573227355;
//                self.maxlval = 9.90348755254;

        //

//
//        self.minlval = 2.99573227355;
//        self.maxlval = 9.90348755254;
////
//        self.scale = (9.90348755254 - 2.99573227355) / (270);
        
//        var test1:Double = minpos + (log(value) - self.minlval) / self.scale
//        self.rotation = valueToRotation(value: Double(param.value))

//        self.rotation = test1
//        // Calculate value from a slider position
//        value: function(position) {
//           return Math.exp((position - this.minpos) * this.scale + this.minlval);
//        },
//        // Calculate slider position from a value
//        position: function(value) {
//           return this.minpos + (Math.log(value) - this.minlval) / this.scale;
//        }
        }
    
    func valueToRotation(value:Double) -> Double{
//    return Double(-135.0 + 270.0 * (value - lowerBound) / range)
        if self.scale == Scale.linear {
            return clamp(value: Double(-135.0 + 270.0 * (value - lowerBound) / range), low: -135, high: 135)
        } else if self.scale == Scale.logarithmic {
                return clamp(value: minpos + (log(value) - minlval) / logscale, low: minpos, high: maxpos)
        } else {
            return 0;
        }
        
    }
    
    func rotationToValue(rotation:Double) -> Double{
        if self.scale == Scale.linear {
            return clamp(value: lowerBound + range * (rotation + 135.0) / 270.0, low: lowerBound, high: upperBound)
        } else if self.scale == Scale.logarithmic {
                return clamp(value: exp((rotation - minpos) * logscale + minlval), low: lowerBound, high: upperBound)
        } else {
            return 0;
        }
    }

    var specifier: String {
        switch param.unit {
        case .midiNoteNumber:
            return "%.0f"
        default:
            return "%.2f"
        }
    }
    var body: some View {
        VStack {
            KnobView(param: param, value: $value, manipulating: $manipulating).tickStyle(count: 5, offset: 0.0, length: 0.1, width: 5.0, color: Color.white)
            //            .trackStyle(widthFactor: trackWidthFactor, color: trackColor)
            //            .progressStyle(widthFactor: progressWidthFactor, color: progressColor)
            //            .indicatorStyle(widthFactor: progressWidthFactor, color: progressColor, length: 0.3)
                .accessibilityIdentifier("delay knob")
                .frame(minWidth: 40, maxWidth: 240, minHeight: 40, maxHeight: 240)
                .aspectRatio(1.0, contentMode: .fit)
            Text("\(param.displayName): \(param.value)")
        }
    }
    
}


