# AUTOETH

在 Linux 上运行的**车载以太网通信与端到端保护 SDK**。提供 SOME/IP 通信、服务发现与订阅、TP 大报文分段重组、E2E 数据保护，并配套可运行的服务端、客户端与运维工具。

C++14（`.cpp`）+ C99（公开头），仅依赖标准库、POSIX 与 Threads，无第三方依赖。

> [!NOTE]
> 当前版本为 `0.1.0`，处于骨架阶段（S0/S1）。基础库与公开接口已可用，网络传输层尚未接入，详见[项目现状](#项目现状)。

## 功能范围

| 领域 | 内容 |
|---|---|
| SOME/IP 线协议 | 16B 报文头 / 20B TP 头加解帧、消息校验顺序（protocol → service → interface → method → type → ReturnCode → correlation → payload） |
| 编解码 | 按 schema 的序列化与反序列化、checked 长度运算、严格 UTF-8/UTF-16、对齐与端序 |
| TP | FO_CONTIGUOUS 与 CP_STREAMING 两套独立状态机；非末段上界「先取 min 再对齐」 |
| E2E | 14 个 profile（01/02/04/04m/05/06/07/07m/08/08m/11/22/44/44m），各自独立状态与计数域；SM 状态机；宽域 P08/P08m |
| 服务发现 | SD 原始编解码、接收端分类流水线、服务选择分层、订阅与字段初值 |
| Runtime | 服务注册与可用性、RPC 调用与取消、句柄代数、资源账本、诊断环、快照 |
| Classic C 兼容 | E2E、CRC、SoAd、PduR、SomeIpTp、Sd、SomeIpXf、E2EXf 的 C 接口 |
| 配置 | UTF-8 JSON 加载、固定校验顺序、结构化诊断（`ConfigDetail`） |

## 目录结构

```
include/autoeth/          C++14 公开头
include/autoeth/classic/  C99 公开头（Platform_Types.h、E2E.h、Crc.h …）
src/core/                 编解码、线协议、TP、SD、CRC
src/e2e/                  14 个 E2E profile + 宽域 + 状态机
src/runtime/              配置加载、Runtime
src/classic_host/         Classic C 适配层
src/apps/                 autoethd / autoeth-client / autoethctl
examples/                 独立 C99 与 C++14 消费者
share/autoeth/examples/   示例配置与数据集
tests/unit/               自研轻量断言 + CTest
tools/                    验证与分析脚本（Python 3）
docs/                     需求、设计与实施记录
```

## 构建

需要 CMake ≥ 3.20 与支持 C++14 的编译器（开发环境为 GCC 13）。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

构建选项：

| 选项 | 默认 | 说明 |
|---|---|---|
| `AUTOETH_BUILD_SHARED` | `ON` | 构建共享库 |
| `AUTOETH_BUILD_STATIC` | `ON` | 构建静态库 |
| `AUTOETH_BUILD_TESTS` | `ON` | 构建单元测试 |
| `AUTOETH_BUILD_CONSUMERS` | `ON` | 构建独立的 C99 / C++14 消费样例 |
| `AUTOETH_ANALYSIS_HOST` | `OFF` | 静态分析专用头文件适配，产品构建不需要 |

产物为三个库与三个程序：

- `autoeth_core`、`autoeth_e2e`、`autoeth_runtime`（导入名 `autoeth::core`、`autoeth::e2e`、`autoeth::runtime`）
- `autoethd`、`autoeth-client`、`autoethctl`

> [!TIP]
> 也可只构建单一形态，例如 `-DAUTOETH_BUILD_STATIC=OFF` 仅构建共享库。导入别名会自动指向实际存在的目标。

## 使用

### C++14

```cpp
#include "autoeth/wire.hpp"

autoeth::Header header;
std::size_t frame_bytes = 0;
autoeth::Status status = autoeth::decode_header(
    autoeth::ByteView(frame.data(), frame.size()), header, frame_bytes);
if (status.IsOk()) {
  // header.service_id, header.method_id, header.message_id ...
}
```

### C99

```c
#include "autoeth/classic/Crc.h"

const uint8 data[4] = {0u, 0u, 0u, 0u};
uint8 crc = Crc_CalculateCRC8(data, 4u, 0u, TRUE);
```

### 安装与集成

```bash
cmake --install build --prefix /opt/autoeth
```

安装后可用 `find_package(autoeth REQUIRED)` 引入，安装树可整体搬迁而不失效：

```cmake
find_package(autoeth REQUIRED)
target_link_libraries(app PRIVATE autoeth::runtime)
```

## 程序

### autoethd

VehicleService 服务端守护进程。

```bash
autoethd --config server.json \
         --dataset vehicle-dataset.hex --dataset-format hex \
         --event-period-ns 100000000
```

数据集在发布可用性前被完整读入并记录哈希，后续文件变更不影响该不可变数据集。

### autoeth-client

演示客户端，支持 `discover`、`read-snapshot`、`read-bulk`、`apply-calibration`、`record-command`、`field-get`、`field-set`、`subscribe`。

```bash
autoeth-client read-snapshot --config client.json --service vehicle --timeout-ns 1000000000
```

退出码：`0` 成功 / `2` 参数或配置 / `3` 无服务或超时 / `4` 远端应用错误 / `5` 协议或 E2E / `6` IO 或资源 / `130` 中断。

### autoethctl

运维工具，支持 `validate-config`、`validate-schema`、`decode-frame`、`inspect-capture`、`e2e-protect`、`e2e-check`、`inspect-state`。

```bash
# 校验配置
autoethctl validate-config --config server.json

# 解析 16 字节报文头
autoethctl decode-frame --file frame.hex
```

## 测试

```bash
cd build && ctest --output-on-failure
```

当前 **122 项断言全部通过**，覆盖 checked 运算边界、CRC 校验值与空指针防护、报文头编解码、消息校验顺序、TP 分段预算、编解码对齐与容量、E2E 计数器域与宽域窄化、Runtime 准入、配置加载等。

位几何换算另有独立等价性验证：

```bash
python3 tools/verify_bit_geometry.py
```

## 项目现状

已实测可用：

- 三个库（shared/static）× 三个程序，构建 **0 error 0 warning**
- 15 个 C 头逐条对齐接口清单，14 个 E2E profile 各自独立源文件
- 安装、重定位、外部 C99/C++14 消费者编译运行成功
- `validate-config` 校验示例配置；`decode-frame` 正确解析报文头；`autoethd` 读取数据集并正常停止

尚未达成，如实标注：

- **传输层未接入**。`autoeth-client` 命令只到「接受」为止，不产生真实 RPC；socket 与 owner 循环待实现。
- **IPv6 仅有编解码与静态用例**，未做跨主机实机验证。
- **静态分析**报告的 warning 级发现尚未处理；error 级已收口（MISRA C++ 1-0-3，详见 `docs/08_实施/`）。

## 文档

| 文档 | 内容 |
|---|---|
| `docs/05_需求澄清/` | 需求梳理、环境实测、已确认结论 |
| `docs/06_需求/需求规格.md` | 248 条编号需求，逐条标注原件出处 |
| `docs/06_需求/需求规格审查报告.md` | 需求与原件逐条回查结论 |
| `docs/07_设计/详细设计.md` | 内部实现设计与分阶段实施计划 |
| `docs/08_实施/` | 代码生成、静态分析与模型检验记录 |

需求与设计的正文为中文，引用合同条款时附英文原文编号与摘录。