import SwiftUI
import Swift
import Foundation
import AudioToolbox

struct KnobView: View {
    @ObservedObject var param: ObservableAUParameter
    
    var bounds: ClosedRange<AUValue> = 0...1
    var range: Double = 0
    var lowerBound: Double = 0
    var upperBound: Double = 0
    
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
        
        self.rotation = valueToRotation(value: Double(param.value))
        }
    
    func valueToRotation(value:Double) -> Double{
    return Double(-135.0 + 270.0 * (value - lowerBound) / range)
    }
    
    func rotationToValue(rotation:Double) -> Double{
        return lowerBound + range * (rotation + 135.0) / 270.0
    }

    
    var body: some View {
        return Knob()
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

