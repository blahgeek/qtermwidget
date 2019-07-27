#include <Cocoa/Cocoa.h>

extern void macos_hide_titlebar(long winid) {
    NSView *nativeView = (NSView *)winid;
    NSWindow* nativeWindow = [nativeView window];

    [nativeWindow setStyleMask:
        [nativeWindow styleMask] | NSWindowStyleMaskFullSizeContentView | NSWindowTitleHidden];

    [nativeWindow setTitlebarAppearsTransparent:YES];
}
