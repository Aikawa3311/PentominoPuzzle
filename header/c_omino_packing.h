/**
 * @brief コンソール上で動くポリオミノパッキングパズル
*/

#pragma once

#include <utility>
#include <stack>
#include "polyomino.h"
#include "omino_packing.h"
#include "console_printer.h"
#include "console_option.h"
// #include "console_printer.h"
// #include "console_color.h"
// #include <curses.h>

namespace PolyominoPuzzle{
using namespace ConsoleOutput;

/**
 * @brief コンソール描画(console_printer.h)に対応したポリオミノクラス
 * @note console_option.hに対応(draw()とdraw_size())
*/
template <size_t omino_size>
struct DrawablePolyomino : public Polyomino<omino_size> {
    /**
     * @brief 描画
    */
    void draw(int const y, int const x) const {
        draw_helper(y, x);
    }

    /**
     * @brief 描画時のサイズ
     * @note omino_size分だけ取る(aabbではない)
    */
    std::pair<int, int> draw_size_about() const {
        return std::make_pair((int)omino_size * 2 + 1, (int)omino_size * 4 + 1);
    }

    /**
     * @brief 描画時のサイズ
     * @note ちゃんとサイズを測る
    */
    std::pair<int, int> draw_size() const {
        auto [_min, _max] = Polyomino<omino_size>::get_aabb();
        return std::make_pair((_max.y - _min.y + 1) * 2 + 1, (_max.x - _min.x + 1) * 4 + 1);
    }

private:
    /**
     * @brief 描画ヘルパ
     * @note 現状ギザつくバグあり
    */
    void draw_helper(int const y, int const x) const {
        Board b = Polyomino<omino_size>::get_board();

        // 一番上
        bool flag_left = true;
        for(int i=0; i<(int)b.w_size; ++i){
            if(b[i][0] < 0) continue;
            if(flag_left){
                std::cout << MovCursor(y, x+i*4) << "┏";
                flag_left = false;
            }
            if(i == (int)b.w_size-1 || b[i+1][0] < 0){
                std::cout << "━━━┓";
                flag_left = true;
            }else{
                std::cout << "━━━━";
            }
        }

        for(int i=0; i<(int)b.h_size; ++i){
            // 各行
            flag_left = true;
            for(int j=0; j<(int)b.w_size; ++j){
                // 真ん中
                if(b[j][i] < 0) continue;
                if(flag_left){
                    std::cout << MovCursor(y+i*2+1, x+j*4) << "┃";
                    flag_left = false;
                }

                if(j == (int)b.w_size - 1) break;
                if(b[j+1][i] < 0){
                    std::cout << "   ┃";
                    flag_left = true;
                }else{
                    std::cout << "    ";
                }
            }
            // 一番右
            if(b.board.back()[i] >= 0){
                std::cout << "   ┃";
            }

            // 下側
            flag_left = true;
            // 一番左
            std::cout << MovCursor(y+i*2+2, x);
            if(b.board.front()[i] >= 0){
                if(i+1 >= (int)b.h_size || b.board.front()[i+1] < 0){
                    std::cout << "┗";
                }else{
                    std::cout << "┃";
                }
                flag_left = false;
            }else{
                if(i+1 < (int)b.h_size && b.board.front()[i+1] >= 0){
                    std::cout << "┏";
                    flag_left = false;
                }
            }
            // 交差部分
            for(int j=0; j<(int)b.w_size; ++j){
                int flag = 0x00000000;
                flag |= (b[j  ][i  ] >= 0);                                     // 十字の左上
                flag |= (j < (int)b.w_size-1 && b[j+1][i  ] >= 0) << 1;         // 十字の右上
                flag |= (i < (int)b.h_size-1 && b[j  ][i+1] >= 0) << 2;         // 十字の左下
                flag |= ( ((i < (int)b.h_size-1) && (j < (int)b.w_size-1)) && b[j+1][i+1] >= 0) << 3;    // 十字の右下

                std::cout << MovCursor(y+i*2+2, x+j*4+1);
                if(flag != 15 && flag >= 8) flag = 15 - flag;
                // 空白が3つ付く部分は調整する
                switch(flag){
                case  0: break;
                case  1: std::cout << "━━━┛"; break;
                case  2:
                    if(flag_left){
                        std::cout << MovCursor(y+i*2+2, x+j*4+4) << "┗";
                        flag_left = false;
                    } else {
                        std::cout << "   ┗";
                    }
                    break;
                case  3: std::cout << "━━━━"; break;
                case  4: std::cout << "━━━┓"; break;
                case  5: 
                    if(flag_left){
                        std::cout << MovCursor(y+i*2+2, x+j*4+4) << "┃";
                        flag_left = false;
                    } else {
                        std::cout << "   ┃";
                    }
                    break;
                case  6: std::cout << "━━━╋"; break;
                case  7: 
                    if(flag_left){
                        std::cout << MovCursor(y+i*2+2, x+j*4+4) << "┏";
                        flag_left = false;
                    } else {
                        std::cout << "   ┏";
                    }
                    break;
                case  8: 
                    if(flag_left){
                        std::cout << MovCursor(y+i*2+2, x+j*4+4) << "┏";
                        flag_left = false;
                    } else {
                        std::cout << "   ┏";
                    }
                    break;
                case  9: std::cout << "━━━╋"; break;
                case 15: std::cout << "    "; break;
                default: std::cout << "   ?";
                }
            }
        }
    }
};


/**
 * @brief コンソール上のパッキングパズル
 * @note DrawablePolyominoを与える
*/
template <typename DOmino>
struct PackingPuzzleGame{
    PackingPuzzle<DOmino> puzzle;
    ItemsOption<DOmino> omino_option;
    std::stack<int> pre_put;            // 前回配置したピースid, 現在ピースをいくつ使用しているか
    Coord put_pos;                      // ピースの配置位置
    int selection_piece{}, piece_pattern{};
    bool flag_putting{};                // ピース配置中を示すフラグ
    bool flag_complete{};               // パズルを完成させたかどうか
    int board_draw_x, board_draw_y;     // ボードの描画位置
    int remain = -1;                    // 現状から作れる解の個数
    int remain_threshold = 12;          // 残り何ピースになってから解の個数更新を開始するか

