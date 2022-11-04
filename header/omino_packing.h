/**
 * @brief ポリオミノパッキングの解を再帰的に探索する
*/

#pragma once

#include "polyomino.h"

namespace PolyominoPuzzle{


/**
 * @brief ポリオミノパッキング全般
*/
template <typename Omino>
struct PackingPuzzle{
    std::vector<Omino> base;                    // ベースとなるポリオミノ
    std::vector<std::vector<Omino>> pattern;    // baseの回転や鏡像を考えたポリオミノ
    std::vector<int> unuse;                     // 各ピースの使用状況
    Board board;                                // 現在の盤面
    std::vector<Board> ans;                     // 答えのパターン
    int ignore_piece = -1;                      // 鏡像対称性があり回転対称性がないポリオミノ番号(未使用)
    long long int iterate_num{};

    PackingPuzzle(size_t const _w = 0, size_t const _h = 0) : board(_w, _h, EMPTY){
        init();
    }

    PackingPuzzle(std::vector<std::string> const & b) : board(b){
        init();
    }

    /**
     * @brief 初期化
    */
    void init(){
        // まずbase取得
        Omino::enumeration(base);
        // 回転・鏡像のパターンを取得
        Omino::pattern_enumeration(pattern, base);
        // 盤面をEMPTYに
        board.fill(EMPTY);
        // サイズなど変更
        unuse.resize(base.size(), true);
        ans.clear();
        // 重複を除去するためのポリオミノを一つ選択
        // for(int i=0; i<(int)base.size(); ++i){
        //     if(!base[i].reflectionity()) continue;
        //     if((int)pattern[i].size() == 4){
        //         ignore_piece = i;
        //         break;
        //     }
        // }
    }

    /**
     * @brief パズルを解き、ansを更新する
     * @param[in] place 配置場所(再帰の際の効率化用, 設定の必要なし)
     * @param[in] depth 現在の深さ
     * @param[in] flag_ignore 特定のピースの回転を固定化し重複解の出現を抑える
    */
    void solve(Coord place = {0, 0}, int const depth = 0, bool flag_ignore = true){
        // 末端まで来たら終了
        if(depth >= (int)pattern.size()){
            // std::cout << "【解に追加】" << std::endl;
            ans.emplace_back(board);
            return;
        }

        // 置く場所の決定
        place = board.get_topleft(place, EMPTY);

        // もし残りの空間がomino_sizeの倍数でない場合は枝刈り
        // if(board.calc_empty_area(place) % 5 != 0) return;
        // if(board.is_isolated_space(place)) return;

        ++iterate_num;

        // ポリオミノを選択
        for(int i=0; i<(int)pattern.size(); ++i){
            // 使っていないかどうか
            if(!unuse[i]) continue;
            
            // 各回転・鏡像のパターンを見る
            for(int j=0; j<(int)pattern[i].size(); ++j){
                // if(flag_ignore && i == ignore_piece && j > 0) break;
                // 置けるかどうかをチェック
                Omino const & shape = pattern[i][j];
                bool flag_empty = true;
                for(int k=0; k<(int)shape.size(); ++k){
                    if(!board.in(place + shape[k]) || board[place + shape[k]] != EMPTY) {
                        flag_empty = false;
                        continue;
                    }
                }
                if(!flag_empty) continue;

                // std::cout << "depth[" << depth << "] use[" << i << "]: place(" << place << ")   " << shape.moved_by(place) << std::endl;

                // 置ける場合は置いて使用状況をfalseに
                for(int k=0; k<(int)shape.size(); ++k){
                    board[place + shape[k]] = i;
                }
                unuse[i] = false;

                // 深さ+1へ
                solve(place, depth+1);

                // ボードから取り出す＆使用状況をリセット(重要)
                for(int k=0; k<(int)shape.size(); ++k){
                    board[place + shape[k]] = EMPTY;
                }
                unuse[i] = true;
            }
        }
    }
};


} // namespace PolyominoPuzzle