/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of renderer class which performs Metal setup and per frame rendering
*/

@import simd;
@import MetalKit;

#import "AAPLRenderer.h"

// Header shared between C code here, which executes Metal API commands, and .metal files, which
//   uses these types as input to the shaders
#import "AAPLShaderTypes.h"

// Main class performing the rendering
@implementation AAPLRenderer
{
    // The device (aka GPU) we're using to render
    id<MTLDevice> _device;

    // Our render pipeline composed of our vertex and fragment shaders in the .metal shader file
    id<MTLRenderPipelineState> _pipelineState;

    // The command Queue from which we'll obtain command buffers
    id<MTLCommandQueue> _commandQueue;

    // The Metal texture object
    id<MTLTexture> _texture;

    // The Metal buffer in which we store our vertex data
    id<MTLBuffer> _vertices;

    // The number of vertices in our vertex buffer
    NSUInteger _numVertices;

    // The current size of our view so we can use this in our render pipeline
    vector_uint2 _viewportSize;
}

/// Initialize with the MetalKit view from which we'll obtain our metal device
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        _device = mtkView.device;

        NSURL *imageFileLocation = [[NSBundle mainBundle] URLForResource:@"Image"
                                                           withExtension:@"tga"];

//        AAPLImage * image = [[AAPLImage alloc] initWithTGAFileAtLocation:imageFileLocation];
        _image = [[AAPLImage alloc] initWithTGAFileAtLocation:imageFileLocation];

        _motman = [[CMMotionManager alloc] init];
        [_motman startAccelerometerUpdates];
        _motman.startGyroUpdates;
        _motman.startMagnetometerUpdates;
        _motman.startDeviceMotionUpdates;

        if(!_image)
        {
            NSLog(@"Failed to create the image from %@", imageFileLocation.absoluteString);
            return nil;
        }

        MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];

        // Indicate that each pixel has a blue, green, red, and alpha channel, where each channel is
        // an 8-bit unsigned normalized value (i.e. 0 maps to 0.0 and 255 maps to 1.0)
        textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;

         // Set the pixel dimensions of the texture
        textureDescriptor.width = _image.width;
        textureDescriptor.height = _image.height;

        // Create the texture from the device by using the descriptor
        _texture = [_device newTextureWithDescriptor:textureDescriptor];

        // Calculate the number of bytes per row of our image.
        NSUInteger bytesPerRow = 4 * _image.width;

        MTLRegion region = {
            { 0, 0, 0 },                   // MTLOrigin
            {_image.width, _image.height, 1} // MTLSize
        };

//      image.data[10] = 255;
        // Copy the bytes from our data object into the texture
        [_texture replaceRegion:region
                    mipmapLevel:0
                      withBytes:_image.data.bytes
                    bytesPerRow:bytesPerRow];

// https://www.paintcodeapp.com/news/ultimate-guide-to-iphone-resolutions
#define dimx 750/2
#define dimy 1334/2

      // Set up a simple MTLBuffer with our vertices which include texture coordinates
        static const AAPLVertex quadVertices[] =
        {
            // Pixel positions, Texture coordinates
            { {  dimx,  -dimy },  { 1.f, 0.f } },
            { { -dimx,  -dimy },  { 0.f, 0.f } },
            { { -dimx,   dimy },  { 0.f, 1.f } },

            { {  dimx,  -dimy },  { 1.f, 0.f } },
            { { -dimx,   dimy },  { 0.f, 1.f } },
            { {  dimx,   dimy },  { 1.f, 1.f } },
        };

        // Create our vertex buffer, and initialize it with our quadVertices array
        _vertices = [_device newBufferWithBytes:quadVertices
                                         length:sizeof(quadVertices)
                                        options:MTLResourceStorageModeShared];

        // Calculate the number of vertices by dividing the byte length by the size of each vertex
        _numVertices = sizeof(quadVertices) / sizeof(AAPLVertex);

        /// Create our render pipeline

        // Load all the shader files with a metal file extension in the project
        id<MTLLibrary> defaultLibrary = [_device newDefaultLibrary];

        // Load the vertex function from the library
        id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"vertexShader"];

        // Load the fragment function from the library
        id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"samplingShader"];

        // Set up a descriptor for creating a pipeline state object
        MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineStateDescriptor.label = @"Texturing Pipeline";
        pipelineStateDescriptor.vertexFunction = vertexFunction;
        pipelineStateDescriptor.fragmentFunction = fragmentFunction;
        pipelineStateDescriptor.colorAttachments[0].pixelFormat = mtkView.colorPixelFormat;

        NSError *error = NULL;
        _pipelineState = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                 error:&error];
        if (!_pipelineState)
        {
            // Pipeline State creation could fail if we haven't properly set up our pipeline descriptor.
            //  If the Metal API validation is enabled, we can find out more information about what
            //  went wrong.  (Metal API validation is enabled by default when a debug build is run
            //  from Xcode)
            NSLog(@"Failed to created pipeline state, error %@", error);
        }

        // Create the command queue
        _commandQueue = [_device newCommandQueue];
    }

    return self;
}

