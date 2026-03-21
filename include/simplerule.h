#pragma once

#include <string>

struct SimpleRule {
  std::string name;
  std::string keyword;
  std::string tooltipText;
};

const SimpleRule simpleRules[] = {
    {.name = "Center window",
     .keyword = "center",
     .tooltipText =
         "If the window is floating, will center it on the monitor."},
    {.name = "No inital focus",
     .keyword = "no_initial_focus",
     .tooltipText = "Disables the initial focus to the window"},
    {.name = "Pin window",
     .keyword = "pin",
     .tooltipText = "Pins the window (i.e. show it on all workspaces). Note: "
                    "floating only."},
    {.name = "persistent_size",
     .keyword = "persistent_size",
     .tooltipText =
         "For floating windows, internally store their size. When a new "
         "floating window opens with the same class and title, restore the "
         "saved size."},
    {.name = "no max size",
     .keyword = "no_max_size",
     .tooltipText = "Removes max size limitations. Especially useful with "
                    "windows that report invalid max sizes (e.g. winecfg)."},
    {.name = "stay focused",
     .keyword = "stay_focused",
     .tooltipText = "Forces focus on the window as long as it’s visible."},
    {.name = "allows input",
     .keyword = "allows_input",
     .tooltipText = "Forces an XWayland window to receive input, even if it "
                    "requests not to do so. (Might fix issues like Game "
                    "Launchers not receiving focus for some reason)"},
    {.name = "Dim around",
     .keyword = "dim_around",
     .tooltipText = "Dims everything around the window. Please note that this "
                    "rule is meant for floating windows and using it on tiled "
                    "ones may result in strange behavior."},
    {.name = "No decorate",
     .keyword = "decorate",
     .tooltipText = "Do not draw window decorations"},
    {.name = "Focus on activate",
     .keyword = "focus_on_activate",
     .tooltipText = "Whether Hyprland should focus an app that requests to be "
                    "focused (an activate request)."},
    {.name = "Keep aspect ratio",
     .keyword = "keep_aspect_ratio",
     .tooltipText = "Forces aspect ratio when resizing window with the mouse."},
    {.name = "Nearest neighbor",
     .keyword = "nearest_neighbor",
     .tooltipText = "Forces the window to use nearest neighbor filtering."},
    {.name = "No anim",
     .keyword = "no_anim",
     .tooltipText = "Disables the animations for the window."},
    {.name = "No blur",
     .keyword = "no_blur",
     .tooltipText = "Disables blur for the window."},
    {.name = "No dim",
     .keyword = "no_dim",
     .tooltipText = "Disables window dimming for the window."},
    {.name = "No focus",
     .keyword = "no_focus",
     .tooltipText = "Disables focus to the window."},
    {.name = "No follow mouse",
     .keyword = "no_follow_mouse",
     .tooltipText = "Prevents the window from being focused when the mouse "
                    "moves over it when input:follow_mouse=1 is set."},
    {.name = "No shadow",
     .keyword = "no_shadow",
     .tooltipText = "Disables shadows for the window."},
    {.name = "No shortcuts inhibit",
     .keyword = "no_shortcuts_inhibit",
     .tooltipText = "Disallows the app from inhibiting your shortcuts."},
    {.name = "No screen share",
     .keyword = "no_screen_share",
     .tooltipText = "Hides the window and its popups from screen sharing by "
                    "drawing black rectangles in their place. The rectangles "
                    "are drawn even if other windows are above."},
    {.name = "No vrr",
     .keyword = "no_vrr",
     .tooltipText = "Disables VRR for the window. Only works when misc:vrr is "
                    "set to 2 or 3."},
    {.name = "Opaque",
     .keyword = "opaque",
     .tooltipText = "Forces the window to be opaque."},
    {.name = "Force rgbx",
     .keyword = "force_rgbx",
     .tooltipText =
         "Forces Hyprland to ignore the alpha channel on the whole window’s "
         "surfaces, effectively making it actually, fully 100% opaque."},
    {.name = "Sync fullscreen",
     .keyword = "sync_fullscreen",
     .tooltipText = "Whether the fullscreen mode should always be the same as "
                    "the one sent to the window (will only take effect on the "
                    "next fullscreen mode change)."},
    {.name = "Immediate",
     .keyword = "immediate",
     .tooltipText =
         "Forces the window to allow tearing. See the Tearing page."},
    {.name = "Xray",
     .keyword = "xray",
     .tooltipText = "Sets blur xray mode for the window."},
    {.name = "Render unfocused",
     .keyword = "render_unfocused",
     .tooltipText = "Forces the window to think it’s being rendered when it’s "
                    "not visible. See also Variables - Misc for setting "
                    "render_unfocused_fps"}

};

const SimpleRule simpleProps[] = {
    {.name = "Xwayland",
     .keyword = "xwayland",
     .tooltipText = "Xwayland windows."},
    {.name = "Float", .keyword = "float", .tooltipText = "Floating windows."},
    {.name = "Fullscreen",
     .keyword = "fullscreen",
     .tooltipText = "Fullscreen windows."},
    {.name = "Pinned", .keyword = "pin", .tooltipText = "Pinned windows."},
    {.name = "Focused",
     .keyword = "focus",
     .tooltipText = "Currently focused window."},
    {.name = "Grouped", .keyword = "group", .tooltipText = "Grouped windows."},
    {.name = "Modal",
     .keyword = "modal",
     .tooltipText = "Modal windows (e.g. “Are you sure” popups)"}};

const SimpleRule regexFields[] = {
    {.name = "Window Class",
     .keyword = "class",
     .tooltipText = "Windows with class matching RegEx below"},
    {.name = "Window Title",
     .keyword = "title",
     .tooltipText = "Windows with title matching RegEx below"},
    {.name = "Initial Class",
     .keyword = "initial_class",
     .tooltipText = "Windows with initialClass matching RegEx below"},
    {.name = "Inital tittle",
     .keyword = "initial_title",
     .tooltipText = "Windows with initialTitle matching RegEx below"},
    {.name = "Xdg tag",
     .keyword = "xdg_tag",
     .tooltipText = "Match a window by its xdgTag (see hyprctl clients to "
                    "check if it has one)"},
};
