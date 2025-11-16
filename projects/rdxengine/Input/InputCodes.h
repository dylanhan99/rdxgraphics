#ifndef INPUTCODES_H
#define INPUTCODES_H

namespace rdx
{
	enum class KeyCode
	{
		Unknown,

		Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9, Alpha0,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

		LCtrl, RCtrl,
		LAlt, RAlt,
		LShift, RShift,
		Apostrophe, Semicolon, Comma, Period,
		Slash, BackSlash,
		Minus, Equal,
		LBracket, RBracket,
		GraveAccent, // `

		Left, Right, Up, Down,
		Escape, Enter, Space, Backspace, Tab,
		Insert, Delete,
		PageUp, PageDown,
		Home, End, PrintScreen,
		CapsLock, ScrollLock, NumLock, Pause,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

		KeyPad_1, KeyPad_2, KeyPad_3, KeyPad_4, KeyPad_5, KeyPad_6, KeyPad_7, KeyPad_8, KeyPad_9, KeyPad_0,
		KeyPad_Decimal, KeyPad_Divide, KeyPad_Multiply, KeyPad_Subtract, KeyPad_Add,
		KeyPad_Enter, KeyPad_Equal,

		MAX
	};
	enum class MouseCode
	{

	};
}

#endif