/// Called whenever view changes orientation or is resized
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    // Save the size of the drawable as we'll pass these
    //   values to our vertex shader when we draw
    _viewportSize.x = size.width;
    _viewportSize.y = size.height;
}

/// Called whenever the view needs to render a frame
- (void)drawInMTKView:(nonnull MTKView *)view
{

    // Create a new command buffer for each render pass to the current drawable
    id<MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
    commandBuffer.label = @"MyCommand";

    // Obtain a renderPassDescriptor generated from the view's drawable textures
    MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;

    if(renderPassDescriptor != nil)
    {
        // Create a render command encoder so we can render into something
        id<MTLRenderCommandEncoder> renderEncoder =
        [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        renderEncoder.label = @"MyRenderEncoder";

        // Set the region of the drawable to which we'll draw.
        [renderEncoder setViewport:(MTLViewport){0.0, 0.0, _viewportSize.x, _viewportSize.y, -1.0, 1.0 }];

        [renderEncoder setRenderPipelineState:_pipelineState];

        [renderEncoder setVertexBuffer:_vertices
                                offset:0
                              atIndex:AAPLVertexInputIndexVertices];

        [renderEncoder setVertexBytes:&_viewportSize
                               length:sizeof(_viewportSize)
                              atIndex:AAPLVertexInputIndexViewportSize];


        CMGyroData * gd;
        gd = [_motman gyroData];
        float rot_y = gd.rotationRate.y; // s. https://developer.apple.com/documentation/coremotion/getting_raw_gyroscope_events
//      if (rot_y > 0) rot_y = 1;
//      if (rot_y < 0) rot_y = -1;

        // -----------------------------
        // fill buffer with random bytes
//        AAPLImage * image = [AAPLImage alloc];
      //  [image randlines];
      [_image do_some_galaga:(rot_y*5)]; // 10 zu schnell
      
        NSUInteger bytesPerRow = 4 * _image.width;
      
        MTLRegion region = {
          { 0, 0, 0 },                   // MTLOrigin
          {_image.width, _image.height, 1} // MTLSize
        };

        [_texture replaceRegion:region
                    mipmapLevel:0
                      withBytes:_image.data.bytes
                    bytesPerRow:bytesPerRow];
        // fill buffer with random bytes
        // -----------------------------

      
      
        // Set the texture object.  The AAPLTextureIndexBaseColor enum value corresponds
        ///  to the 'colorMap' argument in our 'samplingShader' function because its
        //   texture attribute qualifier also uses AAPLTextureIndexBaseColor for its index
        [renderEncoder setFragmentTexture:_texture
                                  atIndex:AAPLTextureIndexBaseColor];

        // Draw the vertices of our triangles
        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:0
                          vertexCount:_numVertices];

        [renderEncoder endEncoding];
      
        // Schedule a present once the framebuffer is complete using the current drawable
        [commandBuffer presentDrawable:view.currentDrawable];
    }

    
    // Finalize rendering here & push the command buffer to the GPU
    [commandBuffer commit];
}

-(void) tapped
{
  [_image tapped];
}

@end

