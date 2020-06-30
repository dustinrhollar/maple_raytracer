#ifndef ENGINE_GRAPHICS_RENDER_COMMAND_H
#define ENGINE_GRAPHICS_RENDER_COMMAND_H

enum render_command_type
{
    RenderCmd_Draw,
    RenderCmd_UpdateGame,
    RenderCmd_DrawDevUi,
};

struct render_command_draw
{
    asset_id Asset;
};

struct render_command_draw_dev_ui
{
    maple_dev_ui_callback *Callback;
};

struct render_command
{
    render_command_type Type;
    
    union
    {
        render_command_draw        DrawCmd;
        render_command_draw_dev_ui DrawDevUi;
    };
};

#endif //ENGINE_GRAPHICS_RENDER_COMMAND_H
