/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for renderer class which performs Metal setup and per frame rendering
*/

@import MetalKit;
@import CoreMotion;

#import "AAPLImage.h"

// Our platform independent render class
@interface AAPLRenderer : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView;

@property (nonnull,nonatomic,readonly) AAPLImage * image;
@property (nonnull,nonatomic,readonly) CMMotionManager * motman;

- (void) tapped;

@end
