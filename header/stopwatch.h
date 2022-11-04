#include <chrono>
#include <vector>

// 最低限の実時間(ms)計測用
struct Stopwatch{
    std::chrono::system_clock::time_point s, e;
    std::vector<double> lap; // ラップタイム

    // 再生
    void start(){
        s = std::chrono::system_clock::now();
    }

    // 計測終了, ミリ秒を返す
    double stop(){
        e = std::chrono::system_clock::now();
        return static_cast<double>( std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count() );
    }

    // 計測時間を返す(stop後でないと無意味)
    double time() const{
        return static_cast<double>( std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count() );
    }

    // 経過時間をラップタイムとして保存 & 再スタート
    void section(){
        e = std::chrono::system_clock::now();
        lap.emplace_back(static_cast<double>( std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count() ));
        s = std::move(e);
    }

    // 計測時間・ラップタイムのクリア
    void clear(){
        s = std::chrono::system_clock::now();
        e = std::chrono::system_clock::now();
        lap.clear();
    }
};