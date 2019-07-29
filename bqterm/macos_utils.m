#include <Cocoa/Cocoa.h>

extern void macos_hide_titlebar(long winid) {
    NSView *nativeView = (NSView *)winid;
    NSWindow* nativeWindow = [nativeView window];

    int style = [nativeWindow styleMask];

    // use this would make the window not resizable
    /* style &= ~NSWindowStyleMaskTitled; */
    style |= NSWindowStyleMaskFullSizeContentView;
    style |= NSWindowTitleHidden;
    [nativeWindow setStyleMask: style];

    [[nativeWindow standardWindowButton:NSWindowCloseButton] setHidden:YES];
    [[nativeWindow standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
    [[nativeWindow standardWindowButton:NSWindowZoomButton] setHidden:YES];

    [nativeWindow setTitlebarAppearsTransparent:YES];
}
