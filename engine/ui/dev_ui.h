#ifndef ENGINE_UI_DEV_UI_H
#define ENGINE_UI_DEV_UI_H

struct frame_params;

#define MAPLE_DEV_UI_CALLBACK(fn) void fn(frame_params *FrameParams)
typedef MAPLE_DEV_UI_CALLBACK(maple_dev_ui_callback);

MAPLE_DEV_UI_CALLBACK(RaytraceCallback);

#endif //ENGINE_UI_DEV_UI_H
