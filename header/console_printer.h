#pragma once

#include <iostream>
#include <iomanip>
#include <string>
// #include <curses.h>

namespace ConsoleOutput{

/**
 * @brief カーソル位置変更マニピュレータ
*/
struct MovCursor{
    int x, y;
    MovCursor(int const _y, int const _x) : x(_x), y(_y){}

    friend std::ostream & operator << (std::ostream & output, MovCursor const & rhs){
        output << "\033[" << rhs.y << ";" << rhs.x << "H";
        return output;
    }
};

/**
 * @brief カーソルの行位置を相対的に変更する, +で下方向, -で上方向
*/
struct MovCursorLine{
    int line;
    MovCursorLine(int const _line) : line(_line){}

    friend std::ostream & operator << (std::ostream & output, MovCursorLine const & rhs){
        if(rhs.line > 0) output << "\033[" << rhs.line << "E";
        else output << "\033[" << -rhs.line << "F";
        return output;
    }
};

/**
 * @brief ターミナルの出力状況をクリア
*/
struct OutputClear{
    int mode;

    /**
     * @brief 画面消去
     * @param[in] _mode 消去する範囲を指定(0:カーソルより後 1:カーソルより前 2:全画面)
    */
    OutputClear(int const _mode = 2) : mode(_mode){}

    friend std::ostream & operator << (std::ostream & output, OutputClear const & rhs){
        output << "\033[" << rhs.mode << "J";
        return output;
    }
};

/**
 * @brief カーソルがある行の出力状況をクリア
*/
struct OutputClearLine{
    int mode;

    /**
     * @brief 画面消去
     * @param[in] _mode 消去する範囲を指定(0:カーソルより後 1:カーソルより前 2:行全体)
    */
    OutputClearLine(int const _mode = 2) : mode(_mode){}

    friend std::ostream & operator << (std::ostream & output, OutputClearLine const & rhs){
        output << "\033[" << rhs.mode << "K";
        return output;
    }
};

/**
 * @brief 位置を指定して文字列を出力
*/
void printstr_at(int const y, int const x, std::string_view str){
    std::cout << MovCursor(y, x) << str;
}

/**
 * @brief 画面クリア
*/
void clear_print(){
    std::cout << OutputClear();
}

/**
 * @brief 行を相対的に指定して消去
*/
void clear_print_line(int const line){
    std::cout << MovCursorLine(line) << OutputClearLine();
}

/**
 * @brief 指定領域の出力状況を半角スペースによってクリア
 * @param[in] y 領域の左上y座標(1-indexed)
 * @param[in] x 領域の左上x座標(1-indexed)
 * @param[in] h 領域の高さ
 * @param[in] w 領域の幅
 * @note 大きな領域を消してリフレッシュすると画面がチラつくため注意
*/
void clear_print_area(int const y, int const x, int const h, int const w){
    for(int i=0; i<h; ++i){
        std::cout << MovCursor(y+i, x);
        for(int j=0; j<w; ++j){
            std::cout << ' ';
        }
    }
}


} // namespace ConsoleOutput