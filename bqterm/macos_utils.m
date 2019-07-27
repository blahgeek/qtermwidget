#include <Cocoa/Cocoa.h>

extern void macos_hide_titlebar(long winid) {
    NSView *nativeView = (NSView *)winid;
    NSWindow* nativeWindow = [nativeView window];

    [nativeWindow setStyleMask: [nativeWindow styleMask] & ~NSWindowStyleMaskTitled];

    [nativeWindow setTitlebarAppearsTransparent:YES];
}
