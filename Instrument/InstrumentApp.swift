//
//  InstrumentApp.swift
//  Instrument
//
//  Created by Eugene Clewlow on 1/25/23.
//

import CoreMIDI
import SwiftUI

@main
class InstrumentApp: App {
    @ObservedObject private var hostModel = AudioUnitHostModel()

    required init() {}

    var body: some Scene {
        WindowGroup {
            ContentView(hostModel: hostModel)
//            .background(
//                Image("plaits_v50.svg")
//                .resizable()
//                 .aspectRatio(contentMode: .fit)
//                     .foregroundColor(Color.white).background(Color.white))
//                .frame(width:617/1.5,height:1280/1.5)
        }
//        .windowResizability(.contentSize)
            
    }
}
