/*
* Created by boil on 2022/8/27.
*/

#ifndef RENDU_PROGRAM_H_
#define RENDU_PROGRAM_H_

#include <map>
#include "component.h"
#include "system.h"
#include "options_system.h"
#include "singleton.h"

namespace rendu {

  enum class ProgramState {
    IDLE = 0,           //闲置
    INITIALIZED,    //已经初始化
    RUNNING,        //正在运行
    STOPPING,       //正在停止
    STOPPED         //已经停止
  };


  class Program : public Singleton<Program> {
  public:
    void Initialize(int argc, char **argv);

    void Run();

    bool IsStopped();

    void Exit();

    ISystem& AddSystem(ISystem &system);

  private:

    void Start();

    void Stop();

    void Update();

    ProgramState _state;
    std::map<size_t, ISystem *> _updates;
  };

#define sProgram Program::get_inst()
}//namespace rendu

#endif //RENDU_PROGRAM_H_