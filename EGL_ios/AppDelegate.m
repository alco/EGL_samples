#import "AppDelegate.h"
#include "es1_demo.h"

#import <QuartzCore/QuartzCore.h>


@interface EAGLView: UIView
@end

@implementation EAGLView
+ (id)layerClass { return [CAEAGLLayer class]; }
@end

#pragma mark -

@implementation AppDelegate

@synthesize window = _window, dlink = _dlink;

- (void)dealloc
{
    [_window release];
    [_dlink invalidate];
    [_dlink release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window = window;
    window.backgroundColor = [UIColor whiteColor];
    [window release];
    
    [window makeKeyAndVisible];
    
    ///
    
    EAGLView *glView = [[EAGLView alloc] initWithFrame:window.bounds];
    [window addSubview:glView];
    [glView release];
    
    UIViewController *controller = [[UIViewController alloc] init];
    controller.view = glView;
    window.rootViewController = controller;
    [controller release];
    
    createRenderContext(glView);
    setupGL();
    
    CADisplayLink *link = [CADisplayLink displayLinkWithTarget:self selector:@selector(update:)];
    link.frameInterval = 1;
    [link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    self.dlink = link;
    [link release];

    return YES;
}

- (void)update:(CADisplayLink *)dlink
{
    renderFrame();
    swapBuffers();
}

#pragma mark

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    self.dlink.paused = YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    self.dlink.paused = NO;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    destroyRenderContext();
}

@end

#pragma mark -

int main(int argc, char *argv[])
{
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
