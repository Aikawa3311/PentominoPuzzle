#pragma once

#include <iostream>
#include <iomanip>

namespace ConsoleOutput{

/**
 * @brief 標準出力で文字色を扱うためのクラス
*/
struct ConsoleColor{
    unsigned char r{}, g{}, b{};
    bool c256 = true;       // rgb256色対応
    bool bg = false;        // 背景色

    /**
     * @brief 文字色/背景色の色指定
     * @param[in] r 赤, c256がtrueなら 0-黒|1-赤|2-緑|3-黄|4-青|5-マゼンタ|6-シアン|7-白|9-デフォルト
     * @param[in] g 緑
     * @param[in] b 青
     * @param[in] c256 rgb256色を使う場合true
     * @param[in] bg 背景色変更ならtrue
    */
    constexpr ConsoleColor(unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0, bool _c256 = true, bool _bg = false) : r(_r), g(_g), b(_b), c256(_c256), bg(_bg){}
    // ConsoleColor(unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0, bool _c256 = true, bool _bg = false) : r(_r), g(_g), b(_b), c256(_c256), bg(_bg){}

    /**
     * @brief マニピュレータ
    */
    friend std::ostream & operator << (std::ostream & out, ConsoleColor const & col){
        if(col.c256) out << "\033[" << 3 + col.bg << "8;2;" << (int)col.r << ';' << (int)col.g << ';' << (int)col.b << 'm';
        else out << "\033[" << 3 + col.bg << (int)col.r << "m";
        return out;
    }
};

ConsoleColor constexpr C_Black(0, 0, 0, false, false);
ConsoleColor constexpr C_Red(1, 0, 0, false, false);
ConsoleColor constexpr C_Green(2, 0, 0, false, false);
ConsoleColor constexpr C_Yellow(3, 0, 0, false, false);
ConsoleColor constexpr C_Blue(4, 0, 0, false, false);
ConsoleColor constexpr C_Magenta(5, 0, 0, false, false);
ConsoleColor constexpr C_Cyan(6, 0, 0, false, false);
ConsoleColor constexpr C_White(7, 0, 0, false, false);
ConsoleColor constexpr C_Clear(9, 0, 0, false, false);

ConsoleColor constexpr CB_Black(0, 0, 0, false, true);
ConsoleColor constexpr CB_Red(1, 0, 0, false, true);
ConsoleColor constexpr CB_Green(2, 0, 0, false, true);
ConsoleColor constexpr CB_Yellow(3, 0, 0, false, true);
ConsoleColor constexpr CB_Blue(4, 0, 0, false, true);
ConsoleColor constexpr CB_Magenta(5, 0, 0, false, true);
ConsoleColor constexpr CB_Cyan(6, 0, 0, false, true);
ConsoleColor constexpr CB_White(7, 0, 0, false, true);
ConsoleColor constexpr CB_Clear(9, 0, 0, false, true);

}
/*
std::cout << C_Red << 12 << C_Clear << std::endl;
std::cout << ConsoleColor(122, 122, 122) << 12 << C_Clear << std::endl;
std::cout << CB_Red << C_Red << 12 << C_Clear << CB_Clear << std::endl;
*/