    PackingPuzzleGame(std::vector<std::string> const & b, int const per_page = 5) : puzzle(b), omino_option(puzzle.base, per_page){
        omino_option.set_pos(puzzle.board.h_size*2+1 + 2, 2);
        board_draw_x = (omino_option.item_size_x * per_page + omino_option.margin * (per_page - 1)) / 2 - ((int)puzzle.board.w_size * 4 + 1) / 2;
        board_draw_y = 1;

        // 一旦解いて解の個数を計算
        update_remain();
    }

    /**
     * @brief ピースを選択する操作、キー受付待ちあり
    */
    void select_piece(){
        char key = getch();
        switch(key){
        case 'q': exit(0); break;
        case 'p': reset(); break;
        case 'u': undo(); break;
        }

        int id = omino_option.choice_without_keyinput(key);
        if(id < 0) return;
        // 選択したピースidを取得、フラグ更新
        selection_piece = omino_option.choiceid();
        put_pos = Coord(0, 0);
        piece_pattern = 0;
        flag_putting = true;
    }

    /**
     * @brief 選択したピースの配置操作、キー受付待ちあり
    */
    void select_put_pos(){
        char key = getch();
        int dx = 0, dy = 0;
        switch(key){
        case 'a': dx = -1; break;
        case 'w': dy = -1; break;
        case 's': dy =  1; break;
        case 'd': dx =  1; break;
        case 'r': next_pattern(); break;
        case 'z': put_piece(); break;
        case 'x': flag_putting = false; break;
        }
        
        // ピース位置を更新
        if(puzzle.board.in(puzzle.pattern[selection_piece][piece_pattern], put_pos.moved_by(dx, dy))){
            put_pos.move_by(dx, dy);
        }
    }

    /**
     * @brief 選択中ピースの回転/鏡像パターン変更
    */
    void next_pattern(){
        ++piece_pattern;
        if(piece_pattern == (int)puzzle.pattern[selection_piece].size()) piece_pattern = 0;

        // 回転/鏡像でボードをはみ出してしまう場合ははみ出さない位置までput_posを戻す
        // if(puzzle.board.in(puzzle.pattern[selection_piece][piece_pattern], put_pos)) return;
        auto [_min, _max] = puzzle.pattern[selection_piece][piece_pattern].get_aabb();
        Coord over = put_pos + _max - _min - Coord((int)puzzle.board.w_size - 1, (int)puzzle.board.h_size - 1);
        if(over.x > 0) put_pos.x -= over.x;
        if(over.y > 0) put_pos.y -= over.y;
    }

