#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <queue>
#include <utility>
#include "console_color.h"

namespace PolyominoPuzzle{

using namespace ConsoleOutput;

const static int EMPTY   = -1; // 盤面に何もない状態を表す数値
const static int HOLE    = -2; // 盤面上で置くことができない状態を表す数値、EMPTYにもならない
const static int INVALID = -3; // 盤面上で無効な値、探索などに使用

/**
 * @brief 二次元整数座標
*/
struct Coord{
    int x;
    int y;

    // Coord(int const _x = 0, int const _y = 0) : x(_x), y(_y){}
    // Coord(std::initializer_list<int> const & list) : x(*(list.begin())), y(*(list.end())){}

    bool operator == (Coord const & rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool operator != (Coord const & rhs) const {
        return !(*this == rhs);
    }

    Coord & operator = (Coord const & rhs){
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    Coord operator - () const {
        Coord res = *this;
        res.x *= -1;
        res.y *= -1;
        return res;
    }

    Coord operator + (Coord const & rhs) const {
        Coord res = *this;
        res += rhs;
        return res;
    }

    Coord & operator += (Coord const & rhs){
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Coord operator - (Coord const & rhs) const {
        Coord res = *this;
        res -= rhs;
        return res;
    }

    Coord & operator -= (Coord const & rhs){
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    bool operator < (Coord const & rhs) const {
        return x == rhs.x ? (y < rhs.y) : (x < rhs.x);
    }

    bool operator > (Coord const & rhs) const {
        return x == rhs.x ? (y > rhs.y) : (x > rhs.x);
    }

    friend std::ostream & operator << (std::ostream & output, Coord const & rhs){
        output << rhs.x << " " << rhs.y;
        return output;
    }

    /**
     * @brief 座標を移動させる
     * @param[in] dx x移動量
     * @param[in] dy x移動量
    */
    void move_by(int const dx, int const dy){
        this->x += dx;
        this->y += dy;
    }

    /**
     * @brief 座標を移動させる
     * @param[in] c 移動量
    */
    void move_by(Coord const & c){
        move_by(c.x, c.y);
    }

    /**
     * @brief 移動させた座標を返す
     * @param[in] dx x移動量
     * @param[in] dy y移動量
    */
    Coord moved_by(int const dx, int const dy) const {
        Coord res = *this;
        res.x += dx;
        res.y += dy;
        return res;
    }

    /**
     * @brief 移動させた座標を返す
     * @param[in] c 移動量
    */
    Coord moved_by(Coord const & c) const {
        Coord res = moved_by(c.x, c.y);
        return res;
    }
};



/**
 * @brief 盤面、右がx軸正、**下がy軸正**
*/
struct Board{
    std::vector<std::vector<int>> board;
    size_t w_size, h_size;

    Board(size_t const _w = 0, size_t const _h = 0, int const _init = EMPTY) : board(_w, std::vector<int>(_h, _init)), w_size(_w), h_size(_h){}
    Board(std::vector<std::string> const & b) : board(b.front().size(), std::vector<int>(b.size())), w_size(b.front().size()), h_size(b.size()){
        // stringの.はEMPTYに、#はHOLEに置き換え
        string_to_board(b);
    }

    /**
     * @brief 左下から右上にかけて探索し、検索対象の値があるかどうか調べる 無かったら-1, -1
     * @param[in] pos 探索を開始する位置
     * @param[in] n 調べる対象となる値
    */
    Coord get_bottomleft(Coord pos, int const n){
        while(pos.x < (int)w_size){
            if(board[pos.x][pos.y] == n) return pos;
            --pos.y;
            if(pos.y < 0){
                ++pos.x;
                pos.y = h_size - 1;
            }
        }
        return {-1, -1};
    }

    /**
     * @brief 左上から右下にかけて探索し、検索対象の値があるかどうか調べる 無かったら-1, -1
     * @param[in] pos 探索を開始する位置
     * @param[in] n 調べる対象となる値
    */
    Coord get_topleft(Coord pos, int const n){
        while(pos.x < (int)w_size){
            // std::cout << "[探索中]:(" << pos << ") : " << board[pos.x][pos.y] << " == " << n << std::endl;
            if(board[pos.x][pos.y] == n) return pos;
            ++pos.y;
            if(pos.y >= (int)h_size){
                ++pos.x;
                pos.y = 0;
            }
        }
        return {-1, -1};
    }

    /**
     * @brief HOLE以外の全てのマスをある値で埋める
    */
    void fill(int const val){
        for(int i=0; i<(int)w_size; ++i){
            for(int j=0; j<(int)h_size; ++j){
                if(board[i][j] != HOLE) board[i][j] = val;
            }
        }
    }

    /**
     * @brief 盤面に収まる座標かをチェック
    */
    inline bool in(Coord const & c) const {
        return c.x >= 0 && c.x < (int)w_size && c.y >= 0 && c.y < (int)h_size;
    }

    /**
     * @brief ポリオミノが盤面に収まるかチェック
     * @param[in] omino ポリオミノ
     * @param[in] coord 配置場所
    */
    template <typename OminoType>
    bool in(OminoType const & omino, Coord const coord) const {
        Coord base = omino.get_aabb_min_pos();
        for(int i=0; i<(int)omino.elem.size(); ++i){
            Coord c = coord + omino.elem[i] - base;
            if(!in(c)) return false;
        }
        return true;
    }

    /**
     * @brief ポリオミノが置けるかどうかをチェック
     * @param[in] omino ポリオミノ
     * @param[in] coord 配置場所
    */
    template <typename OminoType>
    bool putable(OminoType const & omino, Coord const coord) const {
        Coord base = omino.get_aabb_min_pos();
        for(int i=0; i<(int)omino.elem.size(); ++i){
            Coord c = coord + omino.elem[i] - base;
            if(!in(c) || board[c.x][c.y] != EMPTY) return false;
        }
        return true;
    }
    
    /**
     * @brief ポリオミノを配置、判定は含まない
     * @param[in] omino ポリオミノ
     * @param[in] coord 配置場所
     * @param[in] val ポリオミノのid
    */
    template <typename OminoType>
    void put_piece(OminoType const & omino, Coord const coord, int const val){
        Coord base = omino.get_aabb_min_pos();
        for(int i=0; i<(int)omino.elem.size(); ++i){
            Coord c = coord + omino.elem[i] - base;
            board[c.x][c.y] = val;
        }
    }

    /**
     * @brief 指定したポリオミノを取り除きEMPTYにする
     * @param[in] id 取り除くポリオミノのid
    */
    void remove_piece(int const id){
        for(int i=0; i<(int)w_size; ++i){
            for(int j=0; j<(int)h_size; ++j){
                if(board[i][j] == id) board[i][j] = EMPTY;
            }
        }
    }

    /**
     * @brief 文字列から盤面を生成 .がEMPTY #がHOLE
     * @param[in] b 文字列の配列
    */
    void string_to_board(std::vector<std::string> const & b){
        w_size = b.front().size(), h_size = b.size();
        board.resize(w_size, std::vector<int>(h_size));

        for(int i=0; i<(int)w_size; ++i){
            for(int j=0; j<(int)h_size; ++j){
                if(b[j][i] == '.'){
                    board[i][j] = EMPTY;
                }else if(b[j][i] == '#'){
                    board[i][j] = HOLE;
                }
            }
        }
    }

    /**
     * @brief EMPTYなマスがどれくらい隣接しているかを計算 
    */
    int calc_empty_area(Coord c){
        int res = 0;
        std::queue<Coord> q;
        std::queue<Coord> q_copy;
        std::vector<Coord> near = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        q.push(c);
        q_copy.push(c);
        operator[](c) = INVALID;
        while(!q.empty()){
            ++res;
            c = q.front();
            q.pop();

            for(int i=0; i<4; ++i){
                Coord tmp_c = c + near[i];
                if(in(tmp_c) && operator[](tmp_c) == EMPTY) { q.push(tmp_c); q_copy.push(tmp_c); operator[](tmp_c) = INVALID; }
            }
        }

        while(!q_copy.empty()){
            c = q_copy.front();
            q_copy.pop();
            operator[](c) = EMPTY;
        }

        return res;
    }

    /**
     * @brief 与えられたマスが孤立したスペースかどうかを計算
     * @param[in] p 座標
    */
    bool is_isolated_space(Coord p){
        bool x_out = p.x+1 >= (int)w_size, y_out = p.y+1 >= (int)h_size;
        bool x_imp = x_out || board[p.x+1][p.y] != EMPTY, y_imp = y_out || board[p.x][p.y+1] != EMPTY;
        // if((!y_out && board[p.x][p.y+1] == EMPTY) && (!x_out && board[p.x+1][p.y] == EMPTY)) return false;
        // if((y_out || board[p.x][p.y+1] != EMPTY) && (x_out || board[p.x+1][p.y] != EMPTY)) return true;
        if(x_imp && y_imp) return true;
        if(x_imp && (p.y+2 >= (int)h_size || board[p.x][p.y+2] != EMPTY) && (x_out || board[p.x+1][p.y+1] != EMPTY)) return true;
        if(y_imp && (p.x+2 >= (int)w_size || board[p.x+2][p.y] != EMPTY) && (y_out || board[p.x+1][p.y+1] != EMPTY) && (p.y-1 < 0 || board[p.x+1][p.y-1] != EMPTY)) return true;
        return false;
    }

    /**
     * @brief ボード同士が等しいかどうかを計算
    */
    bool is_same(Board const & b) const {
        if(w_size != b.w_size || h_size != b.h_size) return false;
        for(int i=0; i<(int)w_size; ++i){
            for(int j=0; j<(int)h_size; ++j){
                if(board[i][j] != b[i][j]) return false;
            }
        }
        return true;
    }

    /**
     * @brief 描画1
    */
    void print_primitive() const {
        // 一番上
        std::cout << "+";
        for(int i=0; i<(int)w_size; ++i){
            std::cout << "---+";
        }
        std::cout << std::endl;

        for(int i=0; i<(int)h_size; ++i){
            // 各行
            std::cout << "|"; // 一番左
            for(int j=0; j<(int)w_size-1; ++j){
                if(board[j][i] == board[j+1][i]) std::cout << "    ";
                else std::cout << "   |";
            }
            std::cout << "   |" << std::endl; // 一番右

            // 縦区切り
            if(i == (int)(int)h_size - 1) break;
            std::cout << "+";
            for(int j=0; j<(int)w_size; ++j){
                if(board[j][i] == board[j][i+1]) std::cout << "   +";
                else std::cout << "---+";
            }
            std::cout << std::endl;
        }

        // 一番下
        std::cout << "+";
        for(int i=0; i<(int)w_size; ++i){
            std::cout << "---+";
        }
        std::cout << std::endl;
    }

    /**
     * @brief 描画2
    */
    void print_primitive2() const {
        // 一番上
        std::cout << "┏";
        for(int i=0; i<(int)w_size-1; ++i){
            if(board[i][0] == board[i+1][0]) std::cout << "━━━━";
            else std::cout << "━━━┳";
        }
        std::cout << "━━━┓" << std::endl;

        for(int i=0; i<(int)h_size; ++i){
            // 各行
            std::cout << "┃"; // 一番左
            for(int j=0; j<(int)w_size-1; ++j){
                if(board[j][i] == HOLE){
                    if(board[j][i] == board[j+1][i]) std::cout << "ﾇﾇﾇﾇ";
                    else std::cout << "ﾇﾇﾇ┃";
                }else{
                    if(board[j][i] == board[j+1][i]) std::cout << "    ";
                    else std::cout << "   ┃";
                }
            }
            if(board.back()[i] == HOLE){
                std::cout << "ﾇﾇﾇ┃" << std::endl; // 一番右
            }else{
                std::cout << "   ┃" << std::endl; // 一番右
            }

            // 縦区切り
            if(i == (int)h_size - 1) break;
            if(board[0][i] == board[0][i+1]) std::cout << "┃";
            else std::cout << "┣";
            for(int j=0; j<(int)w_size-1; ++j){
                int flag = 0x00000000;
                flag |= (board[j  ][i  ] != board[j+1][i  ]);       // 十字の上
                flag |= (board[j  ][i  ] != board[j  ][i+1]) << 1;  // 十字の左
                flag |= (board[j  ][i+1] != board[j+1][i+1]) << 2;  // 十字の下
                flag |= (board[j+1][i  ] != board[j+1][i+1]) << 3;  // 十字の右
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
            if(board.back()[i] == board.back()[i+1]) std::cout << "   ┃";
            else std::cout << "━━━┫";
            std::cout << std::endl;
        }

        // 一番下
        std::cout << "┗";
        for(int i=0; i<(int)w_size-1; ++i){
            if(board[i].back() == board[i+1].back()) std::cout << "━━━━";
            else std::cout << "━━━┻";
        }
        std::cout << "━━━┛" << std::endl;
    }

    bool operator == (Board const & rhs) const {
        return is_same(rhs);
    }

    std::vector<int> & operator [] (int const idx){
        return board[idx];
    }

    std::vector<int> const & operator [] (int const idx) const {
        return board[idx];
    }

    int & operator [] (Coord const & rhs){
        return board[rhs.x][rhs.y];
    }

    int const operator [] (Coord const & rhs) const {
        return board[rhs.x][rhs.y];
    }

    int & at(Coord const & rhs){
        assert(in(rhs));
        return board[rhs.x][rhs.y];
    }

    int const & at(Coord const & rhs) const {
        assert(in(rhs));
        return board[rhs.x][rhs.y];
    }
};


/**
 * @brief ポリオミノを表現するクラス
*/
template <size_t omino_size>
struct Polyomino{
    std::vector<Coord> elem;

    Polyomino() : elem(omino_size){}
    Polyomino(std::vector<Coord> const & _elem) : elem(_elem){}
    Polyomino(std::initializer_list<Coord> const & _elem) : elem(_elem.begin(), _elem.end()){}
    // Polyomino(Polyomino const & p) : elem(p.elem){}

    Coord & operator [](int const idx){
        return elem[idx];
    }

    Coord const & operator [](int const idx) const {
        return elem[idx];
    }

    Coord & at(int const idx){
        assert(idx < (int)elem.size());
        return elem[idx];
    }

    Coord const & at(int const idx) const {
        assert(idx < (int)elem.size());
        return elem[idx];
    }

    /**
     * @brief オミノの数を返す
    */
    size_t size() const {
        return omino_size;
    }

    /**
     * @brief 一番左下に相当する正方形のインデックスを返す
    */
    int get_bottomleft_pos() const {
        int res = 0;
        for(int i=1; i<(int)omino_size; ++i){
            if(elem[i].x == elem[res].x ? (elem[i].y > elem[res].y) : (elem[i].x < elem[res].x)) res = i;
        }
        return res;
    }

    /**
     * @brief 一番左上に相当する正方形のインデックスを返す
    */
    int get_topleft_pos() const {
        int res = 0;
        for(int i=1; i<(int)omino_size; ++i){
            if(elem[i] < elem[res]) res = i;
            // if(elem[i].x == elem[res].x ? (elem[i].y > elem[res].y) : (elem[i].x < elem[res].x)) res = i;
        }
        return res;
    }

    /**
     * @brief ポリオミノを包含する最小の長方形の左下に相当するマスの座標を返す
    */
    Coord get_aabb_min_pos() const {
        Coord _min=elem[0];
        for(int i=1; i<(int)omino_size; ++i){
            if(elem[i].x < _min.x) _min.x = elem[i].x;
            if(elem[i].y < _min.y) _min.y = elem[i].y;
        }
        return _min;
    }

    /**
     * @brief ポリオミノを包含する最小の長方形の左上と右下の座標を返す
    */
    std::pair<Coord, Coord> get_aabb() const {
        Coord _min=elem[0], _max=elem[0];
        for(int i=1; i<(int)omino_size; ++i){
            if(elem[i].x < _min.x) _min.x = elem[i].x;
            if(elem[i].y < _min.y) _min.y = elem[i].y;
            if(elem[i].x > _max.x) _max.x = elem[i].x;
            if(elem[i].y > _max.y) _max.y = elem[i].y;
        }
        return std::make_pair(_min, _max);
    }

    /**
     * @brief xyサイズ
    */
    Coord get_size() const {
        Coord _min=elem[0], _max=elem[0];
        for(int i=1; i<(int)omino_size; ++i){
            if(elem[i].x < _min.x) _min.x = elem[i].x;
            if(elem[i].y < _min.y) _min.y = elem[i].y;
            if(elem[i].x > _max.x) _max.x = elem[i].x;
            if(elem[i].y > _max.y) _max.y = elem[i].y;
        }
        return _max - _min;
    }

    /**
     * @brief 二次元的な表現を得る、サイズ可変
     * @return 四角形が乗らないマスにEMPTY, それ以外に1が入ったサイズがaabbと等しいボード
    */
    Board get_board() const {
        // bb計算
        auto [_min, _max] = get_aabb();

        // 小さめのボードを作る
        Board b(_max.x - _min.x + 1, _max.y - _min.y + 1, EMPTY);
        for(int i=0; i<(int)elem.size(); ++i){
            b[elem[i] - _min] = 1;
        }

        return b;
    }

    /**
     * @brief 回転・鏡像を考えず同じ形かどうか
     * @note O(omino_size^2)
    */
    bool is_same_pose(Polyomino const & p) const {
        // 左下の座標に合わせる
        // std::cout << p.get_base_pos() << " " << get_base_pos() << std::endl;
        Coord base = p[p.get_bottomleft_pos()] - elem[get_bottomleft_pos()];
        bool flag_found;
        for(int i=0; i<(int)omino_size; ++i){
            flag_found = false;
            for(int j=0; j<(int)omino_size; ++j){
                // 各マスについて同じ座標のマスがあるか調べる
                if(p[i] == elem[j] + base){
                    flag_found = true;
                    break;
                }
            }
            if(!flag_found) return false;
        }
        return true;
    }

    /**
     * @brief 回転・鏡像を考えて同じ形かどうか
     * @note O(omino_size^2) * 8
    */
    bool is_same(Polyomino const & p) const {
        Polyomino origin = *this;
        // std::cout << "元の形状: ";
        // p.print_debug();
        for(int i=0; i<2; ++i){
            for(int j=0; j<4; ++j){
                // std::cout << "rot";
                // origin.print_debug();
                if(origin.is_same_pose(p)) return true;
                origin.rotate_clockwise();
            }
            origin.reflecte();
        }
        return false;
    }

    /**
     * @brief ペントミノの各座標を移動させる
     * @param[in] c 移動する座標値
    */
    void move_by(Coord const & c) {
        for(auto & i : elem){
            i.move_by(c);
        }
    }

    /**
     * @brief 各座標を移動させたペントミノを返す
     * @param[in] c 移動させる座標値
    */
    Polyomino moved_by(Coord const & c) const {
        Polyomino res = *this;
        res.move_by(c);
        return res;
    }

    /**
     * @brief 左下が0,0になるように調整
    */
    void bottomleft_adjustment(){
        Coord min_c = elem[get_bottomleft_pos()];
        for(int i=0; i<(int)omino_size; ++i){
            elem[i] -= min_c;
        }
    }

    /**
     * @brief 左上が0,0になるように調整
    */
    void topleft_adjustment(){
        Coord min_c = elem[get_topleft_pos()];
        for(int i=0; i<(int)omino_size; ++i){
            elem[i] -= min_c;
        }
    }

    /**
     * @brief 時計回りに90度回転してできるポリオミノを返す
    */
    Polyomino rotated_clockwise() const {
        Polyomino res = *this;
        res.rotate_clockwise();
        return res;
    }

    /**
     * @brief このポリオミノを時計回りに90度回転させる
    */
    Polyomino & rotate_clockwise(){
        for(int i=0; i<(int)omino_size; ++i){
            std::swap(elem[i].x, elem[i].y);
            elem[i].y *= -1;
        }
        return *this;
    }

    /**
     * @brief 180度回転してできるポリオミノを返す
    */
    Polyomino rotated_180() const {
        Polyomino res = *this;
        res.rotate_180();
        return res;
    }

    /**
     * @brief このポリオミノを180度回転させる
    */
    Polyomino & rotate_180(){
        for(int i=0; i<(int)omino_size; ++i){
            elem[i].x *= -1;
            elem[i].y *= -1;
        }
        return *this;
    }

    /**
     * @brief y軸に関して鏡像変換してできるポリオミノを返す
    */
    Polyomino reflected() const {
        Polyomino res = *this;
        res.reflecte();
        return res;
    }

    /**
     * @brief このポリオミノをy軸に関して鏡像変換させる
    */
    Polyomino & reflecte(){
        for(int i=0; i<(int)omino_size; ++i){
            elem[i].x *= -1;
        }
        return *this;
    }

    /**
     * @brief 鏡像同一性を調べる
    */
    bool reflectionity() const {
        Polyomino p = *this, ref = reflected();
        for(int i=0; i<4; ++i){
            if(p.is_same_pose(ref)) return true;
            p.rotate_clockwise();
        }
        return false;
    }

    /**
     * @brief 90度回転同一性を調べる
    */
    bool rotationity_90() const {
        return is_same_pose(rotated_clockwise());
    }

    /**
     * @brief 180度回転同一性を調べる
    */
    bool rotationity_180() const {
        return is_same_pose(rotated_180());
    }

    /**
     * @brief 回転・鏡像を考慮した独立なポリオミノの列挙
     * @note DrawablePolyominoも処理できるようにするためtemplateを使用
    */
    template <typename OminoType>
    static void enumeration(std::vector<OminoType> & polyominos, OminoType now = OminoType(),
    std::map<Coord, int> board = std::map<Coord, int>(), std::vector<Coord> candidate = {{0, 0}}, int const depth = 0){
        if(depth == omino_size){
            // これまでの形状と比較する
            #ifdef DEBUG
            now.print_debug();
            std::cout << "----" << std::endl;
            #endif
            for(auto const & it : polyominos){
                if(now.is_same(it)) return;
            }
            polyominos.emplace_back(now);
            return;
        }

        #ifdef DEBUG
        std::cout << "depth[" << depth << "]" << std::endl;
        now.print_debug();
        for(int i=0; i<(int)candidate.size(); ++i){
            std::cout << "(" << candidate[i] << ") ";
        }
        std::cout << std::endl;
        #endif

        for(int i=0; i<(int)candidate.size(); ++i){
            if(board[candidate[i]] == 2) continue;
            std::map<Coord, int> tmp_board = board;
            std::vector<Coord> tmp_cand = candidate;
            tmp_board[candidate[i]] = 2;
            now[depth] = candidate[i];

            // 次の候補を追加する
            if(depth == 0){
                tmp_cand.emplace_back(1, 0);
            }else{
                if(tmp_board[tmp_cand[i] + Coord( 1,  0)] == 0) { tmp_cand.emplace_back(tmp_cand[i] + Coord( 1,  0)); tmp_board[tmp_cand[i] + Coord( 1,  0)] = 1; }
                if(tmp_board[tmp_cand[i] + Coord( 0,  1)] == 0) { tmp_cand.emplace_back(tmp_cand[i] + Coord( 0,  1)); tmp_board[tmp_cand[i] + Coord( 0,  1)] = 1; }
                if(tmp_board[tmp_cand[i] + Coord(-1,  0)] == 0) { tmp_cand.emplace_back(tmp_cand[i] + Coord(-1,  0)); tmp_board[tmp_cand[i] + Coord(-1,  0)] = 1; }
                if(tmp_board[tmp_cand[i] + Coord( 0, -1)] == 0) { tmp_cand.emplace_back(tmp_cand[i] + Coord( 0, -1)); tmp_board[tmp_cand[i] + Coord( 0, -1)] = 1; }
            }

            enumeration(polyominos, now, tmp_board, tmp_cand, depth+1);
        }
    }

    // static void enumeration(std::vector<Polyomino> & polyominos, Polyomino now = Polyomino(),
    // std::map<Coord, int> board = std::map<Coord, int>(), std::vector<Coord> candidate = {{0, 0}}, int const depth = 0){
    //     if(depth == omino_size){
    //         // これまでの形状と比較する
    //         #ifdef DEBUG
    //         now.print_debug();
    //         std::cout << "----" << std::endl;
    //         #endif
    //         for(auto const & it : polyominos){
    //             if(now.is_same(it)) return;
    //         }
    //         polyominos.emplace_back(now);
    //         return;
    //     }

    //     #ifdef DEBUG
    //     std::cout << "depth[" << depth << "]" << std::endl;
    //     now.print_debug();
    //     for(int i=0; i<(int)candidate.size(); ++i){
    //         std::cout << "(" << candidate[i] << ") ";
    //     }
    //     std::cout << std::endl;
    //     #endif

    //     for(int i=0; i<(int)candidate.size(); ++i){
    //         if(board[candidate[i]] == 2) continue;
    //         std::map<Coord, int> tmp_board = board;
    //         std::vector<Coord> tmp_cand = candidate;
    //         tmp_board[candidate[i]] = 2;
    //         now[depth] = candidate[i];

    //         // 次の候補を追加する
    //         if(depth == 0){
    //             tmp_cand.emplace_back(1, 0);
    //         }else{
    //             if(tmp_board[tmp_cand[i] + Coord( 1,  0)] == 0) { tmp_cand.emplace_back(tmp_cand[i] + Coord( 1,  0)); tmp_board[tmp_cand[i] + Coord( 1,  0)] = 1; }
    //             if(tmp_board[tmp_cand[i] + Coord( 0,  1)] == 0) { tmp_cand.emplace_back(tmp_cand[i] + Coord( 0,  1)); tmp_board[tmp_cand[i] + Coord( 0,  1)] = 1; }
    //             if(tmp_board[tmp_cand[i] + Coord(-1,  0)] == 0) { tmp_cand.emplace_back(tmp_cand[i] + Coord(-1,  0)); tmp_board[tmp_cand[i] + Coord(-1,  0)] = 1; }
    //             if(tmp_board[tmp_cand[i] + Coord( 0, -1)] == 0) { tmp_cand.emplace_back(tmp_cand[i] + Coord( 0, -1)); tmp_board[tmp_cand[i] + Coord( 0, -1)] = 1; }
    //         }

    //         enumeration(polyominos, now, tmp_board, tmp_cand, depth+1);
    //     }
    // }

    /**
     * @brief baseをもとに回転・鏡像のパターンを列挙する
     * @note DrawablePolyominoも処理できるようにするためtemplateを使用
    */
    template <typename OminoType>
    static void pattern_enumeration(std::vector<std::vector<OminoType>> & pattern, std::vector<OminoType> const & base){
        // 回転・鏡像を得る
        pattern.resize(base.size());
        for(int i=0; i<(int)base.size(); ++i){
            auto const & p = base[i];
            int rotationity = 0, reflectionity = 0;
            // 回転性・鏡像性を調べて重複する形状はスルー
            rotationity = (p.rotationity_90()) ? 1 : ((p.rotationity_180()) ? 2 : 4);
            reflectionity = p.reflectionity() ? 1 : 2;

            // 実際に回転・鏡像を追加
            OminoType origin = p;
            for(int j=0; j<reflectionity; ++j){
                for(int k=0; k<rotationity; ++k){
                    pattern[i].emplace_back(origin);
                    origin.rotate_clockwise();
                }
                origin.reflecte();
            }
        }

        // 左上が0,0になるように調整する
        for(int i=0; i<(int)pattern.size(); ++i){
            for(int j=0; j<(int)pattern[i].size(); ++j){
                pattern[i][j].topleft_adjustment();
            }
        }
    }

    // static void pattern_enumeration(std::vector<std::vector<Polyomino>> & pattern, std::vector<Polyomino> const & base){
    //     // 回転・鏡像を得る
    //     pattern.resize(base.size());
    //     for(int i=0; i<(int)base.size(); ++i){
    //         auto const & p = base[i];
    //         int rotationity = 0, reflectionity = 0;
    //         // 回転性・鏡像性を調べて重複する形状はスルー
    //         rotationity = (p.rotationity_90()) ? 1 : ((p.rotationity_180()) ? 2 : 4);
    //         reflectionity = p.reflectionity() ? 1 : 2;

    //         // 実際に回転・鏡像を追加
    //         Polyomino origin = p;
    //         for(int j=0; j<reflectionity; ++j){
    //             for(int k=0; k<rotationity; ++k){
    //                 pattern[i].emplace_back(origin);
    //                 origin.rotate_clockwise();
    //             }
    //             origin.reflecte();
    //         }
    //     }

    //     // 左上が0,0になるように調整する
    //     for(int i=0; i<(int)pattern.size(); ++i){
    //         for(int j=0; j<(int)pattern[i].size(); ++j){
    //             pattern[i][j].topleft_adjustment();
    //         }
    //     }
    // }

#if 0
    /**
     * @brief 回転・鏡像を考慮した独立なポリオミノの列挙, Redelmeier'sアルゴリズムを使用
    */
    static void enumration_redelmeier(){
        Board board(omino_size, omino_size*2-1);
    }
#endif

    /**
     * @brief 特定の位置に出力, 1-indexedに注意
    */
    void print_color(int const w=1, int const h=1) const {
        for(int i=0; i<(int)omino_size; ++i){
            std::cout << "\033[" << h + elem[i].y << ";" << w * 2 - 1 + elem[i].x * 2 << "H" << CB_Blue << "  " << CB_Clear;
        }
    }

    /**
     * @brief デバグ用出力
    */
    void print_debug() const {
        std::cout << "{";
        for(int i=0; i<(int)omino_size; ++i){
            if(i == omino_size-1) std::cout << "(" << elem[i] << ")";
            else std::cout << "(" << elem[i] << "), ";
        }
        std::cout << "}" << std::endl;
    }

    friend std::ostream & operator << (std::ostream & output, Polyomino const & p){
        output << "{";
        for(int i=0; i<(int)omino_size; ++i){
            if(i == omino_size-1) output << "(" << p[i] << ")";
            else output << "(" << p[i] << "), ";
        }
        output << "}";
        return output;
    }
};

using Tromino = Polyomino<3>;
using Tetromino = Polyomino<4>;
using Pentomino = Polyomino<5>;
using Hexomino = Polyomino<6>;
using Heptomino = Polyomino<7>;
using Octomino = Polyomino<8>;
using Nonomino = Polyomino<9>;
using Decomino = Polyomino<10>;

} // namespace PentominoPuzzle
