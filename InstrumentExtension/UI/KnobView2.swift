import SwiftUI
import Swift
import Foundation
import AudioToolbox

func clamp(value:Double, low: Double, high: Double) -> Double {
    return min(max(value, low), high)
}

struct KnobView2: View {
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
    
    @State var value : Double = 0.0
    @State private var startDragValue : Double = -1.0
    
    init(param: ObservableAUParameter, scale: Scale=Scale.linear) {
        self.param = param

        self.scale = scale

        self.bounds = param.min...param.max
        self.range = Double(param.max - param.min)
    
        self.upperBound = Double(param.max)
        self.lowerBound = Double(param.min)
                
        self.minpos = -135;
        self.maxpos = 135;

        if scale == Scale.logarithmic {
            self.minlval = log(self.lowerBound);
            self.maxlval = log(self.upperBound);
            self.logscale = (self.maxlval - self.minlval) / (self.maxpos - self.minpos);
        } else {
            self.minlval = 0
            self.maxlval = 0
            self.logscale = 0
        }
        }
    
    func valueToRotation(value:Double) -> Double{

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
        return VStack {
            Knob2()
                .rotationEffect(.degrees(valueToRotation(value: Double(param.value))))
                .gesture(DragGesture(minimumDistance: 0)
                    .onEnded({ _ in
                        startDragValue = -1
                        param.onEditingChanged(false)
                    })
                    .onChanged { dragValue in
                        print("called")
                        let diff =  dragValue.startLocation.y - dragValue.location.y
                        if startDragValue == -1 {
                            startDragValue = valueToRotation(value: Double(param.value))
                        }
                        let newValue = startDragValue + Double(diff)
                        rotation = newValue < -135 ? -135 : newValue > 135 ? 135 : newValue
                        
                        param.value = AUValue(rotationToValue(rotation: rotation))
                        param.onEditingChanged(true)
                    })
            Text("\(param.displayName): \(param.value)")
        }
        
    }
}

// convience Knob to checkout this Stack Overflow answer
// that can be later replaced with your own
struct Knob2: View {
    var body: some View {
        VStack{
            Image("knob3").resizable().scaledToFit()
        }
    }
}

