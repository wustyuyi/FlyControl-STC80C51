
# 四轴飞行器
- 四轴复合版，大四轴 & 小四轴
- 时间：2015-8-19
- 作者: JIN Kai & JIANGH He-jun
- 算法：PID(内环PID, 外环PI) + 四元数解算 + 互补滤波

### 程序架构方面
- 实现一键切换大小四轴
- 实现将需要修改的参数定位到同一个文件
- 实现参数关联修改(修改主频的同时将所有参数自动修改)
 
### PWM电机驱动
- 动态大四轴PWM调节(自动进入正常模式)
- 动态小四轴PWM初始化计数初值
 
### 飞机保护程序
- 实现飞行器失联保护
- 实现飞行器一键刹车

### 通讯程序
- 实现无线通信的伪双工通讯(兼容单工)
- 实现uart串口的读写
- 定时器t0初始化过程消除uart串口中断

### 飞控算法调整
- 对Speed的取值进行估计
- 重新调整了大小四轴的PID
- 将飞行器飞行方式由十字型转换为X型

### 飞机调试模块
- 增加matlab对MPU6050的曲线实时显示

### 遥控器模块
- 对遥控器程序进行了重构
- 解决遥控器控制不灵敏的问题(非线性函数)
- 遥控器部分实现了方向微调
- 实现LCD显示飞行器数据(PWM,MPU6050)
- 增加LCD大小四轴遥控器方式选择启动(Important)
- 增加LCD对油门杆误拉高启动的提醒

### 辅助MCU模块(超声波程序, 灯光等)
- 增加超声波测高传感器
- 对超声波测距数据增加卡尔曼滤波

### 陀螺仪校准方面
- 对陀螺仪进行校准
- 对加速度计进行了初始化过程的自校准
- 增加了卡尔曼滤波函数(针对加速度计)
 
### **待实现**
- 实现另一个MPU的传感器GPS/温度/灯光等
- 实现定高算法