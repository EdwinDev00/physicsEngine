#pragma once

#include "config.h"

namespace input
{
#pragma region KeyCode
    class Key
    {
    public:
        /// key codes
        enum Code
        {
            Back,
            Tab,
            Clear,
            Return,
            Shift,
            Control,
            Alt,
            Menu,
            Pause,
            Capital,
            Escape,
            Convert,
            NonConvert,
            Accept,
            ModeChange,
            Space,
            Prior,
            Next,
            End,
            Home,
            PageUp,
            PageDown,
            Left,
            Right,
            Up,
            Down,
            Select,
            Print,
            Execute,
            Snapshot,
            Insert,
            Delete,
            Help,
            LeftWindows,
            RightWindows,
            Apps,
            Sleep,
            NumPad0,
            NumPad1,
            NumPad2,
            NumPad3,
            NumPad4,
            NumPad5,
            NumPad6,
            NumPad7,
            NumPad8,
            NumPad9,
            NumPadEnter,
            Multiply,
            Add,
            Subtract,
            Separator,
            Decimal,
            Divide,
            F1,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,
            F13,
            F14,
            F15,
            F16,
            F17,
            F18,
            F19,
            F20,
            F21,
            F22,
            F23,
            F24,
            NumLock,
            Scroll,
            Semicolon,
            Slash,
            Tilde,
            LeftBracket,
            RightBracket,
            BackSlash,
            Quote,
            Comma,
            Underbar,
            Period,
            Equality,
            LeftShift,
            RightShift,
            LeftControl,
            RightControl,
            LeftAlt,
            RightAlt,
            LeftMenu,
            RightMenu,
            BrowserBack,
            BrowserForward,
            BrowserRefresh,
            BrowserStop,
            BrowserSearch,
            BrowserFavorites,
            BrowserHome,
            VolumeMute,
            VolumeDown,
            VolumeUp,
            MediaNextTrack,
            MediaPrevTrack,
            MediaStop,
            MediaPlayPause,
            LaunchMail,
            LaunchMediaSelect,
            LaunchApp1,
            LaunchApp2,
            Key0,
            Key1,
            Key2,
            Key3,
            Key4,
            Key5,
            Key6,
            Key7,
            Key8,
            Key9,
            A,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,

            NumKeyCodes,
            InvalidKey,
        };

        static Key::Code FromGLFW(int32 key);
    };
#pragma endregion

#pragma region Mouse Button 

    class MouseButton
    {
    public:
        // mouse codes
        enum Code
        {
            left,
            right,
            middle,

            numButtons,
            InvalidButton,
        };

        static MouseButton::Code FromGLFW(int32 button);
    };
#pragma endregion

    struct Keyboard
    {
        bool pressed[Key::NumKeyCodes];
        bool held[Key::NumKeyCodes];
        bool released[Key::NumKeyCodes];
    };

    struct Mouse
    {
        //position
        float x, y;
        float dx, dy;

        // NDC position
        float nx, ny;

        float lastX, lastY;

        //buttons
        bool pressed[MouseButton::numButtons];
        bool held[MouseButton::numButtons];
        bool released[MouseButton::numButtons];
    };
    class InputHandler
    {
        private:
           static InputHandler* instance;
           InputHandler(){};
           ~InputHandler(){};
        public:
           void operator=(const InputHandler&) = delete;
           InputHandler(InputHandler& other) = delete;
           static InputHandler* Instance();

           Keyboard keyboard;
           Mouse mouse;

           void BeginFrame();
           void HandleKeyEvent(int32 key, int32 action);
           void HandleMousePosition(float64 x, float64 y, int32 win_width, int32 win_height);
           void HandleMouseEvent(int32 button, int32 action);
    };
}
