// Thế gian không nợ ta điều gì, nhân quả báo ứng không chừa một ai \\
//                         .oo00oo.                                 \\
//                        o888888888o                               \\
//                         88" . "88                                \\
//                         (| -_- |)                                \\
//                          0\ = /0                                 \\
//                       ___/'---'\___                              \\
//                     .' \\|     |// '.                            \\
//                    / \\|||  :  |||// \                           \\
//                   / _||||| -:- |||||_ \                          \\
//                  |   | \\\  -  /// |   |                         \\
//                  | \_|  ''\---/''  |_/ |                         \\
//                  \  .-\__  '-'  ___/-. /                         \\
//                ___'. .'  /--.--\  `. .'___                       \\
//             ."" '<  `.___\_<|>_/___.' >' "".                     \\
//            | | :  `- \`.;`\ _ /`;.`/ - `  : | |                  \\
//            \  \ `_.   \_ __\ /__ _/   .-` /  /                   \\
//        =====`-.____`.___ \_____/___.-`___.-'=====                \\
//                          `=---='                                 \\
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^    \\
//                Phật pháp từ bi, quay đầu là bờ                   \\
//            Mong đức phật phù hộ code con không bugs              \\
//------------------------------------------------------------------\\

#include <Renderer.h>
#include <util/OSUtil.h>

int main()
{
    util::setCurrentDirToExeDir();
    Renderer renderer;
    renderer.init();
    renderer.run();
    renderer.cleanup();
}