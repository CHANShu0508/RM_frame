# 电容-旧步兵测试
@尹云鹏
### 操作改动
>1. 左右拨杆互换，更加符合键鼠相对操作位置
>2. 原键鼠模式拆分，左上=normal，左中串口发送，左下为键鼠；需要controlTask中做改动（串口发送@唐欣阳）

### 功率限制微调改动
>功率限制是掉血原因，增加了斜坡函数以进行修正，但似乎影响了底盘pid
