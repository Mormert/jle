/*********************************************************************************************
*                                                                                           *
*               ,     .     ,                      .   ,--.                                 *
*               |     |     |                      |   |            o                       *
*               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
*               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
*              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
*                                                                                           *
*     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
*          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
*     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
*                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
*                                                                                           *
*********************************************************************************************/

#import <Cocoa/Cocoa.h>
#import <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#import <GLFW/glfw3native.h>

#include "ImGui/imgui.h"

@interface CustomTitleBarView : NSView
@end

@implementation CustomTitleBarView

- (void)mouseDown:(NSEvent *)event {
    if (![self handleImGuiMouseEvent:event]) {
        if ([event clickCount] == 2) {
            [self toggleMaximize];
        } else {
            NSWindow *window = [self window];
            [window performWindowDragWithEvent:event];
        }
    }
}

- (void)mouseDragged:(NSEvent *)event {
    if (![self handleImGuiMouseEvent:event]) {
        NSWindow *window = [self window];
        [window performWindowDragWithEvent:event];
    }
}

- (void)mouseUp:(NSEvent *)event {
    [self handleImGuiMouseEvent:event];
}

- (BOOL)handleImGuiMouseEvent:(NSEvent *)event {
    ImGuiIO& io = ImGui::GetIO();

    switch (event.type) {
    case NSEventTypeLeftMouseDown:
        io.MouseDown[0] = true;
        break;
    case NSEventTypeLeftMouseUp:
        io.MouseDown[0] = false;
        break;
    case NSEventTypeMouseMoved:
    case NSEventTypeLeftMouseDragged:
        io.MouseDown[0] = (event.type == NSEventTypeLeftMouseDragged);
        break;
    default:
        return NO;
    }

    auto isAnyItemHovered = ImGui::IsAnyItemHovered();
    bool retVal = isAnyItemHovered;
    return isAnyItemHovered;
}

- (void)toggleMaximize {
    NSWindow *window = [self window];
    if ([window isZoomed]) {
        [window zoom:nil]; // Restore original size
    } else {
        [window zoom:nil]; // Maximize the window
    }
}

@end

// Function to initialize Cocoa and modify the GLFW window
void customizeTitleBarMacOS(GLFWwindow* window) {
    // Get the NSWindow from the GLFW window
    auto nsWindow = (NSWindow*)glfwGetCocoaWindow(window);

    // Hide the default title bar buttons
    [[nsWindow standardWindowButton:NSWindowCloseButton] setHidden:YES];
    [[nsWindow standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
    [[nsWindow standardWindowButton:NSWindowZoomButton] setHidden:YES];

    // Hide the title bar
    [nsWindow setTitleVisibility:NSWindowTitleHidden];

    // Remove the title bar style mask
    [nsWindow setStyleMask:([nsWindow styleMask] & ~NSWindowStyleMaskTitled) | NSWindowStyleMaskFullSizeContentView];

    // Ensure the content view covers the entire window
    NSView* contentView = [nsWindow contentView];
    [contentView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [nsWindow setContentView:contentView];

    constexpr int customTitleBarHeight = 30;

    // Create and add the custom title bar view
    NSRect titleBarFrame = NSMakeRect(0, [nsWindow frame].size.height - customTitleBarHeight, [nsWindow frame].size.width, customTitleBarHeight);
    CustomTitleBarView* customTitleBar = [[CustomTitleBarView alloc] initWithFrame:titleBarFrame];
    [customTitleBar setAutoresizingMask:NSViewWidthSizable | NSViewMinYMargin];
    [contentView addSubview:customTitleBar];

    // Maximize the window
    [nsWindow zoom:nil];
}