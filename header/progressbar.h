/**
 * @file プログレスバー
*/

#pragma once

#include <iostream>
#include <iomanip>

namespace ConsoleOutput{
    /**
     * @brief プログレスバーの出力クラス
    */
    struct ProgressBar{
        int count_max;
        int count = 0;
        bool countup_per_print = true;
        
        static int constexpr progress_bar_length = 20;
        
        /**
         * @brief 最大カウント100で初期化
        */
        ProgressBar() : count_max(100){}

        /**
         * @brief 最大カウントを渡す
        */
        ProgressBar(int const _count_max, bool cnt_p_print = true) : count_max(_count_max), countup_per_print(cnt_p_print){}

        /**
         * @brief カウントアップする
        */
        ProgressBar & operator ++(){
            ++count;
            return *this;
        }

        /**
         * @brief カウントアップ
        */
        inline void countup(){
            ++count;
        }

        /**
         * @brief カウントアップ & 経過パーセント出力
        */
        void countup_and_print_percent(){
            int pre = int((double)count / count_max * 100);
            ++count;
            int post = int((double)count / count_max * 100);
            if(post != pre) print_percent();
        }

        /**
         * @brief カウントアップ & #プログレスバー出力
        */
        void countup_and_print_progressbar(){
            int pre = int(((double)count / count_max) / (1.0 / progress_bar_length));
            ++count;
            int post = int(((double)count / count_max) / (1.0 / progress_bar_length));
            if(post != pre) print_progressbar();
        }

        /**
         * @brief 経過を一行表示
         * @note 自動カウントアップあり
        */
        inline void print() {
            if(countup_per_print) countup();
            std::cout << "\033[1G" << count << " / " << count_max;
        }

        /**
         * @brief 経過のパーセントを一行表示
         * @note 自動カウントアップなし
        */
        inline void print_percent() {
            double percent = ((double)count / count_max) * 100;
            std::cout << "\033[1G" << std::setprecision(2) << std::fixed << percent << " / 100";
            if(std::abs(percent - 100.0) < 10e-9) std::cout << std::endl;
        }

        /**
         * @brief 経過のパーセントを#で一行表示
         * @note 自動カウントアップなし
        */
        inline void print_progressbar() {
            std::cout << "\033[1G[";
            double percent = (double)count / count_max;
            for(int i=0; i<progress_bar_length; ++i){
                if(percent >= (double)(i+1) / progress_bar_length) std::cout << '#';
                else std::cout << ' ';
            }
            std::cout << "]";
            if(std::abs(percent - 1.0) < 10e-9) std::cout << std::endl;
        }
    };
}