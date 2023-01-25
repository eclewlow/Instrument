//
//  InstrumentExtensionAudioUnit.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

@interface InstrumentExtensionAudioUnit : AUAudioUnit
- (void)setupParameterTree:(AUParameterTree *)parameterTree;
@end
