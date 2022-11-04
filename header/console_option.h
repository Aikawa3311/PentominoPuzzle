#pragma once

#include <vector>
#include <utility>
#include <iomanip>
#include <conio.h> // getch()
// #include <curses.h> // getch()
#include "console_color.h"
#include "console_printer.h"

namespace ConsoleOutput{

/**
 * @brief 選択肢描画・実行用クラス
 * @note 選択肢にするクラスにはdraw(y, x)(コンソール上に描画する関数)とdraw_size()(描画時のサイズを求める関数)が必要
*/
template <typename item_type>
struct ItemsOption{
    std::vector<std::vector<std::pair<item_type, bool>>> items; // 各選択肢要素/選択可能状態
    int now_page{}, now_choice{};
    int base_y = 1, base_x = 1;
    int choice_col = 1;
    int item_size_x{}, item_size_y{};   // 選択肢1つに費やす描画スペース
    int margin = 2;                     // 横方向の間隔
    int per_page;

    ItemsOption(std::vector<item_type> _items, int const _per_page) : per_page(_per_page){
        items.resize((_items.size() - 1) / per_page + 1);
        for(int i=0; i<(int)_items.size(); ++i){
            items[i/per_page].emplace_back(_items[i], true);
            auto [_y, _x] = _items[i].draw_size();
            item_size_x = std::max(item_size_x, _x);
            item_size_y = std::max(item_size_y, _y);
        }
        // int omino_size = (int)_items.front().elem.size();
        // item_size_x = omino_size * 4 + 1;
        // item_size_y = omino_size * 2 + 1;
    }

    /**
     * @brief 選択肢の描画・選択状況の描画
    */
    void draw() const {
        // 以前の描画状況をクリア
        clear_draw();
        int draw_x = base_x, draw_y = base_y;
        
        // 各アイテムの描画
        for(int i=0; i<(int)items[now_page].size(); ++i){
            if(i == now_choice) std::cout << CB_White << C_Black;
            if(!items[now_page][i].second) std::cout << C_Red;
            items[now_page][i].first.draw(draw_y, draw_x);
            std::cout << CB_Clear << C_Clear;
            draw_x += item_size_x + margin;
        }

        // ページの描画
        std::cout << MovCursor(draw_y + item_size_y - 1, draw_x - margin + 1) << "(" << now_page + 1 << "/" << items.size() << ")";
    }

    /**
     * @brief 選択肢の描画の消去
    */
    void clear_draw() const {
        // 範囲消去 重い
        // for(int i=0; i<per_page; ++i){
        //     clear_print_area(base_y, base_x + i * (item_size_x + margin), item_size_y, item_size_x);
        // }
        
        // 行消去 若干重い
        std::cout << MovCursor(base_y-1, base_x);
        for(int i=0; i<item_size_y; ++i){
            clear_print_line(1);
        }
    }

    /**
     * @brief 描画領域の右下の座標を取得
    */
    std::pair<int, int> get_bottomright() const {
        return std::make_pair(base_y + item_size_y, base_x + per_page * (item_size_x + margin));
    }

    /**
     * @brief 描画位置の設定
    */
    void set_pos(int const y, int const x){
        base_x = x;
        base_y = y;
    }

    void page_decrement(bool const choice_update = true){
        --now_page;
        if(now_page < 0){
            now_page = (int)items.size() - 1;
        }
        if(choice_update) now_choice = (int)items[now_page].size() - 1;
        // 要素数が少ない最終ページへ飛んだ場合に選択中のidがオーバーしないようにする
        now_choice = std::min(now_choice, (int)items[now_page].size() - 1);
    }

    void page_increment(bool const choice_update = true){
        ++now_page;
        if(now_page >= (int)items.size()){
            now_page = 0;
        }
        if(choice_update) now_choice = 0;
        // 要素数が少ない最終ページへ飛んだ場合に選択中のidがオーバーしないようにする
        now_choice = std::min(now_choice, (int)items[now_page].size() - 1);
    }

    void choice_decrement(bool const page_update = true){
        --now_choice;
        if(now_choice < 0){
            if(page_update) page_decrement();
            else now_choice = (int)items[now_page].size() - 1;
        }
    }

    void choice_increment(bool const page_update = true){
        ++now_choice;
        if(now_choice >= (int)items[now_page].size()){
            if(page_update) page_increment();
            else now_choice = 0;
        }
    }

    /**
     * @brief 選択肢の操作、キー受付待ちアリ
     * @return カーソルの場合-1、決定の場合選択肢ID
    */
    int choice(){
        /*
        draw();
        do{
            int key = getch();
            if(key == 452) key = KEY_LEFT;
            if(key == 454) key = KEY_RIGHT;
            if(key == 10) key = KEY_ENTER;

            switch(key){
            case KEY_LEFT:  choice_decrement(); draw(); break;
            case KEY_RIGHT: choice_increment(); draw(); break;
            case KEY_ENTER: return items[now_page][now_choice];
            default: mvprintw(1, 1, "%d : %d %d %d", key, KEY_LEFT, KEY_RIGHT, KEY_ENTER);
            }
        }while(true);
        */
        char key = getch();
        switch(key){
        case 'a': choice_decrement(); break;
        case 'd': choice_increment(); break;
        case 'z': if(items[now_page][now_choice].second) return now_choice; break;
        case 'q': exit(1);
        // default : std::cout << MovCursor(8, 5) << "you touched " << key;
        }
        
        return -1;
    }

    /**
     * @brief キー待ち受け無しの選択肢操作、外部からキーidを受け取る 外部でキーの追加操作を行いたい場合に使う
     * @return カーソルの場合-1、決定の場合選択肢ID
    */
    int choice_without_keyinput(char const key){
        switch(key){
        case 'a': choice_decrement(); break;
        case 'd': choice_increment(); break;
        case 'z': if(items[now_page][now_choice].second) return now_choice; break;
        // default : std::cout << MovCursor(8, 5) << "you touched " << key;
        }

        return -1;
    }

    /**
     * @brief 現在選択している選択肢のIDを1次元にして返す
    */
    inline int choiceid() const {
        return now_page * per_page + now_choice;
    }

    /**
     * @brief 選択不可・選択可の切り替え
    */
    inline void set_selectability(int const idx, bool const flag = true){
        items[idx / per_page][idx % per_page].second = flag;
    }
};

} // namespace ConsoleOutput