    /**
     * @brief 選択中のピースを選択した位置に配置、置けるかどうかの判定含む
    */
    void put_piece(){
        // 置けない場合終了
        if(!puzzle.board.putable(puzzle.pattern[selection_piece][piece_pattern], put_pos)) return;

        // boardの更新
        puzzle.board.put_piece(puzzle.pattern[selection_piece][piece_pattern], put_pos, selection_piece);
        // unuseのフラグなどを更新
        puzzle.unuse[selection_piece] = false;
        omino_option.set_selectability(selection_piece, false);
        flag_putting = false;

        // undo用に配置したピース記憶
        pre_put.emplace(selection_piece);
        if((int)pre_put.size() == (int)puzzle.base.size()){
            flag_complete = true;
        }

        // 解の個数を更新
        update_remain();
    }

    /**
     * @brief 直前に配置したピースを除く
    */
    void undo(){
        if(pre_put.empty()) return;
        int id = pre_put.top();
        pre_put.pop();

        // ボード更新(愚直に探す)
        puzzle.board.remove_piece(id);
        // unuseのフラグを戻す
        puzzle.unuse[id] = true;
        omino_option.set_selectability(id, true);

        // 解の個数を更新
        update_remain();
    }

    /**
     * @brief ボード初期化/ピースの使用状況初期化
    */
    void reset(){
        puzzle.board.fill(EMPTY);
        for(int i=0; i<(int)puzzle.unuse.size(); ++i){
            puzzle.unuse[i] = true;
            omino_option.set_selectability(i, true);
        }

        // 解の個数を更新
        update_remain();
    }

    /**
     * @brief 解の個数を更新する
     * @note ポリオミノパズルを解くため計算が重い
    */
    void update_remain(){
        if((int)puzzle.base.size() - (int)pre_put.size() > remain_threshold) return;
        puzzle.ans.clear();
        puzzle.solve({0, 0}, (int)pre_put.size());
        remain = (int)puzzle.ans.size();
        if(pre_put.empty()) remain /= 4;
    }

    /**
     * @brief 任意位置にボード描画
     * @note DrawableBoardを作ってそっちに移住させたい
    */
    void draw_board(int const y, int const x) const {
        // 一番上
        Board const & b = puzzle.board;
        std::cout << MovCursor(y, x) << "┏";
        for(int i=0; i<(int)b.w_size-1; ++i){
            if(b[i][0] == b[i+1][0]) std::cout << "━━━━";
            else std::cout << "━━━┳";
        }
        std::cout << "━━━┓";

        for(int i=0; i<(int)b.h_size; ++i){
            // 各行
            std::cout << MovCursor(y+i*2+1, x) << "┃"; // 一番左
            for(int j=0; j<(int)b.w_size-1; ++j){
                std::string c;
                if(b[j][i] == HOLE) c = "ﾇ";
                else if(b[j][i] == EMPTY) c = "#";
                else c = " ";

                if(b[j][i] == b[j+1][i]) std::cout << c << c << c << c;
                else std::cout << c << c << c << "┃";
            }
            if(b.board.back()[i] == HOLE){
                std::cout << "ﾇﾇﾇ┃"; // 一番右
            }else if(b.board.back()[i] == EMPTY){
                std::cout << "###┃"; // 一番右
            }else{
                std::cout << "   ┃"; // 一番右
            }

            // 縦区切り
            std::cout << MovCursor(y+i*2+2, x);
            if(i == (int)b.h_size - 1) break;
            if(b[0][i] == b[0][i+1]) std::cout << "┃";
            else std::cout << "┣";
            for(int j=0; j<(int)b.w_size-1; ++j){
                int flag = 0x00000000;
                flag |= (b[j  ][i  ] != b[j+1][i  ]);       // 十字の上
                flag |= (b[j  ][i  ] != b[j  ][i+1]) << 1;  // 十字の左
                flag |= (b[j  ][i+1] != b[j+1][i+1]) << 2;  // 十字の下
                flag |= (b[j+1][i  ] != b[j+1][i+1]) << 3;  // 十字の右
                switch(flag){
                case  0: std::cout << "    "; break;
                case  3: std::cout << "━━━┛"; break;
                case  5: std::cout << "   ┃"; break;
                case  6: std::cout << "━━━┓"; break;
                case  7: std::cout << "━━━┫"; break;
                case  9: std::cout << "   ┗"; break;
                case 10: std::cout << "━━━━"; break;
                case 11: std::cout << "━━━┻"; break;
                case 12: std::cout << "   ┏"; break;
                case 13: std::cout << "   ┣"; break;
                case 14: std::cout << "━━━┳"; break;
                case 15: std::cout << "━━━╋"; break;
                default: std::cout << "   ?";
                }
            }
            if(b.board.back()[i] == b.board.back()[i+1]) std::cout << "   ┃";
            else std::cout << "━━━┫";
        }

        // 一番下
        std::cout << MovCursor(y+(int)b.h_size*2, x) << "┗";
        for(int i=0; i<(int)b.w_size-1; ++i){
            if(b[i].back() == b[i+1].back()) std::cout << "━━━━";
            else std::cout << "━━━┻";
        }
        std::cout << "━━━┛" << std::endl;
    }

