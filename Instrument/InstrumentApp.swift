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
            ContentView(hostModel: hostModel).background(Color.white)
        }
    }
}
