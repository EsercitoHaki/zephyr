// Thế gian không nợ ta điều gì, nhân quả báo ứng không chừa một ai \\
//                         .oo00oo.                                 \\
//                        o88888888o                                \\
//                         88" . "88                                \\
//                         (| -_- |)                                \\
//                          0\ = /0                                 \\
//                       ___/'---'\___                              \\
//                     .' \\|     |// '.                            \\
//                     / \\|||  :  |||// \                          \\
//                    / _||||| -:- |||||_ \                         \\
//                   |   | \\\  - /// |   |                         \\
//                   | \_|  ''\---/''  |_/ |                        \\
//                   \  .-\__  '-'  ___/-. /                        \\
//                 ___'. .'  /--.--\  `. .'___                      \\
//               ."" '<  `.___\_<|>_/___.' >' "".                   \\
//              | | :  `- \`.;`\ _ /`;.`/ - ` : | |                 \\
//              \  \ `_.   \_ __\ /__ _/   .-` /  /                 \\
//           =====`-.____`.___ \_____/___.-`___.-'=====             \\
//                             `=---='                              \\
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^    \\
//                Phật pháp từ bi, quay đầu là bờ                   \\
//            Mong đức phật phù hộ code con không bugs              \\
//------------------------------------------------------------------\\

#include <Renderer.h>
#include <OSUtil.h>

#include <filesystem>
int main()
{
    const auto exeDir = osutil::getExecutableDir();
    std::filesystem::current_path(exeDir);
    Renderer renderer;
    renderer.init();
    renderer.run();
    renderer.cleanup();
}