    /**
     * @brief 実行
    */
    void run(){
        // ピース選択中・ボードの配置場所選択中
        if(flag_putting){
            // ボードの配置場所選択中
            select_put_pos();
        }else{
            // ピース選択中、キー入力1回で抜ける
            // int res = omino_option.choice();
            // if(res >= 0){
            //     selection_piece = omino_option.choiceid();
            //     put_pos = Coord(0, 0);
            //     piece_pattern = 0;
            //     flag_putting = true;
            // }
            select_piece();
        }
    }

    /**
     * @brief 描画
    */
    void draw() const {
        // clear_print();
        draw_board(board_draw_y, board_draw_x);
        omino_option.draw();
        if(flag_putting){
            puzzle.pattern[selection_piece][piece_pattern].draw(board_draw_y + put_pos.y * 2, board_draw_x + put_pos.x * 4);
        }
        auto [_y, _x] = omino_option.get_bottomright();
        draw_description(board_draw_y, _x + 10);
    }

    /**
     * @brief 右枠の描画
    */
    void draw_description(int const y, int const x) const {
        if(flag_putting){
            std::cout << MovCursor(y  , x) << OutputClearLine(0) << "awsd : ピース移動";
            std::cout << MovCursor(y+1, x) << OutputClearLine(0) << "r    : 回転/反転";
            std::cout << MovCursor(y+2, x) << OutputClearLine(0) << "z    : 置く";
            std::cout << MovCursor(y+3, x) << OutputClearLine(0) << "x    : キャンセル";
            std::cout << MovCursor(y+4, x) << OutputClearLine(0);
            std::cout << MovCursor(y+6, x) << OutputClearLine(0) << "ここから作れる解の個数: " << remain;
        }else{
            std::cout << MovCursor(y  , x) << OutputClearLine(0) << "ad   : ピース選択";
            std::cout << MovCursor(y+1, x) << OutputClearLine(0) << "z    : ピースの決定";
            std::cout << MovCursor(y+2, x) << OutputClearLine(0) << "k    : 盤面のクリア";
            std::cout << MovCursor(y+3, x) << OutputClearLine(0) << "u    : アンドゥ";
            std::cout << MovCursor(y+4, x) << OutputClearLine(0) << "q    : 終了";
            std::cout << MovCursor(y+6, x) << OutputClearLine(0) << "ここから作れる解の個数: " << remain;
            if(flag_complete) std::cout << MovCursor(y+8, x) << "完成！！";
        }
    }
};

using DTromino = DrawablePolyomino<3>;
using DTetromino = DrawablePolyomino<4>;
using DPentomino = DrawablePolyomino<5>;
using DHexomino = DrawablePolyomino<6>;
using DHeptomino = DrawablePolyomino<7>;
using DOctomino = DrawablePolyomino<8>;
using DNonomino = DrawablePolyomino<9>;
using DDecomino = DrawablePolyomino<10>;

} // namespace PolyominoPuzzle