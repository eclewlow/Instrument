import SwiftUI
import Swift
import Foundation
import AudioToolbox

func clamp(value:Double, low: Double, high: Double) -> Double {
    return min(max(value, low), high)
}

struct KnobView: View {
    @ObservedObject var param: ObservableAUParameter
    
    var bounds: ClosedRange<AUValue> = 0...1
    var range: Double = 0
    var lowerBound: Double = 0
    var upperBound: Double = 0
    
    var minpos: Double;
    var maxpos: Double;

    var minlval: Double;
    var maxlval: Double;

    var scale: Double;
    
    
    @State private var rotation: Double = 0.0
    @State private var startRotation: Double = 0.0
    
    @State var value : Double = 0.0
    @State private var startDragValue : Double = -1.0
    
    init(param: ObservableAUParameter) {
        self.param = param
//            self.bounds = bounds!
        self.bounds = param.min...param.max
        self.range = Double(param.max - param.min)
    
        self.upperBound = Double(param.max)
        self.lowerBound = Double(param.min)
                
        self.minpos = -135;
        self.maxpos = 135;
//
        self.minlval = log(self.lowerBound);
        self.maxlval = log(self.upperBound);
//
        self.scale = (self.maxlval - self.minlval) / (self.maxpos - self.minpos);
        
        self.rotation = valueToRotation(value: Double(param.value))

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
        return clamp(value: minpos + (log(value) - self.minlval) / self.scale, low: minpos, high: maxpos)
    }
    
    func rotationToValue(rotation:Double) -> Double{
//        return lowerBound + range * (rotation + 135.0) / 270.0
        return clamp(value: exp((rotation - minpos) * scale + minlval), low: lowerBound, high: upperBound)
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
            Knob()
                .rotationEffect(.degrees(valueToRotation(value: Double(param.value))))
    //            .gesture(
    //                RotationGesture()
    //                    .onChanged({ angle in
    //                        if startRotation == 0 {
    //                            startRotation = rotation
    //                        }
    //                        rotation = startRotation + angle.degrees
    //                    })
    //                    .onEnded({ _ in
    //                        startRotation = 0
    //                    })
    //            )
                .gesture(DragGesture(minimumDistance: 0)
                    .onEnded({ _ in
                        startDragValue = -1
    //                    startRotation = 0
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
            Text("\(param.displayName): \(param.value, specifier: specifier)")
        }
        
    }
}

// convience Knob to checkout this Stack Overflow answer
// that can be later replaced with your own
struct Knob: View {
    var body: some View {
        VStack{
            Image("knob3").resizable().scaledToFit()
        }
    }
}

