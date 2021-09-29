#ifndef LOGICSCANNER_WORK_H
#define LOGICSCANNER_WORK_H

__NO_RETURN void ReadSignalThread(void *param);
__NO_RETURN void TestSignalToggleThread(void *param);
__NO_RETURN void WaitCommandThread(void *param);

#endif //LOGICSCANNER_WORK_H
