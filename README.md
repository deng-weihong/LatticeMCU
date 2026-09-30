# LatticeMCU

LatticeMCU 是一个面向资源受限 MCU 的轻量级、可移植裸机框架。
公共 C API 统一使用 `lmcu_` 前缀。

当前框架采用裸机事件驱动模型，默认使用静态内存和协作式调度，不依赖
具体 MCU 厂商库或 RTOS。

## 设计目标

- 默认不使用动态内存；
- 支持协作式周期任务；
- 使用可处理 tick 回绕的时间比较；
- 将中断数据通过队列或延迟工作交给任务处理；
- 通过 `port`、`bsp` 和 `hal` 隔离芯片差异；
- 让调度器、服务和应用代码可以跨 MCU 复用。

## 目录结构

```text
include/lmcu/   对外公开的框架头文件
src/core/       基础类型、状态码和通用工具
src/kernel/     调度器、软件定时器、事件队列和延迟工作
src/hal/        GPIO、UART、ADC、Timer、Flash 等硬件接口
src/ports/      CPU、编译器和 MCU 相关实现
src/bsp/        芯片和开发板配置、启动和引脚定义
services/       日志、通信协议、参数存储和 OTA
drivers/        电机、编码器、按键、传感器等设备驱动
examples/       示例应用
tests/          可在主机运行的模块测试
```

## 调度器示例

```c
static void heartbeat_task(void *context)
{
    (void)context;
    /* 翻转 LED，或者投递一个应用事件。 */
}

static struct lmcu_task tasks[1];
static struct lmcu_scheduler scheduler;

void app_init(void)
{
    lmcu_scheduler_init(&scheduler, tasks, 1, platform_get_tick_ms);
    lmcu_scheduler_add(&scheduler, "heartbeat", heartbeat_task, 0, 500);
    lmcu_scheduler_start(&scheduler);
}

void app_run(void)
{
    lmcu_scheduler_run_once(&scheduler);
}
```

任务应当执行时间可控并且不阻塞。UART 接收、GPIO 中断等中断处理函数只
负责读取硬件状态、保存数据或投递事件，具体处理放在任务或延迟工作中。

## 当前状态

仓库当前包含第一版可移植协作式调度器。后续会在 `ports/` 和 `bsp/` 下
增加具体 MCU 的适配代码。

## 文档语言

- 中文文档：当前页面
- [English README](README.en.md)
