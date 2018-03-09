/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of our cross-platform view controller
*/

#import "AAPLViewController.h"
#import "AAPLRenderer.h"

@implementation AAPLViewController
{
    MTKView *_view;

    AAPLRenderer *_renderer;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Set the view to use the default device
    _view = (MTKView *)self.view;
    _view.device = MTLCreateSystemDefaultDevice();

    if(!_view.device)
    {
        NSLog(@"Metal is not supported on this device");
        return;
    }

    _renderer = [[AAPLRenderer alloc] initWithMetalKitView:_view];

    if(!_renderer)
    {
        NSLog(@"Renderer failed initialization");
        return;
    }

    // Initialize our renderer with the view size
    [_renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];

    _view.delegate = _renderer;
  
  [self addGestureRecognizers];
}
// screen rotation verhindern: nicht programmatisch, sondern durch Einstellung in der plist.info
// https://stackoverflow.com/questions/31794317/how-can-i-lock-orientation-for-a-specific-view-an-objective-c-iphone-app-in-ios
/*
-(void) viewDidAppear:(BOOL)animated
{
  [super viewDidAppear:false];
//  [[UIDevice currentDevice] setValue:@(UIInterfaceOrientationLandscapeLeft) forKey:@"orientation"];
  [[UIDevice currentDevice] setValue:@(UIInterfaceOrientationPortraitUpsideDown) forKey:@"orientation"];
}
*/
// wie man die Tipp events zu seinem Projekt hinzufügt: https://www.youtube.com/watch?v=mdvoDAoTMd4
- (void) addGestureRecognizers
{
  // here we have tappi, our little taprecognizer
  UITapGestureRecognizer *tappi = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(TapRecognized:)];
  [_view addGestureRecognizer:tappi];
}

// hier auch gut, dort sieht man, wie die location ermittelt wird: https://stackoverflow.com/questions/4660371/how-to-add-a-touch-event-to-a-uiview
- (void) TapRecognized:(UITapGestureRecognizer *) recognizer
{
//  CGPoint location = [recognizer locationInView:[recognizer.view superview]];
  [_renderer tapped];
}

@end
